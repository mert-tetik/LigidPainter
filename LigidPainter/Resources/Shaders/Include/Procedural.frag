/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
    *Include that file using : #pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Procedural.frag)
*/

#define PI 3.141592

/* --------------------------------------------------------- PATTERN FUNCTIONS --------------------------------------------------------- */

#define RandomSign sign(cos(1234.*cos(h.x+9.*h.y)));  // random -1 or 1

/* 0
---------------------------------------------------------
    Returns antialiased hexagonal grid color
---------------------------------------------------------
*/
float hexagonalPattern(
                        vec2 p
                    )
{
    float gridSize = 0.8;
    float gridThickness = 0.2;

    vec2 pos = p / gridSize;
    pos.x *= 0.57735 * 2.0;
    pos.y += 0.5 * mod(floor(pos.x), 2.0);
    pos = abs(fract(pos) - 0.5);
    
    float d = abs(max(pos.x*1.5 + pos.y, pos.y*2.0) - 1.0);
    return smoothstep(0.0, gridThickness, d);
}

/* 1
---------------------------------------------------------
    Returns antialiased hexagonal grid color
---------------------------------------------------------
*/
float hexagonalPattern2(
                        vec2 p
	                 )
{
    
    float gridSize = 0.8;
    float gridThickness = 0.2;

    vec2 pos = p / gridSize;
    pos.x *= 1.1;
    pos.y += 0.5 * mod(floor(pos.x), 2.0);
    pos = abs(fract(pos) - 0.5);
    
    float d = abs(max(pos.x*2.5 + pos.y, pos.y*3.0) - 1.0);
    return smoothstep(0.30, gridThickness, d);
}
/* 2
---------------------------------------------------------
    Returns hexagonal grid pattern with 3 colors
---------------------------------------------------------
*/
float hexagonalPattern3(
                            vec2 p
                        )
{
    p*=4.;
    p.y = p.y * 0.866 + p.x*0.5;
    p = mod(p, vec2(3.0));

    if(
        p.y < p.x+1.0 && p.y > 0.0 && p.x > 0.0
        && 
        p.y > p.x-1.0 && p.x < 2.0 && p.y < 2.0
    )
        return 0.0;
    
    else if(p.y > 1.0 && (p.y < p.x || p.x < 1.0))
        return 0.5;
    
    return 1.0;
}

/* 3
---------------------------------------------------------
    https://www.shadertoy.com/view/Xdt3D8 by FabriceNeyret2
---------------------------------------------------------
*/
float hexagonalTruchetPattern(
                                vec2 p
                            )
{
    p*=4.;
    vec2 h = p + vec2(0.58, 0.15)*p.y;
    vec2 f = fract(h);
    h -= f;
    float v = fract((h.x + h.y) / 3.0);
    (v < 0.6) ? (v < 0.3) ?  h : h++ : h += step(f.yx,f);
    p += vec2(0.5, 0.13)*h.y - h;        // -1/2, sqrt(3)/2
    v = RandomSign;
    return 0.06 / abs(0.5 - min (min
      (length(p - v*vec2(-1., 0.00)  ),  // closest neighbor (even or odd set, dep. s)
       length(p - v*vec2(0.5, 0.87)) ),  // 1/2, sqrt(3)/2
       length(p - v*vec2(0.5,-0.87))));
}

/* 4
---------------------------------------------------------
    Renders a checker texture - repeated squares
---------------------------------------------------------
*/
float checkerPattern(
                        vec2 p
                    )  
{
    p = 0.5 - fract(p);
    return 0.5 + 0.5*sign(p.x*p.y);
}

/* 5
---------------------------------------------------------
    Renders repeated triangles
---------------------------------------------------------
*/
float trianglePattern(
                        vec2 p
                    )  
{
    p.y = p.y * 0.866 + p.x * 0.5;
    if(fract(p.y) > fract(p.x)) return 1.0;
    return 0.0;
}

/* 6
---------------------------------------------------------
    Renders repeated rhombs with different colored edges
---------------------------------------------------------
*/
float rhombPattern1(
                        vec2 p
                    ) 
{
    p.y = p.y * 0.866 + p.x * 0.5;
    float t = fract(p.y) - fract(p.x);
    return smoothstep(.50, 0.0, t*t);
}

/* 7
---------------------------------------------------------
    Renders checker board pattern with AA
---------------------------------------------------------
*/
float karoPattern(
                    vec2 p
                )
{
    return 0.5 * clamp(10. * sin(PI * p.x), 0.0, 1.0) + 0.5 * clamp(10. * sin(PI * p.y), 0.0, 1.0);
}

