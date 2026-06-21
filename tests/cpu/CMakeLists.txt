

file(GLOB_RECURSE CPU_TEST_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_executable(cpu_tests ${CPU_TEST_SOURCES})

target_link_libraries(cpu_tests
    PRIVATE
        YANGBOY_CORE
        GTest::gtest
        GTest::gtest_main
        nlohmann_json::nlohmann_json
)

target_compile_definitions(cpu_tests PRIVATE
    JSON_TEST_DIR="${PROJECT_SOURCE_DIR}/JSON"
)

set_target_properties(cpu_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/tests
)

include(GoogleTest)
gtest_discover_tests(cpu_tests)