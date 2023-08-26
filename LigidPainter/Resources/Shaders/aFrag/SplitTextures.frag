#version 400 core

in vec2 TexCoords;
uniform sampler2D texture1;
uniform sampler2D texture2;
out vec4 fragColor;

vec2 rotate2D(vec2 v, float angleDegrees) {
    float angleRadians = radians(angleDegrees);
    float cosA = cos(angleRadians);
    float sinA = sin(angleRadians);

    mat2 rotationMatrix = mat2(cosA, -sinA, sinA, cosA);

    return rotationMatrix * v;
}

float checkerboard(vec2 coord, float size){
    vec2 pos = floor(coord/size); 
    return mod(pos.x+pos.y,2.0);
}

void main()
{
    vec2 uv = TexCoords;
    uv = rotate2D(uv, 50.);
    uv += 0.0;
    float size = 1.6;
    float c = checkerboard(uv,size);
    vec4 txtr1 = texture(texture1, TexCoords);
    vec4 txtr2 = texture(texture2, TexCoords);

    fragColor = mix(txtr1, txtr2 , c);
}