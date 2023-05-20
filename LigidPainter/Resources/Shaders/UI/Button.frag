//Most of the GUI elements is rendered here
//Such as buttons, range bars, checkbox etc.

#version 330 core

out vec4 fragColor;

in vec2 texCoords;
//in vec3 fragPos;

uniform vec4 color;
uniform int outline;
uniform float thickness;
uniform float radius;

uniform float width;
uniform float height;

float udRoundBox( vec2 p, vec2 b, float r )
{
    return length(max(abs(p)-b+r,0.0))-r;
}

float roundUp(vec2 uv) //! https://www.shadertoy.com/view/ldfSDj
{
   float divider = 1.0;
    // setup
    if(outline == 1)
        divider = 1.15;

    float iRadius = radius;
    vec2 halfRes = vec2(0.5*vec2(1920*width,1080*height));

    // compute box
    float b = udRoundBox( uv.xy*vec2(1920*width,1080*height) * vec2(1.05,divider) - halfRes * vec2(1.05,divider), halfRes, iRadius );
    
    // colorize (red / black )
	vec3 c = mix( vec3(1.0,0.0,0.0), vec3(0.0,0.0,0.0), smoothstep(0.0,1.0,b) );
   
   vec3 circleColor = vec3(1.0, 1.0, 1.0);
   float fade = 3.115;
   
   vec3 color = vec3(smoothstep(0.0, fade, b));
   color *= vec3(smoothstep(thickness + fade, thickness, b)) * circleColor;
   
   if(outline == 1)
      return color.r;
   else
      return c.r;
}

void main(){
    fragColor = vec4(color);
    if(radius != 0)
        fragColor.a = roundUp(texCoords);
}