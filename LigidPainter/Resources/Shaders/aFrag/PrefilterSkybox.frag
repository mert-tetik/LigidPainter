/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Skybox fragment shader.

    Is used to prefilter a skybox

*/

//OpenGL version used in the app (Major 4 , Minor 0, Profile core)
#version 400 core

//Physics & math calculations
#pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Physics_Math.frag)

//Get the position data from the vertex shader
in vec3 Pos;

//The skybox texture (is a cubemap)
uniform samplerCube environmentMap;

//Roughness value used in the process of prefiltering (increase the value gradually as the mip level increases)
uniform float roughness;

//Fragment shader output
out vec4 FragColor;

  
// Main function that performs prefiltering of an environment map using importance sampling and GGX distribution.
void main()
{		
    vec3 N = normalize(Pos);    // Normalize the surface normal vector
    vec3 R = N;                 // Set the reflection vector to be the same as the surface normal
    vec3 V = R;                 // Set the view vector to be the same as the reflection vector

    const uint SAMPLE_COUNT = 1024u;    // Number of samples to take
    float totalWeight = 0.0;             // Total weight of the samples
    vec3 prefilteredColor = vec3(0.0);   // Accumulated color for prefiltering
    
    // Iterate over the samples
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        // Generate a sample point using Hammersley sequence
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        
        // Importance sample the GGX distribution to get the half vector
        vec3 H = ImportanceSampleGGX(Xi, N, roughness);
        
        // Compute the light vector by reflecting the view vector
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        // Calculate the dot product between the surface normal and the light vector
        float NdotL = max(dot(N, L), 0.0);
        
        // Accumulate the prefiltered color by sampling the environment map and multiplying with the dot product
        if(NdotL > 0.0)
        {
            prefilteredColor += texture(environmentMap, L).rgb * NdotL;
            totalWeight += NdotL;
        }
    }
    
    // Normalize the accumulated color by dividing by the total weight
    prefilteredColor = prefilteredColor / totalWeight;

    FragColor = vec4(prefilteredColor, 1.0);   // Set the output color as the prefiltered color
}
