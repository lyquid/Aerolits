add_executable(Aerolits
  ../main.cpp
  ../lib/pugixml1.11/pugixml.cpp
  aerolite.cpp
  background.cpp
  camera.cpp
  config_parser.cpp
  contact_listener.cpp
  debug_draw.cpp
  emitter.cpp
  explosion.cpp
  game.cpp
  game_state.cpp
  input_component.cpp
  particle.cpp
  player.cpp
  projectile.cpp
  resources.cpp
  testing.cpp
)
target_compile_features(Aerolits PUBLIC cxx_std_17)
set_target_properties(Aerolits PROPERTIES CXX_EXTENSIONS OFF)

if(DEFINED CMAKE_TOOLCHAIN_FILE)
 target_include_directories(Aerolits PRIVATE ${STB_INCLUDE_DIRS})
endif()

#include_directories(${Aerolits_SOURCE_DIR}/lib/imgui/**)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
	set(MY_DEBUG_OPTIONS /Wall /RTC)
	set(MY_RELEASE_OPTIONS /w3 /O2)
	target_compile_options(Aerolits PUBLIC "$<$<CONFIG:DEBUG>:${MY_DEBUG_OPTIONS}>")
	target_compile_options(Aerolits PUBLIC "$<$<CONFIG:RELEASE>:${MY_RELEASE_OPTIONS}>")

elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
	set(MY_DEBUG_OPTIONS -Wall -Wconversion -Wdouble-promotion -Weffc++ -Wextra -Wfloat-equal -Wmain -Wshadow -fstack-usage -pedantic -g3)
	set(MY_RELEASE_OPTIONS -O2)
	target_compile_options(Aerolits PUBLIC "$<$<CONFIG:DEBUG>:${MY_DEBUG_OPTIONS}>")
	target_compile_options(Aerolits PUBLIC "$<$<CONFIG:RELEASE>:${MY_RELEASE_OPTIONS}>")

elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
	set(MY_DEBUG_OPTIONS -Wall -Wconversion -Wdouble-promotion -Weffc++ -Wextra -Wfloat-equal -Wmain -Wshadow -pedantic -g3)
	set(MY_RELEASE_OPTIONS -O2)
	target_compile_options(Aerolits PUBLIC "$<$<CONFIG:DEBUG>:${MY_DEBUG_OPTIONS}>")
	target_compile_options(Aerolits PUBLIC "$<$<CONFIG:RELEASE>:${MY_RELEASE_OPTIONS}>")
endif()

if(DEFINED CMAKE_TOOLCHAIN_FILE)
  target_link_libraries(Aerolits PRIVATE
    box2d::box2d
    glm::glm
    imgui
    KUGE
    SDL2::SDL2
    SDL2_wrappers
  )
else()
  target_link_libraries(Aerolits PRIVATE
    box2d::box2d
    glm::glm
    imgui
    KUGE
    ${SDL2_LIBRARY}
    SDL2_wrappers
  )
endif()

target_include_directories(Aerolits PRIVATE "${Aerolits_SOURCE_DIR}/src/include")

install(TARGETS Aerolits RUNTIME DESTINATION ${BIN_DIR})

add_subdirectory(tests)
