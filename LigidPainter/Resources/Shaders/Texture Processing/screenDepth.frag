#version 330 core

out vec4 color;

float far = 10.0;
float near = 0.1;
float linearizeDepth(float depth){
   return (2.0 * near * far) / (far + near -(depth * 2.0 - 1.0) *(far-near));
}

void main() {
    color = vec4(vec3(linearizeDepth(gl_FragCoord.z)/far), 1.0);
}