add_library(Aerolits_src
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

target_link_libraries(Aerolits_src PRIVATE box2d::box2d KUGE)
target_include_directories(Aerolits_src PRIVATE "${Aerolits_SOURCE_DIR}/include")
