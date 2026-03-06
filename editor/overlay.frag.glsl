#version 330 core

in vec2 TexCoord;

uniform sampler2D tex1;
uniform sampler2D tex2;

out vec4 FragColor;

void main()
{
    vec4 base = texture(tex1, TexCoord);
    vec4 overlay = texture(tex2, TexCoord);

    FragColor = mix(base, overlay, overlay.a);
}