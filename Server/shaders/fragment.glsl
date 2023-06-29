#version 410 core

uniform sampler2D image;

in vec2 texCoord;

out vec4 color;
void main() {
  color = texture2D(image, texCoord);
}
