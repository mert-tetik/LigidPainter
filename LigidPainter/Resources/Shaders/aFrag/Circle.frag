
/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Renders a smooth circle

    ! Source : https://www.shadertoy.com/view/ssdSD2
*/

//OpenGL version used in the app (Major 4 , Minor 0, Profile core)
#version 400 core

out vec4 fragColor;
in vec2 TexCoords;

void main()
{
    float thickness = 0.05;
    float fade = 0.04;
    vec2 uv = TexCoords * 2.0 - 1.0;
    float aspect = 1.;
    uv.x *= aspect;
    float distance = 1.0 - length(uv);
    vec3 color = vec3(smoothstep(0.0, fade, distance));
    color *= vec3(smoothstep(thickness + fade, thickness, distance));
    fragColor = vec4(color.r);
}