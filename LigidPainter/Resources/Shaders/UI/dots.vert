#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 TextProjection;

out vec2 TexCoords;
uniform vec2 transform;
uniform float zoom;

void main() {
    TexCoords = aTexCoords;
    int id = gl_InstanceID;
    vec2 pos;

    float z = 1;

    if((zoom > 0.1 && zoom < 0.2) || (zoom > 0.3 && zoom < 0.4) || (zoom > 0.5 && zoom < 0.6) || (zoom > 0.7 && zoom < 0.8) || (zoom > 0.9 && zoom < 1.0) || (zoom > 1.1 && zoom < 1.2) || (zoom > 1.3 && zoom < 1.4) || (zoom > 1.5 && zoom < 1.6) || (zoom > 1.7 && zoom < 1.8) || (zoom > 1.9 && zoom < 2.))
        z = 1.1;

    pos.x = id*(100.*z) - (floor((id) / 20.)*((100.*z)*20));
        
    pos.y = floor((id) / 20.)*(100.*z);

    vec2 trans = transform;

    trans.x -= floor(trans.x / (100.*z))*(100.*z);
    trans.y -= floor(trans.y / (100.*z))*(100.*z);
    
    pos+=trans;

    gl_Position = TextProjection * vec4(aPos + vec3(pos,0), 1.0);
}