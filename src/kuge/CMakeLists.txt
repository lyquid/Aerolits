add_library(KUGE
  event.cpp
  system.cpp
)

#target_link_libraries(KUGE PRIVATE SDL2::SDL2 SDL2_wrappers box2d::box2d)

add_subdirectory(tests)

if(DEFINED CMAKE_TOOLCHAIN_FILE)
  target_link_libraries(KUGE PRIVATE
    SDL2::SDL2
    box2d::box2d
    SDL2_wrappers
  )
else()
  target_link_libraries(KUGE PRIVATE
    ${SDL2_LIBRARY}
    SDL2_wrappers
    box2d::box2d
  )
endif()
