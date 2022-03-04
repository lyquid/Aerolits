#version 330 core

layout (location = 0) in vec3 pos_in;
layout (location = 1) in vec2 offset_in;
layout (location = 2) in vec4 color_in;

uniform mat4 mvp;

out vec4 color;

void main() {
  gl_Position = mvp * vec4(pos_in + vec3(offset_in, 0.0), 1.0);
  color = color_in;
}
