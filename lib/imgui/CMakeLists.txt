add_library(imgui
  imgui_demo.cpp
  imgui_draw.cpp
  imgui_impl_opengl3.cpp
  imgui_impl_sdl.cpp
  imgui_stdlib.cpp
  imgui_tables.cpp
  imgui_widgets.cpp
  imgui.cpp
)

if(DEFINED CMAKE_TOOLCHAIN_FILE)
  target_link_libraries(imgui PRIVATE
    SDL2::SDL2
  )
else()
  target_link_libraries(imgui PRIVATE
    ${SDL2_LIBRARY}
  )
endif()
