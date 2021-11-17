cmake_minimum_required(VERSION 3.11...3.20)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

project(
  Aerolits
  VERSION 0.1.0
  DESCRIPTION "Asteroids clone"
  LANGUAGES CXX )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Let's nicely support folders in IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  # Testing only available if this is the main app
  # Note this needs to be done in the main CMakeLists
  # since it calls enable_testing, which must be in the
  # main CMakeLists.
  include(CTest)
	enable_testing()

  # Docs only available if this is the main app
  find_package(Doxygen)
  if(Doxygen_FOUND)
    #add_subdirectory(docs)
  else()
    message(STATUS "Doxygen not found, not building docs")
  endif()
endif()

add_executable(Aerolits main.cpp
                        lib/pugixml1.11/pugixml.cpp
                        src/kuge/event.cpp
                        src/kuge/system.cpp
                        src/sdl2_wrappers/sdl2_audio.cpp
                        src/sdl2_wrappers/sdl2_geometry.cpp
                        src/sdl2_wrappers/sdl2_renderer.cpp
                        src/sdl2_wrappers/sdl2_sound.cpp
                        src/sdl2_wrappers/sdl2_texture.cpp
                        src/sdl2_wrappers/sdl2_window.cpp
                        src/aerolite.cpp
                        src/background.cpp
                        src/config_parser.cpp
                        src/contact_listener.cpp
                        src/emitter.cpp
                        src/explosion.cpp
                        src/game.cpp
                        src/game_state.cpp
                        src/input_component.cpp
                        src/particle.cpp
                        src/player.cpp
                        src/projectile.cpp
                        src/testing.cpp)

target_compile_features(Aerolits PUBLIC cxx_std_17)
set_target_properties(Aerolits PROPERTIES CXX_EXTENSIONS OFF)

# Use our modified FindSDL2* modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${Aerolits_SOURCE_DIR}/cmake")

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

# Set an output directory for our binaries
set(BIN_DIR ${Aerolits_SOURCE_DIR}/bin)

if(DEFINED CMAKE_TOOLCHAIN_FILE)
	message(STATUS "Using vcpkg toolchain file")
	find_package(SDL2 CONFIG REQUIRED)
	find_package(sdl2-image CONFIG REQUIRED)
	find_package(sdl2-ttf CONFIG REQUIRED)
	find_package(sdl2-mixer CONFIG REQUIRED)
	find_package(box2d CONFIG REQUIRED)
	target_link_libraries(Aerolits PRIVATE SDL2::SDL2 SDL2::SDL2main SDL2::SDL2_image SDL2::SDL2_ttf SDL2::SDL2_mixer box2d::box2d)
else()
	message(STATUS "Using cmake find modules")
	find_package(SDL2 REQUIRED)
	find_package(SDL2_image REQUIRED)
	find_package(SDL2_ttf REQUIRED)
	find_package(SDL2_mixer REQUIRED)
	include_directories(${SDL2_MIXER_INCLUDE_DIR})
	set(box2d_DIR "H:/GRUPOS/ACREDITACIONES/TRABAJADORES_CA/ALEX/PrOG/repos/box2d-2.4.1/build/src/CMakeFiles/Export/lib/cmake/box2d")
	find_package(box2d CONFIG REQUIRED)
	target_link_libraries(Aerolits PRIVATE ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${SDL2_TTF_LIBRARY} ${SDL2_MIXER_LIBRARY} box2d::box2d)
endif()

install(TARGETS Aerolits RUNTIME DESTINATION ${BIN_DIR})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
