#version 400 core

#define pow2(x) (x * x)

const float pi = atan(1.0) * 4.0;
const int samples = 35;
const float sigma = float(samples) * 0.25;

in vec2 TexCoords;

uniform sampler2D txtr;
uniform vec2 txtrRes;

out vec4 fragColor;

float gaussian(vec2 i) {
    return 1.0 / (2.0 * pi * pow2(sigma)) * exp(-((pow2(i.x) + pow2(i.y)) / (2.0 * pow2(sigma))));
}

vec3 blur(sampler2D sp, vec2 uv, vec2 scale) {
    vec3 col = vec3(0.0);
    float accum = 0.0;
    float weight;
    vec2 offset;
    
    for (int x = -samples / 2; x < samples / 2; ++x) {
        for (int y = -samples / 2; y < samples / 2; ++y) {
            offset = vec2(x, y);
            weight = gaussian(offset);
            col += texture(sp, uv + scale * offset).rgb * weight;
            accum += weight;
        }
    }
    
    return col / accum;
}

void main() {
    vec2 ps = vec2(1.0) / txtrRes;
    vec2 uv = TexCoords;
    
    fragColor.rgb = vec3(blur(txtr, uv, ps).r);
    fragColor.a = 1.0;
    
}
