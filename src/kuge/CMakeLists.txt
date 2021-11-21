add_library(KUGE
  event.cpp
  system.cpp
)

target_link_libraries(KUGE PRIVATE SDL2::SDL2 SDL2_wrappers box2d::box2d)

# if(DEFINED CMAKE_TOOLCHAIN_FILE)
#   target_link_libraries(KUGE PRIVATE
#     SDL2::SDL2
#     SDL2::SDL2main
#     SDL2::SDL2_image
#     SDL2::SDL2_ttf
#     SDL2::SDL2_mixer
#     box2d::box2d
#     SDL2_wrappers
#   )
# else()
#   target_link_libraries(KUGE PRIVATE SDL2_wrappers box2d::box2d)
# endif()
