#version 330 core

struct Material {
   sampler2D diffuse;
};
uniform Material material;

uniform sampler2D screenMaskTexture;
uniform sampler2D mirroredScreenMaskTexture;
uniform int isRenderScreenMaskMode;
uniform int tdMaskRendering;
uniform vec3 drawColor;
uniform sampler2D depthTexture; 
uniform sampler2D mirroredDepthTexture; 
uniform int renderDepth;
uniform int renderMaskBrush;
uniform sampler2D modifiedMaskTexture;
uniform int whiteRendering;
uniform sampler2D uvMask;
uniform int interpretWithUvMask;

uniform int maskMode;
uniform int normalPainting;

uniform int mirrormaskrenderingX;
uniform int mirrormaskrenderingY;
uniform int mirrormaskrenderingZ;

in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;

uniform int paintThrough; 

in vec4 projectedPos;

out vec4 color;

uniform sampler2D previous3DMaskTxtrs;

float far = 10.0;
float near = 0.1;
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


   // vec3 direction;
   // if(isMirrored){
   //    direction = mirroredViewPos - Pos;
   // }
   // else{
   //    direction = viewPos - Pos;
   // }
   // float dotProd = dot(normalize(direction),normalize(Normal));
   if(paintThrough == 1)
      return true;
 
   if(true){
      return abs(drawZ - linearizeDepth(uv.z)/far) < 0.0025;
   }
}





vec3 getPaintedDiffuse(){
      //Painting
   vec3 screenPos = projectedPos.xyz / projectedPos.w / vec3(2.0, 2.0, 2.0) + 0.5 / vec3(1.0, 1.0, 1.0);

   float intensity = 0.0;
   float mirroredIntensity = 0.0;

   //if(texture(previous3DMaskTxtrs,TexCoords).r > 0.05)
      //return vec3(texture(previous3DMaskTxtrs,TexCoords).r);

   if(isPainted(screenPos,false)) {
      if(maskMode == 1)
         intensity = texture(screenMaskTexture, screenPos.xy).r;
      else{
         if(normalPainting == 1){
            if(texture(screenMaskTexture, screenPos.xy).r > 0.7f || texture(screenMaskTexture, screenPos.xy).g > 0.7f || texture(screenMaskTexture, screenPos.xy).b > 0.7f)
               intensity = 1.0;
         }
         else{
            if(texture(screenMaskTexture, screenPos.xy).r > 0.02f || texture(screenMaskTexture, screenPos.xy).g > 0.02f || texture(screenMaskTexture, screenPos.xy).b > 0.02f)
               intensity = 1.0;
         }
      }
   }

    // ambient
   vec3 diffuseClr = vec3(texture(material.diffuse, TexCoords));
   vec3 diffuseDrawMix;
   
   if(maskMode == 1)
      diffuseDrawMix = mix(diffuseClr, drawColor, intensity);
   else
      diffuseDrawMix = mix(diffuseClr, texture((screenMaskTexture), screenPos.xy).rgb, intensity);

   return diffuseDrawMix;
}


void main() {
   
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
                  if(texture(uvMask,vec2(TexCoords.x - 0.004,TexCoords.y)).r > 0.95 && texture(uvMask,TexCoords).r < 0.95){
                     color = vec4(texture(material.diffuse,vec2(TexCoords.x - 0.004,TexCoords.y)));
                  }
                  else if(texture(uvMask,vec2(TexCoords.x + 0.004,TexCoords.y)).r > 0.95 && texture(uvMask,TexCoords).r < 0.95){
                     color = vec4(texture(material.diffuse,vec2(TexCoords.x + 0.004,TexCoords.y)));
                  }
                  else if(texture(uvMask,vec2(TexCoords.x,TexCoords.y - 0.004)).r > 0.95 && texture(uvMask,TexCoords).r < 0.95){
                     color = vec4(texture(material.diffuse,vec2(TexCoords.x ,TexCoords.y - 0.004)));
                  }
                  else if(texture(uvMask,vec2(TexCoords.x,TexCoords.y + 0.004)).r > 0.95 && texture(uvMask,TexCoords).r < 0.95){
                     color = vec4(texture(material.diffuse,vec2(TexCoords.x ,TexCoords.y + 0.004)));
                  }
                  else{
                     color = vec4(texture(material.diffuse,TexCoords));
                  }
               }
               
            }
       }
    }
    else{
      if(tdMaskRendering == 0){
         //Mirrored mask texture here
         if((mirrormaskrenderingX == 1 && mirrormaskrenderingY == 1) || (mirrormaskrenderingX== 1 && mirrormaskrenderingZ== 1) || (mirrormaskrenderingY== 1 && mirrormaskrenderingZ== 1))
            color = vec4(texture(screenMaskTexture, vec2(1.0 - TexCoords.x,1. - TexCoords.y)).rgb,1);
         else if(mirrormaskrenderingX== 1 || mirrormaskrenderingZ== 1)
            color = vec4(texture(screenMaskTexture, vec2(1.0 - TexCoords.x,TexCoords.y)).rgb,1);
         else if(mirrormaskrenderingY== 1)
            color = vec4(texture(screenMaskTexture, vec2(TexCoords.x , 1.0 - TexCoords.y)).rgb,1);
      }
      else{
            vec3 screenPos = projectedPos.xyz / projectedPos.w / vec3(2.0, 2.0, 2.0) + 0.5 / vec3(1.0, 1.0, 1.0);

            float intensity = 0.0;

            if(isPainted(screenPos,false)){
               intensity = texture(screenMaskTexture, screenPos.xy).r;
            }
            vec3 res = mix(texture(previous3DMaskTxtrs,TexCoords).rgb,vec3(intensity),intensity);

            color = vec4(res,1);
      }
   }
}