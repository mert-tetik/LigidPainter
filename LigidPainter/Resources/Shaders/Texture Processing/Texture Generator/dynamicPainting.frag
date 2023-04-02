//! I've stolen it from https://www.shadertoy.com/view/4s33WB

#version 330 core

out vec4 outClr;

in vec2 texCoords;
const int maxPosSize = 50;
uniform vec2 positions[maxPosSize];
uniform int posCount;

uniform float opacity;
uniform float radius;
uniform float hardness;

uniform int displayingTheBrush;

uniform sampler2D bgtxtr;

void strokeBlendUniColor(vec4 src, float srcA, vec4 dst, out vec4 color)
{
    color.rgb = src.rgb;
    color.a = dst.a + (1.0-dst.a)*src.a; // This can be done either way, I like adding to dst.
    if (color.a > srcA) {
	    color.a = max(dst.a, srcA);
    }
}

void main()
{   
    //TODO : Update ratio (1.7)
    const float ratio =1.77777777778;
 
    vec2 uv = vec2(texCoords.x*ratio,texCoords.y);
    float bg = texture(bgtxtr,vec2(texCoords.x * 1000,1-texCoords.y*1000)).r;
    vec4 fRes;
        
    float hardnessV = min(hardness,1.0);


    for(int i = 0; i < min(posCount,maxPosSize); i++) {
        vec4 currentColor = vec4(1);

        vec2 lastPoint = positions[i]*1000;
        lastPoint.x *= ratio;
        vec2 currentPoint = positions[i]*1000;
        currentPoint.x *= ratio;
        vec4 fragColor = vec4(bg); 

        float d = length(uv*1000 - currentPoint) / radius;
        if (d < 1.0) {
            vec4 src = currentColor;
            src.a *= smoothstep(1.0, hardnessV * max(0.1, 1.0 - (2.0 / (radius))), d);
            vec4 dst = fragColor;
            strokeBlendUniColor(src, 1., dst, fragColor);
        }

        fRes += fragColor;
    }

    if(displayingTheBrush == 1)
        outClr = vec4(vec3((fRes.a)*100),1.);
     else
        outClr = fRes*opacity;
}



    

