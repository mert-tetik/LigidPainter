#version 330 core

uniform sampler2D screenMaskTexture;
uniform sampler2D depthTexture; 
uniform sampler2D mirroredDepthTexture; 
uniform int paintThrough; 


in vec4 projectedPos;

out vec4 color;

float far = 10.0;
float near = 0.1;
float linearizeDepth(float depth){
   return (2.0 * near * far) / (far + near -(depth * 2.0 - 1.0) *(far-near));
}

bool isPainted(vec3 uv, bool isMirrored) { //Use mirrored depth texture if isMirrored is true
   float drawZ;
      drawZ = texture(depthTexture, uv.xy).b;

   if(paintThrough == 1)
      return true;
 
   if(true){
      return abs(drawZ - linearizeDepth(uv.z)/far) < 0.0025;
   }
}

void main() {
    vec3 screenPos = projectedPos.xyz / projectedPos.w / vec3(2.0, 2.0, 2.0) + 0.5 / vec3(1.0, 1.0, 1.0);
    
    float intensity = 0.;
    if(isPainted(screenPos,false))
        intensity = texture(screenMaskTexture, screenPos.xy).r;
    
    color = vec4(vec3(intensity),1);
}