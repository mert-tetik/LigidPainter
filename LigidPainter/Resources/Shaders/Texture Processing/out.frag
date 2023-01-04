#version 330 core

struct Material {
   sampler2D diffuse;
};
uniform Material material;

uniform sampler2D screenMaskTexture;
uniform sampler2D mirroredScreenMaskTexture;
uniform int useMirror;
uniform int isRenderScreenMaskMode;
uniform int verticalMirror;
uniform vec3 drawColor;
uniform sampler2D depthTexture; 
uniform sampler2D mirroredDepthTexture; 
uniform int renderDepth;
uniform int renderMaskBrush;
uniform sampler2D modifiedMaskTexture;
uniform int whiteRendering;
uniform sampler2D uvMask;
uniform int interpretWithUvMask;

uniform sampler2D paintedTxtrMask;


in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;

in vec4 projectedPos;
in vec4 mirroredProjectedPos;

out vec4 color;

float far = 10.0;
float near = 0.1;
float linearizeDepth(float depth){
   return (2.0 * near * far) / (far + near -(depth * 2.0 - 1.0) *(far-near));
}

float isPainted(vec3 uv, bool isMirrored) { //Use mirrored depth texture if isMirrored is true
   float drawZ;

   if(!isMirrored){
      drawZ = texture2D(depthTexture, uv.xy).b;
   }
   else{
      drawZ = texture2D(mirroredDepthTexture, uv.xy).b; 
   }

   float uvCoordVal;
   if(!isMirrored){
      uvCoordVal = texture2D(paintedTxtrMask,TexCoords).r;
   }
   else{
      uvCoordVal = texture2D(paintedTxtrMask,TexCoords).g;
   }

   // vec3 direction;

   // if(isMirrored){
   //    direction = mirroredViewPos - Pos;
   // }
   // else{
   //    direction = viewPos - Pos;
   // }

   // float dotProd = dot(normalize(direction),normalize(Normal));

   if(abs(drawZ - linearizeDepth(uv.z)/far) < 0.005){
      //True
      return 2.0;
   }
   else if(uvCoordVal > 0.0){
      return uvCoordVal;
   }
   else{
      //False
      return 3.0;
   }
}





vec3 getPaintedDiffuse(){
      //Painting
   vec3 screenPos = projectedPos.xyz / projectedPos.w / vec3(2.0, 2.0, 2.0) + 0.5 / vec3(1.0, 1.0, 1.0);
   vec3 mirroredScreenPos = mirroredProjectedPos.xyz / mirroredProjectedPos.w / vec3(2.0, 2.0, 2.0) + 0.5 / vec3(1.0, 1.0, 1.0);

   float intensity = 0.0;
   float mirroredIntensity = 0.0;

   if(isPainted(screenPos,false) <= 1.0){
      intensity = min(isPainted(screenPos,false),1.0);
   }
   else if(isPainted(screenPos,false) == 2.0) {
      intensity = texture2D(screenMaskTexture, screenPos.xy).r;
   }


   if(isPainted(mirroredScreenPos,true) <= 1.0){
      mirroredIntensity = min(isPainted(mirroredScreenPos,true),1.0);
   }
   else if(isPainted(mirroredScreenPos, true) == 2.0) {
      mirroredIntensity = texture2D((mirroredScreenMaskTexture), mirroredScreenPos.xy).r;
   }
   
    // ambient
   vec3 diffuseClr = vec3(texture(material.diffuse, TexCoords));
   vec3 diffuseDrawMix;
   
   diffuseDrawMix = mix(diffuseClr, drawColor, intensity);

   vec3 mirroredDiffuseDrawMix;

   if(useMirror == 1){
         mirroredDiffuseDrawMix = mix(diffuseDrawMix, drawColor, mirroredIntensity);
   }
   else{
         mirroredDiffuseDrawMix = diffuseDrawMix;
   }
   
   gl_FragDepth = 1.0 - (intensity/2.0 + 0.5);

   return mirroredDiffuseDrawMix;
}


void main() {
   gl_FragDepth = 0.5;

   vec3 screenPos = projectedPos.xyz / projectedPos.w / vec3(2.0, 2.0, 2.0) + 0.5 / vec3(1.0, 1.0, 1.0);


    if(isRenderScreenMaskMode == 0){
       if(renderMaskBrush == 1)
       {
          //Mask texture here
          color = texture(modifiedMaskTexture, TexCoords);
       }
       else
       {
            if(renderDepth == 0)
            {
               //Diffuse result here
               if(interpretWithUvMask == 0){
                  if(whiteRendering == 0){
                     vec3 diffuseDrawMix = getPaintedDiffuse();
                     color = vec4(diffuseDrawMix, 1);
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