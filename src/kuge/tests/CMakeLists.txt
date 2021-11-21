find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(KUGE_tests hello_test.cpp)
target_link_libraries(KUGE_tests GTest::GTest GTest::Main)
gtest_discover_tests(KUGE_tests)
