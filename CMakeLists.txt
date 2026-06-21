cmake_minimum_required(VERSION 3.20)
project(YANGBoy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Redirect output to bin directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin)

include(CTest)
include(FetchContent)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.14.1
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(spdlog)

# Core library
add_library(YANGBOY_CORE
    src/core/Bus.cpp
    src/core/IBus.cpp
    src/core/Cartridge.cpp
    src/core/SM83.cpp
    src/core/Ram.cpp
    src/core/IO.cpp
)

# Include directories
target_include_directories(YANGBOY_CORE PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Add spdlog as a public library
target_link_libraries(YANGBOY_CORE PUBLIC spdlog::spdlog)

# Main executable
add_executable(yangboy src/main.cpp)

target_link_libraries(yangboy PRIVATE YANGBOY_CORE)

# ================= TESTING =================
if(BUILD_TESTING)

    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    FetchContent_Declare(
        nlohmann_json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    FetchContent_MakeAvailable(nlohmann_json)

    add_subdirectory(tests)
endif()