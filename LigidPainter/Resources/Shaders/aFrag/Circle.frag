
/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Renders a smooth circle

*/

//OpenGL version used in the app (Major 4 , Minor 0, Profile core)
#version 400 core

out vec4 fragColor;
in vec2 TexCoords;

void main()
{
	vec2 p = 2. * (TexCoords.xy - 0.5);
    float tau = 3.1415926535*2.0;
    float a = atan(p.x,p.y);
    float r = length(p)*0.75;
    vec2 uv = vec2(a/tau,r);
	
	vec3 horColour = vec3(0.25, 0.25, 0.25);

	// draw color beam
	uv = (2.0 * uv) - 1.0;
	float beamWidth = (0.7+0.5*cos(uv.x*10.0*tau*0.15*clamp(0., 0.0, 10.0))) * abs(1.0 / (30.0 * uv.y));
	beamWidth *= 1.4;
    vec3 horBeam = vec3(beamWidth);

	fragColor.rgb = ( horBeam) * horColour;
    fragColor.a = fragColor.r;
}