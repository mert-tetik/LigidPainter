#version 400 core

in vec3 fragPos;
in vec2 texCoords;

uniform sampler2D theTexture;
uniform int state;

out vec4 fragColor;

void getImage(sampler2D txtr,out vec4 result){
    result = texture(txtr,texCoords);
}

void main(){
    if(state == 0)
        fragColor = texture(theTexture,texCoords);
    if(state == 1)
        fragColor = texture(theTexture,texCoords);
    if(state == 2)
        fragColor = texture(theTexture,texCoords);
    if(state == 3)
        fragColor = texture(theTexture,texCoords);
    if(state == 4)
        fragColor = texture(theTexture,texCoords);
    if(state == 5)
        fragColor = texture(theTexture,texCoords);
}