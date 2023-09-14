
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

#define bpm 120.

#define AA 5.

#define pi 3.141592

mat2 r2d(float a) {
    float c = cos(a), s = sin(a);
    return mat2(c, s, -s, c);
}

float fill(float d) {
    return 1. - smoothstep(0., 0.001, d);
}

// inspired by Pixel Spirit Deck: https://patriciogonzalezvivo.github.io/PixelSpiritDeck/
// + https://www.shadertoy.com/view/tsSXRz
float stroke(float d, float width) {
	return 1. - smoothstep(0., 0.001, abs(d) - width * .5);
}

float bridge(float mask, float sdf, float w) {
    mask *= 1. - stroke(sdf, w * 2.);
    return mask + stroke(sdf, w);
}

float circle(vec2 p, float radius) {
  return length(p) - radius;
}

float rect(vec2 p, vec2 size) {
  vec2 d = abs(p) - size;
  return min(max(d.x, d.y), 0.0) + length(max(d,0.0));
}

float easeInOutQuad(float t) {
    if ((t *= 2.) < 1.) {
        return .5 * t * t;
    } else {
        return -.5 * ((t - 1.) * (t - 3.) - 1.);
    }
}

float circleX(vec2 uv, float cross_height, float cross_width, float circle_radius, float circle_stroke)
{
    uv /= 10.;
    
    float t1 = fract(1. * .25);// sliding
    float t2 = fract(1.);// rotation
    t2 = easeInOutQuad(t2);
    
    uv *= r2d(pi * .25);
    vec2 uv1 = fract((uv + t1) * 4.) - .5;
    vec2 uv2 = fract(((uv-vec2(t1, 0)) * 4.)+.5) - .5;

    // layer1 - cross
    float mask = fill(rect(uv1 * r2d(t2 * pi), vec2(cross_height, cross_width)));
    mask += fill(rect(uv1 * r2d(t2 * pi), vec2(cross_width, cross_height)));
    
    // layer2 - circle
    mask = bridge(mask, circle(uv2, circle_radius), circle_stroke) ;
    
    mask = clamp(mask, 0., 1.);
    
    return mask;
}

#define lmac(a)  2.6*(a)*(a)                             // approx of spiral arc-length

float greekFriezeX(vec2 U, float val) //From : https://www.shadertoy.com/view/XtSBDK
{
    float O = 0.;
    
    U -= 40.;
    
	vec2 V;
    U = vec2( atan(U.y,U.x)/6.283 +.5, length(U) );   // polar coordinates
    U.y-= U.x;                                        // lenght along spiral
    U.x = lmac( ceil(U.y)+U.x );                 // arc-length
    O   = 1.- pow( abs( 2.*fract(U.y)-1.),10.); // inter-spires antialiasing
    V   = ceil(U); 
    U = fract(U)-.5;                   // cell along spiral: id + loc coords
 // vortices (small spirals) : assume col = step(0,y) then rotate( (0,0), space&time*(.5-dist) )
    U.y = dot( U, cos( vec2(-33,0)                    // U *= rot, only need U.y -> (-sin,cos)
                       +  .3*(val)          // rot amount inc with space/time
                         * max( 0., .5 - length(U) )  // rot amount dec with dist
             )       );
	O *= smoothstep( -1., 1., U / fwidth(U) ).y;        // draw antialiased vortices

    return O;
}

float basket(vec2 u, int s){ //From https://www.shadertoy.com/view/tdVGzz
    vec2 uv = u / 10.;
    float col = 1.;
   
	vec2 fuv = fract(uv*6.);
    vec2 id = floor(uv*6.);
    
    float chk = mod(id.y+id.x,2.);
    
    if(s == 0){
        if(chk > 0.){
            col -= (sin(fuv.x*76.+.5))*.5;	
            col *= smoothstep(.6, 0.,abs(fuv.y-0.5));
        }
        else{
           col -= (sin(fuv.y*76.))*.5;
           col *= smoothstep(.6, 0.,abs(fuv.x-0.5));
        }
    }
    else{
        col -= (sin(fuv.x*76.+.5))*.5;	
        col *= smoothstep(.6, 0.,abs(fuv.y-0.5));
    }
    
    
    return col * .8;
}

vec2 s = vec2(1,1.73);

vec2 rot(vec2 a, float t)
{
    float s = sin(t);
    float c = cos(t);
    return vec2(a.x*c-a.y*s,
                a.x*s+a.y*c);
}

//thanks to BigWIngs/The Art Of Code's videos on hex tiling and truchet patterns
vec4 hexCoords(vec2 p)
{
    vec2 hs = s*.5,
         c1 = mod(p,s)-hs,
         c2 = mod(p-hs,s)-hs;
    vec2 hc= dot(c1, c1)<dot(c2,c2)?c1:c2;
    return vec4(hc, p-hc);
    
}

float li(float px, float lt, float d, vec2 hc)
{
    return smoothstep(0.,px,length(hc.x+d)-lt);
}

float hexaWeawing(vec2 U, float s)
{
    float N = 3.,              // number of axis
          S = 1.,              // scale
          a = 3.14/N;

    float O = 0.; 
    
    O -= O;
    
    for (float i=0.; i<N; i++)
        O = max( O,    smoothstep(0.01, 0., abs(fract(U.x+.1)-.6)-s)            // strip
                   * ( .7 + .3* sin(6.28* ( U.y*1.73/2. + .5*floor(U.x) ))) ),  // waves
        U *= mat2(cos(a),-sin(a),sin(a),cos(a));
    
    return O;
} 

float hexaWeawing2(vec2 U, float s)
{
    float N = 3.,              // number of axis
          S = 1.,              // scale
          a = 3.14/N;

    float O = 0.; 
    
    O -= O;
    
    for (float i=0.; i<N; i++)
        O = max( O,    smoothstep(.0, 0., abs(fract(U.x+.1)-.6)-s)            // strip
                   * ( .7 + .3* sin(6.28* ( U.y*1.73/2. + .5*floor(U.x) ))) ),  // waves
        U *= mat2(cos(a),-sin(a),sin(a),cos(a));
    
    return O;
} 

// Fork of "Hexagonal Interlacing" https://shadertoy.com/view/llfcWs
// isolating the base weaving

float rad2 = .666,
      TAU = 6.283;

