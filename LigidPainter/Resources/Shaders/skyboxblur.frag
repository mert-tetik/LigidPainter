#version 330 core 

out vec4 color;

in vec3 Pos;

uniform samplerCube skybox;



const float PI = 3.14159265359;

void main(){
    vec3 N = normalize(Pos);

    vec3 irradiance = vec3(0.0);

    vec3 right = cross(vec3(0.0,1.0,0.0),N);
    vec3 up = cross(N,right);

    float sampleDelta = 0.25;
    float nrSamples = 0.0;

    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta){
        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta){
             vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));

             vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;

             irradiance += texture(skybox,sampleVec).rgb * cos(theta) * sin(theta);
             nrSamples++;
        }
    }
    irradiance *= PI / nrSamples;

    color = vec4(irradiance,1.0);
}