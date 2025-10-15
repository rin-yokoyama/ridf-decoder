#include <iostream>
#include <fstream>

#include <arrow/api.h>
#include <arrow/io/api.h>
#include <arrow/ipc/api.h>
#include <arrow/pretty_print.h>
#include <arrow/record_batch.h>
#include <arrow/type.h>
#include <parquet/arrow/writer.h>

// word size for RIDF (=4)
static const size_t kWordSize = 4;

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

        // Read the block
        auto buff = std::make_unique<char[]>(block_size * 2);
        ridf.read(buff.get(), block_size * 2);

        PARQUET_THROW_NOT_OK(block_id_builder->Append(block_count));
        PARQUET_THROW_NOT_OK(block_builder->Append((const uint8_t *)buff.get(), block_size * 2));
        ++block_count;

        if (!(block_count % 1000))
        {
            double progress = (double)ridf.tellg() / (double)size * 100.;
            std::cout << "block_count: " << block_count << ", " << ridf.tellg() << "bytes/" << size << "bytes (" << progress << "\%)" << std::endl;
        }

        //  Break if the block size is greater than EoF
        if (block_count > n_block)
            break;

        if (ridf.tellg() == -1)
            break;
    }
    ridf.close();

    // finalize and write to parquet file
    std::shared_ptr<arrow::Array> block_id_array, block_array;
    PARQUET_THROW_NOT_OK(block_id_builder->Finish(&block_id_array));
    PARQUET_THROW_NOT_OK(block_builder->Finish(&block_array));
    auto schema = arrow::schema({arrow::field("block_id", arrow::int32()), arrow::field("block_data", arrow::binary())});
    auto table = arrow::Table::Make(schema, {block_id_array, block_array});
    std::shared_ptr<arrow::io::FileOutputStream> outfile;
    PARQUET_ASSIGN_OR_THROW(
        outfile,
        arrow::io::FileOutputStream::Open(output_file_name));
    PARQUET_THROW_NOT_OK(
        parquet::arrow::WriteTable(*table, pool, outfile, 1048576L));

    std::cout
        << "Terminating" << std::endl;
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