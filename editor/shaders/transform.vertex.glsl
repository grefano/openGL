#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTex;

out vec2 TexCoord;

uniform vec2 offset;
uniform vec2 scale;

void main()
{
    
    TexCoord = aTex * scale + offset;
    gl_Position = vec4(aPos, 0.0, 1.0);
}