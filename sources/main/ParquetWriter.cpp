#include "ParquetWriter.hpp"

ParquetWriter::ParquetWriter()
{
    // Define the memory pool
    pool_ = arrow::default_memory_pool();

    // Define the structure for the hit list items
    auto geo_field = arrow::field("geo", arrow::int32());
    auto ch_field = arrow::field("ch", arrow::int32());
    auto value_field = arrow::field("value", arrow::uint32());
    auto edge_field = arrow::field("edge", arrow::int32());
    hit_struct_type_ = arrow::struct_({geo_field, ch_field, value_field, edge_field});

    // Define the structure for the seg list items
    auto device_id_field = arrow::field("dev", arrow::int32());
    auto fp_id_field = arrow::field("fp", arrow::int32());
    auto module_id_field = arrow::field("mod", arrow::int32());
    auto detector_id_field = arrow::field("det", arrow::int32());
    auto hit_list_field = arrow::field("hits", arrow::list(hit_struct_type_));
    seg_struct_type_ = arrow::struct_({device_id_field, fp_id_field, module_id_field, detector_id_field, hit_list_field});

    // Define schema
    schema_ = arrow::schema({arrow::field("event_id", arrow::uint64()),
                             arrow::field("runnumber", arrow::int32()),
                             arrow::field("ts", arrow::uint64()),
                             arrow::field("segdata", arrow::list(seg_struct_type_))});

    // Define Arrow builders
    geo_builder_ = std::make_shared<arrow::Int32Builder>(pool_);
    channel_builder_ = std::make_shared<arrow::Int32Builder>(pool_);
    value_builder_ = std::make_shared<arrow::UInt32Builder>(pool_);
    edge_builder_ = std::make_shared<arrow::Int32Builder>(pool_);
    std::vector<std::shared_ptr<arrow::ArrayBuilder>> hit_field_builders;
    hit_field_builders.emplace_back(geo_builder_);
    hit_field_builders.emplace_back(channel_builder_);
    hit_field_builders.emplace_back(value_builder_);
    hit_field_builders.emplace_back(edge_builder_);
    hit_struct_builder_ = std::make_shared<arrow::StructBuilder>(hit_struct_type_, pool_, hit_field_builders);
    hit_list_builder_ = std::make_shared<arrow::ListBuilder>(pool_, hit_struct_builder_);

    device_id_builder_ = std::make_shared<arrow::Int32Builder>(pool_);
    fp_id_builder_ = std::make_shared<arrow::Int32Builder>(pool_);
    module_id_builder_ = std::make_shared<arrow::Int32Builder>(pool_);
    detector_id_builder_ = std::make_shared<arrow::Int32Builder>(pool_);
    std::vector<std::shared_ptr<arrow::ArrayBuilder>> seg_field_builders;
    seg_field_builders.emplace_back(device_id_builder_);
    seg_field_builders.emplace_back(fp_id_builder_);
    seg_field_builders.emplace_back(module_id_builder_);
    seg_field_builders.emplace_back(detector_id_builder_);
    seg_field_builders.emplace_back(hit_list_builder_);
    seg_struct_builder_ = std::make_shared<arrow::StructBuilder>(seg_struct_type_, pool_, seg_field_builders);
    seg_list_builder_ = std::make_shared<arrow::ListBuilder>(pool_, seg_struct_builder_);

    event_id_builder_ = std::make_shared<arrow::UInt64Builder>(pool_);
    runnumber_builder_ = std::make_shared<arrow::Int32Builder>(pool_);
    ts_builder_ = std::make_shared<arrow::UInt64Builder>(pool_);
}

void ParquetWriter::Fill(TArtRawEventObject *rawevent, const u_int64_t &event_id)
{
    //  Append Data
    PARQUET_THROW_NOT_OK(event_id_builder_->Append(event_id));
    PARQUET_THROW_NOT_OK(runnumber_builder_->Append(rawevent->GetRunNumber()));
    PARQUET_THROW_NOT_OK(ts_builder_->Append(rawevent->GetTimeStamp()));
    PARQUET_THROW_NOT_OK(seg_list_builder_->Append());
    for (int i = 0; i < rawevent->GetNumSeg(); ++i)
    {
        PARQUET_THROW_NOT_OK(seg_struct_builder_->Append());
        TArtRawSegmentObject *seg = rawevent->GetSegment(i);

        PARQUET_THROW_NOT_OK(static_cast<arrow::Int32Builder *>(seg_struct_builder_->field_builder(0))->Append(seg->GetDevice()));
        PARQUET_THROW_NOT_OK(static_cast<arrow::Int32Builder *>(seg_struct_builder_->field_builder(1))->Append(seg->GetFP()));
        PARQUET_THROW_NOT_OK(static_cast<arrow::Int32Builder *>(seg_struct_builder_->field_builder(2))->Append(seg->GetModule()));
        PARQUET_THROW_NOT_OK(static_cast<arrow::Int32Builder *>(seg_struct_builder_->field_builder(3))->Append(seg->GetDetector()));
        PARQUET_THROW_NOT_OK(static_cast<arrow::ListBuilder *>(seg_struct_builder_->field_builder(4))->Append());
        for (int j = 0; j < seg->GetNumData(); ++j)
        {
            PARQUET_THROW_NOT_OK(hit_struct_builder_->Append());
            auto data = seg->GetData(j);
            PARQUET_THROW_NOT_OK(static_cast<arrow::Int32Builder *>(hit_struct_builder_->field_builder(0))->Append(data->GetGeo()));
            PARQUET_THROW_NOT_OK(static_cast<arrow::Int32Builder *>(hit_struct_builder_->field_builder(1))->Append(data->GetCh()));
            PARQUET_THROW_NOT_OK(static_cast<arrow::UInt32Builder *>(hit_struct_builder_->field_builder(2))->Append(data->GetVal()));
            PARQUET_THROW_NOT_OK(static_cast<arrow::Int32Builder *>(hit_struct_builder_->field_builder(3))->Append(data->GetEdge()));
        }
    }
}

