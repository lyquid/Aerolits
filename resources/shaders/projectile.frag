#version 330 core

uniform vec4 projectile_color;

out vec4 frag_color;

void main() {
  frag_color = projectile_color;
}
