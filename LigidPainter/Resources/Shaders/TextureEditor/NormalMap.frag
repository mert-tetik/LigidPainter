#version 400 core

in vec2 TexCoords;

uniform sampler2D txtr;
uniform vec2 txtrResolution;
uniform int grayScale;
uniform float strength;
uniform float blurVal;
uniform float rot;
uniform int mode;

out vec4 color;

#define textureOffset 1.0

vec2 rotate2D(vec2 v, float angleDegrees) {
    float angleRadians = radians(angleDegrees);
    float cosA = cos(angleRadians);
    float sinA = sin(angleRadians);

    mat2 rotationMatrix = mat2(cosA, -sinA, sinA, cosA);

    return rotationMatrix * v;
}

vec2 texNormalMap(in vec2 uv)
{
    vec2 s = vec2(1.0 / txtrResolution);
    
    float p = texture(txtr, uv).x;

    vec2 uvX = vec2(textureOffset,0);
    uvX = rotate2D(uvX.rg, rot); 
    vec2 uvY = vec2(0,textureOffset);
    uvY = rotate2D(uvY.rg, rot);
    
    float h1 = texture(txtr, uv + s * uvX).x;
    float v1 = texture(txtr, uv + s * uvY).x;
       
   	return (p - vec2(h1, v1));
}

    #define pow2(x) (x * x)

const float pi = atan(1.0) * 4.0;
const int samples = 16;
const float sigma = float(samples) * 0.25;

float gaussian(vec2 i) {
    return 1.0 / (2.0 * pi * pow2(sigma)) * exp(-((pow2(i.x) + pow2(i.y)) / (2.0 * pow2(sigma))));
}

void main()
{
    vec2 uv = TexCoords;
    if(mode == 0){
        float accum = 0.0;
        float weight;
        vec2 offset;
        
        vec2 normal = vec2(0.);
        for (int x = -samples / 2; x < samples / 2; ++x) {
            for (int y = -samples / 2; y < samples / 2; ++y) {
                offset = vec2(x, y);
                weight = gaussian(offset);
                    
                normal += texNormalMap(uv + (1./txtrResolution) * offset * (blurVal * 5.)) * weight;
                accum += weight;
            }
        }

        //normal /= accum;

        normal *= (strength * 2.5);
        normal += 0.5;
        
        if(grayScale == 0)
            color = vec4(normal, 1., 1.);
        else
            color = vec4(vec3(normal.x), 1.);
    
    }
    else{
        float accum = 0.0;
        float weight;
        vec2 offset;

        vec3 n = vec3(0.);
        for (int x = -samples / 2; x < samples / 2; ++x) {
            for (int y = -samples / 2; y < samples / 2; ++y) {
                offset = vec2(x, y);
                weight = gaussian(offset);
                
                n += texture(txtr, uv + (1./txtrResolution) * offset * (blurVal * 5.)).rgb * weight;
                accum += weight;
            }
        }

        n.rg = rotate2D(n.rg - vec2(0.5), rot);
        
        float m = n.r + n.g;

        color.rgb = vec3(m * strength);
    }
    
    color.a = texture(txtr, uv).a;
}