std::shared_ptr<arrow::Table> ParquetWriter::GenerateTable()
{
    // Finalize the arrays
    std::shared_ptr<arrow::Array> event_id_array, runnumber_array, ts_array, seg_list_array;
    PARQUET_THROW_NOT_OK(event_id_builder_->Finish(&event_id_array));
    PARQUET_THROW_NOT_OK(runnumber_builder_->Finish(&runnumber_array));
    PARQUET_THROW_NOT_OK(ts_builder_->Finish(&ts_array));
    PARQUET_THROW_NOT_OK(seg_list_builder_->Finish(&seg_list_array));

    std::shared_ptr<arrow::Array> device_id_array, fp_id_array, module_id_array, detector_id_array, hit_list_array;
    PARQUET_THROW_NOT_OK(device_id_builder_->Finish(&device_id_array));
    PARQUET_THROW_NOT_OK(fp_id_builder_->Finish(&fp_id_array));
    PARQUET_THROW_NOT_OK(module_id_builder_->Finish(&module_id_array));
    PARQUET_THROW_NOT_OK(detector_id_builder_->Finish(&detector_id_array));
    PARQUET_THROW_NOT_OK(hit_list_builder_->Finish(&hit_list_array));

    std::shared_ptr<arrow::Array> geo_array, channel_array, value_array, edge_array;
    PARQUET_THROW_NOT_OK(geo_builder_->Finish(&geo_array));
    PARQUET_THROW_NOT_OK(channel_builder_->Finish(&channel_array));
    PARQUET_THROW_NOT_OK(value_builder_->Finish(&value_array));
    PARQUET_THROW_NOT_OK(edge_builder_->Finish(&edge_array));

    // Create the table
    auto table = arrow::Table::Make(schema_, {event_id_array, runnumber_array, ts_array, seg_list_array});

    return table;
}

std::shared_ptr<arrow::RecordBatch> ParquetWriter::GenerateRecordBatch()
{
    // Finalize the arrays
    std::shared_ptr<arrow::Array> event_id_array, runnumber_array, ts_array, seg_list_array;
    PARQUET_THROW_NOT_OK(event_id_builder_->Finish(&event_id_array));
    PARQUET_THROW_NOT_OK(runnumber_builder_->Finish(&runnumber_array));
    PARQUET_THROW_NOT_OK(ts_builder_->Finish(&ts_array));
    PARQUET_THROW_NOT_OK(seg_list_builder_->Finish(&seg_list_array));

    // Create the record batch
    return arrow::RecordBatch::Make(schema_, seg_list_builder_->length(), {event_id_array, runnumber_array, ts_array, seg_list_array});
}

void ParquetWriter::WriteParquetFile(std::string name, std::shared_ptr<arrow::Table> table)
{
    std::shared_ptr<arrow::io::FileOutputStream> outfile;
    PARQUET_ASSIGN_OR_THROW(
        outfile,
        arrow::io::FileOutputStream::Open(name));

    auto writer_properties = parquet::WriterProperties::Builder()
                                 .compression(parquet::Compression::ZSTD)
                                 ->build();

    PARQUET_THROW_NOT_OK(
        parquet::arrow::WriteTable(*table, pool_, outfile, 1048576L, writer_properties));
}

std::shared_ptr<arrow::Buffer> ParquetWriter::WriteStream(std::shared_ptr<arrow::Table> table)
{
    auto sink = arrow::io::BufferOutputStream::Create().ValueOrDie();
    arrow::TableBatchReader reader(table);
    std::shared_ptr<arrow::RecordBatch> batch;
    auto readState = reader.ReadNext(&batch);
    if (readState.ok() && batch.get())
    {
        auto writer = arrow::ipc::MakeStreamWriter(sink, schema_).ValueOrDie();
        writer->WriteRecordBatch(*batch.get());
        writer->Close();
    }
    return *sink->Finish();
}

std::shared_ptr<arrow::Buffer> ParquetWriter::WriteStream(std::shared_ptr<arrow::RecordBatch> batch)
{
    auto sink = arrow::io::BufferOutputStream::Create().ValueOrDie();
    auto writer = arrow::ipc::MakeStreamWriter(sink, schema_).ValueOrDie();
    auto status = writer->WriteRecordBatch(*batch.get());
    status = writer->Close();
    return *sink->Finish();
}