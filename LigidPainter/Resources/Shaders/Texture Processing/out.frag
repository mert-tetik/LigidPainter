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

uniform int usePrevious3DMaskTxtrs;
uniform sampler2D previous3DMaskTxtrs;
uniform sampler2D tdRenderedMaskTexture;
uniform sampler2D paintOverTxtr;
uniform int doPaintOver;
uniform vec2 paintOverScale;
uniform vec2 paintOverPos;

uniform float paintingOpacity;

uniform vec3 mirroredScreenMaskCamPos;

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
         intensity = texture(screenMaskTexture, screenPos.xy).a;
   }

   intensity*=paintingOpacity;

    // ambient
   vec3 diffuseDrawMix;
   
   if(maskMode == 1)
      diffuseDrawMix = mix(diffuseClr, paintingColor, intensity);//TODO : Change

   else
      diffuseDrawMix = mix(diffuseClr, texture((screenMaskTexture), screenPos.xy).rgb, intensity);

   if(maskMode == 1)
      return mix(diffuseDrawMix, paintingColor, texture(tdRenderedMaskTexture,TexCoords).a);
   else
      return mix(diffuseDrawMix, texture(tdRenderedMaskTexture,TexCoords).rgb, texture(tdRenderedMaskTexture,TexCoords).a);

   
   return diffuseDrawMix;
}


void main() {
   vec3 screenPos = projectedPos.xyz / projectedPos.w / vec3(2.0, 2.0, 2.0) + 0.5 / vec3(1.0, 1.0, 1.0);

    if(isRenderScreenMaskMode == 0){
       if(renderMaskBrush == 1)
       {
          //Mask texture here
          color = texture(modifiedMaskTexture, TexCoords);
          if(maskMode == 1)
            color.a = color.r;
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
            float uvx = TexCoords.x;
            float uvy = TexCoords.y;
            
            if(abs(mirroredScreenMaskCamPos.x) < abs(mirroredScreenMaskCamPos.z)){
               if(mirrormaskrenderingX == 1)
                  uvx = 1.-uvx;
               if(mirrormaskrenderingY == 1)
                  uvy = 1.-uvy;
            }
            else {
               if(mirrormaskrenderingZ == 1)
                  uvx = 1.-uvx;
               if(mirrormaskrenderingY == 1)
                  uvy = 1.-uvy;
            }


            color = vec4(texture(screenMaskTexture, vec2(uvx , uvy)).rgba);
      }
      else{
            vec3 screenPos = projectedPos.xyz / projectedPos.w / vec3(2.0, 2.0, 2.0) + 0.5 / vec3(1.0, 1.0, 1.0);

            float intensity = 0.0;
            
            if(isPainted(screenPos,false)){
                  intensity = texture(screenMaskTexture, screenPos.xy).a;
            }
            if(usePrevious3DMaskTxtrs == 1)
               color = vec4(mix(texture(previous3DMaskTxtrs,TexCoords),texture(screenMaskTexture, screenPos.xy),intensity));
            else
               color = vec4(mix(vec4(0),texture(screenMaskTexture, screenPos.xy),intensity));
      }
   }
}