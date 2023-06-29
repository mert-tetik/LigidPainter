#version 400 core

//Painting
#pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Painting.frag)

struct VertexData{
    vec2 TexCoords;
    vec3 Normal;
    vec3 Pos;
    vec3 Tangent;
    vec3 Bitangent;
    vec4 ProjectedPos;
};

//Vertex data : pos , normal , texture coordinates , tangent , bitangent , projected position
in VertexData vertexData;

//The texture that will be updated
uniform sampler2D txtr; 

//Contains the brush strokes
uniform sampler2D paintingTexture;

//3D Model rendered with depth shader (to compare depth)
uniform sampler2D depthTexture;

//0 = paint
//1 = soften
//2 = smear
uniform int brushModeState; 

//Selected opacity for painting
uniform float paintingOpacity;

//Selected color for painting
uniform vec3 paintingColor;

//Fragment shader output
out vec4 fragColor;

void main(){

    vec4 brushTxtr = getBrushValue(paintingTexture, depthTexture, vertexData.ProjectedPos, paintingOpacity);

    float txtrAlpha = texture(txtr, vertexData.TexCoords).a; 

    fragColor = vec4(getBrushedTexture(txtr, brushTxtr, vertexData.TexCoords, paintingColor, brushModeState),txtrAlpha);

}