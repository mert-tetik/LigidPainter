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
    
    float d = abs(max(pos.x * 1.5 + pos.y, pos.y*2.0) - 1.0);
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


mat2 brickm = mat2( 0.80,  0.60, -0.60,  0.80 );
float brickHash( float n )
{
	return fract(sin(n)*43758.5453);
}
float brickNoise( in vec2 x )
{
	vec2 p = floor(x);
	vec2 f = fract(x);
    	f = f*f*(3.0-2.0*f);
    	float n = p.x + p.y*57.0;
    	float res = mix(mix( brickHash(n+  0.0), brickHash(n+  1.0),f.x), mix( brickHash(n+ 57.0), brickHash(n+ 58.0),f.x),f.y);
    	return res;
}
float brickFbm( vec2 p )
{
    	float f = 0.0;
    	f += 0.50000*brickNoise( p ); p = brickm*p*2.02;
    	f += 0.25000*brickNoise( p ); p = brickm*p*2.03;
    	f += 0.12500*brickNoise( p ); p = brickm*p*2.01;
    	f += 0.06250*brickNoise( p ); p = brickm*p*2.04;
    	f += 0.03125*brickNoise( p );
    	return f/0.984375;
}
float brickGetBricks(vec2 pos, int style) 
{
	vec2 p = pos;
	vec2 offset = vec2(0.0, 1.0);
	float rowX = floor((pos.y) / 1.0);
	//if (mod(rowX, 2.0) < 1.0)
	//	offset.x = 0.5 + fbm(p*0.3);
	
	float rowY = floor((pos.y)/1.0);
    
    if(style == 1){
        if (mod(rowY, 2.0) < 1.0) {
            offset.y = 2.0;
            offset.x = 0.75 * brickFbm(p*0.5);
            rowX = floor((pos.y)/0.5);
            if (mod(rowX, 2.0) < 1.0)
                offset.x += 0.25 + brickFbm(p*0.3);
        }
    }
    else if (style == 2 || style == 3){
        offset.y = 2.0;
        offset.x = 0.75 * brickFbm(p*0.5);
        rowX = floor((pos.y)/0.5);
        if (mod(rowX, 2.0) < 1.0)
            offset.x += 0.25 + brickFbm(p*0.3);
    }
    
	p.x += offset.x;
	float n1 = brickFbm(pos * 5.0);
	pos.x=fract(pos.x + offset.x * brickFbm(p*0.9) +.5)-0.5;
	pos.y=fract(pos.y * offset.y +.5)-0.5;
	pos = abs(pos);
   	float a = atan(pos.y, pos.x);
	float b = atan(pos.x, pos.y);
	float n2 = brickFbm(pos) * (a*b);
	float n3 = n1 * 0.15 / n2 * .75;
	float s = min(pos.x,pos.y) - n3;
    if(style == 3)
        return 1.-n3;
    else 
        return mix(s,1.-n3,0.5);
}

//25
float brickPattern1(vec2 p){
    return brickGetBricks(p, 0) + 0.4;
}

//26
float brickPattern2(vec2 p){
    return brickGetBricks(p, 1) + 0.4;
}

//27
float brickPattern3(vec2 p){
    return brickGetBricks(p, 2) + 0.4;
}

