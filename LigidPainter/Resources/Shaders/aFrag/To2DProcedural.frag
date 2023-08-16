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
    // Calculate the bitangent (cross product of normal and tangent)
    
    // Map the transformed x and y components of the tangent space position to [0, 1]
    vec2 uv;
    if(iabs(Pos).x > iabs(Pos).z)
        uv.x = iabs(Pos).x;
    else
        uv.x = iabs(Pos).z;
    uv.y = Pos.y;  // Combining y and z

    vec4 procedural = getProcedural(Pos, proceduralID, proceduralTexture, uv, proceduralScale, proceduralInverted);
    fragColor = vec4(procedural);
}