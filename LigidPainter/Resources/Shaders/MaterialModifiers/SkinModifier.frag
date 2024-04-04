/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    TODO : Fix skin height

*/

#version 400 core

/* Droplets */
uniform float dropletsCount = 10.;
uniform float dropletsOpacityJitter = 1.;
uniform float dropletsSize = 1.5;

/* Veins */
uniform float veinsScale = 10.;
uniform float veinsStrength = 1.;

/* Blushing */
uniform float blushingStrength = 2.;

/* Skin Prints */
uniform float skinPrintsScale = 10.;
uniform float skinPrintsStrength = 1.;

/* Noise */
uniform float noiseStrength = 2.;

/* Skin Properties*/
uniform vec3 skinColor;
uniform vec3 veinColor;
uniform float skinScale = 10.;
uniform float skinWetness = 1.;
uniform float skinMetallic = 0.;
uniform float skinHeight = 1.; //0 - 2
uniform float skinAmbientOcclusion = 1.0; 

/* Channel Properties*/
uniform int state;
uniform sampler2D mask;
uniform sampler2D previousTxtr;
uniform float opacity;
uniform float depthValue;
uniform sampler2D depthTxtr;
uniform sampler2D selectedPrimitiveIDS;
uniform sampler2D meshMask;
uniform int primitiveCount;
uniform int useMeshMask;

/* Fragment Inputs */
in vec2 TexCoords;
in vec3 Normal;
in vec3 Pos;
in vec3 Tangent;
in vec3 Bitangent;
in vec4 ProjectedPos;

/* Fragment Output*/
out vec4 fragColor;


float hash(in vec3 p)
{ 
    return fract(sin(dot(p.xyz, vec3(12.9898,78.233, 62.485))) * 43758.5453123); 
}

float dotNoise2D(in float x, in float y, in float z, in float fractionalMaxDotSize, in float dDensity)
{
    float integer_x = x - fract(x);
    float fractional_x = x - integer_x;

    float integer_y = y - fract(y);
    float fractional_y = y - integer_y;
    
    float integer_z = z - fract(z);
    float fractional_z = z - integer_z;

    if (hash(vec3(integer_x + 1.0, integer_y + 1.0, integer_z + 1.0)) > dDensity){
        return 0.0;
    }

    float xoffset = (hash(vec3(integer_x, integer_y, integer_z)) -0.5);
    float yoffset = (hash(vec3(integer_x + 1.0, integer_y, integer_z)) - 0.5);
    float zoffset = (hash(vec3(integer_x + 1.0, integer_y + 1.0, integer_z)) - 0.5);
    float dotSize = 0.5 * fractionalMaxDotSize * max(0.25,hash(vec3(integer_x, integer_y+1.0, integer_z + 1.0))); //TODO Change the z axis values

    vec3 truePos = vec3(0.5 + xoffset * (1.0 - 2.0 * dotSize) , 0.5 + yoffset * (1.0 -2.0 * dotSize), 0.5 + zoffset * (1.0 - 2.0 * dotSize));

    float distance = length(truePos - vec3(fractional_x, fractional_y, fractional_z));

    return 1.0 - smoothstep(0.3 * dotSize, 1.0* dotSize, distance);

}

float DotNoise2D(in vec3 coord, in float wavelength, in float fractionalMaxDotSize, in float dDensity)
{
   return dotNoise2D(coord.x/wavelength, coord.y/wavelength, coord.z/wavelength, fractionalMaxDotSize, dDensity);
}






float getDroplets(vec3 uv){
    float splash_speed = 1.;
    float transform = 0.;
    int steps = 5;

    float rain = 0.;
    float base_rate = 6.0 + 3.0 * dropletsCount + 4.0 * (splash_speed - 1.0);
    float base_density = 0.6 * dropletsCount + 0.4 * (splash_speed -1.0);

    float opacityJitter = 1. - dropletsOpacityJitter;

    for(int j = 0; j < steps;j++){

        
        float hash = hash(vec3(float(j),float(j)*1.785, (j)*2.3485));
        float opacity = 1. * hash;
        float opacityGap = opacity - 1.;
        opacity -= opacityGap * opacityJitter;        
        
        for(int i = 0; i < 4;i++){
        
            float time_fact = (sin(base_rate + (1.570*float(i))));
            time_fact = smoothstep(0.0,1.0,time_fact);
            rain += (DotNoise2D(uv.xyz, 0.02 * dropletsSize ,0.5, base_density) * time_fact * (opacity));
        }
    }
    
    return rain;
}




