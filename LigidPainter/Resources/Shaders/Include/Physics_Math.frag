/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    *This file is included to other shader code using math & physics stuff

    *Include that file using : #pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Physics_Math.frag)

    And don't forget to disable the squiggles for the GLSL 
    (squiggles becomes annoying since the vscode compiler doesn't recognise including other shader files) 
*/


//Const values
const float PI = 3.14159265359;
const float EULER = 2.71828;

/**
 * Compute the GGX distribution function for a given normal vector, half vector, and roughness.
 * 
 * @param N The surface normal vector.
 * @param H The half vector.
 * @param roughness The roughness value.
 * @return The computed GGX distribution value.
 */
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

/**
 * Compute the Schlick-GGX geometry term for a given NdotV and roughness.
 * 
 * @param NdotV The dot product of the surface normal and the view vector.
 * @param roughness The roughness value.
 * @return The computed Schlick-GGX geometry term.
 */
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

/**
 * Compute the Smith geometry term for a given normal vector, view vector, light vector, and roughness.
 * 
 * @param N The surface normal vector.
 * @param V The view vector.
 * @param L The light vector.
 * @param roughness The roughness value.
 * @return The computed Smith geometry term.
 */
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

/**
 * Compute the Schlick approximation of the Fresnel term for a given cosine theta and F0.
 * 
 * @param cosTheta The cosine of the angle between the normal and the view vector.
 * @param F0 The F0 value.
 * @return The computed Schlick Fresnel term.
 */
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

/**
 * Compute the Schlick approximation of the Fresnel term with roughness for a given cosine theta, F0, and roughness.
 * 
 * @param cosTheta The cosine of the angle between the normal and the view vector.
 * @param F0 The F0 value.
 * @param roughness The roughness value.
 * @return The computed Schlick Fresnel term with roughness.
 */
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}