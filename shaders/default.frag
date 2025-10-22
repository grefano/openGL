#version 330 core
out vec4 FragColor;

in vec3 Color;
in vec2 texCoord;
in float texIndex;

uniform sampler2D textures[2];

void main()
{
   int index = int(texIndex);
   FragColor = texture(textures[index], texCoord);
};