/**
This tab contains all the necessary noise functions required to model a cloud shape.
*/

// Hash by David_Hoskins
#define UI0 1597334673U
#define UI1 3812015801U
#define UI2 uvec2(UI0, UI1)
#define UI3 uvec3(UI0, UI1, 2798796415U)
#define UIF (1.0 / float(0xffffffffU))

vec3 hash33(vec3 p)
{
	uvec3 q = uvec3(ivec3(p)) * UI3;
	q = (q.x ^ q.y ^ q.z)*UI3;
	return -1. + 2. * vec3(q) * UIF;
}

float remap(float x, float a, float b, float c, float d)
{
    return (((x - a) / (b - a)) * (d - c)) + c;
}

// Gradient noise by iq (modified to be tileable)
float gradientNoise(vec3 x, float freq)
{
    // grid
    vec3 p = floor(x);
    vec3 w = fract(x);
    
    // quintic interpolant
    vec3 u = w * w * w * (w * (w * 6. - 15.) + 10.);

    
    // gradients
    vec3 ga = hash33(mod(p + vec3(0., 0., 0.), freq));
    vec3 gb = hash33(mod(p + vec3(1., 0., 0.), freq));
    vec3 gc = hash33(mod(p + vec3(0., 1., 0.), freq));
    vec3 gd = hash33(mod(p + vec3(1., 1., 0.), freq));
    vec3 ge = hash33(mod(p + vec3(0., 0., 1.), freq));
    vec3 gf = hash33(mod(p + vec3(1., 0., 1.), freq));
    vec3 gg = hash33(mod(p + vec3(0., 1., 1.), freq));
    vec3 gh = hash33(mod(p + vec3(1., 1., 1.), freq));
    
    // projections
    float va = dot(ga, w - vec3(0., 0., 0.));
    float vb = dot(gb, w - vec3(1., 0., 0.));
    float vc = dot(gc, w - vec3(0., 1., 0.));
    float vd = dot(gd, w - vec3(1., 1., 0.));
    float ve = dot(ge, w - vec3(0., 0., 1.));
    float vf = dot(gf, w - vec3(1., 0., 1.));
    float vg = dot(gg, w - vec3(0., 1., 1.));
    float vh = dot(gh, w - vec3(1., 1., 1.));
	
    // interpolation
    return va + 
           u.x * (vb - va) + 
           u.y * (vc - va) + 
           u.z * (ve - va) + 
           u.x * u.y * (va - vb - vc + vd) + 
           u.y * u.z * (va - vc - ve + vg) + 
           u.z * u.x * (va - vb - ve + vf) + 
           u.x * u.y * u.z * (-va + vb + vc - vd + ve - vf - vg + vh);
}

// Tileable 3D worley noise
float worleyNoise(vec3 uv, float freq)
{    
    vec3 id = floor(uv);
    vec3 p = fract(uv);
    
    float minDist = 10000.;
    for (float x = -1.; x <= 1.; ++x)
    {
        for(float y = -1.; y <= 1.; ++y)
        {
            for(float z = -1.; z <= 1.; ++z)
            {
                vec3 offset = vec3(x, y, z);
            	vec3 h = hash33(mod(id + offset, vec3(freq))) * .5 + .5;
    			h += offset;
            	vec3 d = p - h;
           		minDist = min(minDist, dot(d, d));
            }
        }
    }
    
    // inverted worley noise
    return 1. - minDist;
}

// Fbm for Perlin noise based on iq's blog
float perlinfbm(vec3 p, float freq, int octaves)
{
    float G = exp2(-.85);
    float amp = 1.;
    float noise = 0.;
    for (int i = 0; i < octaves; ++i)
    {
        noise += amp * gradientNoise(p * freq, freq);
        freq *= 2.;
        amp *= G;
    }
    
    return noise;
}