#define rnd(p)   fract(sin(dot(p, vec2(411.3, 2899.7)))*43758.5453)
#define rotX(a)   mat2(cos(a), sin(a), -sin(a), cos(a))

vec4 getHex(vec2 p){
    vec4 hC = floor( vec4( p, p - vec2(vec2( 3, sqrt(3.)).y, vec2( 3, sqrt(3.)).y/2.))/vec2( 3, sqrt(3.)).xyxy ) + .5;
    vec2 a = p -  hC.xy *vec2( 3, sqrt(3.)), 
         b = p - (hC.zw + .5) *vec2( 3, sqrt(3.));
    return dot(a,a) < dot(b,b) 
        ? vec4(a, hC.xy) 
        : vec4(b, hC.zw+.5); 
}

float mask( vec2 p) {
    float ia = floor( atan(p.y,p.x) *3./TAU) + .5;
    p *= rotX(ia *TAU/3.); 
    p.x -= 1.; 
    return length(p) - rad2;
}

float r1 = .40,     // original: .38
      r2 = .41,     // original: .45
      S =  .577;    // 1./sqrt(3.) 

#define DY(U,r) smoothstep(-0.01, 0.01, r - length(U) ) // disc

float C( vec2 U, vec2 d )   // --- draw one layer
{
    U.y *= S+S;
    U -= d;
    if ( mod(U.y, 2.) > 1.) U.x += .5;
 
    U = fract(U) - S;
    U.y += .1;      // required. why ?
	U.y /= S+S;
    
    float v = DY(U, r2), a = 0.;
    for ( ; a++ < 3.;  ) 
        v -= DY(U + sin( a*2.1 + vec2(0,33) )*S, r2);

   return v * DY(U, r1);
}

// IQ's vec2 to float hash.
float hash21(vec2 p){  return fract(sin(dot(p, vec2(27.619, 57.583)))*43758.5453); }

// IQ's animated vec2 to float hash.
float hash21A(vec2 p){  
    float x = fract(sin(dot(p, vec2(27.619, 57.583)))*43758.5453); 
    return sin(x*6.2831)*.5 + .5;
}

// The Truchet distance field. Truchet patterns, in their various forms, are 
// pretty easy to put together; Render some rotationally symmetric tiles, then
// randomly rotate them. If you know how to render simple 2D objects like 
// squares, circles, arcs, etc, you should be good to go. As you can see from
// the imagery, these tiles consist of a line of circles and some chopped
// out circles.
// Written by the Shadertoy user FabriceNeyret2
float distField(vec2 p, int randS){
    
    // Saving the original position.
    vec2 oP = p;
 
    // Cell ID and local coordinates.
    vec2 ip = floor(p);
    p -= ip + .5;
    
    // Random value for the tile. This one is time based.
    float rndX;
    if(randS == 0)
        rndX = hash21A(ip);
    else if(randS == 1){
        rndX = 1.;
    }
    else if(randS == 2){
        rndX = 0.;
    }
    
    // If the random number generated by the unique ID for the
    // tile is above a certain threshold, flip it upside down.
    if(rndX<.5) p.y = -p.y;
  
    
    // Distance field.
    float d = 1e5;
    
    // Radius.
    float r = .09;
    
    // Two circles on diagonally opposite corners.
    d = min(d, length((p) - vec2(-.5, .5)) - .5);
    d = min(d, length((p) - vec2(.5, -.5)) - .5);

    // Some small circles down the center.
    float d2 = length(p - .3) - r*.9;
    d2 = min(d2, length(p - .1) - r*.7); 
    d2 = min(d2, length(p + .1) - r*.7); 
    d2 = min(d2, length(p + .3) - r*.9);  
    
    // Flip checkered tiles. It's a necessary operation to perform
    // for this style of Truchet tile.
    if(mod(ip.x + ip.y, 2.)<.5) d = -d;
    if(rndX>=.5) d = -d;
    
    // Put in some decorative borders. I like them, but you can take
    // them out if you feel it's too much.
    d = max((d - r*1.25), -(abs(abs(d ) - .05) - r/3.));
    
    // Combining the dotted lines and circles, whilst allowing for
    // the checkerboard flipping.
    if(mod(ip.x + ip.y, 2.)<.5){
        if(rndX<.5) d = max(d, -d2);
        else d = min(d, d2);
    }
    else {
       if(rndX<.5) d = min(d, d2);
        else d = max(d, -d2); 
    }
    
    // Adding a grid. Not necessary, but I like it.
    p = abs(p);
    float grid = abs(max(p.x, p.y) - .5) - .015;
    d = min(d, grid);
    
   
    // Rendering circles at the grid vertices, whilst accounting
    // for checkerboard flipping.
    vec2  q = oP - .5;
    vec2 iq = floor(q);
    q -= iq + .5;

    if(mod(iq.x + iq.y, 2.)<.5){
        d = min(d, length(q) - r*1.4);
    }
    else {
       d = max(d, -(length(q) - r*1.4));
    }
    

    // Return the distance field value.
    return d;
}

// Hexagonal grid coordinates. This returns the local coordinates and the cell's center.
// The process is explained in more detail here:
//
// Minimal Hexagon Grid - Shane
// https://www.shadertoy.com/view/Xljczw
//
vec4 getGrid(vec2 p){
    
    vec4 ip = floor(vec4(p/s, p/s - .5));
    vec4 q = p.xyxy - vec4(ip.xy + .5, ip.zw + 1.)*s.xyxy;
    return dot(q.xy, q.xy)<dot(q.zw, q.zw)? vec4(q.xy, ip.xy) : vec4(q.zw, ip.zw + .5);
    //return getHex(q.xy)<getHex(q.zw)? vec4(q.xy, ip.xy) : vec4(q.zw, ip.zw + .5);

}

// Hexagonal bound: Not technically a distance function, but it's
// good enough for this example.
float getHexX(vec2 p){
    
    // Flat top and pointed top hexagons.
    #ifdef FLAT_TOP
    return max(dot(abs(p.xy), s/2.), abs(p.y*s.y));
    #else   
    return max(dot(abs(p.xy), s/2.), abs(p.x*s.x));
    #endif
}

// Domain (space) warp

// http://wiki.inkscape.org/wiki/index.php/Tiled-Clones
// https://en.wikipedia.org/wiki/Wallpaper_group

