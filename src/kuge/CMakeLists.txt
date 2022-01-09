add_library(KUGE
  audio_system.cpp
  backend_system.cpp
  event.cpp
  gui_system.cpp
)

add_subdirectory(tests)

if(DEFINED CMAKE_TOOLCHAIN_FILE)
  target_link_libraries(KUGE PRIVATE
    SDL2::SDL2
    box2d::box2d
    SDL2_wrappers
    glm::glm
  )
else()
  target_link_libraries(KUGE PRIVATE
    ${SDL2_LIBRARY}
    SDL2_wrappers
    box2d::box2d
    glm::glm
  )
endif()
