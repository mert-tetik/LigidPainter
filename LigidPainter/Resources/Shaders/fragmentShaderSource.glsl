#version 330 core







struct Material {
   sampler2D diffuse;
   sampler2D specular; //Not used
   float shininess; //32.0f
};
uniform Material material;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
in vec3 Pos;

in vec4 projectedPos;
in vec4 mirroredProjectedPos;

uniform sampler2D screenMaskTexture;
uniform sampler2D mirroredScreenMaskTexture;
uniform int useMirror;
uniform int isRenderScreenMaskMode;
uniform int verticalMirror;
uniform vec3 drawColor;


uniform sampler2D depthTexture; //Screen rendered with depth color
uniform sampler2D mirroredDepthTexture; //Screen rendered with depth color

uniform int isRenderTextureMode;
uniform int renderDepth;
uniform int renderMirroredDepth;
uniform int renderMaskBrush;


uniform int isTextF;
uniform sampler2D text;
uniform sampler2D modifiedMaskTexture;
uniform int isUiTextureUsed;
uniform vec3 textColor;

uniform int is2D;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform vec3 mirroredViewPos;
uniform int isLightSource;
uniform int isAxisPointer;

uniform vec3 uiColor;
uniform vec3 uiTransitionColor;
uniform float uiTransitionMixVal;

uniform float uiOpacity;

uniform int isColorBox;
uniform int isRect;
uniform vec3 boxColor = vec3(0.0,1.0,0.0);

uniform int drawBrushIndicator;

out vec4 color;

uniform int whiteRendering;

uniform sampler2D uvMask;
uniform int interpretWithUvMask;


uniform samplerCube skybox;

float far = 10.0f;
float near = 0.1f;
float linearizeDepth(float depth){
   return (2.0 * near * far) / (far + near -(depth * 2.0 - 1.0) *(far-near));
}

float rand(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
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

   if(dotProd < 0.2){
      return false;
   }  
   else{
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
   vec3 diffuse = texture(skybox,N).rgb * albedo * KD;

   vec3 ambient = diffuse * ao;

   vec3 result = ambient + Lo;

   result = result / (result + vec3(1.0));

   result = pow(result,vec3(1.0/2.2));

   return result;
}

void main() {
   //Color box

      if(isRenderTextureMode == 0) {
      if(isColorBox == 0) {
         if(isTextF == 0) {
            if(is2D == 0) {
               if(isLightSource == 1) {
                  //Light source here
                  color = vec4(1.0, 1.0, 1.0, 1.0);
               } 
               else if(isLightSource == 0) {
                  if(isAxisPointer == 0) 
                  {
                     float roughness = 0.7;
                     //3D model here
                     vec3 paintedDiffuse = getPaintedDiffuse();
                     vec3 result = getRealisticResult(paintedDiffuse);

                     color = vec4(result,1);
                  } 
                  else 
                  {
                     //Axis pointer here
                     color = vec4((abs(Pos.x) / 20), (abs(Pos.y) / 20), (abs(Pos.z) / 20), 1);
                  }
               }
            } 
            else 
            { //is2D == true
               if(isUiTextureUsed == 0) 
               {
                  if(drawBrushIndicator == 1)
                  {
                     //Brush indicator here
                     color = vec4(uiColor, max(texture(modifiedMaskTexture, TexCoords)-0.5,0.0)); 
                  }
                  else
                  {
                     //Ui here
                     color = vec4(mix(uiColor,uiTransitionColor,uiTransitionMixVal), uiOpacity);
                  }
               } 
               else 
               {
                  //Mask texture box here
                  color = texture(modifiedMaskTexture, TexCoords);
               }
            }
         }
         else 
         {
            //Text here
            vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
            color = vec4(textColor,1) * sampled;
         }
      } 
      else {
         if(isRect == 0)
         {
            //Color box here
            vec3 interpretedColorWhite = ((vec3(1.0)-boxColor) * vec3(TexCoords.x)) + boxColor;
            vec3 interpretedColorBlack = vec3(TexCoords.y)*interpretedColorWhite;
            color = vec4(interpretedColorBlack,1);
         }
         else
         {
            //Color rectangle slide bar here
            color = vec4(Normal,1);
         }
      }
   } 
   else { //Render texture
      if(isRenderScreenMaskMode == 0){
         if(renderMaskBrush == 1)
         {
            //Mask texture here
            color = texture(modifiedMaskTexture, TexCoords);
         }
         else
         {
            if(isColorBox == 1 && isRect == 1)
            {
               //Color rect slide bar texture rendering here
               color = vec4(Normal,1);
            }
            else if(isColorBox == 1 && isRect == 0)
            {
               //Color box texture rendering here
               vec3 interpretedColorWhite = ((vec3(1.0)-boxColor) * vec3(TexCoords.x)) + boxColor;
               vec3 interpretedColorBlack = vec3(TexCoords.y)*interpretedColorWhite;
               color = vec4(interpretedColorBlack,1);
            }
            else
            {
               if(renderDepth == 1)
               {
                  //Depth here
                  if(renderMirroredDepth == 0){
                     color = vec4(vec3(linearizeDepth(gl_FragCoord.z)/far), 1.0);
                  }
                  else{
                     color = vec4(vec3(linearizeDepth(gl_FragCoord.z)/far), 1.0);
                  }
               }
               else
               {
                  //Diffuse result here
                  if(interpretWithUvMask == 0){
                     if(whiteRendering == 0){
                        vec3 paintedDiffuse = getPaintedDiffuse();
                        color = vec4(paintedDiffuse, 1);
                     }
                     else{
                        color = vec4(1);
                     }
                  }
                  else{
                     //Paint around
                     if(texture2D(uvMask,vec2(TexCoords.x - 0.004,TexCoords.y)).r > 0.95 && texture2D(uvMask,TexCoords).r < 0.95){
                        color = vec4(texture2D(material.diffuse,vec2(TexCoords.x - 0.004,TexCoords.y)));
                     }
                     else if(texture2D(uvMask,vec2(TexCoords.x + 0.004,TexCoords.y)).r > 0.95 && texture2D(uvMask,TexCoords).r < 0.95){
                        color = vec4(texture2D(material.diffuse,vec2(TexCoords.x + 0.004,TexCoords.y)));
                     }
                     else if(texture2D(uvMask,vec2(TexCoords.x,TexCoords.y - 0.004)).r > 0.95 && texture2D(uvMask,TexCoords).r < 0.95){
                        color = vec4(texture2D(material.diffuse,vec2(TexCoords.x ,TexCoords.y - 0.004)));
                     }
                     else if(texture2D(uvMask,vec2(TexCoords.x,TexCoords.y + 0.004)).r > 0.95 && texture2D(uvMask,TexCoords).r < 0.95){
                        color = vec4(texture2D(material.diffuse,vec2(TexCoords.x ,TexCoords.y + 0.004)));
                     }
                     else{
                        color = vec4(texture2D(material.diffuse,TexCoords));
                     }
                  }
                  
               }
            }
         }
      }
      else{
         //Mirrored mask texture here
         if(verticalMirror == 0){
            color = texture(screenMaskTexture, vec2(1.0 - TexCoords.x,TexCoords.y));
         }
         else{
            color = texture(screenMaskTexture, vec2(TexCoords.x , 1.0 - TexCoords.y));
         }
      }
   }
}