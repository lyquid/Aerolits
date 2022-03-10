#version 330

layout(location = 0) in vec2 pos_in;
layout(location = 1) in vec4 color_in;
layout(location = 2) in float size_in;

uniform mat4 mvp;

out vec4 color;

void main() {
  gl_Position = mvp * vec4(pos_in, 0.0f, 1.0f);
  gl_PointSize = size_in;
  color = color_in;
}
