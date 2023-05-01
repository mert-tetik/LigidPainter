#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 TextProjection;

out vec2 texCoords;

uniform vec3 pos;
uniform mat4 scale;

void main() {
   texCoords = aTexCoords;

   vec3 scaledPos = vec3(scale * vec4(aPos,1));

   gl_Position = TextProjection * vec4(scaledPos+pos, 1.0);
}