/* 8
---------------------------------------------------------
    Returns grid pattern with AA
---------------------------------------------------------
*/
float gridPattern1(
                    vec2 p
                )
{
    float col = max(sin(p.x*10.1), sin(p.y*10.1));
    return smoothstep(0.5,1.,col);
}

/* 9
---------------------------------------------------------
    Returns grid pattern with AA
---------------------------------------------------------
*/
float gridPattern2(
                    vec2 p
                )
{
  return 0.5*clamp(10.*sin(PI*p.x), 0.0, 1.0)
       / 0.5*clamp(10.*sin(PI*p.y), 0.0, 1.0);
}

/* 10
---------------------------------------------------------
    Returns rounded square holes grid with AA
---------------------------------------------------------
*/
float squareHolePattern(
                            vec2 p
                        )
{
    float thickness = 0.8;

    float t = cos(p.x*2.0) * cos(p.y*2.0) / thickness;
    return smoothstep(0.1, 0.0, t*t);
}

/* 11
---------------------------------------------------------
    Returns rounded square holes grid with AA
---------------------------------------------------------
*/
float roundSquarePattern(
                            vec2 p
                        )
{
    float thickness = 0.8;
    float t = cos(p.x*2.0) * cos(p.y*2.0) / thickness;
    return smoothstep(0.1, 0.0, t*t);
}

/* 12
---------------------------------------------------------
    Returns sinus wave pattern
---------------------------------------------------------
*/
float sinePattern(
                    vec2 p
                )
{
    return sin(p.x * 20.0 + cos(p.y * 12.0));
}

/* 13
---------------------------------------------------------
    Returns a brick wall texture
-------------------------------------------------------
*/
float brickPattern(
                    vec2 p
                )
{
    const float vSize = 0.10;
    const float hSize = 0.05;
    
    p.y *= 2.5;    // scale y
    
    if(mod(p.y, 2.0) < 1.0) 
        p.x += 0.5;
    
    p = p - floor(p);
    
    if((p.x+hSize) > 1.0 || (p.y < vSize)) 
        return 1.0;
    
    return 0.0;
}

/* 14
---------------------------------------------------------
    Returns pattern of gears
---------------------------------------------------------
*/
float gearPattern(
                    vec2 uv
                )
{
    float wn = 1.5;
    int tn = 12;

    float g = (step(1.0, uv.x * wn) - 0.5);
    uv = fract(uv * wn) - 0.5;
    float r = clamp(0.48, 0.4, 0.45 + 0.12*sin(atan(uv.x,uv.y) * float(tn) + g));
    return smoothstep(r, r + 0.01, 1.1*length(uv));
}

/* 15
---------------------------------------------------------
    Returns repeating layers of circles 
---------------------------------------------------------
*/
float lengthN(in vec2 v, in float n)
{
  return pow(pow(abs(v.x), n)+pow(abs(v.y), n), 0.89/n);
}
float layeredCirclePattern(
                            vec2 p
                        )
{
    vec2 p2 = mod(p * 8.0, 4.0) - 2.0;
    return sin(lengthN(p2, 4.0) * 16.0);
}

/* 16
---------------------------------------------------------
    https://www.shadertoy.com/view/4sKXzy by aiekick
---------------------------------------------------------
*/
float starPattern(
                    vec2 p
                )
{
  p = abs(fract(p*1.5)-0.5);
  return max(max(p.x, p.y), min(p.x, p.y)*2.);
}

/* 17
---------------------------------------------------------
    https://www.shadertoy.com/view/llfyDn by FabriceNeyret2
---------------------------------------------------------
*/
#define S1(x,y) abs(fract(x))<0.8 ? 0.65 +0.35* sin(3.1415*(y-ceil(x))) : 0.0
#define S2(x,y) abs(fract(x))<0.8 ? 0.65 +0.35* sin(1.5707*(y-ceil(x))) : 0.0
float basketPattern1(
                        vec2 uv
                    )
{
  vec2 p = uv * 4.0;
  return max (S1(p.x, p.y), S1(p.y+1.0, p.x));
}

/* 18
---------------------------------------------------------
    https://www.shadertoy.com/view/ltXcDn by FabriceNeyret2
---------------------------------------------------------
*/
float basketPattern2(
                        vec2 uv
                    )
{
  vec2 p = uv * 4.0;
  return max (S2( p.x, p.y), S2(p.y, p.x+1.) );
}

