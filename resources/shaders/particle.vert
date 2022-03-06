#version 330 core

layout (location = 0) in vec3 pos_in;
layout (location = 1) in vec2 tex_coord_in;
layout (location = 2) in vec3 offset_in;
layout (location = 3) in vec4 color_in;
layout (location = 4) in float size_in;

uniform mat4 mvp;

out vec2 tex_coord;
out vec4 color;

void main() {
  gl_Position = mvp * vec4((pos_in * size_in) + offset_in, 1.0);
  tex_coord = tex_coord_in;
  color = color_in;
}
