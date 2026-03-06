#version 330 core

in vec2 TexCoord;

uniform sampler2D baseTex;
uniform sampler2D overlayTex;

out vec4 FragColor;

void main()
{
    vec4 base = texture(baseTex, TexCoord);
    vec4 overlay = texture(overlayTex, TexCoord);

    FragColor = mix(base, overlay, overlay.a);
}