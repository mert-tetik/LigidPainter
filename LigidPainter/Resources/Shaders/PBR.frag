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


uniform sampler2D depthTexture; //Screen rendered with depth color
uniform sampler2D mirroredDepthTexture; //Screen rendered with depth color

uniform vec3 viewPos;
uniform vec3 mirroredViewPos;


out vec4 color;

uniform samplerCube bluryskybox;

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

   float intensity = 0.0f;
   float mirroredIntensity = 0.0f;
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


float distributionGGX(float NdotH, float roughness){
   float a     = roughness * roughness;
   float a2    = a * a;
   float denom = NdotH * NdotH * (a2 - 1.0) + 1.0;
   denom = PI * denom * denom;
   return a2 / max(denom,0.0000001);
}
float geometrySmith(float NdotV, float NdotL , float roughness){
   float r = roughness + 1.0;
   float k = (r * r) / 8.0;
   float ggx1 = NdotV / (NdotV * (1.0 - k) + k);
   float ggx2 = NdotL / (NdotL * (1.0 - k) + k);
   return ggx1 * ggx2;
}
vec3 fresnelSchlick(float HdotV, vec3 baseReflectivity){
   return baseReflectivity + (1.0 - baseReflectivity) * pow(1.0 - HdotV,5.0);
}
vec3 getRealisticResult(vec3 paintedDiffuse){
   vec3 albedo = paintedDiffuse;
   float metallic = 0.0;
   float roughness = 1.0;
   float ao = 1;

   vec3 lightPosX = vec3(10);
   vec3 lightColorX = vec3(300);


   vec3 N = normalize(Normal);
   vec3 V = normalize(viewPos - Pos);

   vec3 baseReflectivity = mix(vec3(0.04), albedo, metallic);

   vec3 Lo = vec3(0.0);

   //for

   vec3 L = normalize(lightPosX - Pos);
   vec3 H = normalize(V + L);
   float distance = length(lightPosX - Pos);
   float attenuation = 1.0 / (distance*distance);
   vec3 radiance = lightColorX * attenuation;

   float NdotV = max(dot(N,V),0.0000001);
   float NdotL = max(dot(N,L),0.0000001);
   float HdotV = max(dot(H,V),0.0);
   float NdotH = max(dot(N,H),0.0);
   
   float D = distributionGGX(NdotH,roughness);
   float G = geometrySmith(NdotV,NdotL,roughness);
   vec3 F = fresnelSchlick(HdotV, baseReflectivity);

   vec3 specular = D * G * F;
   specular /= 4.0 * NdotV * NdotL;

   vec3 KD = vec3(1.0) - F;

   KD *=  1.0 - metallic;

   Lo += (KD * albedo / PI + specular) * radiance * NdotL;

   //For

    F = fresnelSchlick(NdotV, baseReflectivity);
    KD = (1.0 - F) * (1.0 - metallic);
   vec3 diffuse = texture(bluryskybox,N).rgb * albedo * KD;

   vec3 ambient = diffuse * ao;

   vec3 result = ambient + Lo;

   result = result / (result + vec3(1.0));

   result = pow(result,vec3(1.0/2.2));

   return result;
}

void main() {
    vec3 paintedDiffuse = getPaintedDiffuse();
    vec3 result = getRealisticResult(paintedDiffuse);

    color = vec4(result,1);
}