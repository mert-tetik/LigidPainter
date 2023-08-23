#version 400 core

in vec2 TexCoords;

uniform sampler2D txtr;
uniform vec2 txtrResolution;
uniform int blurIndex;
uniform vec2 radialPos;
uniform float directionalDirection;
uniform float strength;

out vec4 color;

vec4 boxBlur(vec2 uv)
{
    const int kSize = 5;
    vec4 avg = vec4(0.0);
    for (int i=-kSize; i <= kSize; ++i) {
        for (int j = -kSize; j <= kSize; ++j) {
            vec2 coord = (uv + vec2(float(i), float(j)) / txtrResolution.xy * (strength * 6.));
            avg += texture(txtr, coord);
        }
    }
    int area = (2*kSize + 1) * (2*kSize + 1);
    avg = avg / vec4(area);
    
	return avg;
}

#define pow2(x) (x * x)

const float pi = atan(1.0) * 4.0;
const int samples = 35;
const float sigma = float(samples) * 0.25;

float gaussian(vec2 i) {
    return 1.0 / (2.0 * pi * pow2(sigma)) * exp(-((pow2(i.x) + pow2(i.y)) / (2.0 * pow2(sigma))));
}

vec4 blur(sampler2D sp, vec2 uv, vec2 scale) {
    vec4 col = vec4(0.0);
    float accum = 0.0;
    float weight;
    vec2 offset;
    
    for (int x = -samples / 2; x < samples / 2; ++x) {
        for (int y = -samples / 2; y < samples / 2; ++y) {
            offset = vec2(x, y);
            weight = gaussian(offset); 
                
            col += texture(sp, uv + scale * offset * (strength * 6.)) * weight;
            accum += weight;
        }
    }
    
    return col / accum;
}

vec4 gaussBlur(vec2 uv) {
    vec2 ps = vec2(1.0) / txtrResolution;
    
    return blur(txtr, uv, ps);
}

vec4 dirBlur(sampler2D tex, vec2 uv, vec2 angle)
{
    vec4 acc = vec4(0);
    
    const float delta = 2.0 / float(samples);
    for(float i = -1.0; i <= 1.0; i += delta)
    {
        acc += texture(tex, uv - vec2(angle.x * i, angle.y * i)) * delta * .5;
    }
    
    return acc;  
}

vec4 directionalBlur(vec2 uv)
{
    float r = radians(directionalDirection);
    vec2 direction = vec2(sin(r), cos(r));
    
    return dirBlur(txtr, uv, (strength / 5.) * direction);
}

// radial blur via https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch13.html

vec4 radialBlur()
{	
    const float WEIGHT = 1.;

    vec2 currentPosition = TexCoords.xy;
    
    vec2 toCenter = radialPos - currentPosition;
    vec2 stepSize = toCenter / float(samples);
    
    vec4 color = texture(txtr, currentPosition);
    
    float illuminationDecay = 1.0;
    
    for (int i = 0; i < samples; i++) {
    	currentPosition += stepSize * strength;
        vec4 smp = texture(txtr, currentPosition);
        smp *= illuminationDecay * WEIGHT;
        
        color += smp;
    }
    
    color /= vec4(samples);
    
    return color;
    
    // uncomment to attenuate by distance
    //float blendFactor = 1.0 - distance(originalPosition, BLUR_CENTER);
    //vec3 finalColor = blendFactor * color + (1.0 - blendFactor) * originalColor;
    //fragColor = vec4(finalColor * EXPOSURE, 1.0);
    
    // uncomment to see original texture
	//vec2 originalUv = fragCoord.xy / iResolution.xy;
    //fragColor = texture(iChannel0, originalUv);
}

float gamma = 1.;
float BRIGHT_SPOT_TRESHOLD = 0.5;
int CONV_SIDE = int(strength * 20.); //20 (bigger blur and performance comparation with separable one)
float BLUR_RADIUS = strength * 20.; //20 (bigger blur and performance comparation with separable one)

vec4 BriSp(vec4 p){
    vec4 a = p;
    if(p.x + p.y + p.z > BRIGHT_SPOT_TRESHOLD * 3. )
        p = (1. / (1. - p) - 1.) * (1. - BRIGHT_SPOT_TRESHOLD);

    p = clamp(p,0.,500.);
    
    p = mix(a, p, strength);
    
    return p;
}

vec4 clring(vec2 n){
    vec4 p = texture(txtr, n);
    p = BriSp(p);
    return p;
}

vec4 lensBlur(){
    
    vec2 un = TexCoords * txtrResolution.y;
    
    vec2 u_=vec2(0);
    float n=0.;
    vec4 res=vec4(0);
    for(int x=-CONV_SIDE;x<=CONV_SIDE;x++){
        for(int y=-CONV_SIDE;y<=CONV_SIDE;y++){
        	u_=un+vec2(x,y);
            if(distance(u_,un)<=BLUR_RADIUS){
            	u_ /= txtrResolution.y;
                res += pow(clring(u_),vec4(gamma));
                n++;
            }
        }
    }
    res/=n;
    return pow(res, vec4(1. / gamma));
}

void main(){
    vec2 uv = TexCoords;

    if(blurIndex == 0)
        color = boxBlur(uv);
    else if(blurIndex == 1)
        color = gaussBlur(uv);
    else if(blurIndex == 2)
        color = directionalBlur(uv);
    else if(blurIndex == 3)
        color = radialBlur();
    else if(blurIndex == 4)
        color = lensBlur();
    else
        color = vec4(0.);
}