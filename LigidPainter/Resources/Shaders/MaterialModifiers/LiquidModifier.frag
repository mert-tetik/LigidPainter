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

// Scaling
uniform float scale = 20.;
uniform float yScale = 1.;

// Noise
uniform int octaves = 8;
uniform float fbmXOffset = 0.;
uniform float fbmYOffset = 7.7;
uniform float fbmXScale = 1.;
uniform float fbmYScale = 1.;
uniform float roughness = 0.3;
uniform float frequencyAcceleration = 2.;
uniform float fbmTotal = 0.0;
uniform float seed = 0.3183099;

//General
uniform float noiseInfluence = 6.;
uniform float positionInfluence = 1.8;

//Colors
uniform vec3 color1;
uniform vec3 color2;
uniform vec3 color3;
uniform vec3 color4;

/* Element Properties */
uniform float wetness = 1.;
uniform float metallic = 0.;
uniform float height = 1.;
uniform float ambientOcclusion = 1.;

/* Channel Properties*/
uniform int state;
uniform sampler2D mask;
uniform sampler2D previousTxtr;
uniform float opacity;
uniform float depthValue;
uniform sampler2D depthTxtr;
uniform sampler2D selectedPrimitiveIDS;
uniform sampler2D meshMask;
uniform int primitiveCount;
uniform int useMeshMask;

/* Fragment inputs */
in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;
in vec3 Tangent;
in vec3 Bitangent;
in vec4 ProjectedPos;

/* Fragment Output*/
out vec4 fragColor;

float innerHash(vec3 p)  // replace this by something better
{
    p  = fract( p * seed + .1 );
	p *= 17.0;
    return fract( p.x*p.y*p.z*(p.x+p.y+p.z) );
}

float innerNoise2D( in vec3 x )
{
    vec3 i = floor(x);
    vec3 f = fract(x);
    f = f*f*(3.0-2.0*f);
	
    return mix(mix(mix( innerHash(i+vec3(0,0,0)), 
                        innerHash(i+vec3(1,0,0)),f.x),
                   mix( innerHash(i+vec3(0,1,0)), 
                        innerHash(i+vec3(1,1,0)),f.x),f.y),
               mix(mix( innerHash(i+vec3(0,0,1)), 
                        innerHash(i+vec3(1,0,1)),f.x),
                   mix( innerHash(i+vec3(0,1,1)), 
                        innerHash(i+vec3(1,1,1)),f.x),f.y),f.z);
}

float innerFbm(vec3 p) {
    
    float freq = 1.;
    
    float amplitude = 1.0;
    
    float total = fbmTotal;
    float maxTotal = 0.0;
    
    for (int i = 0; i < octaves; ++i) {
        total += amplitude * innerNoise2D(p * freq);
        maxTotal += amplitude;
        
        freq *= frequencyAcceleration;
        amplitude *= roughness;
    }
    
    return total / maxTotal;
}

vec3 fbm33( vec3 p )
{
    return vec3(innerFbm(p * fbmXScale + vec3(fbmXOffset)), innerFbm(p * fbmYScale + vec3(fbmYOffset)), innerFbm(p * fbmXScale + vec3(fbmXOffset + 4.6)));
}

//====================================================================

// Function to calculate a value and store intermediate results in 'result'
float calculateSurfaceValue(vec3 position, out vec4 result)
{
    // Generate two levels of fractal Brownian motion (fbm)
    vec3 offset = fbm33(0.9 * position);
    // offset += 0.04 * sin(length(offset));

    vec3 noise = fbm33(3.0 * offset);

    // Store the intermediate results in 'result'
    result = vec4(offset, noise);

    // Calculate a final value using 'innerFbm' and apply some mixing
    float finalValue = 0.5 + 0.5 * innerFbm(positionInfluence * position + noiseInfluence * noise);

    return mix(finalValue, finalValue * finalValue * finalValue * 3.5, finalValue * abs(noise.x));
}

