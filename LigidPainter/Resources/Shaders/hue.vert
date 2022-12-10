#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColors;
layout(location = 2) in vec2 aTexCoords;

out vec3 Colors;


uniform int useTexCoords;
uniform mat4 renderTextureProjection;


void main() {
    Colors = aColors;

    if(useTexCoords == 1){
       gl_Position = renderTextureProjection * vec4(aTexCoords, 0, 1);
    }
    else{
       gl_Position = renderTextureProjection * vec4(aPos, 1.0);
    }
}