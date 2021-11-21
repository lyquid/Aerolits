find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(Aerolites_src_tests hello_test.cpp)
target_link_libraries(Aerolites_src_tests GTest::GTest GTest::Main)
gtest_discover_tests(Aerolites_src_tests)
