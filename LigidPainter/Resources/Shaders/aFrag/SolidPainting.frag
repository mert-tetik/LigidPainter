#version 400 core

#pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Painting.frag)

/*
struct FaceSelectionData{
    int meshSelectionEditing;
    int hideUnselected;
    int usingMeshSelection;
    sampler2D selectedPrimitiveIDS;
    sampler2D meshMask;
    int primitiveCount;
};
face_selection_is_current_prim_selected or face_selection_is_current_prim_hovered
*/
#pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Face_Selection.frag)
uniform FaceSelectionData face_selection_data;

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

uniform int paintingMode;

uniform PaintingData painting_data;

uniform vec3 mirrorState = vec3(0.);
uniform vec3 mirrorOffsets = vec3(0.);

//Fragment shader output
out vec4 fragColor;
void main() {
    

    gl_FragDepth = gl_FragCoord.z;

    bool selectedPrim = face_selection_is_current_prim_selected(face_selection_data, TexCoords);
    bool hoveredPrim = face_selection_is_current_prim_hovered(face_selection_data, TexCoords);

    if(!selectedPrim && face_selection_data.meshSelectionEditing == 0 && face_selection_data.hideUnselected == 1){
        fragColor.rgba = vec4(0.);
        gl_FragDepth = 1.;
        return;
    }

    Light light;
    Material material;

    vec3 color;
    //Get Albedo
    if(paintingMode == 1){
        color = getBrushedTexture(painting_data, 0, TexCoords).rgb;
    }
    else
        color = texture(painting_data.painting_buffers.albedo_txtr, TexCoords).rgb;


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

    if(!selectedPrim && face_selection_data.usingMeshSelection == 1 && face_selection_data.meshSelectionEditing == 0){
        fragColor.rgb = mix(fragColor.rgb, vec3(1.), 0.6);
    }

    if(selectedPrim && face_selection_data.meshSelectionEditing == 1){
        fragColor.rgb = mix(fragColor.rgb, vec3(0.18, 0.42, 0.64), 0.6);
    }

    if(hoveredPrim && face_selection_data.meshSelectionEditing == 1)
        fragColor.rgb /= 2.;

    float mirrorDisplayerLineThickness = 0.005;
    if(mirrorState.x == 1.){
        if(Pos.x + mirrorOffsets.x > -mirrorDisplayerLineThickness && Pos.x + mirrorOffsets.x < mirrorDisplayerLineThickness)
            fragColor = vec4(0., 0.5, 1., 1.);
    }
    if(mirrorState.y == 1.){
        if(Pos.y + mirrorOffsets.y > -mirrorDisplayerLineThickness && Pos.y + mirrorOffsets.y < mirrorDisplayerLineThickness)
            fragColor = vec4(0., 0.5, 1., 1.);
    }
    if(mirrorState.z == 1.){
        if(Pos.z + mirrorOffsets.z > -mirrorDisplayerLineThickness && Pos.z + mirrorOffsets.z < mirrorDisplayerLineThickness)
            fragColor = vec4(0., 0.5, 1., 1.);
    }
}