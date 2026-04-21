cmake_minimum_required(VERSION 3.20)
project(YANGBoy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

# Core library
add_library(YANGBOY_CORE
    src/core/Bus.cpp
    src/core/Cart.cpp
    src/core/SM83.cpp
    src/core/Ram/WRAM.cpp
    src/core/Ram/HRAM.cpp
)

# Logger library
add_library(LOGGER
    src/Logger.cpp
)

# Include directories
target_include_directories(YANGBOY_CORE PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(LOGGER PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Link dependencies
target_link_libraries(YANGBOY_CORE PUBLIC LOGGER)

# Main executable
add_executable(yangboy src/main.cpp)

target_link_libraries(yangboy PRIVATE YANGBOY_CORE)

# ================= TESTING =================
if(BUILD_TESTING)
    include(FetchContent)

    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(
        cpu_tests
        tests/cpu/cpu_testing.cpp
    )

    target_link_libraries(
        cpu_tests
        PRIVATE
            YANGBOY_CORE
            LOGGER
            GTest::gtest
            GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(cpu_tests)
endif()