vec2 p2mm_symmetry(vec2 uv)
{
	return abs(uv);
}

vec2 p6m_symmetry(vec2 uv)
{
	float s = 0.5;
	float c = sqrt(3.0) / 2.0;

	mat2 rot = mat2(c, s, -s, c);

	uv = p2mm_symmetry(uv);
	uv = p2mm_symmetry(rot*uv);
	uv = p2mm_symmetry(rot*uv);

	return uv;
}


vec2 tile(vec2 uv, vec2 dimensions)
{
    return mod(uv, dimensions) - dimensions / 2.0;
}


// Primitives

float dist_capsule(vec2 p, vec2 a, vec2 b, float r)
{
	vec2 pa = p - a;
	vec2 ba = b - a;
	float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );

	return length( pa - ba*h ) - r;
}


// Compound objects

float dist_star_fragment(vec2 uv, float r)
{
	return min(
		dist_capsule(uv, vec2(0.0, 0.0), vec2(0.0,             0.5), r),
		dist_capsule(uv, vec2(0.0, 0.0), vec2(sqrt(3.0) / 6.0, 0.5), r)
	);
}

float dist_star(vec2 uv, float r)
{
	return dist_star_fragment(
		p6m_symmetry(uv),
		r
	);
}

float dist_tiled_stars(vec2 uv, float r)
{
	return dist_star(
		tile(uv, vec2(sqrt(3.0), 1.0)),
		r
	);
}

float dist_asa_no_ha(vec2 uv, float r)
{
	return min(
		dist_tiled_stars(uv,                              r),
		dist_tiled_stars(uv + vec2(sqrt(3.0) / 2.0, 0.5), r)
	);
}


// Main scene

float scene(vec2 uv, float AA_epsilon)
{
	float dist = dist_asa_no_ha(uv, 0.01);
	float mask = smoothstep(0.0, AA_epsilon, -dist);

	vec3 col_bg = vec3(0.1, 0.1, 0.8);
	vec3 col_fg = vec3(0.8, 0.8, 0.8);

    return mask;
}



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
#define DX(U) .004/abs(length(mod(U,d+d)-d)-d.x)