void main()
{
    if(useMeshMask == 1){
        float prim = texelFetch(selectedPrimitiveIDS, ivec2(gl_PrimitiveID % int(ceil(sqrt(primitiveCount))), gl_PrimitiveID / int(ceil(sqrt(primitiveCount)))), 0).r;
        bool selectedPrim = prim > 0.9 && texture(meshMask, TexCoords).r > 0.5;
        if(!selectedPrim){
            fragColor = vec4(0.);
            return;
        }
    }

    // Scale and offset the fragment coordinates
    vec3 pixelPosition = Pos * scale + 1.;
    pixelPosition.y *= yScale;
    
    float epsilon = 0.001;

    // Initialize 'surfaceResult' with zeros
    vec4 surfaceResult = vec4(0.0);

    // Calculate the surface value using the 'calculateSurfaceValue' function
    float surfaceValue = calculateSurfaceValue(pixelPosition, surfaceResult);

    // Initialize baseColor with a base value
    vec3 baseColor = vec3(0.0);

    // Apply color mixing based on the surface value 'surfaceValue'
    
    if(state == 0){
        baseColor = mix(vec3(1.) - color1, vec3(1.) - color2, surfaceValue);
        baseColor = mix(baseColor, vec3(1.) - color2, dot(surfaceResult.zw, surfaceResult.zw));
        baseColor = mix(baseColor, vec3(1.) - color3, 0.2 + 0.5 * surfaceResult.y * surfaceResult.y);
        baseColor = mix(baseColor, vec3(1.) - color4, 0.5 * smoothstep(1.2, 1.3, abs(surfaceResult.z) + abs(surfaceResult.w)));
    }
    else{
        baseColor = mix(vec3(1.), vec3(0.), surfaceValue);
        baseColor = mix(baseColor, vec3(0.), dot(surfaceResult.zw, surfaceResult.zw));
        baseColor = mix(baseColor, vec3(0.5), 0.2 + 0.5 * surfaceResult.y * surfaceResult.y);
        baseColor = mix(baseColor, vec3(1.), 0.5 * smoothstep(1.2, 1.3, abs(surfaceResult.z) + abs(surfaceResult.w)));
    }

    // Apply clamping and lighting calculations
    baseColor = clamp(baseColor * surfaceValue * 2.0, 0.0, 1.0);

    vec4 normalResult;
    // Calculate the normal vector for shading
    vec3 surfaceNormal = normalize(
                                        vec3(
                                                calculateSurfaceValue(pixelPosition + vec3(epsilon, 0.0, epsilon), normalResult) - surfaceValue,
                                                2.0 * epsilon,
                                                calculateSurfaceValue(pixelPosition + vec3(0.0, epsilon, 0.0), normalResult) - surfaceValue
                                            )
                                    );

    // Calculate the final color by combining diffuse lighting and material color
    vec3 lClr = vec3(0.25);
    if(state == 0)
        lClr = vec3(1.);
    vec3 lightingColor = lClr * (surfaceNormal.y * 0.5 + 0.5) ;
    baseColor *= 1.2 * lightingColor;
    baseColor = 1.0 - baseColor;
    baseColor = 1.1 * baseColor * baseColor;

    // Set the final fragment color
    fragColor = vec4(baseColor, 1.0);
    
    /* Roughness */
    if(state == 1){
        fragColor.rgb = vec3(mix(fragColor.r + 0.2, 1. - fragColor.r, wetness));
    }
    
    
    /* Metallic */
    else if(state == 2){
        fragColor.rgb = vec3(mix(0., (1. - fragColor.r) * 2.5, metallic));
    }
    
    /* Normal Map */
    else if(state == 3){

    }
    
    /* Height */
    else if(state == 4){
        fragColor.rgb = vec3(mix(1.2 - (1. - fragColor.r) * 2.5, (1. - fragColor.r) * 2.5, height));
    }
    
    /* Ambient Occlusion */
    else if(state == 5){
        fragColor.rgb = vec3(mix((1. - fragColor.r)*2., (1. - fragColor.r) * 1.6 + 0.6, ambientOcclusion));
    }

    float alpha = opacity;
    alpha *= texture(mask, TexCoords).r; 

    float dpth = texture(depthTxtr, TexCoords).r; 
    if(depthValue < dpth)
        alpha *= 0;

    vec3 clrResult = mix(texture(previousTxtr, TexCoords).rgb, fragColor.rgb, alpha);

    fragColor = vec4(clrResult, 1.);
}