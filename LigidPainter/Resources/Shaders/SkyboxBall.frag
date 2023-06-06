#version 330 core

in vec3 Normal;

uniform samplerCube skybox;

out vec4 fragColor;

vec3 getPBR(){
    vec3 N = Normal;

    N = normalize(N * 2.0 - 1.0);  // this normal is in tangent space

    return texture(skybox, N).rgb ;
}

void main() {
    fragColor = vec4(getPBR(),1.);
}