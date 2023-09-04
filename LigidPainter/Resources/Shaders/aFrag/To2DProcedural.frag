#version 400 core

#pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Procedural.frag)

in vec3 Pos;
in vec2 TexCoords;
in vec3 Normal;
in vec3 Tangent;
in vec3 Bitangent;

uniform int proceduralID; 
uniform float proceduralScale; 
uniform int proceduralInverted;
uniform sampler2D proceduralTexture;
uniform int proceduralUseTexCoords;
uniform vec4 smartProperties;
uniform vec2 txtrRes;

out vec4 fragColor;

vec3 iabs(vec3 v){
    vec3 val = v;
    if(val.x > 0.)
        val.x *= -1.;
    if(val.y > 0.)
        val.y *= -1.;
    if(val.z > 0.)
        val.z *= -1.;
        
    return val;
}

void main(){

    vec2 uv = Pos.xy;

    uv = abs(uv);
    if(Pos.z < 0.3 && Pos.z > -0.3){
        if(Pos.z > 0.)
            uv.xy += (0.3 - Pos.z) / 4.;
        else if(Pos.z < 0.)
            uv.xy += (0.3 + Pos.z) / 4.;
    }

    if(proceduralUseTexCoords == 1)
        uv = TexCoords;

    vec4 procedural = getProcedural(Pos, proceduralID, proceduralTexture, uv, proceduralScale, proceduralInverted, smartProperties, txtrRes);
    fragColor = vec4(procedural);
}