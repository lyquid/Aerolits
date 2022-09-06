find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(SDL2_wrappers_tests
  sdl2_audio_tests.cpp ../sdl2_audio.cpp
  sdl2_geometry_tests.cpp ../sdl2_geometry.cpp
)
if(DEFINED CMAKE_TOOLCHAIN_FILE)
  target_link_libraries(SDL2_wrappers_tests
    GTest::GTest
    GTest::Main
    OpenGL::GL
    GLEW::GLEW
    glm::glm
    SDL2::SDL2
    SDL2::SDL2_image
    $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
    $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
    box2d::box2d
  )
else()
  target_link_libraries(SDL2_wrappers_tests
    GTest::GTest
    GTest::Main
    OpenGL::GL
    GLEW::GLEW
    glm::glm
    ${SDL2_LIBRARY}
    ${SDL2_IMAGE_LIBRARY}
    ${SDL2_TTF_LIBRARY}
    ${SDL2_MIXER_LIBRARY}
    box2d::box2d
  )
endif()

gtest_discover_tests(SDL2_wrappers_tests)
