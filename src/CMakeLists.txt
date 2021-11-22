add_executable(Aerolits
  ../main.cpp
  ../lib/pugixml1.11/pugixml.cpp
  aerolite.cpp
  background.cpp
  config_parser.cpp
  contact_listener.cpp
  emitter.cpp
  explosion.cpp
  game.cpp
  game_state.cpp
  input_component.cpp
  particle.cpp
  player.cpp
  projectile.cpp
  testing.cpp
)
target_compile_features(Aerolits PUBLIC cxx_std_17)
set_target_properties(Aerolits PROPERTIES CXX_EXTENSIONS OFF)

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
    SDL2::SDL2
    box2d::box2d
    KUGE)
else()
  target_link_libraries(Aerolits PRIVATE
    ${SDL2_LIBRARY}
    box2d::box2d
    KUGE
  )
endif()

#target_link_libraries(Aerolits PRIVATE SDL2::SDL2 box2d::box2d KUGE)
target_include_directories(Aerolits PRIVATE "${Aerolits_SOURCE_DIR}/src/include")

install(TARGETS Aerolits RUNTIME DESTINATION ${BIN_DIR})

add_subdirectory(tests)
