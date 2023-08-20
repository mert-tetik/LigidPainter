#version 400 core

uniform sampler2D txtr;

in vec2 TexCoords;

uniform vec2 fragScale;
uniform vec2 fragMove;
uniform float opacity;
uniform float rotation;

out vec4 color;

vec2 rotate2D(vec2 v, float angleDegrees) {
    float angleRadians = radians(angleDegrees);
    float cosA = cos(angleRadians);
    float sinA = sin(angleRadians);

    mat2 rotationMatrix = mat2(cosA, -sinA, sinA, cosA);

    return rotationMatrix * v;
}

void main(){
    vec2 uv = TexCoords;
    uv = rotate2D(uv, rotation);
    color = texture(txtr, uv * fragScale + fragMove);
    color.a *= opacity;
}