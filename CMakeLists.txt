cmake_minimum_required(VERSION 3.11...3.20)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

project(
  Aerolits
  VERSION 0.1.0
  DESCRIPTION "Asteroids clone"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# Use our modified FindSDL2* modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${Aerolits_SOURCE_DIR}/cmake")
# Set an output directory for our binaries
set(BIN_DIR ${Aerolits_SOURCE_DIR}/bin)

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

if(DEFINED CMAKE_TOOLCHAIN_FILE)
	message(STATUS "Using vcpkg toolchain file")
  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	find_package(SDL2 CONFIG REQUIRED)
	find_package(sdl2-image CONFIG REQUIRED)
	find_package(SDL2_ttf CONFIG REQUIRED)
	find_package(SDL2_mixer CONFIG REQUIRED)
	find_package(box2d CONFIG REQUIRED)
  find_package(OpenGL REQUIRED)
  find_package(GLEW REQUIRED)
  find_package(glm CONFIG REQUIRED)
  find_package(pugixml CONFIG REQUIRED)
else()
	message(STATUS "Using CMake find modules")
	find_package(SDL2 REQUIRED)
	find_package(SDL2_image REQUIRED)
	find_package(SDL2_ttf REQUIRED)
	find_package(SDL2_mixer REQUIRED)
	include_directories(${SDL2_MIXER_INCLUDE_DIR})
	find_package(box2d CONFIG REQUIRED)
  find_package(OpenGL REQUIRED)
  find_package(GLEW CONFIG REQUIRED)
  set(glm_DIR "C:/Users/castillo_a/source/repos/glm/cmake/glm")
  find_package(glm CONFIG REQUIRED)
  find_package(pugixml CONFIG REQUIRED)
endif()

add_subdirectory(src)
add_subdirectory(src/kuge)
add_subdirectory(src/sdl2_wrappers)
add_subdirectory(lib/imgui)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