//28
float brickPattern4(vec2 p){
    return brickGetBricks(p, 3) + 0.4;
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

vec3 innerHash33_2(vec3 p)
{
    vec3 p3 = fract(p * 0.1031);
    p3 += dot(p3, p3.yzx + 19.19);
    return fract(vec3((p3.x + p3.y) * p3.z, (p3.y + p3.z) * p3.x, (p3.z + p3.x) * p3.y));
}

vec3 innerHash33_1(vec3 p)
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
    vec3 ga = innerHash33_1(mod(p + vec3(0., 0., 0.), freq));
    vec3 gb = innerHash33_1(mod(p + vec3(1., 0., 0.), freq));
    vec3 gc = innerHash33_1(mod(p + vec3(0., 1., 0.), freq));
    vec3 gd = innerHash33_1(mod(p + vec3(1., 1., 0.), freq));
    vec3 ge = innerHash33_1(mod(p + vec3(0., 0., 1.), freq));
    vec3 gf = innerHash33_1(mod(p + vec3(1., 0., 1.), freq));
    vec3 gg = innerHash33_1(mod(p + vec3(0., 1., 1.), freq));
    vec3 gh = innerHash33_1(mod(p + vec3(1., 1., 1.), freq));
    
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
            	vec3 h = innerHash33_1(mod(id + offset, vec3(freq))) * .5 + .5;
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


// 2D 2nd-order Voronoi: Obviously, this is just a rehash of IQ's original. I've tidied
// up those if-statements. Since there's less writing, it should go faster. That's how 
// it works, right? :)
//
float innerVoronoi(in vec3 p){
    
	vec3 g = floor(p), o; p -= g;
	
	vec3 d = vec3(1); // 1.4, etc. "d.z" holds the distance comparison value.
    
	for(int y = -1; y <= 1; y++){
		for(int x = -1; x <= 1; x++){
            for(int z = -1; z <= 1; z++){
                o = vec3(x, y, z);
                o += innerHash33_2(g + o) - p;
            
                d.z = dot(o, o);            
                d.y = max(d.x, min(d.y, d.z));
                d.x = min(d.x, d.z); 
            }
		}
	}
	
	
    return d.y - d.x;
    // return d.x;
    // return max(d.y*.91 - d.x*1.1, 0.)/.91;
    // return sqrt(d.y) - sqrt(d.x); // etc.
}

float innerGetWall(vec3 p, int style){
        
        vec3 tx = vec3(0.); 
        tx *= tx;
        tx = smoothstep(0., .5, tx);
        
        if(style == 4 || style == 5)
            p += sin(p*6.283*2. - cos(p.yxz*6.283*4.))*.04;
        else
            p += sin(p*6.283*2. - cos(p.yxz*6.283*4.))*.01;

        
        float c;
        // More stone looking.
        if(style == 0)
            c = innerVoronoi(p*5.)*.8 + innerVoronoi(p*15.)*.15 + innerVoronoi(p*45.)*.05;
        
        // Less detail.
        if(style == 1 || style == 5)
            c = innerVoronoi(p*5.) - (1.-innerVoronoi(p*15.))*.08 + innerVoronoi(p*45.)*.04;
        
        // Version 1
        if(style == 2)
            c = innerVoronoi(p*5.)*.6 + innerVoronoi(p.yxz*5. + vec3(2.666, 1.666, 0.666))*.4 - (1.-innerVoronoi(p*15.))*.08 + innerVoronoi(p*45.)*.04;
        
        // Version 2
        if(style == 3 || style == 4)
            c = innerVoronoi(p*5. - .35)*.6 + innerVoronoi((p.yxz + .5)*10.)*.3 + (1.-innerVoronoi(p*25.))*.075 + innerVoronoi(p*60.)*.025;
        
        c += dot(tx, vec3(.299, .587, .114))*.1;
        return min(c/1.1, 1.);; // Put the texture color in the XYZ channels.

    
}

float levels(in float x, in float xmin, in float xmax)
{
    return clamp((x - xmin)/(xmax - xmin), 0.0, 1.0);
}

/* discontinuous pseudorandom uniformly distributed in [-0.5, +0.5]^3 */
vec3 random3(vec3 c) {
	float j = 4096.0*sin(dot(c,vec3(17.0, 59.4, 15.0)));
	vec3 r;
	r.z = fract(512.0*j);
	j *= .125;
	r.x = fract(512.0*j);
	j *= .125;
	r.y = fract(512.0*j);
	return r-0.5;
}

/* skew constants for 3d simplex functions */
const float F3 =  0.3333333;
const float G3 =  0.1666667;

/* 3d simplex noise */
float simplex3d(vec3 p) {
	 /* 1. find current tetrahedron T and it's four vertices */
	 /* s, s+i1, s+i2, s+1.0 - absolute skewed (integer) coordinates of T vertices */
	 /* x, x1, x2, x3 - unskewed coordinates of p relative to each of T vertices*/
	 
	 /* calculate s and x */
	 vec3 s = floor(p + dot(p, vec3(F3)));
	 vec3 x = p - s + dot(s, vec3(G3));
	 
	 /* calculate i1 and i2 */
	 vec3 e = step(vec3(0.0), x - x.yzx);
	 vec3 i1 = e*(1.0 - e.zxy);
	 vec3 i2 = 1.0 - e.zxy*(1.0 - e);
	 	
	 /* x1, x2, x3 */
	 vec3 x1 = x - i1 + G3;
	 vec3 x2 = x - i2 + 2.0*G3;
	 vec3 x3 = x - 1.0 + 3.0*G3;
	 
	 /* 2. find four surflets and store them in d */
	 vec4 w, d;
	 
	 /* calculate surflet weights */
	 w.x = dot(x, x);
	 w.y = dot(x1, x1);
	 w.z = dot(x2, x2);
	 w.w = dot(x3, x3);
	 
	 /* w fades from 0.6 at the center of the surflet to 0.0 at the margin */
	 w = max(0.6 - w, 0.0);
	 
	 /* calculate surflet components */
	 d.x = dot(random3(s), x);
	 d.y = dot(random3(s + i1), x1);
	 d.z = dot(random3(s + i2), x2);
	 d.w = dot(random3(s + 1.0), x3);
	 
	 /* multiply d by w^4 */
	 w *= w;
	 w *= w;
	 d *= w;
	 
	 /* 3. return the sum of the four surflets */
	 return dot(d, vec4(52.0));
}

vec3 random3f( vec3 p )
{
    return fract(sin(vec3( dot(p,vec3(1.0,57.0,113.0)), 
                           dot(p,vec3(57.0,113.0,1.0)),
                           dot(p,vec3(113.0,1.0,57.0))))*43758.5453);
}

/* from IQ */
vec3 voronoi( in vec3 x, out vec3 center )
{
    vec3 p = floor( x );
    vec3 f = fract( x );

	float id = 0.0;
    vec2 res = vec2( 100.0 );
    for( int k=-1; k<=1; k++ )
    for( int j=-1; j<=1; j++ )
    for( int i=-1; i<=1; i++ )
    {
        vec3 b = vec3( float(i), float(j), float(k) );
        vec3 r = vec3( b ) - f + random3f( p + b );
        float d = dot( r, r );

        if( d < res.x )
        {
			id = dot( p+b, vec3(1.0,57.0,113.0 ) );
            center = random3f( p + b );
            res = vec2( d, res.x );			
        }
        else if( d < res.y )
        {
            res.y = d;
        }
    }

    return vec3( sqrt( res ), abs(id) );
}

vec3 innerrotate(vec3 v, vec3 axis, float angle)
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

float innerscratch(vec3 uv, vec3 seed, float scratchesWavyness)
{
    seed.x = floor(sin(seed.x * 51024.0) * 3104.0);
    seed.y = floor(sin(seed.y * 1324.0) * 554.0);
    seed.z = floor(sin(seed.z * 4848.0) * 2411.0);
 
    uv = uv * 2.0 - 1.0;
    
    innerrotate(uv, vec3(0,1,0), seed.x + seed.y + seed.z);
    
    uv += sin(seed.x + seed.z - seed.y);
    
    uv = clamp(uv * 0.5 + 0.5, 0.0, 1.0);
    
    float s1 = sin(seed.x + uv.y * 3.1415) * scratchesWavyness;
    float s2 = sin(seed.y + uv.y * 3.1415) * scratchesWavyness;
    float s3 = sin(seed.z + uv.z * 3.1415) * scratchesWavyness;
    
    float x = sign(0.01 - abs(uv.x - 0.5 + s3 + s2 + s1));
    return clamp(((1.0 - pow(uv.y, 2.0)) * uv.y) * 2.5 * x, 0.0, 1.0);
}

float innerlayer(vec3 uv, vec3 frequency, vec3 offset, float angle, float scratchesWavyness)
{
    innerrotate(uv, vec3(0,1,0) ,angle);
    uv = uv * frequency + offset;
    return innerscratch(fract(uv), floor(uv), scratchesWavyness);
}

float innerscratches(vec3 uv,float scratchesWavyness, vec2 scratchesScale, int scratchesLayers, vec2 scratchesBaseFrequency, vec2 scratchesFrequencyStep)
{
    uv *= vec3(scratchesScale,scratchesScale.x);

    vec3 frequency = vec3(scratchesBaseFrequency, scratchesBaseFrequency.x);
    float innerscratches = 0.0;
    for(int i = 0; i < scratchesLayers; ++i)
    {
        float fi = float(i);
    	innerscratches = max(innerscratches, innerlayer(uv, frequency, vec3(fi, fi, fi), fi * 3145.0, scratchesWavyness)); // Nicer blending, thanks Shane!
        frequency += vec3(scratchesFrequencyStep, scratchesFrequencyStep.x);
    }
    return clamp(innerscratches, 0.0, 1.0); // Saturate for AA to work better
}

float innerhash(in vec3 p)
{ 
    return fract(sin(dot(p.xyz, vec3(12.9898,78.233, 62.485))) * 43758.5453123); 
}

float innerdotNoise2D(in float x, in float y, in float z, in float fractionalMaxDotSize, in float dDensity)
{
    float integer_x = x - fract(x);
    float fractional_x = x - integer_x;

    float integer_y = y - fract(y);
    float fractional_y = y - integer_y;
    
    float integer_z = z - fract(z);
    float fractional_z = z - integer_z;

    if (innerhash(vec3(integer_x + 1.0, integer_y + 1.0, integer_z + 1.0)) > dDensity){
        return 0.0;
    }

    float xoffset = (innerhash(vec3(integer_x, integer_y, integer_z)) -0.5);
    float yoffset = (innerhash(vec3(integer_x + 1.0, integer_y, integer_z)) - 0.5);
    float zoffset = (innerhash(vec3(integer_x + 1.0, integer_y + 1.0, integer_z)) - 0.5);
    float dotSize = 0.5 * fractionalMaxDotSize * max(0.25,innerhash(vec3(integer_x, integer_y+1.0, integer_z + 1.0))); //TODO Change the z axis values

    vec3 truePos = vec3(0.5 + xoffset * (1.0 - 2.0 * dotSize) , 0.5 + yoffset * (1.0 -2.0 * dotSize), 0.5 + zoffset * (1.0 - 2.0 * dotSize));

    float distance = length(truePos - vec3(fractional_x, fractional_y, fractional_z));

    return 1.0 - smoothstep(0.3 * dotSize, 1.0* dotSize, distance);

}

float innerDotNoise2D(in vec3 coord, in float wavelength, in float fractionalMaxDotSize, in float dDensity)
{
   return innerdotNoise2D(coord.x/wavelength, coord.y/wavelength, coord.z/wavelength, fractionalMaxDotSize, dDensity);
}

float innergetDroplets(vec3 uv, float dropletsCount, float dropletsOpacityJitter, float dropletsSize, int style, int steps){
    float splash_speed = 1.;
    float transform = 0.;

    float rain = 0.;
    float base_rate = 6.0 + 3.0 * dropletsCount + 4.0 * (splash_speed - 1.0);
    float base_density = 0.6 * dropletsCount + 0.4 * (splash_speed -1.0);

    float opacityJitter = 1. - dropletsOpacityJitter;
    
    for(int j = 0; j < steps;j++){
        
        float innerhash = innerhash(vec3(float(j),float(j)*1.785, (j)*2.3485));
        float opacity = 1. * innerhash;
        float opacityGap = opacity - 1.;
        opacity -= opacityGap * opacityJitter;   

        vec3 p = uv;
        if(style == 1){
            if(j % 2 == 0){
                p.z /= 5.;     
                p.y /= 5.;     
            }
        }
        else if(style == 2){
            p.z /= 5.;     
            p.y /= 5.;     
        }
        
        for(int i = 0; i < 4;i++){
            float time_fact = (sin(base_rate + (1.570*float(i))));
            time_fact = smoothstep(0.0,1.0,time_fact);
            rain += (innerDotNoise2D(p.xyz + innerHash33_2(vec3(float(j),float(j)*1.785, (j)*2.3485)), 0.02 * dropletsSize ,0.5, base_density) * time_fact * (opacity));
        }
    }
    
    return rain;
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

//17
float wallNoise1(vec3 uv){
    return innerGetWall(uv, 0);
}

//18
float wallNoise2(vec3 uv){
    return innerGetWall(uv, 1);
}

//19
float wallNoise3(vec3 uv){
    return innerGetWall(uv, 2);
}

//20
float wallNoise4(vec3 uv){
    return innerGetWall(uv, 3);
}

//21
float wallNoise5(vec3 uv){
    return innerGetWall(uv, 4);
}

//22
float wallNoise6(vec3 uv){
    return innerGetWall(uv, 5);
}

//23
float pebbleNoise1(vec3 pos){

    // warp node
    float noise = simplex3d(27.5*pos);
    /////////
    pos += 0.007 * noise * vec3(1.0, 1.0, 0.0);
    
    // warp node 2
    vec3 org;
    vec3 noise3 = voronoi(70.0 * pos, org);
    //////////
    pos += 0.0025 * noise3.y * vec3(1.0, 1.0, 0.0);

    vec3 col = voronoi(10.0 * pos, org);
    
    // levels
    float lvl = 1.0 - col.x;
    lvl = levels(lvl, 0.50, 1.60);
    return (lvl);  
}

//24
float pebbleNoise2(vec3 pos){
    pos.x += (gradientDefNoiseA(pos * 2.)) / 8.;
    pos.y += (gradientDefNoiseA(pos * 2. + 2)) / 8.;
    pos.z += (gradientDefNoiseA(pos * 2. + 4)) / 8.;

    // warp node
    float noise = simplex3d(27.5*pos);
    /////////
    pos += 0.007 * noise * vec3(1.0, 1.0, 0.0);
    
    // warp node 2
    vec3 org;
    vec3 noise3 = voronoi(70.0 * pos, org);
    //////////
    pos += 0.0025 * noise3.y * vec3(1.0, 1.0, 0.0);

    vec3 col = voronoi(10.0 * pos, org);
    
    // levels
    float lvl = 1.0 - col.x;
    lvl = levels(lvl, 0.50, 1.60);
    return (lvl);  
}

//25
float pebbleNoise3(vec3 pos){
    pos.x += (gradientDefNoiseA(pos * 2.)) / 2.;
    pos.y += (gradientDefNoiseA(pos * 2. + 2)) / 2.;
    pos.z += (gradientDefNoiseA(pos * 2. + 4)) / 2.;

    // warp node
    float noise = simplex3d(27.5*pos);
    /////////
    pos += 0.007 * noise * vec3(1.0, 1.0, 0.0);
    
    // warp node 2
    vec3 org;
    vec3 noise3 = voronoi(70.0 * pos, org);
    //////////
    pos += 0.0025 * noise3.y * vec3(1.0, 1.0, 0.0);

    vec3 col = voronoi(10.0 * pos, org);
    
    // levels
    float lvl = 1.0 - col.x;
    lvl = levels(lvl, 0.50, 1.60);
    return (lvl);  
}

//26
float pebbleNoise4(vec3 pos){

    // warp node
    float noise = simplex3d(27.5*pos);
    /////////
    pos += 0.007 * noise * vec3(1.0, 1.0, 0.0);
    
    // warp node 2
    vec3 org;
    vec3 noise3 = voronoi(70.0 * pos, org);
    //////////
    pos += 0.0025 * noise3.y * vec3(1.0, 1.0, 0.0);

    vec3 col = voronoi(10.0 * pos, org);
    
    // levels
    float lvl = 1.0 - col.x;
    lvl = levels(lvl, 0.80, 1.60);
    return (lvl);  
}

//27
float pebbleNoise5(vec3 pos){
    pos.x += (gradientDefNoiseA(pos * 2.)) / 8.;
    pos.y += (gradientDefNoiseA(pos * 2. + 2)) / 8.;
    pos.z += (gradientDefNoiseA(pos * 2. + 4)) / 8.;

    // warp node
    float noise = simplex3d(27.5*pos);
    /////////
    pos += 0.007 * noise * vec3(1.0, 1.0, 0.0);
    
    // warp node 2
    vec3 org;
    vec3 noise3 = voronoi(70.0 * pos, org);
    //////////
    pos += 0.0025 * noise3.y * vec3(1.0, 1.0, 0.0);

    vec3 col = voronoi(10.0 * pos, org);
    
    // levels
    float lvl = 1.0 - col.x;
    lvl = levels(lvl, 0.80, 1.60);
    return (lvl);  
}

//28
float pebbleNoise6(vec3 pos){
    pos.x += (gradientDefNoiseA(pos * 2.)) / 2.;
    pos.y += (gradientDefNoiseA(pos * 2. + 2)) / 2.;
    pos.z += (gradientDefNoiseA(pos * 2. + 4)) / 2.;

    // warp node
    float noise = simplex3d(27.5*pos);
    /////////
    pos += 0.007 * noise * vec3(1.0, 1.0, 0.0);
    
    // warp node 2
    vec3 org;
    vec3 noise3 = voronoi(70.0 * pos, org);
    //////////
    pos += 0.0025 * noise3.y * vec3(1.0, 1.0, 0.0);

    vec3 col = voronoi(10.0 * pos, org);
    
    // levels
    float lvl = 1.0 - col.x;
    lvl = levels(lvl, 0.80, 1.60);
    return (lvl);  
}

//29
float pebbleNoise7(vec3 pos){
    pos.x += (gradientDefNoiseA(pos * 2.)) * 2.;
    pos.y += (gradientDefNoiseA(pos * 2. + 2)) * 2.;
    pos.z += (gradientDefNoiseA(pos * 2. + 4)) * 2.;

    // warp node
    float noise = simplex3d(27.5*pos);
    /////////
    pos += 0.007 * noise * vec3(1.0, 1.0, 0.0);
    
    // warp node 2
    vec3 org;
    vec3 noise3 = voronoi(70.0 * pos, org);
    //////////
    pos += 0.0025 * noise3.y * vec3(1.0, 1.0, 0.0);

    vec3 col = voronoi(10.0 * pos, org);
    
    // levels
    float lvl = 1.0 - col.x;
    lvl = levels(lvl, 0.80, 1.60);
    return (lvl);  
}

//30
float scratchNoise1(vec3 pos){
    float scratchesWavyness = 0.1;
    vec2 scratchesScale = vec2(3.0, 3.0);
    int scratchesLayers = 4;
    vec2 scratchesBaseFrequency = vec2(0.5, 0.5);
    vec2 scratchesFrequencyStep = vec2(0.25, 0.25); 
    return innerscratches(pos, scratchesWavyness, scratchesScale, scratchesLayers, scratchesBaseFrequency, scratchesFrequencyStep);
}

//31
float scratchNoise2(vec3 pos){
    float scratchesWavyness = 0.8;
    vec2 scratchesScale = vec2(3.0, 3.0);
    int scratchesLayers = 4;
    vec2 scratchesBaseFrequency = vec2(0.5, 0.5);
    vec2 scratchesFrequencyStep = vec2(0.25, 0.25); 
    return innerscratches(pos, scratchesWavyness, scratchesScale, scratchesLayers, scratchesBaseFrequency, scratchesFrequencyStep);
}

//32
float dropletsNoise1(vec3 pos){
    float dropletsCount = 10.;
    float dropletsOpacityJitter = 1.;
    float dropletsSize = 2.5;

    return innergetDroplets(pos, dropletsCount, dropletsOpacityJitter, dropletsSize, 0, 5);
}

//33
float dropletsNoise2(vec3 pos){
    float dropletsCount = 10.;
    float dropletsOpacityJitter = 1.;
    float dropletsSize = 2.5;

    return innergetDroplets(pos, dropletsCount, dropletsOpacityJitter, dropletsSize, 1, 5);
}

//34
float dropletsNoise3(vec3 pos){
    float dropletsCount = 10.;
    float dropletsOpacityJitter = 1.;
    float dropletsSize = 2.5;

    return innergetDroplets(pos, dropletsCount, dropletsOpacityJitter, dropletsSize, 2, 5);
}

//35
float dropletsNoise4(vec3 pos){
    float dropletsCount = 10.;
    float dropletsOpacityJitter = 1.;
    float dropletsSize = 2.5;

    return innergetDroplets(pos, dropletsCount, dropletsOpacityJitter, dropletsSize, 0, 40);
}

//36
float dropletsNoise5(vec3 pos){
    float dropletsCount = 10.;
    float dropletsOpacityJitter = 0.;
    float dropletsSize = 2.5;

    return innergetDroplets(pos, dropletsCount, dropletsOpacityJitter, dropletsSize, 0, 1);
}

//0
float smartPos(vec3 pos, float offset, float yaw, float pitch, float position){

    yaw = radians(yaw);
    pitch = radians(pitch);

    // Calculate the axis of rotation for yaw (vertical axis)
    vec3 yawAxis = vec3(0.0, 1.0, 0.0);

    // Calculate the axis of rotation for pitch (horizontal axis)
    vec3 pitchAxis = vec3(1.0, 0.0, 0.0);

    // Combine the yaw and pitch rotations
    vec3 combinedAxis = normalize(cos(pitch) * yawAxis + sin(pitch) * pitchAxis);
    
    // Calculate the combined angle
    float combinedAngle = length(vec2(yaw, pitch));

    pos = innerrotate(pos, combinedAxis, combinedAngle);

    pos += vec3(position);

    return pos.y * offset;
}

float smartPos2(vec3 pos, float offset, float yaw, float pitch, float position){
    float val = smartPos(pos, offset, yaw, pitch, position);
    
    float noise = musgraveDefNoiseA(pos);

    if(val < 1. && val > 0.){
        if(val > 0.5)
            val = mix(val * noise, val, val);
        else
            val *= noise;
    }

    return val;
}

float smartPos3(vec3 pos, float offset, float yaw, float pitch, float position){
    float val = smartPos(pos, offset, yaw, pitch, position);
    
    float noise = innerWorleyNoiseA(pos) * 2.;

    if(val < 1. && val > 0.){
        if(val > 0.5)
            val = mix(val * noise, val, val);
        else
            val *= noise;
    }

    return val;
}

float smartStripes(vec3 pos, float linesRows, float thickness, float blur, float rotation, vec2 txtrRes){
    
    vec2 iResolution = txtrRes;
    vec2 fragCoord = pos.xy * iResolution.x; 

    // update layout params
    float rows = linesRows * 0.5;
    
    // Initialize color accumulation
    vec3 colorAccumulator = vec3(0.0);

    const int samples = 8;

    // Iterate over samples within the pixel
    for (int i = 0; i < samples; i++) {
        // Offset UV coordinates for supersampling
        vec2 uv = (2.0 * (fragCoord + vec2(mod(float(i), 2.0), floor(float(i) / 2.0)) * 0.5) - iResolution.xy) / iResolution.y;

        vec2 uvRepeat = fract(uv * rows);
        
        float aa = iResolution.y * blur; // Use blur to control blur strength
        float col = smoothstep(thickness - aa, thickness + aa, length(uvRepeat.y - 0.5));

        // Accumulate colors
        colorAccumulator += vec3(col);
    }

    // Average the accumulated colors
    vec3 finalColor = colorAccumulator / float(samples);

    return finalColor.r;
}

float smartStripes2(vec3 pos, float offset, float yaw, float pitch, float position, vec2 txtrRes){
    float val = smartStripes(pos, offset, yaw, pitch, position, txtrRes);
    
    float noise = musgraveDefNoiseA(pos);

    if(val < 1. && val > 0.){
        if(val > 0.5)
            val = mix(val * noise, val, val);
        else
            val *= noise;
    }

    return val;
}

float getProceduralVal(vec3 pos, int proceduralID, float scale, int inverted, vec2 uv, vec4 smartProperties, vec2 txtrRes){
    
    pos *= scale;
    uv *= scale;
    
    float res = 0.;

    if(proceduralID == 0)
        res = hexagonalPattern(uv);
    else if(proceduralID == 1)
        res = hexagonalPattern2(uv);
    else if(proceduralID == 2)
        res = hexagonalPattern3(uv);
    else if(proceduralID == 3)
        res = hexagonalTruchetPattern(uv);
    else if(proceduralID == 4)
        res = checkerPattern(uv);
    else if(proceduralID == 5)
        res = trianglePattern(uv);
    else if(proceduralID == 6)
        res = rhombPattern1(uv);
    else if(proceduralID == 7)
        res = karoPattern(uv);
    else if(proceduralID == 8)
        res = gridPattern1(uv);
    else if(proceduralID == 9)
        res = gridPattern2(uv);
    else if(proceduralID == 10)
        res = squareHolePattern(uv);
    else if(proceduralID == 11)
        res = roundSquarePattern(uv);
    else if(proceduralID == 12)
        res = sinePattern(uv);
    else if(proceduralID == 13)
        res = brickPattern(uv);
    else if(proceduralID == 14)
        res = gearPattern(uv);
    else if(proceduralID == 15)
        res = layeredCirclePattern(uv);
    else if(proceduralID == 16)
        res = starPattern(uv);
    else if(proceduralID == 17)
        res = basketPattern1(uv);
    else if(proceduralID == 18)
        res = basketPattern2(uv);
    else if(proceduralID == 19)
        res = diamondPattern(uv);
    else if(proceduralID == 20)
        res = rosettePattern(uv);
    else if(proceduralID == 21)
        res = smoothHexPattern(uv);
    else if(proceduralID == 22)
        res = smoothSharpSquarePattern(uv);
    else if(proceduralID == 23)
        res = randomTrianglePattern(uv);
    else if(proceduralID == 24)
        res = solidPattern();
    else if(proceduralID == 25)
        res = brickPattern1(uv);
    else if(proceduralID == 26)
        res = brickPattern2(uv);
    else if(proceduralID == 27)
        res = brickPattern3(uv);
    else if(proceduralID == 28)
        res = brickPattern4(uv);




    else if(proceduralID == 29)
        res = basicNoiseA(pos);
    else if(proceduralID == 30)
        res = basicStretchedNoiseA(pos);
    else if(proceduralID == 31)
        res = fbmNoiseA(pos);
    else if(proceduralID == 32)
        res = fbmHightPersistanceNoiseA(pos);
    else if(proceduralID == 33)
        res = fbmLowOctaveNoiseA(pos);
    else if(proceduralID == 34)
        res = fbmWaveNoiseA(pos);
    else if(proceduralID == 35)
        res = fbmWaveLowPersistanceNoiseA(pos);
    else if(proceduralID == 36)
        res = fbmWaveHighPersistanceNoiseA(pos);
    else if(proceduralID == 37)
        res = musgraveDefNoiseA(pos);
    else if(proceduralID == 38)
        res = musgraveHighDimensionalNoiseA(pos);
    else if(proceduralID == 39)
        res = gradientDefNoiseA(pos);
    else if(proceduralID == 40)
        res = innerWorleyNoiseA(pos);
    else if(proceduralID == 41)
        res = worleyPatternalNoiseA(pos);
    else if(proceduralID == 42)
        res = circularPerlinNoiseA(pos);
    else if(proceduralID == 43)
        res = voronoiNoiseA(pos);
    else if(proceduralID == 44)
        res = voronoiDeeperNoiseA(pos);
    else if(proceduralID == 45)
        res = staticNoiseA(pos);
    else if(proceduralID == 46)
        res = wallNoise1(pos);
    else if(proceduralID == 47)
        res = wallNoise2(pos);
    else if(proceduralID == 48)
        res = wallNoise3(pos);
    else if(proceduralID == 49)
        res = wallNoise4(pos);
    else if(proceduralID == 50)
        res = wallNoise5(pos);
    else if(proceduralID == 51)
        res = wallNoise6(pos);
    else if(proceduralID == 52)
        res = pebbleNoise1(pos);
    else if(proceduralID == 53)
        res = pebbleNoise2(pos);
    else if(proceduralID == 54)
        res = pebbleNoise3(pos);
    else if(proceduralID == 55)
        res = pebbleNoise4(pos);
    else if(proceduralID == 56)
        res = pebbleNoise5(pos);
    else if(proceduralID == 57)
        res = pebbleNoise6(pos);
    else if(proceduralID == 58)
        res = pebbleNoise7(pos);
    else if(proceduralID == 59)
        res = scratchNoise1(pos);
    else if(proceduralID == 60)
        res = scratchNoise2(pos);
    else if(proceduralID == 61)
        res = dropletsNoise1(pos);
    else if(proceduralID == 62)
        res = dropletsNoise2(pos);
    else if(proceduralID == 63)
        res = dropletsNoise3(pos);
    else if(proceduralID == 64)
        res = dropletsNoise4(pos);
    else if(proceduralID == 65)
        res = dropletsNoise5(pos);
    
    


    else if(proceduralID == 66)
        res = smartPos(pos, smartProperties.x, smartProperties.y, smartProperties.z, smartProperties.w);
    else if(proceduralID == 67)
        res = smartPos2(pos, smartProperties.x, smartProperties.y, smartProperties.z, smartProperties.w);
    else if(proceduralID == 68)
        res = smartPos3(pos, smartProperties.x, smartProperties.y, smartProperties.z, smartProperties.w);
    else if(proceduralID == 69)
        res = smartStripes(pos, smartProperties.x, smartProperties.y, smartProperties.z, smartProperties.w, txtrRes);
    else if(proceduralID == 70)
        res = smartStripes2(pos, smartProperties.x, smartProperties.y, smartProperties.z, smartProperties.w, txtrRes);
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

float GMC(vec3 val, int inverted){
    if(inverted == 0)
        return max(max(val.r, val.g), val.b);
    else
        return 1. - max(max(val.r, val.g), val.b);
}

vec4 getProcedural(vec3 pos, int proceduralID, sampler2D texture, vec2 texCoord, float scale, int inverted, vec4 smartProperties, vec2 txtrRes){
    if(proceduralID != -1)
        return vec4(vec3(getProceduralVal(pos, proceduralID, scale, inverted, texCoord, smartProperties, txtrRes)), 1);
    else{
        if(inverted == 0)
            return texture(texture, texCoord * scale);
        else
            return 1. - texture(texture, texCoord * scale);
    }
}