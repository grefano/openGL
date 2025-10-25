#version 330 core
out vec4 FragColor;

in vec3 Color;
in vec2 texCoord;
in float texIndex;

uniform sampler2D texture;

void main()
{
   FragColor = vec4(Color, 1.0);
};