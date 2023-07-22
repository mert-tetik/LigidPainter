//From https://www.shadertoy.com/view/lt3fDS
//---------------------------------------------------------
// Shader: 2dProceduralPattern.glsl  by gPlatl
//
// tags:   procedural, pattern, 2d, basic, texture, collection
// note:   procedural pattern routines will return values from 0.0 .. 1.0
//
// references:
//   Procedural Patterns          http://slideplayer.com/slide/6400090/
//   Antialiasing Proc. Textures  http://www.yaldex.com/open-gl/ch17lev1sec4.html
//---------------------------------------------------------

#version 400 core

in vec2 TexCoords;

uniform vec2 shapeScale;
uniform int shapeIndex;

//0 = albedo
//1 = roughness
//2 = metallic 
//3 = normal map
//4 = height map
//5 = ambient Occlusion
uniform int state;

/* Channel/State Properties*/
uniform float opacity;
uniform vec3 value;
uniform sampler2D previousTxtr;

out vec4 fragColor; 



#define iResolution vec3(1)
#define iTime 1
#define iMouse vec3(1)


#define patternCount 23.0
#define PI 3.141592

#define pi acos(-1.)
#define dd(a) dot(a,a)
#define sat(a) clamp(a,0.,1.)
#define aA(z) (z/min(iResolution.x,iResolution.y))


float mi(vec2 a){return min(a.x,a.y);}
float mi(vec3 a){return min(a.x,mi(a.yz));}
float ma(vec2 a){return max(a.x,a.y);}
float mu(vec2 a){return a.x*a.y;}
float ad(vec2 a){return a.x+a.y;}
float su(vec2 a){return a.x-a.y;}
float sq2(float a){return a*a;}


float GearPattern(vec2 uv,float wn,int tn ,float time){
    float g = (step(1.0, uv.x * wn) - 0.5) * time;
    uv = fract(uv * wn) - 0.5;
    float r = clamp(0.48, 0.4, 0.45 + 0.12*sin(atan(uv.x,uv.y) * float(tn) + g));
    return smoothstep(r, r + 0.01, 1.1*length(uv));
}
float CheckerSignMuFract(vec2 u)
{
    return sign(mu(.5-fract(u)));
}
float CheckerSatMuSin(vec2 u){
    return sat(88.*mu(sin(u*8.)));
}
float Checker1ByMuSin(vec2 u){
    return 1./mu(sin(u));
}
float TartanKaro(vec2 u){
    return .5*ad(sat(10.*sin(u)));
}
float TartanSquare(vec2 u){
    return 4.*mu(sat(10.*sin(u)));
}
float SquareHolePattern(vec2 u){
    u.x=mu(sin(u*2.));
    return smoothstep(.1,.0, sq2(u.x)*2.5);
 }
float Grid(vec2 u){
    return mi(abs(fract(u)-.5));
}
float BrickPattern(vec2 p)
{
    //p*=vec2 (1,2);  // scale
    vec2 f=floor(p);
    p.x-=step(f.y,2.*floor(f.y*.5))*.5;// brick shift
    p=abs (fract (p + 0.5) - 0.5);
    //p=smoothstep (0.03, 0.08, p);
    return min(p.x,p.y);
}
vec2 toTri(vec2 u){
    return vec2(u.x,(u.y*sqrt(3.)+u.x)*.5);
}
float TrianglePattern(vec2 u){
    return step(su(fract(toTri(u))),0.);
}
float RhombStar(vec2 u){
    u.x=sq2(su(fract(toTri(u))));
    return step(.25,u.x);//this is less silly
    //return smoothstep(.5,.0,u.x);//rather silly (optical illusion) //doesnt mix well with above line)
}
float hexBorder(vec2 u){
    u.x*=sqrt(3.)*2./3.;
    u.y+= 0.5 * mod(floor(u.x), 2.0);
    u = abs(fract(u)-.5);
    return abs(max(u.x*1.5+u.y,u.y*2.)-1.);
}
bool hexaGridId(vec2 a){
    float b=(1.-a.y)*.5;
    return a.y>1.&&(abs(b-a.x+a.y)>-b);
}
bool hexaGridIdX(vec2 p){ 
    return 1.>ma(p-p.yx) && max(-mi(p),ma(p)-2.)<0.;
}
float hexId3Hues(vec2 p){
    p =toTri(p);
    p = mod(p,vec2(3));
    
    if(hexaGridIdX(p))
        return .0;
    else if(hexaGridId(p))
        return .5;
    else              
        return 1.;
}

float hexTruchet(vec2 p)
{
    float s=sqrt(3.);
    vec2 h=p+vec2(s,.45)*p.y/3.;
    vec2 f=fract(h);h=floor(h);
    float v=fract(ad(h)/3.);
    h+=mix(vec2(step(.3,v)),step(f.yx,f),step(.6,v));
    p+=vec2(1,2.-sqrt(3.))*h.y*.5-h;
    v=sign(cos(1234.*cos(h.x+9.*h.y)));
    vec3 a=vec3(dd(p-v*vec2(-2, 0)*.5)
             ,dd(p-v*vec2( 1, s)*.5)
             ,dd(p-v*vec2( 1,-s)*.5));
    v=(.5-sqrt(mi(a)))*v;
    
    v=abs(v);
    v=1.-(1.-v*2.);
    float z=8.;
    //return smoothstep(aA(z),-aA(z),v-(cos(iTime)*(.5-aA(z))+.5));
    return v;
}

