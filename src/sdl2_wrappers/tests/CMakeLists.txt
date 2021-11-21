find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(SDL2_wrappers_tests hello_test.cpp)
target_link_libraries(SDL2_wrappers_tests GTest::GTest GTest::Main)
gtest_discover_tests(SDL2_wrappers_tests)
