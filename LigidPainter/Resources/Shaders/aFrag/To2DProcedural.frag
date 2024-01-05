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
uniform float proceduralScaleModelPos; 
uniform int proceduralInverted;
uniform sampler2D proceduralTexture;
uniform int proceduralUseTexCoords;
uniform vec4 smartProperties;
uniform vec2 txtrRes;
uniform int proceduralGrayScale;
uniform float proceduralBrightness;

out vec4 fragColor;

void main(){

    vec3 vPos = Pos;

    vPos /= 2.;
    vPos += 0.5;
    vPos *= 2.;
    vPos *= proceduralScaleModelPos;

    vec2 uv = vPos.xy;
    
    if(Pos.z < 0.3 && Pos.z > -0.3){
        if(Pos.z > 0.)
            uv.x += (0.3 - Pos.z) / 3.;
        else if(Pos.z < 0.)
            uv.x += (0.3 + Pos.z) / 3.;
    }

    uv = abs(uv);
    
    
    if(proceduralUseTexCoords == 1){
        uv = TexCoords;
        vPos.xy = TexCoords;
        vPos.z = 0.;
    }

    vec4 procedural = getProcedural(vPos, proceduralID, proceduralTexture, uv, proceduralScale, proceduralStretch, proceduralInverted, smartProperties, txtrRes, proceduralGrayScale, proceduralBrightness);
    fragColor = vec4(procedural);
}