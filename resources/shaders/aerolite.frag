#version 330 core

uniform vec4 aerolite_color;

out vec4 frag_color;

void main() {
  frag_color = aerolite_color;
}
