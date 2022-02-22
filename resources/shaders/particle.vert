#version 330 core

layout (location = 0) in vec3 pos_in;
layout (location = 1) in vec2 tex_coord_in;
layout (location = 2) in vec3 offset_in;

uniform mat4 mvp;

out vec2 tex_coord;

void main() {
  gl_Position = mvp * vec4(pos_in + offset_in, 1.0);
  tex_coord = tex_coord_in;
}
