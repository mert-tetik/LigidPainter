#version 400 core

in vec2 TexCoords;

uniform sampler2D heightMap;
uniform int txtrRes;
uniform int grayScale;
uniform float strength;
uniform int alphaMode;

out vec4 fragColor;

#define textureOffset 1.0

vec2 texNormalMap(in vec2 uv)
{
    vec2 s = vec2(1.0 / txtrRes);
    
    float p = texture(heightMap, uv).x;
    float h1 = texture(heightMap, uv + s * vec2(textureOffset,0)).x;
    float v1 = texture(heightMap, uv + s * vec2(0,textureOffset)).x;
       
   	return (p - vec2(h1, v1));
}

vec2 texNormalMapAlpha(in vec2 uv)
{
    vec2 s = vec2(1.0 / txtrRes);
    
    float p = texture(heightMap, uv).x;
    float h1 = texture(heightMap, uv + s * vec2(textureOffset,0)).x;
    float v1 = texture(heightMap, uv + s * vec2(0,textureOffset)).x;
       
   	return (p - vec2(h1, v1));
}

void main()
{
    vec2 uv = TexCoords;
    
    vec2 normal = texNormalMap(uv);

    if(alphaMode == 1)
        normal = texNormalMapAlpha(uv);
    
    normal *= strength;
    normal += 0.5;
    
    if(alphaMode == 0){
        if(grayScale == 0)
            fragColor = vec4(normal, 1., texture(heightMap, uv).a);
        else
            fragColor = vec4(vec3(normal.x), texture(heightMap, uv).a);
    }
    else{
        if(grayScale == 0)
            fragColor = vec4(normal, 1., length(normal - 0.5));
        else
            fragColor = vec4(vec3(normal.x), length(normal - 0.5));
    }
}
