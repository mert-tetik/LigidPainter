#version 400 core

out vec4 fragColor;

in vec2 TexCoords;

uniform int usingMeshSelection = 0;
uniform int hideUnselected = 0;
uniform sampler2D selectedPrimitiveIDS;
uniform sampler2D meshMask;
uniform int primitiveCount;

void main(){
    fragColor = vec4(0.);

    gl_FragDepth = gl_FragCoord.z;
    
    float prim = texelFetch(selectedPrimitiveIDS, ivec2(gl_PrimitiveID % int(ceil(sqrt(primitiveCount))), gl_PrimitiveID / int(ceil(sqrt(primitiveCount)))), 0).r;
    bool selectedPrim = prim > 0.9 && texture(meshMask, TexCoords).r > 0.5;
    
    if(!selectedPrim && usingMeshSelection == 1){
        if(hideUnselected == 1){
            gl_FragDepth = 1.;
        }
    }
}