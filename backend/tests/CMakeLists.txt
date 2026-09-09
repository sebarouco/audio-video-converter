cmake_minimum_required(VERSION 3.20)
project(OnlineFileConverterTests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find GTest
find_package(GTest REQUIRED)

# Include parent directory for headers
include_directories(${CMAKE_SOURCE_DIR}/../include)

# Test executables
add_executable(user_test
    unit/domain/entities/UserTest.cpp
)

target_link_libraries(user_test
    GTest::GTest
    GTest::Main
)

add_executable(fileformat_test
    unit/domain/valueobjects/FileFormatTest.cpp
)

target_link_libraries(fileformat_test
    GTest::GTest
    GTest::Main
)

add_executable(logger_test
    unit/common/LoggerTest.cpp
)

target_link_libraries(logger_test
    GTest::GTest
    GTest::Main
    spdlog::spdlog
)

# Enable testing
enable_testing()

# Add tests
add_test(NAME UserTest COMMAND user_test)
add_test(NAME FileFormatTest COMMAND fileformat_test)
add_test(NAME LoggerTest COMMAND logger_test)