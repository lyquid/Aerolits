add_library(SDL2_wrappers
  sdl2_audio.cpp
  sdl2_geometry.cpp
  sdl2_opengl.cpp
  sdl2_renderer.cpp
  sdl2_sound.cpp
  sdl2_texture.cpp
  sdl2_window.cpp
)

if(DEFINED CMAKE_TOOLCHAIN_FILE)
  target_link_libraries(SDL2_wrappers PRIVATE
    OpenGL::GL
    GLEW::GLEW
    glm::glm
    SDL2::SDL2
    SDL2::SDL2main
    SDL2::SDL2_image
    $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
    $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
    box2d::box2d
  )
else()
  target_link_libraries(SDL2_wrappers PRIVATE
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

add_subdirectory(tests)
