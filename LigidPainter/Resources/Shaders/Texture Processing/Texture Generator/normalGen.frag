#version 330 core

uniform sampler2D inputTexture;

in vec2 texCoords;

out vec4 color;

#define normalStrength 10.0
#define textureOffset 1.0

vec2 stdNormalMap(in vec2 uv) 
{

    float height = texture(inputTexture, uv).r;
    return -vec2(dFdx(height), dFdy(height));
}
void main()
{
    vec2 normal = stdNormalMap(texCoords);
    
    normal *= normalStrength;
    normal += 0.5;
    
    color = vec4(normal, 1., 1.);
    
}