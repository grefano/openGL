#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D tex;

void main()
{
    vec2 overlayCoord = TexCoord;

    if (overlayCoord.x >= 0.0 && overlayCoord.x <= 1.0 &&
       overlayCoord.y >= 0.0 && overlayCoord.y <= 1.0)
    {
        vec4 base = texture(tex, overlayCoord);
       FragColor = base;
    }
    else
    {
       FragColor = vec4(0.0,0.0,0.0,0.0);
    }

   //  vec4 color = texture(tex, TexCoord);
   //  color.r /= 2.0;
   //  FragColor = color;
}