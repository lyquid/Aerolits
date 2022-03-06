#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aOffset;

uniform mat4 mvp;

out vec3 fColor;

void main() {
  gl_Position = mvp * vec4(aPos + aOffset, 1.0);
  fColor = aColor;
}