float rosettePattern(
                        vec2 p
                    )
{
    vec2 d = vec2(0.58,1);
    
    vec4 O = vec4(0);
    
    for ( ; O.a++ < 4.; O += DX(p) +DX(p += d*.5)) 
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

//29
float rhombPattern2( 
                        vec2 p
                    ) 
{
    p.y = p.y * 0.866 + p.x * 0.5;
    float t = fract(p.y) - fract(p.x);
    return t;
}

//30
float truchetPattern(vec2 p){
    float o = 0.;
    p *= 50.;
    p.x *= sign(cos(length(ceil(p /= 50.))*99.));
    o = o - o + cos(min(length(p = fract(p)), length(--p))*44.); // --p - Thanks, Coyote.
    return o; 
}

//31
float greekFriezePattern1(vec2 U){
    return greekFriezeX(U, 20.); 
}

//32
float greekFriezePattern2(vec2 U){
    return greekFriezeX(U, 50.); 
}

//33
float basketPattern4(vec2 uv){
    return basket(uv, 0);
}

//34
float basketPattern5(vec2 uv){
    return basket(uv, 1);
}

//35
float pipelinePattern(vec2 p) //From : https://www.shadertoy.com/view/tsV3Rd
{
    vec4 hc = hexCoords(p+10.);
    float id = hc.z*hc.w*2.1238712;
    if(mod(id,3.)<=1.)hc.xy=rot(hc.xy,PI/3.);
    else if(mod(id,4.)<=2.)hc.xy=rot(hc.xy,-PI/3.);
    float px = 0.01, d = 0.25, lt = .075, 
        t =li(px,lt,d, hc.xy)*li(px,lt,-d, hc.xy);

    return (1.-t);
}

//36
float hexaWeawingPattern1(vec2 p){
    return hexaWeawing(p, .2);
}

//37
float hexaWeawingPattern2(vec2 p){
    return hexaWeawing(p, .1);
}

//38
float hexagonalInterlacingPattern(vec2 u){ //From https://www.shadertoy.com/view/tsyXRh

    float O;

	vec2  R = vec2(1.);
    vec2  U = u;
    vec4 hp = getHex(U);
  
    vec2 D = vec2( mask(hp.xy), mask(hp.xy*rotX(-TAU/6.) ) ),      // axial distance of the 2 layers
         M = smoothstep(-5. / 100., 5. / 100., D),                       // masks
         B = smoothstep(0.,.05, D);                               // border decoration
    float m = rnd(hp.zw) > .5 ? M.x : 1.-M.y;                     // layer-preserving random control

    O += mix( B.y*.5, B.x, m );                                   // 2-tints display
                                             // --- for next ones uncomment original random control
    return O;
}


float W( vec2 U ) { //From https://www.shadertoy.com/view/ldGSDG
    U = 2.* mod(U*5.,vec2(1,1.4)) - vec2(1,1.9);           // tiling
    float l = length(U);
    l = U.y/l > -.7 ?  l                                   // loop top = 3/4 ring
                    :  length( U - vec2 (sign(U.x),-1) );  // loop bottom = 2x half 3/4 ring
    return smoothstep(.1,.0, abs(l-.7) -.1) * ( .5+.4*abs(U.y+.6) );    // face A
}

//39
float chainPattern1(vec2 U ) 
{
    U /= 10.;
    return max( W(U), W( U+vec2(0,.7) ) ) ; // inter-weaving    
}

//40
float chainPattern2(vec2 U ) 
{
    U /= 10.;
    return max( W(U), W( U+vec2(0,.5) ) ) ; // inter-weaving    
}

//41
float chainPattern3(vec2 U )
{
    U /= 10.;
    return max( W(U), W( U+vec2(.05,.7) ) ) ; // inter-weaving    
}

//42
float circleXPattern1(vec2 uv){
    float cross_height = .2;
    float cross_width = .4;
    float circle_radius = .25;
    float circle_stroke = .2;
    
    return circleX(uv, cross_height, cross_width, circle_radius, circle_stroke);
}

//43
float circleXPattern2(vec2 uv){
    float cross_height = .1;
    float cross_width = .4;
    float circle_radius = .21;
    float circle_stroke = .2;
    
    return circleX(uv, cross_height, cross_width, circle_radius, circle_stroke);
}

//44
float circleXPattern3(vec2 uv){
    float cross_height = .1;
    float cross_width = .1;
    float circle_radius = .21;
    float circle_stroke = .1;
    
    return circleX(uv, cross_height, cross_width, circle_radius, circle_stroke);
}

//45
float circleXPattern4(vec2 uv){
    float cross_height = .1;
    float cross_width = .4;
    float circle_radius = .0;
    float circle_stroke = .2;
    
    return circleX(uv, cross_height, cross_width, circle_radius, circle_stroke);
}

//46
float circleXPattern5(vec2 uv){
    float cross_height = .1;
    float cross_width = .3;
    float circle_radius = .0;
    float circle_stroke = -.1;
    
    return circleX(uv, cross_height, cross_width, circle_radius, circle_stroke);
}

//47
float circleXPattern6(vec2 uv){
    float cross_height = .1;
    float cross_width = .5;
    float circle_radius = .1;
    float circle_stroke = .1;
    
    return circleX(uv, cross_height, cross_width, circle_radius, circle_stroke);
}

//48
float fabricArmorPattern(vec2 U) { 
    vec2 d = vec2(1,.65);
    
    float a = C(U, d-d );
    float b = C(U, d/2.);
    float c = C(U, d   );
    
    float res = a + (b/2.) + (c/4.);
    
    return res;;
}

//49
float arrowPattern(vec2 p)
{
    float t = 100.;

    vec2 p1 = vec2(p.x+p.y,p.x-p.y);
    vec2 f1xy = abs(fract(p1 / sqrt(8.0)) - 0.5) - 0.25;
    f1xy = clamp(f1xy*t+0.5,0.0,1.0);
    float f1 = mix(f1xy.x, 1.0 - f1xy.x, f1xy.y);

    vec2 fxy = vec2(p.x - sqrt(0.125), p.y);
  	fxy = abs(fract((fxy * sqrt(2.0) + 0.5) / 2.0) - 0.5) - 0.25;
    fxy = clamp(fxy * t / sqrt(2.0) + 0.5, 0.0, 1.0);
    float f = mix(fxy.x, 1.0 - f1, fxy.y);

	return f;
}
#define S(x,y,l) smoothstep(e,-e, length(U+vec2(x,y))-l)

//50
float discPattern1(vec2 U ) 
{
    vec2 V = ceil(U);
    float e = 0.01, l=U.y;
    float O = 0.;
    O = 0.; 
    U = 2.*fract(U)-1.;
	O +=   S(0,0,.9) - S(0,0,.53);         // white disk + central dot
	O -=   S(-1,0,.3) + S(1,0,.3)         // rotating dots
         + S(0,-1,.3) + S(0,1,.3);

    return O;
}

//51 
float discPattern2(vec2 U ) 
{
    vec2 V = ceil(U);
    float e = 0.01, l=U.y;
    float O = 0.;
    O = 0.; 
    U = 2.*fract(U)-1.;
	O +=   S(0,0,.9) - S(0,0,.53);         // white disk + central dot
	O -=   S(-1,0,.6) + S(1,0,.6)         // rotating dots
         + S(0,-1,.6) + S(0,1,.6);

    return O;
}

#define rx l = abs(length(U-vec2(i,0))-.85); z= smoothstep(.06,.0, l-.08) *(.7+.4*(U.x-i++)); if (z>M) L=l, M=z;

//52
float chainClothPattern(vec2 U) 
{
    U /= 10.;
    
    float l = 0.;
    float L = 0.; 
    float z = 0.;
    float M = 0.; 
    float i = -1.;
    
    U = 2.*fract(U*5.) -1.;
    
    rx rx rx     // offset i = -1, 0, 1    // even lines: tile = 1 circle + 2 half circles

    U.x = -U.x;                          // odd line: horizontal 1/2 offset + symmetry
    U.y -= sign(U.y);
    i=-1.; 
    
    rx rx rx    // offset i = -1, 0, 1 

    return (1.1-L/.1) * M; // tore shading * pseudo-deph * gold
}

//53
float circlePattern(vec2 U)
{
    float Pi = 3.14159265359;
    float e = 0.005; 
    float v = 0.;
    
    float O = 1.;
    
    v = ceil(U.y);
    U.x += .5*v;
    U = 2.*fract(U)-1.;

	O *= smoothstep(e,-e, length(U)-.6);
    
    return O;
}

//54
float geometricPattern1(in vec2 uv){
    float d = distField(uv,0);
    // Output to screen
    return (1. - smoothstep(0., 0.01, d));
}

//55
float geometricPattern2(in vec2 uv){
    float d = distField(uv,1);
    // Output to screen
    return (1. - smoothstep(0., 0.01, d));
}

//56
float whirlPattern(vec2 uv)
{ 
    vec4 O;
    vec2 U = uv;          // centered coords
    
    U *= mat2(1,-1./1.73, 0,2./1.73);                     // conversion to
    vec3 g = vec3(U, 1.-U.x-U.y), g2,                     // hexagonal coordinates
         id = floor(g);                                   // cell id
    
    g = fract(g); g.z = 1.-g.x-g.y;                       // triangle coords     
    g2 = abs(2.*g-1.);                                    // distance to borders

    U = id.xy * mat2(1,.5, 0,1.73/2.);
    float l00 = length(U-uv),                    // screenspace distance to nodes
          l10 = length(U+vec2(1,0)-uv), 
          l01 = length(U+vec2(.5,1.73/2.)-uv),
          l11 = length(U+vec2(1.5,1.73/2.)-uv),
            l = min(min(l00, l10), min( l01, l11)); // closest node: l=dist, C=coord
    vec2 C = U+ ( l==l00 ? vec2(0) : l==l10 ? vec2(1,0) : l==l01 ? vec2(.5,1.73/2.) : vec2(1.5,1.73/2.) );
    U = uv-C;
    float  s = 2.*mod(ceil(C.x+C.y),2.)-1.,
           r = length(U)/(1.73/2.)*3.,
           a = atan(U.y,U.x) - 3.;
    O = pow(.5+.5*s*sin(8.*log(r)+a),1.) * exp(-.3*r*r) * sin(r+vec4(1,2.1,-2.1,0));
    return O.r;
}

//57
float hexagonalPipelinePattern(in vec2 fragCoord){

    // Aspect correct screen coordinates.
    vec2 uv = fragCoord;
    uv /= 20.;
    
    // Scaling and translation.
    const float sc = 3.;
    vec2 p = uv*sc - vec2(-1, -.5);
    
    // Smoothing factor.
    float sf = 0.0001;
    
    
    // Hexagonal grid coordinates.
    vec4 p4 = getGrid(p);
    
    
    // Hexagon vertex IDs. They're useful for neighboring edge comparisons, etc.
    // Multiplying them by "s" gives the actual vertex postion.
    #ifdef FLAT_TOP
    // Vertices: Clockwise from the left.

	vec2[6] vID = vec2[6](vec2(-1./3., 0), vec2(-1./6., .5), vec2(1./6., .5), 
                          vec2(1./3., 0), vec2(1./6., -.5), vec2(-1./6., -.5)); 
    
    //vec2[6] eID = vec2[6](vec2(-.25, .25), vec2(0, .5), vec2(.25, .25), 
                         // vec2(.25, -.25), vec2(0, -.5), vec2(-.25, -.25));
    
    #else
    // Vertices: Clockwise from the bottom left. -- Basically, the ones 
    // above rotated anticlockwise. :)
    vec2[6] vID = vec2[6](vec2(-.5, -1./6.), vec2(-.5, 1./6.), vec2(0, 1./3.), 
                          vec2(.5, 1./6.), vec2(.5, -1./6.), vec2(0, -1./3.));
     
    //vec2[6] eID = vec2[6](vec2(-.5, 0), vec2(-.25, .25), vec2(.25, .25), vec2(.5, 0), 
                          //vec2(.25, -.25), vec2(-.25, -.25));
 
    #endif

   
    // The scene color.
    vec3 col = vec3(1);

    
    // Rendering the six overlapping hexagons within each cell.
    for(int i = 0; i<6; i++){
        
  
        // Corner hexagon.
        vec2 q = abs(p4.xy - vID[5-i]*s*.5);
        float hx = getHexX(q) - .265;
        float oHx = hx;

        // Using the neighboring hexagon to chop out one third. This way, the final
        // hexagon will look like it's tucked in behind the previous one... Comment
        // out the third (hx) line to see what I mean. By the way, you don't have to
        // do this, but I prefer this particular look.
        q = abs(p4.xy - vID[(5-i + 5)%6]*s/2.);
        float hx2 = getHexX(q) - .27;
        hx = max(hx, -hx2);

        // Using the triangle wave formula to render some concentric lines on each
        // hexagon.
        float pat = (1. - abs(fract(oHx*16. + .2) - .5)*2.) - .55;
        pat = smoothstep(0., .2, pat);
        
        // Rendering the chopped out hexagon and a smaller white center.
        col = mix(col, vec3(1)*pat, 1. - smoothstep(0., sf, hx));  
        col = mix(col, vec3(1), 1. - smoothstep(0., sf, max(oHx + .22, -hx2)));
        // A colorful center, if preferred.
        //col = mix(col, vec3(1, .05, .1), 1. - smoothstep(0., sf, max(oHx + .22, -hx2))); 
        
        // Applying a shadow behind the hexagon. I thought it added more visual interest, 
        // but for something like wallpaper, or whatever, you could comment it out.
        vec3 sh = mix(col, vec3(0), (1. - smoothstep(0., sf, hx)));
        col = mix(col, sh, (1. - smoothstep(0., sf*8., max(max(hx, hx2), - hx2)))*.5);

    }
    
        
    // Rendering the grid boundaries, or just some black hexagons in the center.
    float gHx = getHexX(p4.xy);
    
    #ifdef SHOW_GRID 
    // Grid lines.
    col = mix(col, vec3(0), 1. - smoothstep(0., sf, abs(gHx - .5) - .035));  
    col = mix(col, vec3(1, .05, .1)*1.5, 1. - smoothstep(0., sf, abs(gHx - .5) - .0075));  
    col = mix(col, vec3(0), (1. - smoothstep(0., sf*5., gHx - .02 - .025))*.5);
    // Colored center hexagon.
    col = mix(col, vec3(0), 1. - smoothstep(0., sf, gHx - .02 - .025));   
    col = mix(col, vec3(1, .05, .2), 1. - smoothstep(0., sf, gHx - .015));   
    #else
    // Small shadowed center hexagon.
    col = mix(col, vec3(0), (1. - smoothstep(0., sf*5., gHx - .02))*.5);   
    col = mix(col, vec3(0), 1. - smoothstep(0., sf, gHx - .02));  
    #endif
    
    // Vignette.
    //uv = fragCoord/iResolution.xy;
    //col *= pow(16.*uv.x*uv.y*(1. - uv.x)*(1. - uv.y) , .0625);

    // Rough gamma correction.
    return sqrt(max(col.r, 0.));
}

//58
float windTurbinePattern(vec2 U )
{
    float O = 0.;
    
    vec2 f = floor(U), u = 2. * fract(U) - 1.;
    float b = mod(f.x + f.y, 2.), y;

    for(int i = 0; i < 4; i++){
        u *= mat2(0,-1,1,0);
        y = 0.7;
	    O += smoothstep(.55,.45, length(u-vec2(.5,1.5*y)));
    } 
        
   
    if (b>0.) 
        O = 1.-O;
        
    return O;
}

//59
float hexaWeawingPattern3(vec2 p){
    return hexaWeawing2(p, .2);
}

//60
float asaNoHaPattern(vec2 uv) //From https://www.shadertoy.com/view/MtlXDS
{
	float scale = 1.7;
	float AA_epsilon = 0.0001;

	return scene(uv, AA_epsilon);
}

//61
float smoothCirclePattern1(vec2 uv)
{
    float v = ceil(uv.y);
    uv.x += .5*v;
    uv = fract(uv);
	
	vec2 muv = vec2(0.001);
	vec2 muv2 = 2.0*(muv-0.5);
	float rigid = length(muv2);
	
	vec2 uv2 = 2.0*(uv-0.5);
	float r = length(uv2);	
	float StartPos = 1.0-rigid;
	float EndPos = 1.0;
	
	float Value =1.0;
	if(r>StartPos)
	{
		Value = 1.0-(r-StartPos)/(EndPos-StartPos);
	}
    
    return Value;
}

//62
float smoothCirclePattern2(vec2 uv)
{
    uv = fract(uv);
	
	vec2 muv = vec2(0.001);
	vec2 muv2 = 2.0*(muv-0.5);
	float rigid = length(muv2);
	
	vec2 uv2 = 2.0*(uv-0.5);
	float r = length(uv2);	
	float StartPos = 1.0-rigid;
	float EndPos = 1.0;
	
	float Value =1.0;
	if(r>StartPos)
	{
		Value = 1.0-(r-StartPos)/(EndPos-StartPos);
	}
    
    return Value;
}

vec3 getFabric(vec2 uv, int style, float stripeFrequency, float gapFrequency){
    
    if(style == 0){
        // Generate a fabric-like pattern using a combination of noise and stripes
        float noise = sin(uv.x * 5.0) * sin(uv.y * 5.0) * 0.2 + 0.8;  // Noise pattern
        float stripesx = sin(uv.x * stripeFrequency) * 0.5 + 0.5;  // Vertical stripes pattern
        float stripesy = sin(uv.y * gapFrequency) * 0.5 + 0.5;
        //float stripesz = sin(uv.z * stripeFrequency) * 0.5 + 0.5;
  
        // Combine the patterns to create the fabric texture
        vec3 color = vec3(noise * stripesx);
        vec3 colory = vec3(noise * stripesy);
        color = cos(color);
        colory = cos(colory);
  
        return color * colory;
    }
    else{
        float stripe = sin(uv.y * stripeFrequency) * 0.5 + 0.5;  // Vertical stripe pattern
        float weave = sin((uv.x + uv.y) * gapFrequency) * 0.5 + 0.5;  // Diagonal weave pattern

        // Combine the patterns to create the fabric texture
        vec3 color = vec3(stripe * weave);

        return vec3(1.) - color;
    }

    return vec3(0.);
}

//63
float fabric1(vec2 uv){
    return getFabric(uv, 1, 10., 30.).r;
}

//64
float fabric2(vec2 uv){
    return getFabric(uv, 0, 10., 30.).r;
}

//64
float fabric3(vec2 uv){
    return getFabric(uv, 0, 10., 10.).r;
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

// marble variant: https://www.shadertoy.com/view/Xs3fR4
// integrated with cracks here: https://www.shadertoy.com/view/Xd3fRN

#define MM 0

#define VARIANT 0              // 1: amplifies Voronoi cell jittering
#if VARIANT
      float ofs = .5;          // jitter Voronoi centers in -ofs ... 1.+ofs
#else
      float ofs = 0.;
#endif
    
int FAULT = 1;                 // 0: crest 1: fault

float RATIO = 2.,              // stone length/width ratio
      STONE_slope = .3,        // 0.  .3  .3  -.3
      STONE_height = 1.,       // 1.  1.  .6   .7
      profile = 1.,            // z = height + slope * dist ^ prof
    
      CRACK_zebra_scale = 1.5, // fractal shape of the fault zebra
      CRACK_zebra_amp = 1.7,
      CRACK_profile = .2,      // fault vertical shape  1.  .2 
      CRACK_slope = 1.4,       //                      10.  1.4
      CRACK_width = .0;
    

// std int hash, inspired from https://www.shadertoy.com/view/XlXcW4
vec3 hash3( uvec3 x ) 
{
#   define scramble  x = ( (x>>8U) ^ x.yzx ) * 1103515245U // GLIB-C const
    scramble; scramble; scramble; 
    return vec3(x) / float(0xffffffffU) +1e-30; // <- eps to fix a windows/angle bug
}

// === Voronoi =====================================================
// --- Base Voronoi. inspired by https://www.shadertoy.com/view/MslGD8

#define hash22(p)  fract( 18.5453 * sin( p * mat2(127.1,311.7,269.5,183.3)) )
#define disp(p) ( -ofs + (1.+2.*ofs) * hash22(p) )

vec3 voronoi( vec2 u )  // returns len + id
{
    vec2 iu = floor(u), v;
	float m = 1e9,d;
#if VARIANT
    for( int k=0; k < 25; k++ ) {
        vec2  p = iu + vec2(k%5-2,k/5-2),
#else
    for( int k=0; k < 9; k++ ) {
        vec2  p = iu + vec2(k%3-1,k/3-1),
#endif
            o = disp(p),
      	      r = p - u + o;
		d = dot(r,r);
        if( d < m ) m = d, v = r;
    }

    return vec3( sqrt(m), v+u );
}

// --- Voronoi distance to borders. inspired by https://www.shadertoy.com/view/ldl3W8
vec3 voronoiB( vec2 u )  // returns len + id
{
    vec2 iu = floor(u), C, P;
	float m = 1e9,d;
#if VARIANT
    for( int k=0; k < 25; k++ ) {
        vec2  p = iu + vec2(k%5-2,k/5-2),
#else
    for( int k=0; k < 9; k++ ) {
        vec2  p = iu + vec2(k%3-1,k/3-1),
#endif
              o = disp(p),
      	      r = p - u + o;
		d = dot(r,r);
        if( d < m ) m = d, C = p-iu, P = r;
    }

    m = 1e9;
    
    for( int k=0; k < 25; k++ ) {
        vec2 p = iu+C + vec2(k%5-2,k/5-2),
		     o = disp(p),
             r = p-u + o;

        if( dot(P-r,P-r)>1e-5 )
        m = min( m, .5*dot( (P+r), normalize(r-P) ) );
    }

    return vec3( m, P+u );
}

// === pseudo Perlin noise =============================================
#define rotXX(a) mat2(cos(a),-sin(a),sin(a),cos(a))
int MOD = 1;  // type of Perlin noise

// --- 3D 
#define hash31(p) fract(sin(dot(p,vec3(127.1,311.7, 74.7)))*43758.5453123)
float noise13(vec3 p) {
    vec3 i = floor(p);
    vec3 f = fract(p); f = f*f*(3.-2.*f); // smoothstep

    float v= mix( mix( mix(hash31(i+vec3(0,0,0)),hash31(i+vec3(1,0,0)),f.x),
                       mix(hash31(i+vec3(0,1,0)),hash31(i+vec3(1,1,0)),f.x), f.y), 
                  mix( mix(hash31(i+vec3(0,0,1)),hash31(i+vec3(1,0,1)),f.x),
                       mix(hash31(i+vec3(0,1,1)),hash31(i+vec3(1,1,1)),f.x), f.y), f.z);
	return   MOD==0 ? v
	       : MOD==1 ? 2.*v-1.
           : MOD==2 ? abs(2.*v-1.)
                    : 1.-abs(2.*v-1.);
}

float fbm3(vec3 p) {
    float v = 0.,  a = .5;
    mat2 R = rotXX(.37);

    for (int i = 0; i < 9; i++, p*=2.,a/=2.) 
        p.xy *= R, p.yz *= R,
        v += a * noise13(p);

    return v;
}

float crackNoise(vec3 U )
{
    vec4 O = vec4(1.);
 // O = vec4( 1.-voronoiB(U).x,voronoi(U).x, 0,0 );   // for tests
    
    vec3 V =  U / vec3(RATIO,1,RATIO),                      // voronoi cell shape
         D = vec3(fbm3(CRACK_zebra_scale*U) / CRACK_zebra_scale / CRACK_zebra_amp);
    vec3  H = vec3(innerVoronoi( V + D ));
    float d = H.x,                                    // distance to cracks
          r = innerVoronoi(V),                           // distance to center
          s = STONE_height-STONE_slope*pow(r,profile);// stone interior
                                                      // cracks
    d = min( 1., CRACK_slope * pow(max(0.,d-CRACK_width),CRACK_profile) );
  
    O = vec4( 
        FAULT==1 ? d * s                              // fault * stone
                 : mix(1.,s, d)                       // crest or stone
            );

    return O.r;
}

// gyroid pattern
float gyroid (vec3 p)
{
    return dot(sin(p),cos(p.yzx));
}

// noise
float fbm (vec3 p)
{
    vec3 q = p;
    float result = 0., a = .5;
    for (float i = 0.; i < 4.; ++i, a /= 2.)
    {
        q += (result-.5);
        result += (innerNoise2D(q/a)*2. + gyroid(q/a)) * a;
    }
    return result;
}

float gyroidNoise1(vec3 p)
{
    // shape
    float e = .3;
    float diff = abs(fbm(p + vec3(0,e,0)) - fbm(p - vec3(0,e,0)))/e;
    float circle = -0.1;
    float thin = smoothstep(.5,.0,circle) * 2.;
    float shape = smoothstep(.01/thin,0.,diff-thin+.1);

    return shape;
}

float gyroidNoise2(vec3 p)
{
    // shape
    float e = .3;
    float diff = abs(fbm(p + vec3(0,e,0)) - fbm(p - vec3(0,e,0)))/e;
    float circle = 0.3;
    float thin = smoothstep(.5,.0,circle) * 2.;
    float shape = smoothstep(.01/thin,0.,diff-thin+.1);

    return shape;
}

//0
float smartPos(vec3 pos, float offset, float yaw, float pitch, float position){

    yaw = radians(yaw);
    pitch = radians(pitch);

    pos = innerrotate(pos, vec3(0.,1.,0.), yaw);
    pos = innerrotate(pos, vec3(1.,0.,0.), pitch);

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
    
    pos = innerrotate(pos, vec3(0., 0., 1.), radians(rotation));

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

#define pow2(x) (x * x)



float gaussian(vec3 i) {
    const int samples = 16;
    const float piX = atan(1.0) * 4.0;
    const float sigma = float(samples) * 0.25;
    
    return 1.0 / (2.0 * piX * pow2(sigma)) * exp(-((pow2(i.x) + pow2(i.y)) / (2.0 * pow2(sigma))));
}

float smartDistance(vec3 pos, float mn, float mx, float blurStrength, float noiseStrength, vec2 txtrRes){
    const vec3 originPos = vec3(0);

    float accum = 0.0;
    float weight;
    vec3 offset;

    const int samples = 8;

    float res = 0.;
    for (int x = -samples / 2; x < samples / 2; ++x) {
        for (int y = -samples / 2; y < samples / 2; ++y) {
            for (int z = -samples / 2; z < samples / 2; ++z){
                offset = vec3(x, y, z);
                weight = gaussian(offset); 
                    
                vec3 p = pos + (1./txtrRes.x) * offset * (blurStrength);

                float dist = distance(p, originPos);
                if(dist > mn && dist < mx)
                    res += 1. * weight;

                accum += weight;
            }
        }
    }
    
    float m = res / accum;
    float orgM = m;

    float noise = musgraveDefNoiseA(pos);

    if(m < 1. && m > 0.){
        if(m > 0.5)
            m = mix(m * noise, m, m);
        else
            m *= noise;
    }

    m = mix(orgM, m, noiseStrength);

    return m;
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
        res = rhombPattern2(uv);
    else if(proceduralID == 30)
        res = truchetPattern(uv);
    else if(proceduralID == 31)
        res = greekFriezePattern1(uv);
    else if(proceduralID == 32)
        res = greekFriezePattern2(uv);
    else if(proceduralID == 33)
        res = basketPattern4(uv);
    else if(proceduralID == 34)
        res = basketPattern5(uv);
    else if(proceduralID == 35)
        res = pipelinePattern(uv);
    else if(proceduralID == 36)
        res = hexaWeawingPattern1(uv);
    else if(proceduralID == 37)
        res = hexaWeawingPattern2(uv);
    else if(proceduralID == 38)
        res = hexagonalInterlacingPattern(uv);
    else if(proceduralID == 39)
        res = chainPattern1(uv);
    else if(proceduralID == 40)
        res = chainPattern2(uv);
    else if(proceduralID == 41)
        res = chainPattern3(uv);
    else if(proceduralID == 42)
        res = circleXPattern1(uv);
    else if(proceduralID == 43)
        res = circleXPattern2(uv);
    else if(proceduralID == 44)
        res = circleXPattern3(uv);
    else if(proceduralID == 45)
        res = circleXPattern4(uv);
    else if(proceduralID == 46)
        res = circleXPattern5(uv);
    else if(proceduralID == 47)
        res = circleXPattern6(uv);
    else if(proceduralID == 48)
        res = fabricArmorPattern(uv);
    else if(proceduralID == 49)
        res = arrowPattern(uv);
    else if(proceduralID == 50)
        res = discPattern1(uv);
    else if(proceduralID == 51)
        res = discPattern2(uv);
    else if(proceduralID == 52)
        res = chainClothPattern(uv);
    else if(proceduralID == 53)
        res = circlePattern(uv);
    else if(proceduralID == 54)
        res = geometricPattern1(uv);
    else if(proceduralID == 55)
        res = geometricPattern2(uv);
    else if(proceduralID == 56)
        res = whirlPattern(uv);
    else if(proceduralID == 57)
        res = hexagonalPipelinePattern(uv);
    else if(proceduralID == 58)
        res = windTurbinePattern(uv);
    else if(proceduralID == 59)
        res = hexaWeawingPattern3(uv);
    else if(proceduralID == 60)
        res = asaNoHaPattern(uv);
    else if(proceduralID == 61)
        res = smoothCirclePattern1(uv);
    else if(proceduralID == 62)
        res = smoothCirclePattern2(uv);
    else if(proceduralID == 63)
        res = fabric1(uv);
    else if(proceduralID == 64)
        res = fabric2(uv);
    else if(proceduralID == 65)
        res = fabric3(uv);






    else if(proceduralID == 66)
        res = basicNoiseA(pos);
    else if(proceduralID == 67)
        res = basicStretchedNoiseA(pos);
    else if(proceduralID == 68)
        res = fbmNoiseA(pos);
    else if(proceduralID == 69)
        res = fbmHightPersistanceNoiseA(pos);
    else if(proceduralID == 70)
        res = fbmLowOctaveNoiseA(pos);
    else if(proceduralID == 71)
        res = fbmWaveNoiseA(pos);
    else if(proceduralID == 72)
        res = fbmWaveLowPersistanceNoiseA(pos);
    else if(proceduralID == 73)
        res = fbmWaveHighPersistanceNoiseA(pos);
    else if(proceduralID == 74)
        res = musgraveDefNoiseA(pos);
    else if(proceduralID == 75)
        res = musgraveHighDimensionalNoiseA(pos);
    else if(proceduralID == 76)
        res = gradientDefNoiseA(pos);
    else if(proceduralID == 77)
        res = innerWorleyNoiseA(pos);
    else if(proceduralID == 78)
        res = worleyPatternalNoiseA(pos);
    else if(proceduralID == 79)
        res = circularPerlinNoiseA(pos);
    else if(proceduralID == 80)
        res = voronoiNoiseA(pos);
    else if(proceduralID == 81)
        res = voronoiDeeperNoiseA(pos);
    else if(proceduralID == 82)
        res = staticNoiseA(pos);
    else if(proceduralID == 83)
        res = wallNoise1(pos);
    else if(proceduralID == 84)
        res = wallNoise2(pos);
    else if(proceduralID == 85)
        res = wallNoise3(pos);
    else if(proceduralID == 86)
        res = wallNoise4(pos);
    else if(proceduralID == 87)
        res = wallNoise5(pos);
    else if(proceduralID == 88)
        res = wallNoise6(pos);
    else if(proceduralID == 89)
        res = pebbleNoise1(pos);
    else if(proceduralID == 90)
        res = pebbleNoise2(pos);
    else if(proceduralID == 91)
        res = pebbleNoise3(pos);
    else if(proceduralID == 92)
        res = pebbleNoise4(pos);
    else if(proceduralID == 93)
        res = pebbleNoise5(pos);
    else if(proceduralID == 94)
        res = pebbleNoise6(pos);
    else if(proceduralID == 95)
        res = pebbleNoise7(pos);
    else if(proceduralID == 96)
        res = scratchNoise1(pos);
    else if(proceduralID == 97)
        res = scratchNoise2(pos);
    else if(proceduralID == 98)
        res = dropletsNoise1(pos);
    else if(proceduralID == 99)
        res = dropletsNoise2(pos);
    else if(proceduralID == 100)
        res = dropletsNoise3(pos);
    else if(proceduralID == 101)
        res = dropletsNoise4(pos);
    else if(proceduralID == 102)
        res = dropletsNoise5(pos);
    else if(proceduralID == 103)
        res = crackNoise(pos);
    else if(proceduralID == 104)
        res = gyroidNoise1(pos);
    else if(proceduralID == 105)
        res = gyroidNoise2(pos);
    
    


    else if(proceduralID == 106)
        res = smartPos(pos, smartProperties.x, smartProperties.y, smartProperties.z, smartProperties.w);
    else if(proceduralID == 107)
        res = smartPos2(pos, smartProperties.x, smartProperties.y, smartProperties.z, smartProperties.w);
    else if(proceduralID == 108)
        res = smartPos3(pos, smartProperties.x, smartProperties.y, smartProperties.z, smartProperties.w);
    else if(proceduralID == 109)
        res = smartStripes(pos, smartProperties.x, smartProperties.y, smartProperties.z, smartProperties.w, txtrRes);
    else if(proceduralID == 110)
        res = smartStripes2(pos, smartProperties.x, smartProperties.y, smartProperties.z, smartProperties.w, txtrRes);
    //else if(proceduralID == 111)
    //   edge wear
    else if(proceduralID == 112)
        res = smartDistance(pos, smartProperties.x, smartProperties.y, smartProperties.z, smartProperties.w, txtrRes);
    else
        res = 1.;

    if(res > 1.)
        res = 1.;
    
    if(res < 0.)
        res = 0.;

    return res;
}

vec4 getProcedural(vec3 pos, int proceduralID, sampler2D txtr, vec2 texCoord, float scale, int inverted, vec4 smartProperties, vec2 txtrRes, int proceduralGrayScale, float proceduralBrightness){
    vec4 res = vec4(0.);
    if(proceduralID != -1)
        res = vec4(vec3(getProceduralVal(pos, proceduralID, scale, inverted, texCoord, smartProperties, txtrRes)), 1);
    else{
        res = texture(txtr, texCoord * scale);
    }
    
    if(proceduralGrayScale == 1)
        res.rgb = vec3(max(max(res.r, res.g), res.b)); 

    res.rgb *= proceduralBrightness;

    if(inverted == 0)
        return res;   
    else
        return vec4(vec3(1.) - res.rgb, res.a);   
}