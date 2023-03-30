//! I've stolen it from https://www.shadertoy.com/view/4s33WB

#version 330 core

out vec4 outClr;

in vec2 texCoords;

uniform vec2 positions[20];
uniform int posCount;

uniform float opacity;
uniform float radius;
uniform float hardness;

uniform int displayingTheBrush;

uniform sampler2D bgtxtr;

const float kGamma = 2.2;   // set this to 1.0 to see the wrong way to blend colors (the way Photoshop does)

vec4 over( in vec4 src, in vec4 dst )
{
        vec4 c = vec4(
        src.rgb * src.a + dst.rgb * dst.a * (1.0-src.a),
        src.a + (1.0-src.a)*dst.a
   	    );
    c.rgb /= c.a;
    return c;
}

void strokeBlendUniColor(vec4 src, float srcA, vec4 dst, out vec4 color)
{
    color.rgb = src.rgb;
    color.a = dst.a + (1.0-dst.a)*src.a; // This can be done either way, I like adding to dst.
    if (color.a > srcA) {
	    color.a = max(dst.a, srcA);
    }
}
vec4 linear2gamma_premultalpha( vec4 c )
{
    return vec4( pow(c.xyz/c.w,vec3(1.0/kGamma) ), 1.0 );
}

void main()
{   
    //TODO : Update ratio (1.7)
    float bufferRatio = 1.7;
    vec2 uv = vec2(texCoords.x*bufferRatio,texCoords.y);//TODO : Fix tha shi
    float bg = texture(bgtxtr,vec2(texCoords.x * 1000,1-texCoords.y*1000)).r;
        vec4 fRes;
        
        float hardnessV = min(hardness,1.0);
        for(int i = 0; i < posCount; i++) {
            vec4 currentColor = vec4(1);

            vec2 lastPoint = positions[i]*1000;
            vec2 currentPoint = positions[i]*1000;
            vec4 fragColor = vec4(bg); 


            vec2 strokeDirection = currentPoint - lastPoint;
            float strokeLength = length(strokeDirection);
            //currentColor.a *= 1.0-pow(texture(iChannel1, 0.5+0.5*(fragCoord-currentPoint.xy)/radius).r, 1.0);

            if (strokeLength == 0.0 && fragColor.a == 0.0) {
                float d = length(uv*1000 - currentPoint) / radius;
                if (d < 1.0) {
                    vec4 src = currentColor;
                    src.a *= smoothstep(1.0, hardnessV * max(0.1, 1.0 - (2.0 / (radius))), d);
                    vec4 dst = fragColor;
                    //src.rgb = vec3(0.5, 0.0, 0.0);
                    strokeBlendUniColor(src, 1., dst, fragColor);
                }
            } else {
                strokeDirection = normalize(strokeDirection);
                float stampSeparation = radius * 0.5;
                float stampCount = floor(strokeLength / stampSeparation + 0.5);

                for (float i=1.0; i<200.0; i++) {
                    if (i > stampCount) {
                        break;
                    }
                    float d = length(uv*1000 - (lastPoint + strokeDirection*i*stampSeparation)) / radius;
                    if (d < 1.0) {
                        vec4 src = currentColor;
                        //src.rgb = vec3(0.0, 0.5, 0.0);
                        src.a *= smoothstep(1.0, hardnessV * max(0.1, 1.0 - (2.0 / (radius))), d);
                        vec4 dst = fragColor;
                        strokeBlendUniColor(src, 1., dst, fragColor);
                    }
                }
            }
            fRes += fragColor;
        }
        if(displayingTheBrush == 1)
            outClr = vec4(vec3((fRes.a)*100),1.);
        else
            outClr = fRes*opacity;
}



    