// Tileable Worley fbm inspired by Andrew Schneider's Real-Time Volumetric Cloudscapes
// chapter in GPU Pro 7.
float worleyFbm(vec3 p, float freq)
{
    return worleyNoise(p*freq, freq) * .625 +
        	 worleyNoise(p*freq*2., freq*2.) * .25 +
        	 worleyNoise(p*freq*4., freq*4.) * .125;
}

float getWorleyNoise( vec3 uv )
{
    vec4 col = vec4(0.);
    
    float slices = 128.; // number of layers of the 3d texture
    float freq = 4.;
    
    float pfbm= mix(1., perlinfbm(vec3(uv), 4., 7), .5);
    pfbm = abs(pfbm * 2. - 1.); // billowy perlin noise
    
    col.g += worleyFbm(vec3(uv), freq);
    col.b += worleyFbm(vec3(uv), freq*2.);
    col.a += worleyFbm(vec3(uv), freq*4.);
    col.r += remap(pfbm, 0., 1., col.g, 1.); // perlin-worley
    
    return 1. - col.z;
}

// returns closest, second closest, and cell id
float getVoronoi( vec3 x )
{
vec3 p1; vec3 p2;

  vec3 p = floor( x );
  vec3 f = fract( x );

  float id = 0.0;
  vec2 res = vec2( 100.0 );
  
  p1 = vec3(1000.);
  p2 = p1;
  
  for( int k=-1; k<=1; k++ )
    for( int j=-1; j<=1; j++ )
      for( int i=-1; i<=1; i++ )
        {
          // b is cell with integer coordinates
          vec3 b = vec3( float(i), float(j), float(k) );
          vec3 r = vec3( b ) - f + hash( p + b );
          float d = dot( r, r );

          if( d < res.x )
            {
              id = dot( p+b, vec3(1.0,57.0,113.0 ) );
              res = vec2( d, res.x );
              p2 = p1;
              p1 = r;
            }
          else if( d < res.y )
            {
              res.y = d;
              p2 = r;
            }
        }

  return res.r;
}

const float PHI = 1000.61803398874989484820459; // Φ = Golden Ratio 

float gold_noise(in vec3 xyz)
{
    return fract(tan(distance(xyz * PHI, xyz)) * xyz.x * PHI);
}

vec3 rotate(vec3 v, vec3 axis, float angle)
{
    float cosAngle = cos(angle);
    float sinAngle = sin(angle);
    float oneMinusCos = 1.0 - cosAngle;

    // Normalize the axis vector
    axis = normalize(axis);

    float x = v.x;
    float y = v.y;
    float z = v.z;

    // Apply the rotation formula
    vec3 rotatedVec;
    rotatedVec.x = (cosAngle + axis.x * axis.x * oneMinusCos) * x + (axis.x * axis.y * oneMinusCos - axis.z * sinAngle) * y + (axis.x * axis.z * oneMinusCos + axis.y * sinAngle) * z;
    rotatedVec.y = (axis.y * axis.x * oneMinusCos + axis.z * sinAngle) * x + (cosAngle + axis.y * axis.y * oneMinusCos) * y + (axis.y * axis.z * oneMinusCos - axis.x * sinAngle) * z;
    rotatedVec.z = (axis.z * axis.x * oneMinusCos - axis.y * sinAngle) * x + (axis.z * axis.y * oneMinusCos + axis.x * sinAngle) * y + (cosAngle + axis.z * axis.z * oneMinusCos) * z;

    return rotatedVec;
}

float antAbs(float val){
    if(val > 0)
        return val * -1;
    else
        return val;
}

float secondBiggestValue(vec3 v)
{
    float maxValue = max(v.x, max(v.y, v.z));
    float minValue = min(v.x, min(v.y, v.z));
    return v.x + v.y + v.z - maxValue - minValue;
}
float maxValue(vec3 v)
{
    return max(max((v.x), (v.y)), (v.z));
}

