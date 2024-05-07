cmake_minimum_required(VERSION 2.8)

#add_executable(main main.cpp)
project(avio_dun_buf_example)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(ZLIB)

find_library(AVFILTER_LIB libavfilter.a HINTS "/opt/software/ffmpeg_build/lib/")
find_library(AVFORMAT_LIB libavformat.a HINTS "/opt/software/ffmpeg_build/lib/")
find_library(AVCODEC_LIB libavcodec.a HINTS "/opt/software/ffmpeg_build/lib/")
find_library(AVSWSSCALE_LIB libswscale.a HINTS "/opt/software/ffmpeg_build/lib/")
find_library(AVUTIL_LIB libavutil.a HINTS "/opt/software/ffmpeg_build/lib/")
find_library(SWRRESAMPLE_LIB libswresample.a HINTS "/opt/software/ffmpeg_build/lib/")

find_library(BLZMA_LIB liblzma.a HINTS "/usr/lib/x86_64-linux-gnu/")
find_library(BZ2_LIB libbz2.a HINTS "/usr/lib/x86_64-linux-gnu/")

find_library(X264_LIB libx264.a HINTS "/opt/software/ffmpeg_build/lib/")
find_library(X265_LIB libx265.a HINTS "/usr/lib/x86_64-linux-gnu/")
find_library(NUMA_LIB libnuma.a HINTS "/usr/lib/x86_64-linux-gnu/")
find_library(MP3LAME_LIB libmp3lame.a HINTS "/usr/lib/x86_64-linux-gnu/")

find_library(SSL_LIB libssl.a HINTS "/usr/local/lib/")
find_library(CRYPTO_LIB libcrypto.a HINTS "/usr/local/lib/")
find_library(TLS_LIB libtls.a HINTS "/usr/local/lib/")

message("AVFILTER_LIB = ${AVFILTER_LIB}")
message("AVFORMAT_LIB = ${AVFORMAT_LIB}")
message("AVCODEC_LIB = ${AVCODEC_LIB}")
message("AVSWSSCALE_LIB = ${AVSWSSCALE_LIB}")
message("AVUTIL_LIB = ${AVUTIL_LIB}")
message("SSL_LIB = ${SSL_LIB}")
message("CRYPTO_LIB = ${CRYPTO_LIB}")
message("BLZMA_LIB = ${BLZMA_LIB}")
message("BZ2_LIB = ${BZ2_LIB}")
message("X264_LIB = ${X264_LIB}")
message("X265_LIB = ${X265_LIB}")
message("SWRRESAMPLE_LIB = ${SWRRESAMPLE_LIB}")
message("NUMA_LIB = ${NUMA_LIB}")
message("MP3LAME_LIB = ${MP3LAME_LIB}")

set(SOURCE_EXE main.cpp)

set(SOURCE_LIB src/Libav.cpp)

add_library(Libav STATIC ${SOURCE_LIB})

include_directories(include)
include_directories(/opt/software/ffmpeg_build/include/)

add_executable(main ${SOURCE_EXE})

target_link_libraries(main PRIVATE Threads::Threads)
target_link_libraries(main Libav ${AVFILTER_LIB} ${AVFORMAT_LIB} ${AVCODEC_LIB} ${AVSWSSCALE_LIB} ${AVUTIL_LIB} ${SWRRESAMPLE_LIB})
target_link_libraries(main ZLIB::ZLIB)

target_link_libraries(main Libav ${BLZMA_LIB} ${BZ2_LIB})

target_link_libraries(main Libav ${X264_LIB} ${X265_LIB} ${NUMA_LIB} ${MP3LAME_LIB})

target_link_libraries(main ${CMAKE_DL_LIBS})

target_link_libraries(main Libav ${SSL_LIB} ${CRYPTO_LIB} ${TLS_LIB})
