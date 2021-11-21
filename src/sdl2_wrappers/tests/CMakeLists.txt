find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(SDL2_wrappers_tests
  sdl2_audio_tests.cpp ../sdl2_audio.cpp
  sdl2_geometry_tests.cpp ../sdl2_geometry.cpp
)
target_link_libraries(
  SDL2_wrappers_tests
  GTest::GTest
  GTest::Main
  SDL2::SDL2
  SDL2::SDL2_mixer
  box2d::box2d
)
gtest_discover_tests(SDL2_wrappers_tests)