float sinePattern(vec2 p){
    return sin(p.x*20.+cos(p.y*12.));
}

float SinePatternCrissCross(vec2 p)
{
    return .5+sinePattern(p)*sinePattern(p.yx);
}

float lengthP(vec2 u, float p){
    return pow(pow(abs(u.x),p)+pow(abs(u.y),p),1./p);
}

float QCirclePattern(vec2 u){
    return sin(lengthP(fract(u*4./2.)*2.-1.,4.)*16.);
}

float StarPattern(vec2 p ){
    p= abs(fract(p*1.5)-.5);
    return max(ma(p),mi(p)*2.);
}

float weaveSub(vec2 u,float p){
    return step(.2,abs(fract(u.x)))*(.65+.35*sin(p*(u.y-ceil(u.x))));
}

float weave(vec2 u,float r)
{
    float a=weaveSub(u,pi/r);
    u=u.yx;
    u.y++;
    float b=weaveSub(u,pi/r);
    return max (a,b);
}

float weave(vec2 u){
    return weave(u,1.);
}

float truchetTiny70s(vec2 p,float time)
{
    p.x*=sign(cos(length(ceil(p))*time));
    return cos(min(length(p=fract(p)),length(--p))*44.);
}

float xof(float a,float b){
    return float(int(a)^int(b));
}
float anf(float a,float b){
    return float(int(a)&int(b));
}
float anf(int   a,float b){
    return float(int(a)&int(b));
}

float weaveInt(vec2 coord){
    ivec2 uv=ivec2(floor(coord*5.));
    float a = floor(mod(iTime,7.))*4.;
    float bg  =.0;
    float warp=.5;
    float weft=1.;
    vec2 f=floor(vec2(uv.xy)*.5);
    a=anf(uv.x^uv.y,a);
    vec2 h=vec2(xof(float(uv.x),f.x),xof(float(uv.y),f.y));
    h=fract(h/2.)*2.;
    vec3 i=smoothstep(1.,0.,vec3(a,h.xy));
    float d=mix(weft,bg,i.z);
    float e=mix(warp,bg,i.y);
    float c=mix(d,e,i.x);;
    return c;
}

float demo(vec2 u){
   return weaveInt(u);
}

// 2D Random
float tissrandom (in vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))
                 * 43758.5453123);
}

// 2D Noise based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
float tissnoise (vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = tissrandom(i);
    float b = tissrandom(i + vec2(1.0, 0.0));
    float c = tissrandom(i + vec2(0.0, 1.0));
    float d = tissrandom(i + vec2(1.0, 1.0));

    // Smooth Interpolation

    // Cubic Hermine Curve.  Same as SmoothStep()
    vec2 u = f*f*(3.0-2.0*f);
    // u = smoothstep(0.,1.,f);

    // Mix 4 coorners percentages
    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

float tissue(){

    vec2 st = gl_FragCoord.xy * shapeScale;
    vec2 pos = vec2(st*5.0);
    float n = tissnoise(pos);

    return n;
}

float getResult(vec2 uv){
    float res;
    
    if      (shapeIndex == 0) 
        res = hexBorder (uv*1.25);
    else if (shapeIndex == 1) 
        res = hexId3Hues(uv*2.0);
    else if (shapeIndex == 2) 
        res = hexTruchet (uv*3.);
    else if (shapeIndex == 3) 
        res = CheckerSignMuFract(uv);
    else if (shapeIndex == 4) 
        res = Checker1ByMuSin(uv*8.0);
    else if (shapeIndex == 5) 
        res = TrianglePattern(uv);
    else if (shapeIndex == 6) 
        res = RhombStar(uv);
    else if (shapeIndex == 7) 
        res = TartanKaro(uv);
    else if (shapeIndex == 8) 
        res = TartanSquare(uv);
    else if (shapeIndex == 9) 
        res = SquareHolePattern(uv);
    else if (shapeIndex == 10) 
        res = sinePattern(uv);
    else if (shapeIndex == 11) 
        res = Grid(uv);
    else if (shapeIndex == 12) 
        res = GearPattern(uv, 1.5, 12, iTime * 6.5);
    else if (shapeIndex == 13) 
        res = QCirclePattern(uv);
    else if (shapeIndex == 14) 
        res = StarPattern(uv);
    else if (shapeIndex == 15) 
        res = weave(uv);
    else if (shapeIndex == 16) 
        res = weave(uv,2.);
    else if (shapeIndex == 17) 
        res = weave(uv,3.);
    else if (shapeIndex == 18) 
        res = weave(uv,4.);
    else if (shapeIndex == 19) 
        res = truchetTiny70s(uv, iTime*0.1);
    else if (shapeIndex == 20) 
        res = weaveInt(uv);
    else if (shapeIndex == 21) 
        res = tissue();
    else                 
        res = demo(uv);

    return res;
}

void main(){
    float aspect = iResolution.y / iResolution.x;
    
    vec2 mpos = iMouse.xy / iResolution.y;
    
    vec2 uv = TexCoords.xy;
    uv*=shapeScale;

    float time = iTime;

    uv *= (0.2 + mpos.y) * 10.0;

    float res = getResult(uv);

    fragColor = vec4(vec3(res),1);
}