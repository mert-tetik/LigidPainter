//! I've stolen it from https://www.shadertoy.com/view/4s33WB

#version 330 core

out vec4 outClr;

in vec2 texCoords;
const int maxPosSize = 50;

uniform vec2 positions[maxPosSize];
uniform int posCount;

uniform vec2 videoScale;

uniform int frame; //How many frames passed since started painting

struct Brush {
    float radius;
    float hardness;

    float sizeJitter;
    float fade;
    int sinWavePattern;
    float rotation;
    float rotationJitter;
    float spacing;
    float scatter;
    float alphaJitter;
    int individualTexture;
    sampler2D txtr;
};

uniform Brush brush;

void strokeBlendUniColor(vec4 src, float srcA, vec4 dst, out vec4 color)
{
    color.rgb = src.rgb;
    color.a = dst.a + (1.0-dst.a)*src.a; // This can be done either way, I like adding to dst.
    if (color.a > srcA) {
	    color.a = max(dst.a, srcA);
    }
}

float hash(float x)
{
    return fract(sin(x) * 43758.5453);
}
vec2 hash(vec2 p)
{
    return vec2(hash(p.x),hash(p.y + 0.1));
}

float getTextureBrush(vec2 pos,float radius){
    vec2 uv = texCoords;
    uv -= (pos/videoScale);

    radius*= 2.;
    
    uv.x += radius/videoScale.x/2.;
    uv.y += radius/videoScale.y/2.;

    uv.x /= radius/videoScale.x;
    uv.y /= radius/videoScale.y;
   
    uv.x /= videoScale.x/videoScale.y;

    // Flip the Y-coordinate to render the texture right-side up
    float random = hash(frame);
    float rotation = brush.rotation * random;
    float rotationGap = rotation - brush.rotation;
    rotation -= rotationGap * brush.rotationJitter;

    mat2 rotMatrix = mat2(cos(radians(rotation)), sin(radians(rotation)), -sin(radians(rotation)), cos(radians(rotation)));
    
    uv = rotMatrix * uv;
    
    uv.y = 1.0 - uv.y;

    return texture(brush.txtr, uv).r;
}

void main()
{   
    float ratio;
    ratio = videoScale.x/videoScale.y;
 
    vec2 uv = vec2(texCoords.x*ratio,texCoords.y);
    vec4 fRes;
        
    float hardnessV = min(brush.hardness,1.0);

    float radius = brush.radius;

    float random = hash(frame);

    radius *= (random);

    float radiusGap = radius - brush.radius;
    radius -= radiusGap * brush.sizeJitter;

    if(brush.sinWavePattern == 1)
        radius *= 1. - clamp(sin(radians(float(frame)*3.)),0.,1.);
    else
        radius *= 1. - clamp(frame/100.,0.,1.);
    
    radiusGap = radius - brush.radius;
    radius -= radiusGap * brush.fade;

    for(int i = 0; i < min(posCount,maxPosSize); i++) {
        
        vec2 pos = positions[i];
        vec2 random2 = hash(vec2(frame + i));
        pos *= (random2*2.);
        vec2 posGap = pos - positions[i];
        pos -= posGap * brush.scatter;
        
        
        vec4 currentColor = vec4(1);

        vec2 lastPoint = pos;
        lastPoint.x *= ratio;
        vec2 currentPoint = pos;
        currentPoint.x *= ratio;
        vec4 fragColor = vec4(fRes); 

        float d = length(uv*videoScale - currentPoint) / radius;
        float txtr = getTextureBrush(pos,radius);
        
        //if (d < 1.0) {
            
            vec4 src = currentColor;
            if(brush.individualTexture != 1){
                src.a *= smoothstep(1.0, hardnessV * max(0.1, 1.0 - (2.0 / (radius))), d);
                src.a *= 1. -txtr;
            }
            else{
                src.a *= txtr;
            }
            vec4 dst = fragColor;
            strokeBlendUniColor(src, 1., dst, fragColor);
        //}

        fRes = fragColor ;
    }
    
    float opacity = 1. * random;
    float opacityGap = opacity - 1.;
    opacity -= opacityGap * brush.alphaJitter;
    
    outClr = fRes;
    outClr.a *= opacity;
}