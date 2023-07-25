#version 400 core

#define PI 3.141592

/* --------------------------------------------------------- NOISE FUNCTIONS ---------------------------------------------------------*/

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
