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

/*
* Function to compute the radical inverse of a given 32-bit unsigned integer using the Van der Corput sequence.
* The function takes a 32-bit unsigned integer 'bits' as input.
* It performs a series of bitwise operations to shuffle the bits and calculate the radical inverse.
* The result is a floating-point value between 0 and 1.
*/
float RadicalInverse_VdC(uint bits) 
{
    // Shuffle the bits by swapping the 16 high bits with the 16 low bits
    bits = (bits << 16u) | (bits >> 16u);

    // Swap adjacent pairs of bits using bitwise AND and OR operations
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);

    // Swap groups of 4 bits using bitwise AND and OR operations
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);

    // Swap groups of 8 bits using bitwise AND and OR operations
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);

    // Swap groups of 16 bits using bitwise AND and OR operations
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);

    // Convert the shuffled bits back to a floating-point value between 0 and 1
    return float(bits) * 2.3283064365386963e-10; // Divide by 0x100000000 (2^32) to normalize the value
}

/*
* Function to generate a 2D Hammersley point based on the given index and total number of points.
* The function takes two unsigned integers 'i' and 'N' as input.
* 'i' represents the index of the point.
* 'N' represents the total number of points.
* The function returns a 2D vector (vec2) representing the Hammersley point.
*/
vec2 Hammersley(uint i, uint N)
{
    // Calculate the first component of the Hammersley point by dividing the index by the total number of points
    float firstComponent = float(i) / float(N);

    // Calculate the second component of the Hammersley point using the RadicalInverse_VdC function
    float secondComponent = RadicalInverse_VdC(i);

    // Return the Hammersley point as a vec2
    return vec2(firstComponent, secondComponent);
}

/*
* Function to perform importance sampling for GGX (Trowbridge-Reitz) distribution.
* The function takes three inputs: 
* - Xi: A 2D vector representing a random sample point in [0,1] range.
* - N: A 3D vector representing the surface normal.
* - roughness: A float representing the roughness parameter.
* The function returns a 3D vector representing the sampled direction.
*/
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    // Calculate squared roughness parameter
    float a = roughness * roughness;

    // Calculate phi angle based on the first component of the random sample point
    float phi = 2.0 * PI * Xi.x;

    // Calculate the cosine and sine of the theta angle based on the second component of the random sample point
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    // Convert spherical coordinates (phi, theta) to cartesian coordinates (H)
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    // Convert tangent-space H vector to world-space sample vector
    vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    // Calculate the final sample vector by combining the tangent, bitangent, and normal vectors
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;

    // Normalize and return the sample vector
    return normalize(sampleVec);
}