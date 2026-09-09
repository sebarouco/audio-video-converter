cmake_minimum_required(VERSION 3.20)
project(OnlineFileConverter VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/domain
    ${CMAKE_SOURCE_DIR}/include/application
    ${CMAKE_SOURCE_DIR}/include/infrastructure
    ${CMAKE_SOURCE_DIR}/include/presentation
    ${CMAKE_SOURCE_DIR}/include/common
    ${CMAKE_SOURCE_DIR}/cmake
)

# Find packages
find_package(Drogon CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

# Find FFmpeg using custom module
include(${CMAKE_SOURCE_DIR}/cmake/FindFFmpeg.cmake)
if(NOT FFMPEG_FOUND)
    message(FATAL_ERROR "FFmpeg not found")
endif()

# SQLite3 is provided by Drogon, no need to find it separately

# Redis and bcrypt disabled for now (not available without sudo)
# find_package(hiredis REQUIRED)
# find_package(bcrypt REQUIRED)

# Third-party libraries
# nlohmann/json (using system version if available, otherwise fetch)
include(FetchContent)
if(NOT TARGET nlohmann_json::nlohmann_json)
    FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
    FetchContent_MakeAvailable(json)
endif()

# spdlog
if(NOT TARGET spdlog::spdlog)
    FetchContent_Declare(spdlog URL https://github.com/gabime/spdlog/archive/refs/tags/v1.12.0.tar.gz)
    FetchContent_MakeAvailable(spdlog)
endif()

# jwt-cpp disabled - using simplified authentication
# FetchContent_Declare(jwt-cpp URL https://github.com/Thalhammer/jwt-cpp/archive/refs/tags/v0.6.0.tar.gz)
# FetchContent_MakeAvailable(jwt-cpp)

# Source files
file(GLOB_RECURSE DOMAIN_SOURCES "src/domain/*.cpp")
file(GLOB_RECURSE APPLICATION_SOURCES "src/application/*.cpp")
file(GLOB_RECURSE INFRASTRUCTURE_SOURCES "src/infrastructure/*.cpp")
file(GLOB_RECURSE PRESENTATION_SOURCES "src/presentation/*.cpp")
file(GLOB_RECURSE COMMON_SOURCES "src/common/*.cpp")

# Add new InMemoryConversionTaskRepository if not caught by glob
list(APPEND INFRASTRUCTURE_SOURCES "src/infrastructure/persistence/repositories/InMemoryConversionTaskRepository.cpp")

# Create library for each layer
add_library(domain_lib ${DOMAIN_SOURCES})
add_library(application_lib ${APPLICATION_SOURCES})
add_library(infrastructure_lib ${INFRASTRUCTURE_SOURCES})
add_library(presentation_lib ${PRESENTATION_SOURCES})
add_library(common_lib ${COMMON_SOURCES})

# Main executable - use functional_main.cpp for full implementation
add_executable(online_file_converter src/functional_main.cpp)

# Test conversion executable
add_executable(test_conversion test_conversion.cpp)

# Link libraries for main executable
target_link_libraries(online_file_converter
    PRIVATE
    domain_lib
    application_lib
    infrastructure_lib
    presentation_lib
    common_lib
    Drogon::Drogon
    OpenSSL::SSL
    OpenSSL::Crypto
    AVCodec::AVCodec
    AVFormat::AVFormat
    AVUtil::AVUtil
    SWResample::SWResample
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    Threads::Threads
)

# Link libraries for test conversion executable
target_link_libraries(test_conversion
    PRIVATE
    domain_lib
    infrastructure_lib
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    Threads::Threads
)

# Link dependencies for libraries
target_link_libraries(domain_lib PRIVATE nlohmann_json::nlohmann_json spdlog::spdlog)
target_link_libraries(application_lib PRIVATE domain_lib nlohmann_json::nlohmann_json spdlog::spdlog)
target_link_libraries(infrastructure_lib PRIVATE domain_lib application_lib nlohmann_json::nlohmann_json spdlog::spdlog)
target_link_libraries(presentation_lib PRIVATE application_lib infrastructure_lib Drogon::Drogon nlohmann_json::nlohmann_json spdlog::spdlog)
target_link_libraries(common_lib PRIVATE spdlog::spdlog)

# Install
install(TARGETS online_file_converter DESTINATION bin)
install(DIRECTORY config/ DESTINATION etc/online-file-converter)
install(DIRECTORY data/ DESTINATION var/lib/online-file-converter)

# Testing (disabled for now - GTest not available without sudo)
# enable_testing()
# find_package(GTest REQUIRED)
# add_subdirectory(tests)