/* 19
---------------------------------------------------------
    https://www.shadertoy.com/view/lsVczV by FabriceNeyret2
---------------------------------------------------------
*/
float diamondPattern(
                        vec2 p
                    )
{
    vec2 dp = abs (fract(p * 2.) - 0.5);
    return 0.3 - cos (19. * max(dp.x, dp.y));
}

/* 20
---------------------------------------------------------
    https://www.shadertoy.com/view/4lGyz3 by FabriceNeyret2
---------------------------------------------------------
*/
#define D(U) .004/abs(length(mod(U,d+d)-d)-d.x)
float rosettePattern(
                        vec2 p
                    )
{
    vec2 d = vec2(0.58,1);
    
    vec4 O = vec4(0);
    
    for ( ; O.a++ < 4.; O += D(p) +D(p += d*.5)) 
        p.x += d.x;
    
    return O.x;
}

/* 21
---------------------------------------------------------
    Returns smooth repeating hexagons
---------------------------------------------------------
*/
float smoothHexPattern(
                        vec2 p
                    )
{
    p.x*=sqrt(3.)*2./3.;
    p.y+= 0.5 * mod(floor(p.x), 2.0);
    p = abs(fract(p)-.5);
    return abs(max(p.x*1.5+p.y,p.y*2.)-1.);
}

/* 22
---------------------------------------------------------
    Returns repeating smooth and sharp corner squares
---------------------------------------------------------
*/
float mi(vec2 a){return min(a.x,a.y);}
float smoothSharpSquarePattern(
                                vec2 p
                            )
{
    return mi(abs(fract(p)-.5));
}

/* 23
---------------------------------------------------------
    Returns repeating triangles with random colors
    From : https://www.shadertoy.com/view/Mtj3Rh
---------------------------------------------------------
*/
// Helper function to generate a random value between 0 and 1 using sine and fract
float random(float n)
{
    return fract(abs(sin(n * 55.753) * 367.34));
}
// Helper function to generate a random value between 0 and 1 for a 2D vector
float random(vec2 n)
{
    return random(dot(n, vec2(2.46, -1.21)));
}
// Helper function to create a cyclical pattern based on the input value
float cycle(float n)
{
    return cos(fract(n) * 2.0 * 3.141592653) * 0.5 + 0.5;
}
float randomTrianglePattern(
                                vec2 uv
                            )
{
    // Angle in radians for the rotation
    float angle = radians(60.0);

    // Zoom level
    float zoom = 0.2;

    // Apply rotation and zoom to the current fragment coordinate
    vec2 c = (uv + vec2(zoom, 0.0)) * vec2(sin(angle), 1.0);
    c = ((c + vec2(c.y, 0.0) * cos(angle)) / zoom) + vec2(floor(4.0 * (c.x - c.y * cos(angle)) / zoom), 0.0);

    // Combine random values with the current time to create a cyclical pattern
    return cycle(random(floor(c * 4.0)) * 0.5 );
}

/* 24
---------------------------------------------------------
    Returns solid white
---------------------------------------------------------
*/
float solidPattern(){
    return 1.;
}





/* --------------------------------------------------------- NOISE FUNCTIONS ---------------------------------------------------------*/




//https://www.shadertoy.com/view/4sfGzS
float innerHash(vec3 p)  // replace this by something better
{
    p  = fract( p*0.3183099+.1 );
	p *= 17.0;
    return fract( p.x*p.y*p.z*(p.x+p.y+p.z) );
}
//TODO REPLACE NOISE FUNCTIONS WITH THAT

