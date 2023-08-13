/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#version 400 core

/* Stripes */
uniform vec3 color = vec3(0.9, 0.2,0.05);

uniform float scale = 32.;
uniform float stripeFrequency = 10.;
uniform float gapFrequency = 30.;
uniform int style;

/* Element Properties */
uniform float wetness = 1.;
uniform float metallic = 0.;
uniform float height = 1.;

/* Channel Properties*/
uniform int state;
uniform sampler2D mask;
uniform sampler2D previousTxtr;
uniform float opacity;
uniform float depthValue;
uniform sampler2D depthTxtr;

/* Fragment inputs */
in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;
in vec3 Tangent;
in vec3 Bitangent;
in vec4 ProjectedPos;

/* Fragment Output*/
out vec4 fragColor;

// copy from https://www.shadertoy.com/view/4l2GzW
float r(float n)
{
 	  return fract(cos(n*89.42)*343.42);
}
vec3 r(vec3 n)
{
 	  return vec3(r(n.x*23.62 - 300.0 + n.y * 34.35 + n.z * 12.864),r(n.x*45.13+256.0+n.y*38.89 + n.z * 14.112),r(n.x*23.62-300.0+n.y*34.35 + n.z * + n.z * 16.468)); 
}

float worley(vec3 n,float s)
{
    float dis = 2.0;
    for(int x = -1;x<=1;x++)
    {
        for(int y = -1;y<=1;y++)
        {
            for(int z = -1;z<=1;z++){
                vec3 p = floor(n/s) + vec3(x,y,z);
                float d = length(r(p)+vec3(x,y,z) - fract(n/s));
                if (dis>d)
                {
                    dis = d;   
                }
            }
        }
    }
    return 1.0 - dis;
	
}

// copy from https://www.shadertoy.com/view/4sc3z2

#define MOD3 vec3(.1031,.11369,.13787)

vec3 hash33(vec3 p3)
{
	  p3 = fract(p3 * MOD3);
    p3 += dot(p3, p3.yxz+19.19);
    return -1.0 + 2.0 * fract(vec3((p3.x + p3.y)*p3.z, (p3.x+p3.z)*p3.y, (p3.y+p3.z)*p3.x));
}
float perlin_noise(vec3 p)
{
    vec3 pi = floor(p);
    vec3 pf = p - pi;
    
    vec3 w = pf * pf * (3.0 - 2.0 * pf);
    
    return 	mix(
        		mix(
                	mix(dot(pf - vec3(0, 0, 0), hash33(pi + vec3(0, 0, 0))), 
                        dot(pf - vec3(1, 0, 0), hash33(pi + vec3(1, 0, 0))),
                       	w.x),
                	mix(dot(pf - vec3(0, 0, 1), hash33(pi + vec3(0, 0, 1))), 
                        dot(pf - vec3(1, 0, 1), hash33(pi + vec3(1, 0, 1))),
                       	w.x),
                	w.z),
        		mix(
                    mix(dot(pf - vec3(0, 1, 0), hash33(pi + vec3(0, 1, 0))), 
                        dot(pf - vec3(1, 1, 0), hash33(pi + vec3(1, 1, 0))),
                       	w.x),
                   	mix(dot(pf - vec3(0, 1, 1), hash33(pi + vec3(0, 1, 1))), 
                        dot(pf - vec3(1, 1, 1), hash33(pi + vec3(1, 1, 1))),
                       	w.x),
                	w.z),
    			w.y);
}
float getNoise(vec3 uv)
{
    
    float dis = (1.0 + perlin_noise(vec3(uv)*8.0)) * (1.0+(worley(uv, 32.0) + 0.5*worley(2.0*uv,32.0) + 0.25*worley(4.0*uv,32.0) ));
	  return dis/4.0;
}

//TODO 3D FABRIC FUNCTIONS
vec3 getFabric1(vec3 uv){
    
    // Generate a fabric-like pattern using a combination of noise and stripes
    float noise = sin(uv.x * 5.0) * sin(uv.y * 5.0) * 0.2 + 0.8;  // Noise pattern
    float stripesx = sin(uv.x + uv.z * stripeFrequency) * 0.5 + 0.5;  // Vertical stripes pattern
    float stripesy = sin(uv.y * gapFrequency) * 0.5 + 0.5;
    //float stripesz = sin(uv.z * stripeFrequency) * 0.5 + 0.5;
  
    // Combine the patterns to create the fabric texture
    vec3 color = vec3(noise * stripesx);
    vec3 colory = vec3(noise * stripesy);
    color = cos(color);
    colory = cos(colory);
  
    return color * colory;
}

vec3 getFabric2(vec3 uv){
    // Generate a fabric-like pattern using sine waves
    float stripe = sin(uv.y * stripeFrequency) * 0.5 + 0.5;  // Vertical stripe pattern
    float weave = sin((uv.x + uv.y + uv.z) * gapFrequency) * 0.5 + 0.5;  // Diagonal weave pattern

    // Combine the patterns to create the fabric texture
    vec3 color = vec3(stripe * weave);

    return color;
}

vec3 getFabric3(vec3 uv){
    return vec3(1.) - getFabric2(uv);
}

void main() {
    vec3 uv = Pos * scale;  // Adjust the scale of the fabric texture

    vec3 fabric = vec3(0.);

    if(style == 0)
      fabric = getFabric1(uv);
    if(style == 1)
      fabric = getFabric2(uv);
    if(style == 2)
      fabric = getFabric3(uv);

    float noi = getNoise(uv);

    fragColor = vec4(mix((fabric),(fabric)/2. , noi * 0.5), 1.0);

    /*Albdo*/
    if(state == 0)
        fragColor.rgb = mix(fragColor.rgb, color, fragColor.r);
  
    /*Roughness*/
    if(state == 1)
        fragColor.rgb = vec3(mix(1., 1. - fragColor.r, wetness));

    /*Metallic*/
    if(state == 2)
       fragColor.rgb = vec3(mix(0., fragColor.r, metallic));

    /*Normal Map*/
    if(state == 3){}

    /*Height*/
    if(state == 4)
       fragColor.rgb = vec3(mix(1. - fragColor.r, fragColor.r, height));

    /*Ambient Occlusion*/
    if(state == 5)
       fragColor.rgb = vec3(fragColor.r);
    

    float alpha = opacity;
    alpha *= texture(mask, TexCoords).r; 

    float dpth = texture(depthTxtr, TexCoords).r; 
    if(depthValue < dpth)
        alpha *= 0;


    vec3 clrResult = mix(texture(previousTxtr, TexCoords).rgb, fragColor.rgb, alpha);

    fragColor = vec4(clrResult, 1.);
}
