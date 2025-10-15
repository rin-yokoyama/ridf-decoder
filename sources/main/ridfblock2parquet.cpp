#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <arrow/api.h>
#include <arrow/io/api.h>
#include <arrow/ipc/api.h>
#include <arrow/pretty_print.h>
#include <arrow/record_batch.h>
#include <arrow/type.h>
#include <parquet/arrow/writer.h>

// word size for RIDF (=4)
static const size_t kWordSize = 4;
// Maximum file size before rolling (2GB)
static const u_int64_t kMaxFileSize = 2ULL * 1024 * 1024 * 1024;

// Helper function to generate filename with sequence number
std::string generateFileName(const std::string &base_name, int sequence)
{
    if (sequence == 0)
    {
        return base_name;
    }

    // Find the extension
    size_t dot_pos = base_name.find_last_of('.');
    if (dot_pos == std::string::npos)
    {
        // No extension
        std::ostringstream oss;
        oss << base_name << "_" << std::setfill('0') << std::setw(3) << sequence;
        return oss.str();
    }
    else
    {
        // Has extension
        std::string name = base_name.substr(0, dot_pos);
        std::string ext = base_name.substr(dot_pos);
        std::ostringstream oss;
        oss << name << "_" << std::setfill('0') << std::setw(3) << sequence << ext;
        return oss.str();
    }
}

// Helper function to write accumulated data to parquet file
void writeParquetFile(const std::string &filename,
                      std::shared_ptr<arrow::Int32Builder> &block_id_builder,
                      std::shared_ptr<arrow::BinaryBuilder> &block_builder,
                      arrow::MemoryPool *pool)
{
    std::shared_ptr<arrow::Array> block_id_array, block_array;
    PARQUET_THROW_NOT_OK(block_id_builder->Finish(&block_id_array));
    PARQUET_THROW_NOT_OK(block_builder->Finish(&block_array));
    auto schema = arrow::schema({arrow::field("block_id", arrow::int32()), arrow::field("block_data", arrow::binary())});
    auto table = arrow::Table::Make(schema, {block_id_array, block_array});
    std::shared_ptr<arrow::io::FileOutputStream> outfile;
    PARQUET_ASSIGN_OR_THROW(
        outfile,
        arrow::io::FileOutputStream::Open(filename));
    PARQUET_THROW_NOT_OK(
        parquet::arrow::WriteTable(*table, pool, outfile, 1048576L));
    std::cout << "Written file: " << filename << std::endl;
}

void readRIDF(std::string ridf_file_name, std::string output_file_name, u_int64_t n_block)
{
    // Open ridf file
    std::ifstream ridf(ridf_file_name, std::ios::binary);
    if (!ridf)
    {
        std::cout << "Cannot open file: " << ridf_file_name << std::endl;
        return;
    }
    std::cout << "File opened: " << ridf_file_name << std::endl;

    // Get total size
    ridf.seekg(0, std::ios::end);
    u_int64_t size = ridf.tellg();
    std::cout << "File size: " << size << " bytes." << std::endl;
    ridf.seekg(0);

    arrow::MemoryPool *pool = arrow::default_memory_pool();
    std::shared_ptr<arrow::Int32Builder> block_id_builder = std::make_shared<arrow::Int32Builder>(pool);
    std::shared_ptr<arrow::BinaryBuilder> block_builder = std::make_shared<arrow::BinaryBuilder>(pool);

    u_int64_t block_count = 0;
    u_int64_t current_file_size = 0;
    int file_sequence = 0;

    while (true)
    {
        // Break if end of file
        if (ridf.eof())
            break;

        u_int32_t block_size;
        u_int32_t class_id;
        u_int32_t layer;

        auto readHeader = [&ridf](u_int32_t &block_size, u_int32_t &class_id, u_int32_t &layer)
        {
            char header[kWordSize];
            ridf.read(header, kWordSize);
            u_int32_t *header32 = (u_int32_t *)header;
            block_size = (header32[0] & 0x003fffff);
            class_id = ((header32[0] & 0x0fc00000) >> 22);
            layer = ((header32[0] & 0x30000000) >> 28);
            ridf.seekg((u_int32_t)ridf.tellg() - kWordSize);
            // std::cout << "read block: " << block_size << ", class id: " << class_id << ", layer: " << layer << std::endl;
        };

        // Read header and get the block size
        readHeader(block_size, class_id, layer);

        // Break if the block size is greater than EoF
        if (((u_int32_t)ridf.tellg() + (2 * block_size)) > size)
            break;

        // Break if the block size is zero
        if (!block_size)
            break;

        // Estimate the size this block will add to the output
        u_int64_t estimated_block_output_size = block_size * 2 + 64; // Add some overhead for metadata

        // Check if adding this block would exceed the file size limit
        if (current_file_size + estimated_block_output_size > kMaxFileSize && current_file_size > 0)
        {
            // Write current accumulated data to file
            std::string current_filename = generateFileName(output_file_name, file_sequence);
            writeParquetFile(current_filename, block_id_builder, block_builder, pool);

            // Reset builders for next file
            block_id_builder = std::make_shared<arrow::Int32Builder>(pool);
            block_builder = std::make_shared<arrow::BinaryBuilder>(pool);
            current_file_size = 0;
            file_sequence++;
        }

        // Read the block
        auto buff = std::make_unique<char[]>(block_size * 2);
        ridf.read(buff.get(), block_size * 2);

        PARQUET_THROW_NOT_OK(block_id_builder->Append(block_count));
        PARQUET_THROW_NOT_OK(block_builder->Append((const uint8_t *)buff.get(), block_size * 2));
        current_file_size += estimated_block_output_size;
        ++block_count;

        if (!(block_count % 1000))
        {
            double progress = (double)ridf.tellg() / (double)size * 100.;
            std::cout << "block_count: " << block_count << ", " << ridf.tellg() << "bytes/" << size << "bytes (" << progress << "\%), current file size: " << current_file_size << " bytes" << std::endl;
        }

        //  Break if the block size is greater than EoF
        if (block_count > n_block)
            break;

        if (ridf.tellg() == -1)
            break;
    }
    ridf.close();

    // Write remaining data to final file
    if (current_file_size > 0)
    {
        std::string final_filename = generateFileName(output_file_name, file_sequence);
        writeParquetFile(final_filename, block_id_builder, block_builder, pool);
    }

    std::cout << "Terminating. Total files created: " << (file_sequence + 1) << std::endl;
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cout << "Usage: ridfblock2parquet [input_file] [output_file] [n_block(optional)]" << std::endl;
        return 1;
    }
    std::string input_file_name = argv[1];  // input file name
    std::string output_file_name = argv[2]; // output file name
    u_int64_t n_block = -1;
    if (argc == 4)
        n_block = std::atoi(argv[3]);

    // main loop
    readRIDF(input_file_name, output_file_name, n_block);
    return 0;
}