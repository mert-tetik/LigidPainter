#version 330 core

struct Material {
   sampler2D diffuse;
   sampler2D specular;
   float shininess;
};

uniform Material material;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
in vec3 Pos;

in vec4 projectedPos;
uniform sampler2D maskTexture;
uniform vec3 drawColor;

uniform int isRenderTextureMode;

in vec2 TextTexCoords;
uniform int isTextF;
uniform sampler2D text;
uniform vec3 textBg;
uniform sampler2D uiMaskTexture;
uniform int isUiTextureUsed;
uniform vec3 textColor;

uniform int is2D;
uniform vec3 lightPos;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform int isLightSource;
uniform int isAxisPointer;
uniform vec3 uiColor;
uniform float uiOpacity;

uniform int isColorBox;
uniform int isRect;
uniform vec3 boxColor = vec3(0.0,1.0,0.0);

out vec4 color;

bool isPainted() {
   vec3 viewDir = normalize(viewPos - Pos);
   vec3 nrml = (Pos + Normal);
   float dotprod = max(dot(viewDir, Normal), 0.0);

   return dotprod > 0;
}

void main() {
   vec3 screenPos = projectedPos.xyz / projectedPos.w / vec3(2, 2, 2) + 0.5 / vec3(1, 1, 1);
   float intensity = 0.0f;
   if(isPainted()) {
      intensity = texture2D(maskTexture, screenPos.xy).r;
   }
    //vec3 paintUv = 
    // ambient
   vec3 diffuseClr = vec3(texture(material.diffuse, TexCoords));
   vec3 diffuseDrawMix = mix(diffuseClr, drawColor, intensity);
   vec3 ambient = vec3(0.2, 0.2, 0.2) * diffuseDrawMix;

    // diffuse 

   vec3 norm = normalize(Normal);
   vec3 lightDir = normalize(lightPos - FragPos);
   float diff = max(dot(norm, lightDir), 0.0);
   vec3 diffuse = vec3(1.0f, 1.0f, 1.0f) * diff * diffuseDrawMix;

    // specular
   vec3 viewDir = normalize(viewPos - FragPos);
   vec3 reflectDir = reflect(-lightDir, norm);
   float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
   vec3 specular = lightColor * spec * vec3(texture(material.specular, TexCoords));

            vec3 interpretedColorWhite = ((vec3(1.0)-boxColor) * vec3(TexCoords.x)) + boxColor;
            vec3 interpretedColorBlack = vec3(TexCoords.y)*interpretedColorWhite;

   vec3 result = ambient + diffuse + specular;
   if(isRenderTextureMode == 0) {
      if(isColorBox == 0) {
         if(isTextF == 0) {
            if(is2D == 0) {
               if(isLightSource == 1) {
                  color = vec4(1.0, 1.0, 1.0, 1.0);
               } else if(isLightSource == 0) {
                  if(isAxisPointer == 0) {
                     color = vec4(result, 1);
                  } else {
                     color = vec4((abs(Pos.x) / 20), (abs(Pos.y) / 20), (abs(Pos.z) / 20), 1);
                  }
               }
            } else {
               if(isUiTextureUsed == 0) {
                  color = vec4(uiColor, uiOpacity);
               } else {
                  color = texture(uiMaskTexture, TexCoords);
               }
            }
         }
         else {
            vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TextTexCoords).r);
            color = vec4(textColor,1) * sampled;
         }
      } 
      else {
         if(isRect == 0){

            color = vec4(interpretedColorBlack,1);
         }
         else{
            color = vec4(norm,1);
         }
      }

   } else {
      if(isColorBox == 1 && isRect == 1){
         color = vec4(Normal,1);
      }
      else if(isColorBox == 1 && isRect == 0){
         color = vec4(interpretedColorBlack,1);

      }
      else{
         color = vec4(diffuseDrawMix, 1);
      }
   }

}
