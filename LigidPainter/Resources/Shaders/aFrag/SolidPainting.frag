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


//Position of the camera
uniform vec3 viewPos;
uniform sampler2D txtr;

uniform int primitiveCount;
uniform int wireframeMode = 0;
uniform int meshSelectionEditing = 0;
uniform int hideUnselected = 0;
uniform int usingMeshSelection = 0;
uniform sampler2D selectedPrimitiveIDS;
uniform sampler2D meshMask;

//Fragment shader output
out vec4 fragColor;
void main() {
    

    gl_FragDepth = gl_FragCoord.z;

    float prim_txtr_res = int(ceil(sqrt(primitiveCount)));
    float prim_height = floor(float(gl_PrimitiveID) / prim_txtr_res);
    float prim = texelFetch(selectedPrimitiveIDS, ivec2(float(gl_PrimitiveID) - (prim_height * prim_txtr_res) , prim_height), 0).r;
    bool selectedPrim = prim > 0.9 && texture(meshMask, TexCoords).r > 0.5;
    bool hoveredPrim = prim > 0.3 && prim < 0.9;

    if(wireframeMode == 1){
        fragColor = vec4(0.18, 0.42, 0.64, 1.);
        return;
    }

    if(!selectedPrim && usingMeshSelection == 1 && meshSelectionEditing == 0 && hideUnselected == 1){
        fragColor.rgba = vec4(0.);
        gl_FragDepth = 1.;
        return;
    }

    Light light;
    Material material;

    vec3 color = texture2D(txtr, TexCoords).rgb;

    material.ambient = vec3(color);
    material.diffuse = vec3(color);
    material.specular = vec3( 0.5, 0.5, 0.5);
    material.shininess = 32.0;

    vec3 lightColor;
    lightColor.x = 0.6;
    lightColor.y = 0.6;
    lightColor.z = 0.6;

    light.position = vec3(-5.2, 0.0, 2.0);
    light.diffuse = lightColor * vec3(0.5); // decrease the influence
    light.ambient = light.diffuse * vec3(2.);
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

    if(hoveredPrim)
        fragColor.rgb /= 2.;
}