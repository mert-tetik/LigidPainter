#version 400 core

in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;
in vec3 Tangent;
in vec3 Bitangent;
in vec4 ProjectedPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform sampler2D txtr;
uniform int use_txtr;

//Position of the camera
uniform vec3 viewPos;

//Fragment shader output
out vec4 fragColor;
void main() {
    
    Light light;
    Material material;

    vec3 txtr_clr = texture(txtr, TexCoords).rgb;

    material.ambient = vec3(0.31, 0.5, 1.0);
    material.diffuse = vec3(0.31, 0.5, 1.0);
    material.specular = vec3( 0.5, 0.5, 0.5);

    if(use_txtr == 1){
        material.ambient = txtr_clr;
        material.diffuse = txtr_clr;

        fragColor = vec4(txtr_clr, 1.);
        return;
    }

    material.shininess = 32.0;

    vec3 lightColor;
    lightColor.x = 0.6;
    lightColor.y = 0.6;
    lightColor.z = 0.6;

    light.position = vec3(-5.2, 0.0, 2.0);
    light.diffuse = lightColor * vec3(0.5); // decrease the influence
    light.ambient = light.diffuse * vec3(0.6);
    light.specular = vec3(0.8);

    // ambient
    vec3 ambient = light.ambient * material.ambient;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - Pos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    
    // specular
    vec3 viewDir = normalize(viewPos - Pos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);  
        
    vec3 result = ambient + diffuse + specular;
    fragColor = vec4(result, 1.0);
}