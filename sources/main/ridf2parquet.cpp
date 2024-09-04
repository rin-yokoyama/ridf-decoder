#include <iostream>
#include <unistd.h>
#include <arrow/type.h>
#include <parquet/arrow/writer.h>
#include "TArtEventStore.hh"
#include "TArtRawEventObject.hh"
#include "TArtDecoderFactory.hh"
#include "ParquetWriter.hpp"

static const int kPrintFreq = 10000;
/** prints usage **/
void usage(char *argv0)
{
    std::cout << "[ridf2parquet]: Usage: \n"
              << argv0 << " -i [input_ridf_file_name]\n"
              << "-o [output_file_name] (default: [input_ridf_file_name].parquet)"
              << "-n [event_number_to_scan] (default: end)"
              << std::endl;
}

int main(int argc, char **argv)
{
    std::string input_file_name = "default";
    std::string output_file_name = "default";
    int max_event = 0;

    /** parsing commandline arguments **/
    if (argc < 3)
    {
        usage(argv[0]);
        return 1;
    }
    int opt = 0;
    while ((opt = getopt(argc, argv, "i:o:n:")) != -1)
    {
        switch (opt)
        {
        case 'i':
            input_file_name = optarg;
            break;
        case 'o':
            output_file_name = optarg;
            break;
        case 'n':
            max_event = std::stoi(optarg);
            break;
        default:
            usage(argv[0]);
            return 1;
            break;
        }
    }
    if (input_file_name == "default")
    {
        std::cerr << "[ridf2parquet]: No input file specified\nexitting..." << std::endl;
        return 1;
    }
    if (output_file_name == "default")
    {
        output_file_name = input_file_name.substr(0, -4) + ".parquet";
    }

    /// Open the input ridf file using anaroot
    TArtEventStore *estore = new TArtEventStore();
    std::cout << "[ridf2parquet]: Opening input file " << input_file_name << std::endl;
    estore->Open(input_file_name.c_str());
    TArtRawEventObject *rawevent = estore->GetRawEventObject();

    ParquetWriter writer;
    bool flag = max_event;
    u_int64_t n_event = 0;
    /// event loop
    while (estore->GetNextEvent())
    {
        writer.Fill(rawevent, n_event);
        estore->ClearData();
        ++n_event;
        if (flag && (n_event > max_event))
        {
            break;
        }
        if (!(n_event % kPrintFreq))
        {
            std::cout << "[ridf2parquet]: " << n_event << " events scanned." << std::endl;
        }
    }
    /// Finalize arrays and generate table
    auto table = writer.GenerateTable();

    /// Write to a parquet file
    std::cout << "[ridf2parquet]: Writing outputs to file: " << output_file_name << std::endl;
    writer.WriteParquetFile(output_file_name, table);

    return 0;
}
