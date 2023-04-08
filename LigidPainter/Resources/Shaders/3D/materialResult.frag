//Material Result Shader Source
//Is used for masking materials & rendering the result.

#version 330 core

in vec2 TexCoords;

uniform sampler3D materials;
uniform sampler3D maskTextures;

//First material won't be masking

uniform int submeshCount; //This times material & mask texture will be usen

//TODO : Fix alpha channel

out vec4 fragColor;

void main(){
    vec3 result;

    for(int i = 0; i < submeshCount; i++) {
        float mask = texture(maskTextures,vec3(TexCoords,i)).r;
        vec3 material = texture(materials,vec3(TexCoords,i)).rgb;
        
        result = mix(result,material,mask);
        result += material*mask;
    }

    fragColor = vec4(result,1);
}