void main()
{
    if(useMeshMask == 1){
        float prim_txtr_res = int(ceil(sqrt(primitiveCount)));
        float prim_height = floor(float(gl_PrimitiveID) / prim_txtr_res);
        float prim = texelFetch(selectedPrimitiveIDS, ivec2(float(gl_PrimitiveID) - (prim_height * prim_txtr_res) , prim_height), 0).r;
        bool selectedPrim = prim > 0.9 && texture(meshMask, TexCoords).r > 0.5;
        if(!selectedPrim){
            fragColor = vec4(0.);
            return;
        }
    }

    // Normalized pixel coordinates (from 0 to 1)
    vec3 uv = Pos;

    float noise = getWorleyNoise(uv * veinsScale);
    
    float spots = getDroplets(uv);
    
    vec3 veins = vec3(0.); 
    
    vec3 skinColorX = vec3(0);
    
    if(state == 0){
        veins = veinColor;
        skinColorX = skinColor;
    }
    else    
        skinColorX = vec3(0.96, 0.76, 0.61);
    
    uv = rotate(uv,vec3(0,1,0),30.);
    noise = getWorleyNoise(uv * veinsScale);
    vec3 beneathSkin = mix(skinColorX, veins, noise/5. * veinsStrength);
    
    uv = rotate(uv,vec3(0,1,0),40.);
    noise = getWorleyNoise(uv * veinsScale);
    beneathSkin = mix(beneathSkin, veins/1.5,noise/5. * veinsStrength);
    
    uv = rotate(uv,vec3(0,1,0),60.);
    noise = getWorleyNoise(uv * veinsScale);
    beneathSkin = mix(beneathSkin, veins/3.,noise/5. * veinsStrength);
    
    uv = rotate(uv,vec3(0,1,0),47.);
    noise = getWorleyNoise(uv * veinsScale);
    beneathSkin = mix(beneathSkin, veins/4.,noise/5. * veinsStrength);
    
    float voronoi = getVoronoi(vec3(uv * skinPrintsScale)) * skinPrintsStrength;
    
    vec3 frackledSkin = mix(beneathSkin, veins, abs(spots / 5.)); 
    
    vec3 voronoidSkin = mix(frackledSkin, frackledSkin/1.5, voronoi/3.); 
    
    voronoidSkin = mix(voronoidSkin, beneathSkin, 0.4);    
    
    vec3 noisedSkin = mix(voronoidSkin, voronoidSkin/1.1, gold_noise(uv) * noiseStrength);
    
    float voronoi2 = getVoronoi(vec3(uv));

    // Albedo
    fragColor = vec4(mix(noisedSkin, vec3(0.67, 0.25, 0.6) * skinColorX, voronoi2/10. * blushingStrength) ,1.);

    fragColor.rgb = mix(frackledSkin, frackledSkin/1.5, voronoi/2.);

    //Roughness
    if(state == 1){
        fragColor.r /= skinWetness;
        fragColor.rgb = vec3(fragColor.r);
    }
    
    //Metallic
    else if(state == 2){
        fragColor.rgb = vec3(1. - fragColor.r) * skinMetallic;
    }
    
    //Normal Map
    else if(state == 3){
        fragColor.rgb = vec3(fragColor.r);
    }
    
    //Height Map
    else if(state == 4){
        fragColor.rgb = vec3((fragColor.r - 0.25) * skinHeight);
    }
    
    //Ambient Occlusion
    else if(state == 5){
        float ao = 1. - skinAmbientOcclusion;
            
        if(fragColor.r < ao)
            fragColor.r -= ao - fragColor.r;

        fragColor.rgb = vec3((fragColor.r + 0.1));

    }
    
    float alpha = opacity;
    alpha *= texture(mask, TexCoords).r; 

    float dpth = texture(depthTxtr, TexCoords).r; 
    if(depthValue < dpth)
        alpha *= 0;

    vec3 clrResult = mix(texture(previousTxtr, TexCoords).rgb, fragColor.rgb, alpha);

    fragColor = vec4(clrResult, 1.);
}