#version 330 core

in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;
in vec3 Tangent;
in vec3 Bitangent;

in vec4 projectedPos;

uniform vec3 viewPos;

uniform samplerCube skybox;
uniform samplerCube prefilterMap;

uniform sampler2D albedoTxtr;
uniform sampler2D roughnessTxtr;
uniform sampler2D metallicTxtr;
uniform sampler2D normalMapTxtr;
uniform sampler2D heightMapTxtr;
uniform sampler2D ambientOcclusionTxtr;

uniform sampler2D paintingTexture;

in float renderTxtr;

out vec4 fragColor;

uniform float paintingOpacity;
uniform vec3 paintingColor;

const float PI = 3.14159265359;



float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}   

vec4 getTexture(sampler2D txtr){
    return texture(txtr,TexCoords);
}
float hash(float x)
{
    return fract(sin(x) * 43758.5453);
}
vec3 getPaintedTexture(sampler2D txtr){
    vec3 screenPos = 0.5 * (vec3(1,1,1) + projectedPos.xyz / projectedPos.w);
   
    float intensity = 0.0;
    
    intensity = texture(paintingTexture, screenPos.xy).a * paintingOpacity;

    return mix(texture(txtr,TexCoords).rgb,paintingColor,intensity);
}

vec3 getSmearedTexture(sampler2D txtr){
    vec3 screenPos = 0.5 * (vec3(1,1,1) + projectedPos.xyz / projectedPos.w);
   
    float intensity = 0.0;
    
    intensity = texture(paintingTexture, screenPos.xy).a * paintingOpacity;

    // Apply blurring
    const int Samples = 64; //multiple of 2
    float Intensity = 0.1;

    vec2 Direction = vec2(0.,-1.);

    vec4 blurredColor = vec4(0.0);  
    
    for (int i=1; i<=Samples/2; i++)
    {
        vec2 mUV = TexCoords - float(i) * (intensity/10.) / float(Samples/2) * Direction;
        vec2 pUV = TexCoords + float(i) * (intensity/10.) / float(Samples/2) * Direction;

        mUV.y += 0.05*intensity;
        pUV.y += 0.05*intensity;

        blurredColor += texture(txtr,pUV);
        blurredColor += texture(txtr,mUV);
    }
    blurredColor = blurredColor/float(Samples);    
    return blurredColor.rgb;

    //return texture(txtr,vec2(TexCoords.x,TexCoords.y + 0.1*intensity)).rgb;
}

vec3 getPBR(){
    vec3 albedo = getPaintedTexture(albedoTxtr).rgb;
    if(renderTxtr == 1)
        return albedo;
    //vec3 normal = getTexture(normalMapTxtr).rgb;
    vec3 normal = vec3(0.5,0.5,1);
    float roughness = getTexture(roughnessTxtr).r;
    float metallic = getTexture(metallicTxtr).r;


    vec3 T = normalize(vec3(vec4(Tangent, 0.0)));
    vec3 aN = normalize(vec3(vec4(Normal, 0.0)));
    T = normalize(T - dot(T, aN) * aN);
    vec3 B = cross(aN, T);
    
    mat3 TBN = transpose(mat3(T, B, aN));    
    
    vec3 tangentViewPos  = TBN * viewPos;
    vec3 tangentPosModel  = TBN * Pos;

    vec3 N = normal;

    N = normalize(N * 2.0 - 1.0);  // this normal is in tangent space

   float ao = 1;

   
   vec3 lightPos1 = vec3(-10.0,  10.0, 10.0);
   vec3 lightPos2 = vec3( 10.0,  10.0, 10.0);
   vec3 lightPos3 = vec3(-10.0, -10.0, 10.0);
   vec3 lightPos4 = vec3( 10.0, -10.0, 10.0);

   vec3 lightColorX = vec3(300.0f, 300.0f, 300.0f);


    vec3 V = normalize(tangentViewPos - tangentPosModel);
    vec3 R = reflect(-V, N); 

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 0; ++i) 
    {
      vec3 lightPosX;

      if(i == 0)
         lightPosX = lightPos1;
      if(i == 1)
         lightPosX = lightPos2;
      if(i == 2)
         lightPosX = lightPos3;
      if(i == 3)
         lightPosX = lightPos4;

         vec3 tangentLightPos = TBN * lightPosX;

        // calculate per-light radiance
        vec3 L = normalize(tangentLightPos - tangentPosModel);
        vec3 H = normalize(V + L);
        float distance = length(tangentLightPos - tangentPosModel);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColorX * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);    
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);        
        
        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        
         // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	                
            
        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL; // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   
    
    // ambient lighting (we now use IBL as the ambient term)
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  
    
    vec3 irradiance = texture(skybox, N).rgb ;
    vec3 diffuse      = albedo;
    
    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
    //vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F);

    vec3 ambient = (kD * diffuse + specular) * ao;

   vec3 result = ambient + Lo;

   result = result / (result + vec3(1.0));

   result = pow(result,vec3(1.0/2.2));
   
   return result;
}

void main() {
    fragColor = vec4(getPBR(),1.);
}