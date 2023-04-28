#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 TextProjection;

out vec2 TexCoords;
out vec2 Pos;

void main() {

   Pos = (TextProjection * vec4(aPos, 1.0)).xy / vec2(2.0, 2.0) + 0.5 / vec2(1.0, 1.0);
   TexCoords = aTexCoords;
   gl_Position = TextProjection * vec4(aPos, 1.0);
}