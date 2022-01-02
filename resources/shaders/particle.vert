#version 330 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aOffset;

uniform mat4 mvp;
//uniform vec2 offsets[100];

out vec3 fColor;

void main() {
  gl_Position = mvp * vec4(pos + aOffset, 0.0, 1.0);
  // gl_Position = mvp * vec4(pos, 0.0, 1.0);
  // vec2 offset = offsets[gl_InstanceID];
  // gl_Position = vec4(pos + offset, 0.0, 1.0);
  fColor = aColor;
}
