#version 400 core

#define MAX_OBJECT_COUNT 200
uniform vec2 objIndices[MAX_OBJECT_COUNT];

uniform int objIndicesArraySize;

out vec4 fragColor;

void main(){
    int val = 0;
    for(int i = 0; i < min(objIndicesArraySize, MAX_OBJECT_COUNT); i++) {
        if(gl_PrimitiveID > objIndices[i].x && gl_PrimitiveID <  objIndices[i].y){
            val = i;
        }
    }

    fragColor = vec4(1);
    fragColor.r = val;
}