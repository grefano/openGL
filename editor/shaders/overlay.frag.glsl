#version 330 core
in vec2 TexCoord;

uniform sampler2D tex1;
uniform sampler2D tex2;

out vec4 FragColor;
uniform vec2 overlayPos;   // posição do canto superior esquerdo (0.0 a 1.0)
uniform vec2 overlaySize;  // tamanho (ex: 0.5, 0.5 = metade da tela)

void main()
{
    vec4 base = texture(tex1, TexCoord);
    vec2 overlayCoord = (TexCoord - overlayPos) / overlaySize;

    if (overlayCoord.x >= 0.0 && overlayCoord.x <= 1.0 &&
        overlayCoord.y >= 0.0 && overlayCoord.y <= 1.0)
    {
        vec4 overlay = texture(tex2, overlayCoord);
        FragColor = overlay * vec4(vec3(0.5), 1.0);
    }
    else
    {
        FragColor = base;
    }
}