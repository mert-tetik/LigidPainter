#version 330 core

in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;
in vec3 Tangent;
in vec3 Bitangent;

in vec4 projectedPos;

out vec4 fragColor;

void main() {
    fragColor = vec4(1);
}