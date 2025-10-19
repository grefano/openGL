
#version 330 core
layout (location =0) in vec3 aPos;
layout (location =1) in vec3 aColor;
layout (location =2) in vec2 aTex;

out vec3 Color;
out vec2 texCoord;
uniform mat4 transform;

//uniform float time;
//out float time;

void main()
{
   gl_Position = transform * vec4(aPos, 1.0);
   // gl_Position = vec4(aPos.x * (1 + scale), aPos.y * (1 + scale), aPos.z * (1 + scale), 1.0);
   Color = aColor;
   texCoord = aTex;
   
};