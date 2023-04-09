//Material Result Shader Source
//Is used for masking materials & rendering the result.

#version 330 core

in vec2 TexCoords;

uniform sampler2D material0;
uniform sampler2D material1;
uniform sampler2D material2;
uniform sampler2D material3;
uniform sampler2D material4;
uniform sampler2D material5;

uniform sampler2D mask0;
uniform sampler2D mask1;
uniform sampler2D mask2;
uniform sampler2D mask3;
uniform sampler2D mask4;
uniform sampler2D mask5;

//First material won't be masking

uniform int submeshCount; //This times material & mask texture will be usen

//TODO : Fix alpha channel

out vec4 fragColor;

void main(){
    vec3 result;

    for(int i = 0; i < submeshCount; i++) {
        float mask;
        vec3 material;
        if(i == 0){
            mask = texture(mask0,TexCoords).r;
            material = texture(material0,TexCoords).rgb;
        }
        if(i == 1){
            mask = texture(mask1,TexCoords).r;
            material = texture(material1,TexCoords).rgb;
        }
        if(i == 2){
            mask = texture(mask2,TexCoords).r;
            material = texture(material2,TexCoords).rgb;
        }
        if(i == 3){
            mask = texture(mask3,TexCoords).r;
            material = texture(material3,TexCoords).rgb;
        }
        if(i == 4){
            mask = texture(mask4,TexCoords).r;
            material = texture(material4,TexCoords).rgb;
        }
        if(i == 5){
            mask = texture(mask5,TexCoords).r;
            material = texture(material5,TexCoords).rgb;
        }

        result = mix(result,material,mask);
        //result += material;
    }

    fragColor = vec4(result,1);
}