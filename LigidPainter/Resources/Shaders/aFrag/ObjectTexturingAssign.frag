#version 400 core

out vec4 fragColor;

in vec2 TexCoords;

uniform sampler2D txtr;
uniform sampler2D selectedPrimitiveIDS;
uniform sampler2D meshMask;
uniform int primitiveCount;


void main(){
    float prim_txtr_res = int(ceil(sqrt(primitiveCount)));
    float prim_height = floor(float(gl_PrimitiveID) / prim_txtr_res);
    float prim = texelFetch(selectedPrimitiveIDS, ivec2(float(gl_PrimitiveID) - (prim_height * prim_txtr_res) , prim_height), 0).r;
    bool selectedPrim = prim > 0.9 && texture(meshMask, TexCoords).r > 0.5;

    fragColor = vec4(0.);

    if(selectedPrim){
       fragColor = texture(txtr, TexCoords); 
    }
}