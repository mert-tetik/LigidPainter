#version 400 core

out vec4 fragColor;

in vec2 TexCoords;

uniform sampler2D txtr;
uniform sampler2D selectedPrimitiveIDS;
uniform int primitiveCount;


void main(){
    float prim = texelFetch(selectedPrimitiveIDS, ivec2(gl_PrimitiveID % int(sqrt(primitiveCount)), gl_PrimitiveID / int(sqrt(primitiveCount))), 0).r;
    bool selectedPrim = prim > 0.9;

    fragColor = vec4(0.);

    if(selectedPrim){
       fragColor = texture(txtr, TexCoords); 
    }
}