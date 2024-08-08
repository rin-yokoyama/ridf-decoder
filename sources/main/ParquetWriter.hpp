#ifndef __PARQUET_WRITER_HPP__
#define __PARQUET_WRITER_HPP__
#include <iostream>
#include <arrow/api.h>
#include <arrow/io/api.h>
#include <arrow/ipc/api.h>
#include <arrow/pretty_print.h>
#include <arrow/record_batch.h>
#include <arrow/type.h>
#include <parquet/arrow/writer.h>
#include <vector>
#include "TArtRawEventObject.hh"

/**
 * @brief Class for writing Apache Arrow data to a file or buffer stream.
 *
 */
class ParquetWriter
{
public:
    ParquetWriter();
    virtual ~ParquetWriter() {}

    std::shared_ptr<arrow::Table> GenerateTable();
    std::shared_ptr<arrow::RecordBatch> GenerateRecordBatch();
    void WriteParquetFile(std::string name, std::shared_ptr<arrow::Table> table);
    std::shared_ptr<arrow::Buffer> WriteStream(std::shared_ptr<arrow::Table> table);
    std::shared_ptr<arrow::Buffer> WriteStream(std::shared_ptr<arrow::RecordBatch> batch);

    void Fill(TArtRawEventObject *rawevent, const u_int64_t &event_id);

protected:
    arrow::MemoryPool *pool_ = nullptr;
    std::shared_ptr<arrow::DataType> seg_struct_type_;
    std::shared_ptr<arrow::DataType> hit_struct_type_;

    std::shared_ptr<arrow::UInt64Builder> event_id_builder_;
    std::shared_ptr<arrow::Int32Builder> runnumber_builder_;
    std::shared_ptr<arrow::UInt64Builder> ts_builder_;

    std::shared_ptr<arrow::Int32Builder> device_id_builder_;
    std::shared_ptr<arrow::Int32Builder> fp_id_builder_;
    std::shared_ptr<arrow::Int32Builder> module_id_builder_;
    std::shared_ptr<arrow::Int32Builder> detector_id_builder_;
    std::shared_ptr<arrow::StructBuilder> seg_struct_builder_;
    std::shared_ptr<arrow::ListBuilder> seg_list_builder_;

    std::shared_ptr<arrow::Int32Builder> geo_builder_;
    std::shared_ptr<arrow::Int32Builder> channel_builder_;
    std::shared_ptr<arrow::UInt32Builder> value_builder_;
    std::shared_ptr<arrow::Int32Builder> edge_builder_;
    std::shared_ptr<arrow::StructBuilder> hit_struct_builder_;
    std::shared_ptr<arrow::ListBuilder> hit_list_builder_;
    std::shared_ptr<arrow::Schema> schema_;
};

#endif /*__PARQUET_WRITER_HPP__*/