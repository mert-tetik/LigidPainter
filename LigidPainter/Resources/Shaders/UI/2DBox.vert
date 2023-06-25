#version 400 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

//All the GUI elements will use that vertex shader 

out vec3 fragPos;
out vec2 texCoords;
out vec2 uniScale;

uniform mat4 projection;
uniform vec3 pos;
uniform vec2 scale;

void main() {
    uniScale = scale;
    texCoords = aTexCoords;
    fragPos = aPos;

    vec3 scaledPos = aPos * vec3(scale,1);

    gl_Position = projection * vec4(scaledPos + pos, 1.0);
}