#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color; // the color variable has attribute position 1

uniform mat4 transform;

out vec3 our_color;

void main() {
  gl_Position =/*  transform *  */vec4(pos, 1.0);
  our_color = color;
}
