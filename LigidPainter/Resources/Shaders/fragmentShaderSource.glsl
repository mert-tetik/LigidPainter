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
uniform int renderMaskBrushBlury;
in vec2 blurTextureCoords[11];


uniform int isTextF;
uniform sampler2D text;
uniform sampler2D modifiedMaskTexture;
uniform int isUiTextureUsed;
uniform vec3 textColor;

uniform int is2D;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
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

uniform int isIcon;
uniform sampler2D icon;

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

   return abs(drawZ - linearizeDepth(uv.z)/far) < 0.005;
}

void main() {
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
   vec3 ambient = vec3(0.7, 0.7, 0.7) * mirroredDiffuseDrawMix;

    // diffuse 
   vec3 norm = normalize(Normal);
   vec3 lightDir = normalize(lightPos - FragPos);
   float diff = max(dot(norm, lightDir), 0.0);
   vec3 diffuse = vec3(1.0f, 1.0f, 1.0f) * diff * mirroredDiffuseDrawMix;

    // specular
   vec3 viewDir = normalize(viewPos - FragPos);
   vec3 reflectDir = reflect(-lightDir, norm);
   float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
   vec3 specular = lightColor * spec * vec3(texture(material.specular, TexCoords));

   //Color box
   vec3 interpretedColorWhite = ((vec3(1.0)-boxColor) * vec3(TexCoords.x)) + boxColor;
   vec3 interpretedColorBlack = vec3(TexCoords.y)*interpretedColorWhite;

   vec3 result = ambient + diffuse + specular;
   if(isIcon == 0){
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
                     //3D model here
                     color = vec4(result, 1);
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
            if(renderMaskBrushBlury == 1)
            {
               //Blury mask texture here
               color = vec4(0.0);
               color += texture(modifiedMaskTexture, blurTextureCoords[0]) * 0.0093;
               color += texture(modifiedMaskTexture, blurTextureCoords[1]) * 0.028002;
               color += texture(modifiedMaskTexture, blurTextureCoords[2]) * 0.065984;
               color += texture(modifiedMaskTexture, blurTextureCoords[3]) * 0.121703;
               color += texture(modifiedMaskTexture, blurTextureCoords[4]) * 0.175713;
               color += texture(modifiedMaskTexture, blurTextureCoords[5]) * 0.198596;
               color += texture(modifiedMaskTexture, blurTextureCoords[6]) * 0.175713;
               color += texture(modifiedMaskTexture, blurTextureCoords[7]) * 0.121703;
               color += texture(modifiedMaskTexture, blurTextureCoords[8]) * 0.065984;
               color += texture(modifiedMaskTexture, blurTextureCoords[9]) * 0.028002;
               color += texture(modifiedMaskTexture, blurTextureCoords[10]) * 0.0093;
            }
            else
            {
               //Mask texture here
               color = texture(modifiedMaskTexture, TexCoords);
            }
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
                     color = vec4(vec3(linearizeDepth(mirroredProjectedPos.z)/far), 1.0);
                  }
               }
               else
               {
                  //Diffuse result here
                  if(interpretWithUvMask == 0){
                     if(whiteRendering == 0){
                        color = vec4(mirroredDiffuseDrawMix, 1);
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
   else{
      //Icon here
      color = vec4(vec3(1), 1.0 - texture2D(icon,TexCoords).r);
   }
}