#version 330 core

in vec2 texCoords;

out vec4 outColor;

uniform float val;


vec3 HUEToRGB(float H) {
  float R = abs(H * 6.0 - 3.0) - 1.0;
  float G = 2.0 - abs(H * 6.0 - 2.0);
  float B = 2.0 - abs(H * 6.0 - 4.0);
  return clamp(vec3(R,G,B),0.0,1.0);
}

vec3 HSVToRGB(vec3 HSV) {
  vec3 RGB = HUEToRGB(HSV.x);
  return ((RGB - 1.0) * HSV.y + 1.0) * HSV.z;
}

vec2 toPolar(vec2 cartesian) {
    return vec2(length(cartesian), atan(cartesian.x, cartesian.y));
}

void main( )
{
    float t = 1 / 3.0;
    vec3 color = vec3(mod(t, 1.0), 0, val);
    float epsilon = 0.05;
    vec3 markerColor = vec3(1.0);
	vec2 uv = vec2(texCoords.x+0.4,texCoords.y);
    uv.x -= 0.40;
    vec2 p = toPolar(uv - 0.5);
    float angle = 1.0 - abs(p.y / 3.14);
    outColor.a = 1.;
    if (p.x < 0.4) {
        // Draw color
        outColor = vec4(0.0);
    } 
    else if (p.x < 0.45) {
        if(color.z > 1.0){
            color.z -= 1.0;
            color.z = 1.0 - color.z;
            
            outColor.rgba = p.y < 0. && color.z > angle - epsilon && color.z < angle + epsilon 
                ? vec4(markerColor,1)
                : vec4(vec3(0.05),0.5);
        }
        else{
            outColor.rgba = p.y > 0. && color.z > angle - epsilon && color.z < angle + epsilon 
                ? vec4(markerColor,1)
                : vec4(vec3(0.05),0.5);
        }    
    } 
    else {
        // Draw background
		outColor = vec4(0.0);
    }
}