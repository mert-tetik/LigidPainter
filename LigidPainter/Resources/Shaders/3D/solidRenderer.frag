#version 330 core

out vec4 fragColor;

in vec3 Pos;
in vec3 Normal;

void main(){


    vec3 objectColor = vec3(0.5,0.5,0.5);
    vec3 lightColor = vec3(1);
    vec3 lightPos = vec3(2);

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - Pos);  

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * objectColor;
    fragColor = vec4(result, 1.0);
}