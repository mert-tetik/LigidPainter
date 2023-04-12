#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 TextProjection;

out vec2 TexCoords;
uniform vec2 transform;

void main() {
    TexCoords = aTexCoords;
    int id = gl_InstanceID;
    vec2 pos;
    pos.x = id*100 - (floor((id) / 20.)*(100.*20));
        
    pos.y = floor((id) / 20.)*100.;

    vec2 trans = transform;

    trans.x -= floor(trans.x / 100)*100;
    trans.y -= floor(trans.y / 100)*100;
    
    pos+=trans;

    gl_Position = TextProjection * vec4(aPos + vec3(pos,0), 1.0);
}