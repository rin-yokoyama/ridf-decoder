find_package(Arrow REQUIRED)
find_package(Parquet REQUIRED)

add_library(decoder ParquetWriter.cpp)
target_link_libraries(decoder arrow parquet anacore)
target_include_directories(decoder PRIVATE ${CMAKE_SOURCE_DIR}/sources/anacore)
install(TARGETS decoder DESTINATION lib)

function(addExec exec_name)
    add_executable(${exec_name} ${exec_name}.cpp)
    target_include_directories(${exec_name} PRIVATE ${CMAKE_SOURCE_DIR}/sources/anacore)
    target_link_libraries(${exec_name} arrow parquet anacore decoder)

    install(TARGETS ${exec_name}
        RUNTIME DESTINATION bin)
endfunction()

addExec(ridf2parquet)