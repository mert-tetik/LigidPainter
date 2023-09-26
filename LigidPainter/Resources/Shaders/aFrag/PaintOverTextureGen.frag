#version 400 core

uniform sampler2D txtr;

in vec2 TexCoords;

uniform vec2 fragScale;
uniform vec2 fragMove;
uniform float opacity;
uniform float rot;

out vec4 color;

vec2 rotate2D(vec2 v, float angleDegrees) {
    vec2 centeredUV = v - 0.5; // Shift UV to center
    float cosRot = cos(radians(angleDegrees));
    float sinRot = sin(radians(angleDegrees));
    v.x = dot(centeredUV, vec2(cosRot, sinRot)) + 0.5;
    v.y = dot(centeredUV, vec2(-sinRot, cosRot)) + 0.5;

    return v;
}

void main(){
    vec2 uv = TexCoords;
    uv = rotate2D(uv, rot);
    color = texture(txtr, uv * fragScale + fragMove);
    color.a *= opacity;
}