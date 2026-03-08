#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D tex;

void main()
{
    vec4 color = texture(tex, TexCoord);
    color.r /= 2.0;
    FragColor = color;
}