#version 330 core

in vec2 tex_coord;

uniform sampler2D our_texture;

out vec4 frag_color;

void main() {
  frag_color = texture(our_texture, tex_coord);
}
