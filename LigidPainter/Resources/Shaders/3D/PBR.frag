#version 330 core

struct Material { //Will be updated
   sampler2D diffuse;
};
uniform Material material;

in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;
in vec3 Tangent;
in vec3 Bitangent;

in vec4 projectedPos;

uniform sampler2D screenMaskTexture;

uniform vec3 drawColor;

uniform int maskMode;

uniform float opacity;

uniform sampler2D depthTexture; //Screen rendered with depth color
uniform sampler2D mirroredDepthTexture; //Screen rendered with depth color

uniform vec3 viewPos;

out vec4 color;

uniform samplerCube bluryskybox;
uniform samplerCube prefilterMap;

uniform int paintThrough; 

uniform int useMirror;

uniform float skyboxExposure;

uniform sampler2D subSelectedTexture;
uniform float subSelectedImagePower;

uniform float mirrorOriginPosX;
uniform float mirrorOriginPosY;
uniform float mirrorOriginPosZ;

uniform sampler2D tdRenderedMaskTexture;
uniform sampler2D paintOverTxtr;

uniform int doPaintOver;
uniform vec2 paintOverScale;
uniform vec2 paintOverPos;

uniform float paintingOpacity;

uniform int channelState;

uniform int dynamicPainting;

uniform int getTheResultF;

float far = 10.0f;
float near = 0.1f;
float linearizeDepth(float depth){
   return (2.0 * near * far) / (far + near -(depth * 2.0 - 1.0) *(far-near));
}

bool isPainted(vec3 uv, bool isMirrored) { //Use mirrored depth texture if isMirrored is true
   float drawZ;
   if(!isMirrored){
      drawZ = texture(depthTexture, uv.xy).b;
   }
   else{
      drawZ = texture(mirroredDepthTexture, uv.xy).b; 
   }

   if(paintThrough == 1)
      return true;
 
   if(true){
      return abs(drawZ - linearizeDepth(uv.z)/far) < 0.005;
   }
}


vec3 getPaintedDiffuse(){
      //Painting
   vec3 diffuseClr = vec3(texture(material.diffuse, TexCoords));

   vec3 screenPos = projectedPos.xyz / projectedPos.w / vec3(2.0, 2.0, 2.0) + 0.5 / vec3(1.0, 1.0, 1.0);

   vec3 paintingColor;
   if(doPaintOver == 0)
      paintingColor = drawColor;
   else{
      vec4 src = texture(paintOverTxtr,screenPos.xy*paintOverScale + paintOverPos);
      paintingColor =  src.rgb;
      if(src.a < 0.05)
         return diffuseClr;
   }

   float intensity = 0.0;

   if(isPainted(screenPos,false)) {
      if(dynamicPainting == 0)
         intensity = texture(screenMaskTexture, screenPos.xy).a;
      if(dynamicPainting == 1)
         intensity = texture(screenMaskTexture, screenPos.xy).r;
   }

   intensity*=paintingOpacity;


    // ambient
   vec3 diffuseDrawMix;
   
   if(maskMode == 1)
      diffuseDrawMix = mix(diffuseClr, paintingColor, intensity);//TODO : Change

   else
      diffuseDrawMix = mix(diffuseClr, texture((screenMaskTexture), screenPos.xy).rgb*drawColor, intensity);

   if(useMirror == 1){
      if(maskMode == 1){
         if(dynamicPainting == 0)
            return mix(diffuseDrawMix, paintingColor, texture(tdRenderedMaskTexture,TexCoords).a*paintingOpacity);
         if(dynamicPainting == 1)
            return mix(diffuseDrawMix, paintingColor, texture(tdRenderedMaskTexture,TexCoords).r*paintingOpacity);
      }
      else
         return mix(diffuseDrawMix, texture(tdRenderedMaskTexture,TexCoords).rgb*drawColor, texture(tdRenderedMaskTexture,TexCoords).a*paintingOpacity);
   }
   
   return diffuseDrawMix;
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
   vec3 albedo; 
   if(channelState == 0)
      albedo = mix(paintedDiffuse,texture(subSelectedTexture,TexCoords).rgb,subSelectedImagePower);
   else
      albedo = vec3(0.0);

   float metallic;
   if(channelState == 1)
      metallic = mix(paintedDiffuse,texture(subSelectedTexture,TexCoords).rgb,subSelectedImagePower).r;
   else
      metallic = (0.0);
   
   float roughness;
   if(channelState == 2)
      roughness = mix(paintedDiffuse,texture(subSelectedTexture,TexCoords).rgb,subSelectedImagePower).r;
   else
      roughness = (0.3);
   
   vec3 normal;
   if(channelState == 3)
      normal = mix(paintedDiffuse,texture(subSelectedTexture,TexCoords).rgb,subSelectedImagePower).rgb;
   else
      normal = vec3(128./255.,127./255.,255./255.);


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
    
    vec3 irradiance = texture(bluryskybox, N).rgb * skyboxExposure;
    vec3 diffuse      = skyboxExposure * albedo;
    
    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb * skyboxExposure;    
    //vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F);

    vec3 ambient = (kD * diffuse + specular) * ao;

   vec3 result = ambient + Lo;

   result = result / (result + vec3(1.0));

   result = pow(result,vec3(1.0/2.2));
   
   const float mirrorLineWidth = 0.003;

   if(Pos.x > mirrorOriginPosX-mirrorLineWidth && Pos.x < mirrorOriginPosX+mirrorLineWidth)
      return vec3(0.043,0.635,0.823);
   if(Pos.y > mirrorOriginPosY-mirrorLineWidth && Pos.y < mirrorOriginPosY+mirrorLineWidth)
      return vec3(0.043,0.635,0.823);
   if(Pos.z > mirrorOriginPosZ-mirrorLineWidth && Pos.z < mirrorOriginPosZ+mirrorLineWidth)
      return vec3(0.043,0.635,0.823);
   return result;
}

void main() {
   vec3 paintedDiffuse = getPaintedDiffuse();
   vec3 result;
   if(getTheResultF == 0)
      result = getRealisticResult(paintedDiffuse);
   else
      result = paintedDiffuse;
   color = vec4(result,opacity);
}