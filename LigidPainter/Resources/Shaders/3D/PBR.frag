#version 330 core

struct Material { //Will be updated
   sampler2D diffuse;
   sampler2D specular; //Not used
   float shininess; //32.0f
};
uniform Material material;

in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;

in vec4 projectedPos;
in vec4 mirroredProjectedPos;

uniform sampler2D screenMaskTexture;
uniform sampler2D mirroredScreenMaskTexture;

uniform int useMirror;
uniform vec3 drawColor;

uniform float opacity;

uniform sampler2D depthTexture; //Screen rendered with depth color
uniform sampler2D mirroredDepthTexture; //Screen rendered with depth color

uniform vec3 viewPos;
uniform vec3 mirroredViewPos;


out vec4 color;

uniform samplerCube bluryskybox;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

float far = 10.0f;
float near = 0.1f;
float linearizeDepth(float depth){
   return (2.0 * near * far) / (far + near -(depth * 2.0 - 1.0) *(far-near));
}

bool isPainted(vec3 uv, bool isMirrored) { //Use mirrored depth texture if isMirrored is true
   float drawZ;
   if(!isMirrored){
      drawZ = texture2D(depthTexture, uv.xy).b;
   }
   else{
      drawZ = texture2D(mirroredDepthTexture, uv.xy).b; 
   }


   vec3 direction;
   if(isMirrored){
      direction = mirroredViewPos - Pos;
   }
   else{
      direction = viewPos - Pos;
   }
   float dotProd = dot(normalize(direction),normalize(Normal));

 
   if(true){
      return abs(drawZ - linearizeDepth(uv.z)/far) < 0.005;
   }
}


vec3 getPaintedDiffuse(){
      //Painting
   vec3 screenPos = projectedPos.xyz / projectedPos.w / vec3(2.0, 2.0, 2.0) + 0.5 / vec3(1.0, 1.0, 1.0);
   vec3 mirroredScreenPos = mirroredProjectedPos.xyz / mirroredProjectedPos.w / vec3(2.0, 2.0, 2.0) + 0.5 / vec3(1.0, 1.0, 1.0);

   float intensity = 0.0;
   float mirroredIntensity = 0.0;
   if(isPainted(screenPos,false)) 
   {
      intensity = texture2D(screenMaskTexture, screenPos.xy).r;
   }
   if(isPainted(mirroredScreenPos, true)) 
   {
      mirroredIntensity = texture2D((mirroredScreenMaskTexture), mirroredScreenPos.xy).r;
   }
   
    // ambient
   vec3 diffuseClr = vec3(texture(material.diffuse, TexCoords));
   vec3 diffuseDrawMix = mix(diffuseClr, drawColor, intensity);
   vec3 mirroredDiffuseDrawMix;
   if(useMirror == 1){
      mirroredDiffuseDrawMix = mix(diffuseDrawMix, drawColor, mirroredIntensity);
   }
   else{
      mirroredDiffuseDrawMix = diffuseDrawMix;
   }
   
   return mirroredDiffuseDrawMix;
}

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
vec3 getRealisticResult(vec3 paintedDiffuse){
   vec3 albedo = paintedDiffuse;
   float metallic = 0.0;
   float roughness = 1.0;
   float ao = 1;

   
   vec3 lightPos1 = vec3(-10.0,  10.0, 10.0);
   vec3 lightPos2 = vec3( 10.0,  10.0, 10.0);
   vec3 lightPos3 = vec3(-10.0, -10.0, 10.0);
   vec3 lightPos4 = vec3( 10.0, -10.0, 10.0);

   vec3 lightColorX = vec3(300.0f, 300.0f, 300.0f);


   vec3 N = Normal;
    vec3 V = normalize(viewPos - Pos);
    vec3 R = reflect(-V, N); 

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 4; ++i) 
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

        // calculate per-light radiance
        vec3 L = normalize(lightPosX - Pos);
        vec3 H = normalize(V + L);
        float distance = length(lightPosX - Pos);
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
    
    vec3 irradiance = texture(bluryskybox, N).rgb;
    vec3 diffuse      = irradiance * albedo;
    
    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F);

    vec3 ambient = (kD * diffuse + specular) * ao;

   vec3 result = ambient + Lo;

   result = result / (result + vec3(1.0));

   result = pow(result,vec3(1.0/2.2));

   return result;
}

void main() {
    vec3 paintedDiffuse = getPaintedDiffuse();
    vec3 result = getRealisticResult(paintedDiffuse);

    color = vec4(result,opacity);
}