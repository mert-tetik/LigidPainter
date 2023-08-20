#version 400 core

in vec2 TexCoords;
in vec3 Pos;

out vec4 fragColor;

#define SIZE 1.0 // size of "pixels" in pixels
#define SPACING 0.005 // size between "pixels" in pixels

uniform vec3 camPos;

void main(){
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = TexCoords;

    // Time varying pixel color
    float m = 0;

    const int gridSize = 1000;

    // dot-by-dot effect
    if (mod(uv.x * gridSize, SIZE + SPACING) < SPACING)
    {
        m = 1.;
    }
    if (mod(uv.y * gridSize, SIZE + SPACING) < SPACING)
    {
        m = 1.;
    }

    float dist = distance(camPos, Pos);

    // Output to screen
    fragColor = vec4(vec3(1),m/4. / (dist/20));
}