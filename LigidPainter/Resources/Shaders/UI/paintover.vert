#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 TextProjection;

out vec2 TexCoords;

void main() {
   TexCoords = aTexCoords;
   gl_Position = TextProjection * vec4(aPos, 1.0);
}