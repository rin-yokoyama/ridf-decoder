set(LIB_NAME anacore)

set(HEADERS
    TArtParserFactory.hh
    TArtDecoderFactory.hh
    TArtDataSource.hh
    segidlist.hh
    TArtRawScalerObject.hh
    TArtRawSegmentObject.hh
    TArtRawDataObject.hh
    TArtRawEventObject.hh
    TArtParser.hh
    TArtParserRDF.hh
    TArtRunInfo.hh
    TArtEventStore.hh
    TArtFileDataSource.hh
    TArtStoreManager.hh
    TArtEventInfo.hh
    TArtParserRIDF.hh
    TArtDecoder.hh
    TArtDecoderV830.hh
    TArtDecoderV7XX.hh
    TArtDecoderV767.hh
    TArtDecoderV550.hh
    TArtDecoderV1290.hh
    TArtDecoderV1190.hh
    TArtDecoderV1190C.hh
    TArtDecoderP716XZ.hh
    TArtDecoderP716X.hh
    TArtDecoderNeulandTS.hh
    TArtDecoderMTDC32.hh
    TArtDecoderMQDC32.hh
    TArtDecoderMDPP16.hh
    TArtDecoderMADC32.hh
    TArtDecoderLUPO.hh
    TArtDecoderL3377d.hh
    TArtDecoderHINP.hh
    TArtDecoder.hh
    TArtDecoderFixed32.hh
    TArtDecoderFixed24.hh
    TArtDecoderFixed16.hh
    TArtDecoderFactory.hh
    TArtDecoderDSP.hh
    TArtDecoderCTM405.hh
    TArtDecoderBigEndianV7XX.hh
    TArtDecoderAMTTDC.hh
    TArtDecoderAD413A.hh
    TArtDecoderA3400.hh
    TArtDecoderA3100.hh
    TArtDecoderRPV130.hh
    TArtDecoderSIS3820.hh
)

set(SOURCES
    TArtRawDataObject.cc
    TArtDecoderFactory.cc
    TArtDataSource.cc
    TArtRawScalerObject.cc
    TArtRawSegmentObject.cc
    TArtRawEventObject.cc
    TArtParserFactory.cc
    TArtParserRDF.cc
    TArtParser.cc
    TArtParserRIDF.cc
    TArtFileDataSource.cc
    TArtEventStore.cc
    TArtStoreManager.cc
    TArtDecoderV830.cc
    TArtDecoderV7XX.cc
    TArtDecoderV767.cc
    TArtDecoderV550.cc
    TArtDecoderV1290.cc
    TArtDecoderV1190C.cc
    TArtDecoderV1190.cc
    TArtDecoderP716XZ.cc
    TArtDecoderP716X.cc
    TArtDecoderNeulandTS.cc
    TArtDecoderMTDC32.cc
    TArtDecoderMQDC32.cc
    TArtDecoderMDPP16.cc
    TArtDecoderMADC32.cc
    TArtDecoderLUPO.cc
    TArtDecoderL3377d.cc
    TArtDecoderHINP.cc
    TArtDecoderFixed32.cc
    TArtDecoderFixed24.cc
    TArtDecoderFixed16.cc
    TArtDecoderFactory.cc
    TArtDecoderDSP.cc
    TArtDecoderCTM405.cc
    TArtDecoderBigEndianV7XX.cc
    TArtDecoderAMTTDC.cc
    TArtDecoderAD413A.cc
    TArtDecoderA3400.cc
    TArtDecoderA3100.cc
    TArtDecoderRPV130.cc
    TArtDecoderSIS3820.cc
)

add_library(${LIB_NAME} ${SOURCES})

target_include_directories(${LIB_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(${LIB_NAME} z)

set_target_properties(
    ${LIB_NAME}
    PROPERTIES
        PUBLIC_HEADER "${headers}"
)

install(TARGETS ${LIB_NAME}
    PUBLIC_HEADER DESTINATION include
    LIBRARY DESTINATION lib
)