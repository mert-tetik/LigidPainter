#version 400 core

in vec2 TexCoords;

uniform sampler2D heightMap;

out vec4 fragColor;


#define heightMapResolution vec2(1000.)
#define normalStrength 10.0
#define textureOffset 1.0

vec2 texNormalMap(in vec2 uv)
{
    vec2 s = 1.0/heightMapResolution.xy;
    
    float p = texture(heightMap, uv).x;
    float h1 = texture(heightMap, uv + s * vec2(textureOffset,0)).x;
    float v1 = texture(heightMap, uv + s * vec2(0,textureOffset)).x;
       
   	return (p - vec2(h1, v1));
}

void main()
{
    vec2 uv = TexCoords;
    
    vec2 normal = texNormalMap(uv);
    
    normal *= normalStrength;
    normal += 0.5;
    
    fragColor = vec4(normal, 1., 1.);
}