#version 330 core
out vec4 FragColor;

in vec3 Color;
in vec2 texCoord;
//in float time;
uniform sampler2D tex0;
//uniform float time;
void main()
{
   FragColor = texture(tex0, texCoord);
   
   //FragColor = vec4(Color, 1.0f);
};