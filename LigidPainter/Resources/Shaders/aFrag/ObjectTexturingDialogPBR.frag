#version 400 core

//Painting
#pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Painting.frag)

//Functions related to PBR 
#pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Physics_Math.frag)

in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;
in vec3 Tangent;
in vec3 Bitangent;
in vec4 ProjectedPos;

//Position of the camera
uniform vec3 viewPos;

//Used for ambient
//TODO : Don't use the skybox use prefilterMap instead & use blury
uniform samplerCube skybox;

//Used for reflection
uniform samplerCube prefilterMap;


uniform sampler2D mask_texture;
uniform int channelI;

//Material channels
uniform sampler2D albedoTxtr; //Albedo
uniform sampler2D roughnessTxtr; //Roughness
uniform sampler2D metallicTxtr; //Metallic
uniform sampler2D normalMapTxtr; //Normal Map
uniform sampler2D heightMapTxtr; //Ambient occlusion (ao)
uniform sampler2D ambientOcclusionTxtr; //Ambient occlusion (ao)

uniform int displayingMode = 0; 

//Fragment shader output
out vec4 fragColor;

float checkerPattern(
                        vec2 p
                    )  
{
    p = 0.5 - fract(p);
    return 0.5 + 0.5*sign(p.x*p.y);
}

void main() {

    vec3 checker_val = vec3(checkerPattern(Pos.xy * 10.) / 4. + 0.5);
    //Material channels
    vec4 albedo = texture(albedoTxtr,TexCoords);
    albedo.rgb = mix(checker_val, albedo.rgb, albedo.a);
    vec4 roughness = texture(roughnessTxtr,TexCoords);
    roughness.rgb = mix(checker_val, roughness.rgb, roughness.a);
    vec4 metallic = texture(metallicTxtr,TexCoords);
    metallic.rgb = mix(checker_val, metallic.rgb, metallic.a);
    vec4 normal = texture(normalMapTxtr,TexCoords);
    normal.rgb = mix(checker_val, normal.rgb, normal.a);
    vec4 height = texture(heightMapTxtr,TexCoords);
    height.rgb = mix(checker_val, height.rgb, height.a);
    vec4 ao = texture(ambientOcclusionTxtr,TexCoords);
    ao.rgb = mix(checker_val, ao.rgb, ao.a);

    if(channelI == 1){
        fragColor.rgb = albedo.rgb;
        fragColor.a = 1.;
    }
    else if(channelI == 2){
        fragColor.rgb = roughness.rgb;
        fragColor.a = 1.;
    }
    else if(channelI == 3){
        fragColor.rgb = metallic.rgb;
        fragColor.a = 1.;
    }
    else if(channelI == 4){
        fragColor.rgb = normal.rgb;
        fragColor.a = 1.;
    }
    else if(channelI == 5){
        fragColor.rgb = height.rgb;
        fragColor.a = 1.;
    }
    else if(channelI == 6){
        fragColor.rgb = ao.rgb;
        fragColor.a = 1.;
    }
    else{
        vec3 pbrResult;

        if(displayingMode == 0)
            pbrResult = getPBR(
                                    albedo.rgb, roughness.r, metallic.r, normal.rgb, ao.r, 
                                    Pos, Normal, Tangent, Bitangent, 
                                    skybox, prefilterMap, viewPos, 1 
                                );
                                
        if(displayingMode == 1)
            pbrResult = albedo.rgb;
        if(displayingMode == 2)
            pbrResult = roughness.rgb;
        if(displayingMode == 3)
            pbrResult = metallic.rgb;
        if(displayingMode == 4)
            pbrResult = normal.rgb;
        if(displayingMode == 5)
            pbrResult = height.rgb;
        if(displayingMode == 6)
            pbrResult = ao.rgb;

        
        fragColor = vec4(
                            pbrResult, 
                            1.
                        );
    }

    float mask = texture2D(mask_texture, TexCoords).r * texture2D(mask_texture, TexCoords).a;
    fragColor.rgb = mix(fragColor.rgb / 4., fragColor.rgb, mask);
}