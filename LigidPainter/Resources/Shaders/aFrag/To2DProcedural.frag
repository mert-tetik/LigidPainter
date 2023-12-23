#version 400 core

#pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Procedural.frag)

in vec3 Pos;
in vec2 TexCoords;
in vec3 Normal;
in vec3 Tangent;
in vec3 Bitangent;

uniform int proceduralID; 
uniform float proceduralScale; 
uniform float proceduralStretch; 
uniform int proceduralInverted;
uniform sampler2D proceduralTexture;
uniform int proceduralUseTexCoords;
uniform vec4 smartProperties;
uniform vec2 txtrRes;
uniform int proceduralGrayScale;
uniform float proceduralBrightness;

out vec4 fragColor;

void main(){

    vec2 uv = Pos.xy;
    vec3 vPos = Pos;

    uv = abs(uv);
    if(Pos.z < 0.3 && Pos.z > -0.3){
        if(Pos.z > 0.)
            uv.xy += (0.3 - Pos.z) / 4.;
        else if(Pos.z < 0.)
            uv.xy += (0.3 + Pos.z) / 4.;
    }

    if(proceduralUseTexCoords == 1){
        uv = TexCoords;
        vPos.xy = TexCoords;
        vPos.z = 0.;
    }

    vec4 procedural = getProcedural(vPos, proceduralID, proceduralTexture, uv, proceduralScale, proceduralStretch, proceduralInverted, smartProperties, txtrRes, proceduralGrayScale, proceduralBrightness);
    fragColor = vec4(procedural);
}