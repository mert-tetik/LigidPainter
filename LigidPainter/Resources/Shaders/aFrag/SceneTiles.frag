#version 400 core

in vec3 Pos;

out vec4 fragColor;

uniform vec3 camPos;

void main(){

    // Distance between the line's fragment and the camera position
    float dist = distance(vec3(0.), Pos);

    // Output to screen
    fragColor = vec4(vec3(0.7), ((100 - min(dist, 100.)) / 100.) / (dist / 20.) / 4.);
}