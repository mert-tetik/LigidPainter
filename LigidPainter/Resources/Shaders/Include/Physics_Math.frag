/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

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





/*
*   Function to perform Physically Based Rendering (PBR) calculations.
*   
*   The function takes several inputs:
*   
*   albedo: A 3D vector representing the surface albedo color.
*   roughness: A float representing the roughness of the material.
*   metallic: A float representing the metallic property of the material.
*   normal: A 3D vector representing the surface normal.
*   ao: A float representing the ambient occlusion factor.
*   The function returns a 3D vector representing the final rendered color.
*   
*    Calculate tangent, binormal, and normal vectors in tangent space
*    to transform view and model positions and convert the normal to tangent space.
*    ...
*   
*    Define light positions and colors.
*    ...
*   
*    Calculate view vector and reflected vector.
*    ...
*   
*    Calculate reflectance at normal incidence (F0) based on albedo and metallic values.
*    ...
*   
*    Calculate outgoing radiance (Lo) by iterating over light sources.
*    ...
*   
*    Calculate ambient lighting using image-based lighting (IBL).
*    ...
*   
*    Sample pre-filter map texture and compute specular contribution.
*    ...
*   
*    Combine ambient and specular components, multiply by ambient occlusion factor.
*    ...
*   
*    Normalize and gamma-correct the result.
*    ...
*   
*    Return the final rendered color.
*    ...
*/
vec3 getPBR
            (
                /* Material Channels */     vec3 albedo, float roughness, float metallic, vec3 normal, float ao, 
                /* Vertex Data */           vec3 Pos, vec3 Normal, vec3 Tangent, vec3 Bitangent, 
                /* Other Stuff */           samplerCube skybox, samplerCube prefilterMap, vec3 viewPos, int useLights 
            ) 
{
    
    // Tangent, Binormal, Normal (TBN) basis calculation
    vec3 T = normalize(vec3(vec4(Tangent, 0.0)));
    vec3 aN = normalize(vec3(vec4(Normal, 0.0)));
    T = normalize(T - dot(T, aN) * aN);
    vec3 B = cross(aN, T);
    mat3 TBN = transpose(mat3(T, B, aN));    
    
    // Transform view and model positions to tangent space
    vec3 tangentViewPos  = TBN * viewPos;
    vec3 tangentPosModel  = TBN * Pos;

    // Transform the normal to tangent space
    vec3 N = normal;
    N = normalize(N * 2.0 - 1.0);  // this normal is in tangent space

    // Light positions and color
    vec3 lightPos1 = vec3(-10.0,  10.0, 10.0);
    vec3 lightPos2 = vec3( 10.0,  10.0, 10.0);
    vec3 lightPos3 = vec3(-10.0, -10.0, 10.0);
    vec3 lightPos4 = vec3( 10.0, -10.0, 10.0);
    vec3 lightColorX = vec3(300.0f, 300.0f, 300.0f);

    // Calculate the view direction and reflection vector
    vec3 V = normalize(tangentViewPos - tangentPosModel);
    vec3 R = reflect(-normalize(viewPos - Pos), Normal); 

    // Calculate the F0 (Fresnel at normal incidence) based on whether the material is a dielectric or a metal
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // Initialize the outgoing radiance (Lo)
    vec3 Lo = vec3(0.0);

    int lightSourceCount = 2;
    if(useLights  == 0)
        lightSourceCount = 0;

    // Calculate lighting for each light source
    for (int i = 0; i < lightSourceCount; ++i) {
        vec3 lightPosX;

        if (i == 0)
            lightPosX = lightPos1;
        if (i == 1)
            lightPosX = lightPos2;
        if (i == 2)
            lightPosX = lightPos3;
        if (i == 3)
            lightPosX = lightPos4;

        vec3 tangentLightPos = TBN * lightPosX;

        // Calculate per-light radiance
        vec3 L = normalize(tangentLightPos - tangentPosModel);
        vec3 H = normalize(V + L);
        float distance = length(tangentLightPos - tangentPosModel);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColorX * attenuation;

        // Cook-Torrance BRDF components
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);    
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);        
        
        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        
        // For energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship, the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        
        // Multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	                
            
        // Scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // Add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
        // Note that we already multiplied the BRDF by the Fresnel (kS), so we won't multiply by kS again
    }   
    
    // Ambient lighting (using IBL as the ambient term)
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  
    
    vec3 irradiance = texture(skybox, N).rgb ;
    vec3 diffuse      = albedo;
    
    // Sample both the pre-filter map and the BRDF LUT and combine them together
    // as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;    
    //vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F);

    vec3 ambient = (kD * diffuse + specular) * ao;

    vec3 result = ambient + Lo;

    result = result / (result + vec3(1.0));

    result = pow(result, vec3(1.0/2.2));
   
    return result;
}