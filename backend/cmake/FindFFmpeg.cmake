# FindFFmpeg.cmake
# Find FFmpeg libraries

find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h)
find_library(AVCODEC_LIBRARY avcodec)

find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h)
find_library(AVFORMAT_LIBRARY avformat)

find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h)
find_library(AVUTIL_LIBRARY avutil)

find_path(SWRESAMPLE_INCLUDE_DIR libswresample/swresample.h)
find_library(SWRESAMPLE_LIBRARY swresample)

if(AVCODEC_INCLUDE_DIR AND AVCODEC_LIBRARY AND
   AVFORMAT_INCLUDE_DIR AND AVFORMAT_LIBRARY AND
   AVUTIL_INCLUDE_DIR AND AVUTIL_LIBRARY)
    set(FFMPEG_FOUND TRUE)
    
    set(AVCodec_INCLUDE_DIRS ${AVCODEC_INCLUDE_DIR} ${AVUTIL_INCLUDE_DIR})
    set(AVCodec_LIBRARIES ${AVCODEC_LIBRARY} ${AVUTIL_LIBRARY})
    
    set(AVFormat_INCLUDE_DIRS ${AVFORMAT_INCLUDE_DIR} ${AVUTIL_INCLUDE_DIR})
    set(AVFormat_LIBRARIES ${AVFORMAT_LIBRARY} ${AVUTIL_LIBRARY})
    
    set(AVUtil_INCLUDE_DIRS ${AVUTIL_INCLUDE_DIR})
    set(AVUtil_LIBRARIES ${AVUTIL_LIBRARY})
    
    set(SWResample_INCLUDE_DIRS ${SWRESAMPLE_INCLUDE_DIR} ${AVUTIL_INCLUDE_DIR})
    set(SWResample_LIBRARIES ${SWRESAMPLE_LIBRARY} ${AVUTIL_LIBRARY})
else()
    set(FFMPEG_FOUND FALSE)
endif()

if(FFMPEG_FOUND)
    message(STATUS "Found FFmpeg")
    message(STATUS "  AVCodec: ${AVCODEC_LIBRARY}")
    message(STATUS "  AVFormat: ${AVFORMAT_LIBRARY}")
    message(STATUS "  AVUtil: ${AVUTIL_LIBRARY}")
    message(STATUS "  SWResample: ${SWRESAMPLE_LIBRARY}")
    
    # Create imported targets
    add_library(AVCodec::AVCodec UNKNOWN IMPORTED)
    set_target_properties(AVCodec::AVCodec PROPERTIES
        IMPORTED_LOCATION "${AVCODEC_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${AVCODEC_INCLUDE_DIR}"
    )
    
    add_library(AVFormat::AVFormat UNKNOWN IMPORTED)
    set_target_properties(AVFormat::AVFormat PROPERTIES
        IMPORTED_LOCATION "${AVFORMAT_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${AVFORMAT_INCLUDE_DIR}"
    )
    
    add_library(AVUtil::AVUtil UNKNOWN IMPORTED)
    set_target_properties(AVUtil::AVUtil PROPERTIES
        IMPORTED_LOCATION "${AVUTIL_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${AVUTIL_INCLUDE_DIR}"
    )
    
    add_library(SWResample::SWResample UNKNOWN IMPORTED)
    set_target_properties(SWResample::SWResample PROPERTIES
        IMPORTED_LOCATION "${SWRESAMPLE_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${SWRESAMPLE_INCLUDE_DIR}"
    )
else()
    message(WARNING "FFmpeg not found")
endif()