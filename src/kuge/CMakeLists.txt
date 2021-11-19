add_library(KUGE
  event.cpp
  system.cpp
)

target_link_libraries(KUGE PRIVATE SDL2_wrappers box2d::box2d)
