#version 330 core

layout (location = 0) in vec3 pos_in;
layout (location = 1) in vec3 color_in;
layout (location = 2) in vec2 tex_coord_in;
layout (location = 3) in vec3 offset;

uniform mat4 mvp;

out vec3 color;
out vec2 tex_coord;

void main() {
  gl_Position = mvp * vec4(pos_in + offset, 1.0);
  color = color_in;
  tex_coord = tex_coord_in;
}