float innerNoise2D( in vec3 x )
{
    vec3 i = floor(x);
    vec3 f = fract(x);
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

float innerFbm(vec3 p, int octaves, float roughness) {
    
    float freq = 1.;
    
    float amplitude = 1.0;
    
    float total = 0.0;
    float maxTotal = 0.0;
    
    for (int i = 0; i < octaves; ++i) {
        total += amplitude * innerNoise2D(p * freq);
        maxTotal += amplitude;
        
        freq *= 2.0;
        amplitude *= roughness;
    }
    
    return total / maxTotal;
}

float innerFbmX(vec3 p, int maxOctaves, float persistance) {
    vec3 noise = vec3(innerNoise2D(p + vec3(0.)), innerNoise2D(p + vec3(1.)), innerNoise2D(p + vec3(2.)));
    p += noise;
    return innerFbm(p, maxOctaves, persistance);
}

float innerMusgrave(vec3 p, float octaves, float dimension, float lacunarity) {
    float sum = 0.0;
    float amp = 1.0;
    float m = pow(lacunarity, -dimension);
    
    while (octaves-- > 0.0) {
        float n = innerNoise2D(p) * 2.0 - 1.0;
        sum += n * amp;
        amp *= m * 1.;
        p *= lacunarity;
    }
    
    return sum;
}



// Hash by David_Hoskins
#define UI0 1597334673U
#define UI1 3812015801U
#define UI2 uvec2(UI0, UI1)
#define UI3 uvec3(UI0, UI1, 2798796415U)
#define UIF (1.0 / float(0xffffffffU))
vec3 innerHash33(vec3 p)
{
	uvec3 q = uvec3(ivec3(p)) * UI3;
	q = (q.x ^ q.y ^ q.z)*UI3;
	return -1. + 2. * vec3(q) * UIF;
}

// Gradient noise by iq (modified to be tileable)
float innerGradientNoise(vec3 x, float freq)
{
    // grid
    vec3 p = floor(x);
    vec3 w = fract(x);
    
    // quintic interpolant
    vec3 u = w * w * w * (w * (w * 6. - 15.) + 10.);

    
    // gradients
    vec3 ga = innerHash33(mod(p + vec3(0., 0., 0.), freq));
    vec3 gb = innerHash33(mod(p + vec3(1., 0., 0.), freq));
    vec3 gc = innerHash33(mod(p + vec3(0., 1., 0.), freq));
    vec3 gd = innerHash33(mod(p + vec3(1., 1., 0.), freq));
    vec3 ge = innerHash33(mod(p + vec3(0., 0., 1.), freq));
    vec3 gf = innerHash33(mod(p + vec3(1., 0., 1.), freq));
    vec3 gg = innerHash33(mod(p + vec3(0., 1., 1.), freq));
    vec3 gh = innerHash33(mod(p + vec3(1., 1., 1.), freq));
    
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
float innerWorleyNoise(vec3 uv, float freq)
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
            	vec3 h = innerHash33(mod(id + offset, vec3(freq))) * .5 + .5;
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
float innerPerlinfbm(vec3 p, float freq, int octaves)
{
    float G = exp2(-.85);
    float amp = 1.;
    float noise = 0.;
    for (int i = 0; i < octaves; ++i)
    {
        noise += amp * innerGradientNoise(p * freq, freq);
        freq *= 2.;
        amp *= G;
    }
    
    return noise;
}

// Tileable Worley innerFbm inspired by Andrew Schneider's Real-Time Volumetric Cloudscapes
// chapter in GPU Pro 7.
float innerWorleyFbm(vec3 p, float freq)
{
    return innerWorleyNoise(p*freq, freq) * .625 +
        	 innerWorleyNoise(p*freq*2., freq*2.) * .25 +
        	 innerWorleyNoise(p*freq*4., freq*4.) * .125;
}

//0
float basicNoiseA(vec3 uv){
    return innerNoise2D(uv);
}

//1
float basicStretchedNoiseA(vec3 uv){
    return innerNoise2D(vec3(uv.x, uv.y*5., uv.z));
}

//2
float fbmNoiseA(vec3 uv){
    int maxOctaves = 8;
    float persistance = 0.5;
    return innerFbm(uv, maxOctaves, persistance);
} 

//3
float fbmHightPersistanceNoiseA(vec3 uv){
    int maxOctaves = 8;
    float persistance = 1.;
    return innerFbm(uv, maxOctaves, persistance);
} 

//4
float fbmLowOctaveNoiseA(vec3 uv){
    int maxOctaves = 2;
    float persistance = 0.5;
    return innerFbm(uv, maxOctaves, persistance);
} 

//5
float fbmWaveNoiseA(vec3 uv){
    int maxOctaves = 8;
    float persistance = 0.5;
    return innerFbmX(uv, maxOctaves, persistance);
}

//6
float fbmWaveLowPersistanceNoiseA(vec3 uv){
    int maxOctaves = 8;
    float persistance = 0.2;
    return innerFbmX(uv, maxOctaves, persistance);
}

//7
float fbmWaveHighPersistanceNoiseA(vec3 uv){
    int maxOctaves = 8;
    float persistance = 1.;
    return innerFbmX(uv, maxOctaves, persistance);
}

//8
float musgraveDefNoiseA(vec3 uv){
    //octave, dimension, lacunarity
    return innerMusgrave(uv, 8., 0. ,2.5);
}

//9
float musgraveHighDimensionalNoiseA(vec3 uv){
    //octave, dimension, lacunarity
    return innerMusgrave(uv, 8., 1. ,2.5);
}

//10
float gradientDefNoiseA(vec3 uv){
    return innerGradientNoise(uv, 20.);
}

//11
float innerWorleyNoiseA(vec3 uv){
    return innerWorleyFbm(uv / 10., 20.);
}

//12
float worleyPatternalNoiseA(vec3 uv){
    return innerWorleyFbm(uv / 2. , 2.);
}

//13
float circularPerlinNoiseA(vec3 uv){
    return innerPerlinfbm(uv / 4. , 20., 8);
}

//14
float voronoiNoiseA(vec3 uv){
    return innerWorleyNoise(uv, 2.);
}

//15
float voronoiDeeperNoiseA(vec3 uv){
    return innerWorleyNoise(uv, 20.);
}

//16
float staticNoiseA(vec3 uv){
    const float PHI = 1000.61803398874989484820459; // Φ = Golden Ratio 
    return fract(tan(distance(uv * PHI, uv)) * uv.x * PHI);
}

float getProcedural(vec2 p, vec3 pos, int proceduralID, float scale, int inverted){
    
    p *= scale;
    pos = normalize((pos));
    pos *= scale;
    
    float res = 0.;

    if(proceduralID == 0)
        res = hexagonalPattern(p);
    else if(proceduralID == 1)
        res = hexagonalPattern2(p);
    else if(proceduralID == 2)
        res = hexagonalPattern3(p);
    else if(proceduralID == 3)
        res = hexagonalTruchetPattern(p);
    else if(proceduralID == 4)
        res = checkerPattern(p);
    else if(proceduralID == 5)
        res = trianglePattern(p);
    else if(proceduralID == 6)
        res = rhombPattern1(p);
    else if(proceduralID == 7)
        res = karoPattern(p);
    else if(proceduralID == 8)
        res = gridPattern1(p);
    else if(proceduralID == 9)
        res = gridPattern2(p);
    else if(proceduralID == 10)
        res = squareHolePattern(p);
    else if(proceduralID == 11)
        res = roundSquarePattern(p);
    else if(proceduralID == 12)
        res = sinePattern(p);
    else if(proceduralID == 13)
        res = brickPattern(p);
    else if(proceduralID == 14)
        res = gearPattern(p);
    else if(proceduralID == 15)
        res = layeredCirclePattern(p);
    else if(proceduralID == 16)
        res = starPattern(p);
    else if(proceduralID == 17)
        res = basketPattern1(p);
    else if(proceduralID == 18)
        res = basketPattern2(p);
    else if(proceduralID == 19)
        res = diamondPattern(p);
    else if(proceduralID == 20)
        res = rosettePattern(p);
    else if(proceduralID == 21)
        res = smoothHexPattern(p);
    else if(proceduralID == 22)
        res = smoothSharpSquarePattern(p);
    else if(proceduralID == 23)
        res = randomTrianglePattern(p);
    else if(proceduralID == 24)
        res = solidPattern();
    else if(proceduralID == 25)
        res = basicNoiseA(pos);
    else if(proceduralID == 26)
        res = basicStretchedNoiseA(pos);
    else if(proceduralID == 27)
        res = fbmNoiseA(pos);
    else if(proceduralID == 28)
        res = fbmHightPersistanceNoiseA(pos);
    else if(proceduralID == 29)
        res = fbmLowOctaveNoiseA(pos);
    else if(proceduralID == 30)
        res = fbmWaveNoiseA(pos);
    else if(proceduralID == 31)
        res = fbmWaveLowPersistanceNoiseA(pos);
    else if(proceduralID == 32)
        res = fbmWaveHighPersistanceNoiseA(pos);
    else if(proceduralID == 33)
        res = musgraveDefNoiseA(pos);
    else if(proceduralID == 34)
        res = musgraveHighDimensionalNoiseA(pos);
    else if(proceduralID == 35)
        res = gradientDefNoiseA(pos);
    else if(proceduralID == 36)
        res = innerWorleyNoiseA(pos);
    else if(proceduralID == 37)
        res = worleyPatternalNoiseA(pos);
    else if(proceduralID == 38)
        res = circularPerlinNoiseA(pos);
    else if(proceduralID == 39)
        res = voronoiNoiseA(pos);
    else if(proceduralID == 40)
        res = voronoiDeeperNoiseA(pos);
    else if(proceduralID == 41)
        res = staticNoiseA(pos);
    else
        res = 1.;

    if(res > 1.)
        res = 1.;
    
    if(res < 0.)
        res = 0.;

    if(inverted == 0)
        return abs(res);   
    else
        return abs(1. - res);   
}