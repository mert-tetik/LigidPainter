#version 400 core

#define pow2(x) (x * x)

const float pi = atan(1.0) * 4.0;
const int samples = 35;
const float sigma = float(samples) * 0.25;

in vec2 TexCoords;

uniform sampler2D txtr;
uniform sampler2D uvMask;
uniform vec2 txtrRes;
uniform float blurVal;

uniform int horizontal;

out vec4 fragColor;

float gaussian(vec2 i) {
    return 1.0 / (2.0 * pi * pow2(sigma)) * exp(-((pow2(i.x) + pow2(i.y)) / (2.0 * pow2(sigma))));
}

vec3 blur(sampler2D sp, vec2 uv, float scale) {
    vec3 col = vec3(0.0);
    float accum = 0.0;
    float weight;
    float offset;
    
    for (int x = -samples / 2; x < samples / 2; ++x) {
        offset = float(x) / float(samples);
        weight = gaussian(vec2(offset, 1));
        
        if(texture(uvMask, uv + scale * offset * (blurVal)).r < 0.5){
            offset = 0.;
        }
        
        vec2 of = vec2(scale * offset * (blurVal), 0.);
        if(horizontal != 1)
            of = vec2(0, of.x); 

        col += texture(sp, uv + of).rgb * weight;
        accum += weight;
    }
    
    return col / accum;
}

void main() {
    if(texture(txtr, TexCoords).a < 0.1)
        return;

    float ps = float(0.00390625); // 1 / 256
    vec2 uv = TexCoords;
    
    fragColor.rgb = vec3(blur(txtr, uv, ps));
    fragColor.a = 1.0;
}
