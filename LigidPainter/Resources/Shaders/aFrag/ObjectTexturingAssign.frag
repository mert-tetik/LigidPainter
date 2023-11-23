#version 400 core

out vec4 fragColor;

in vec2 TexCoords;

uniform sampler2D txtr;
uniform sampler2D selectedPrimitiveIDS;
uniform sampler2D meshMask;
uniform int primitiveCount;


void main(){
    float prim = texelFetch(selectedPrimitiveIDS, ivec2(gl_PrimitiveID % int(ceil(sqrt(primitiveCount))), gl_PrimitiveID / int(ceil(sqrt(primitiveCount)))), 0).r;
    bool selectedPrim = prim > 0.9 && texture(meshMask, TexCoords).r > 0.5;

    fragColor = vec4(0.);

    if(selectedPrim){
       fragColor = texture(txtr, TexCoords); 
    }
}