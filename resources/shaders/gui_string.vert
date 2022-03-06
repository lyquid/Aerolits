#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex_coord_in;

out vec2 tex_coord;

void main() {
  gl_Position = vec4(pos, 1.0);
  tex_coord = tex_coord_in;
}
