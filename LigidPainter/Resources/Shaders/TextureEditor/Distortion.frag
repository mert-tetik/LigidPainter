#version 400 core

in vec2 TexCoords;

uniform sampler2D txtr;
uniform vec2 txtrResolution;
uniform int distortionIndex;
uniform float fbmRoughness;
uniform int fbmOctaves;
uniform float strength;
uniform float yStrength;
uniform float xStrength;
uniform float effectRadius;
uniform float effectAngle;
uniform float rainScale;
uniform sampler2D distortionTxtr;

out vec4 color;

vec2 random(vec2 uv)
{
    return fract(vec2(dot(uv, vec2(435.6,534.3)), dot(uv, vec2(358.463,246.3))) );
}

float noise(vec2 uv)
{
    
    //get decimal and integer portions from position
    vec2 i = floor(uv);
    vec2 f = fract(uv);
    
    //randomize each position around current position
    vec2 a = random(i);
    vec2 b = random(i + vec2(1.0, 0.0));
    vec2 c = random(i + vec2(0.0, 1.0));
    vec2 d = random(i + vec2(1.0, 1.0));
    
    //interpolate decimal position
    vec2 m = smoothstep(0.0,1.0,f);
    
    //apply interpolation to 4 corners and return result.
    return mix(mix(dot(a,f), dot(b, f-vec2(1.0,0.0)), m.x),
               mix(dot(c,f - vec2(0.0, 1.0)), dot(d,f - vec2(1,1)), m.x), m.y);
}


float fbm(vec2 p, int octaves, float roughness) {
    
    float freq = 1.;
    
    float amplitude = 1.0;
    
    float total = 0.0;
    float maxTotal = 0.0;
    
    for (int i = 0; i < octaves; ++i) {
        total += amplitude * noise(p * freq);
        maxTotal += amplitude;
        
        freq *= 2.0;
        amplitude *= roughness;
    }
    
    return total / maxTotal;
}

vec2 rip(vec2 uv)
{
    return uv + fbm(uv,fbmOctaves,fbmRoughness)*strength;
}

vec4 ripple( vec2 uv )
{
    uv = rip(uv * 10.0)/10.;
    // Time varying pixel color
  	vec4 col = texture(txtr, uv);

    // Output to screen
    return vec4(col);
}

vec4 sinWave(vec2 uv)
{
	
	// Flip that shit, cause shadertool be all "yolo opengl"
	uv.y = -1.0 - uv.y;
	
	// Modify that X coordinate by the sin of y to oscillate back and forth up in this.
	uv.x += sin(uv.y * yStrength) * xStrength;
	
	// Get the pixel color at the index.
	vec4 color = texture(txtr, uv);
	
	return color;
}


vec4 swirl( vec2 uv )
{   
    vec2 center = vec2(0.5);
    center = center == vec2(0., 0.) ? vec2(.5, .5) : center;
    
    float len = length(uv * vec2(txtrResolution.x / txtrResolution.y, 1.));
    float angle = atan(uv.y, uv.x) + effectAngle * smoothstep(effectRadius, 0., len);
    float radius = length(uv);

    return texture(txtr, vec2(radius * cos(angle), radius * sin(angle)) + center);
}

float innerHash(vec3 p)
{
    p  = fract( p*0.3183099+.1 );
	p *= 17.0;
    return fract( p.x*p.y*p.z*(p.x+p.y+p.z) );
}

float innerNoise2D( in vec2 x )
{
    vec3 i = floor(vec3(x,0.));
    vec3 f = fract(vec3(x,0.));
    f = f*f*(3.0-2.0*f);
	
    return mix(mix(mix( innerHash(i+vec3(0,0,0)), 
                        innerHash(i+vec3(1,0,0)),f.x),
                   mix( innerHash(i+vec3(0,1,0)), 
                        innerHash(i+vec3(1,1,0)),f.x),f.y),
               mix(mix( innerHash(i+vec3(0,0,1)), 
                        innerHash(i+vec3(1,0,1)),f.x),
                   mix( innerHash(i+vec3(0,1,1)), 
                        innerHash(i+vec3(1,1,1)),f.x),f.y),f.z);
}

vec4 rain( vec2 uv )
{
	float time = 0.;
	vec3 raintex = vec3(innerNoise2D(vec2(uv.x*2.0,uv.y*0.1+time*0.125) * rainScale)) * strength;
	vec2 where = (uv.xy-raintex.xy);
	vec4 texchur1 = texture(txtr, vec2(where.x,where.y));
	 
	return texchur1;
}

vec3 getNormalTxtr(vec2 uv, float s)
{
    float d = 0.01;
    return normalize(
           vec3(texture(distortionTxtr , uv + vec2(  d, 0.0)).r - texture(distortionTxtr , uv + vec2( -d, 0.0)).r,
                texture(distortionTxtr , uv + vec2(0.0,   d)).r - texture(distortionTxtr , uv + vec2(0.0,  -d)).r,
                d));
}

vec4 txtrDistortion(vec2 uv){
    vec3 normal = getNormalTxtr(uv, 1.);
    vec2 p = uv + strength/4.  * normal.xy;
    return texture(txtr, p);
}

void main(){
    vec2 uv = TexCoords;
    if(distortionIndex == 0)
        color = ripple(uv);
    else if(distortionIndex == 1)
        color = sinWave(uv);
    else if(distortionIndex == 2)
        color = swirl(uv);
    else if(distortionIndex == 3)
        color = rain(uv);
    else if(distortionIndex == 4)
        color = txtrDistortion(uv);
    else 
        color = vec4(0.);
}