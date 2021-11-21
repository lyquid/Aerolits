find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(tests hello_test.cpp)
target_link_libraries(tests GTest::GTest GTest::Main)
gtest_discover_tests(tests)
