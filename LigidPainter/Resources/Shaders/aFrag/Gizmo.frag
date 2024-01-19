
#version 400 core

in vec2 TexCoords;
out vec4 fragColor;

//from : https://www.shadertoy.com/view/mscfzj

uniform mat4 rotMat;
//0 : arrows, 1 : cubes, 2 : circular
uniform int state;
uniform int solidShading;

//https://www.shadertoy.com/view/mscfzj

uniform float rod_radius;
const float rod_length = 0.3;
const float head_length = 0.1;

 vec3 red = vec3(180.0 / 255.0, 82.0 / 255.0, 82.0 / 255.0);
 vec3 green = vec3(138.0 / 255.0, 176.0 / 255.0, 96.0 / 255.0);
 vec3 blue = vec3(75.0 / 255.0, 128.0 / 255.0, 202.0 / 255.0);
 vec3 white = vec3(242.0 / 255.0, 240.0 / 255.0, 229.0 / 255.0);
 vec3 gray = vec3(134.0 / 255.0, 129.0 / 255.0, 136.0 / 255.0);
 vec3 black = vec3(33.0 / 255.0, 33.0 / 255.0, 35.0 / 255.0);

uniform int rHover;
uniform int gHover;
uniform int bHover;
uniform int rHoverN;
uniform int gHoverN;
uniform int bHoverN;

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


vec4 sdCone(vec3 p, vec3 a, vec3 b, float ra, float rb, vec3 color)
{
    float rba  = rb-ra;
    float baba = dot(b-a,b-a);
    float papa = dot(p-a,p-a);
    float paba = dot(p-a,b-a)/baba;

    float x = sqrt( papa - paba*paba*baba );

    float cax = max(0.0,x-((paba<0.5)?ra:rb));
    float cay = abs(paba-0.5)-0.5;

    float k = rba*rba + baba;
    float f = clamp( (rba*(x-ra)+paba*baba)/k, 0.0, 1.0 );

    float cbx = x-ra - f*rba;
    float cby = paba - f;
    
    float s = (cbx < 0.0 && cay < 0.0) ? -1.0 : 1.0;
    
    return vec4(color, s*sqrt( min(cax*cax + cay*cay*baba,
                       cbx*cbx + cby*cby*baba) ));
}

vec4 sdRoundBox( vec3 p, vec3 b, float r, vec3 color )
{
  vec3 q = abs(p) - b;
  return vec4(color, length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0) - r);
}

vec4 opRound(vec4 primitive, float rad )
{
    primitive.w -= rad;
    return primitive;
}

vec4 sdCapsule( vec3 p, vec3 a, vec3 b, float r, vec3 color)
{
  vec3 pa = p - a, ba = b - a;
  float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
  return vec4(color, length( pa - ba*h ) - r);
}

vec4 sdSphere( vec3 p, float s, vec3 color)
{
  return vec4(color, length(p)-s);
}

vec4 sdCappedTorus( vec3 p, vec2 sc, float ra, float rb, vec3 color)
{
  p.x = abs(p.x);
  float k = (sc.y*p.x>sc.x*p.y) ? dot(p.xy,sc) : length(p.xy);
  return vec4(color, sqrt( dot(p,p) + ra*ra - 2.0*ra*k ) - rb);
}


vec4 opSmoothUnion( vec4 d1, vec4 d2, float k ) {
    float h = clamp( 0.5 + 0.5*(d2.w-d1.w)/k, 0.0, 1.0 );
    return mix( d2, d1, h ) - k*h*(1.0-h); }

vec4 opU( vec4 d1, vec4 d2 )
{
  if (d2.w < d1.w) return d2;
  return d1;
}

vec4 arrow(vec3 pos, vec3 dir, vec3 color) {
    float head_radius = 0.05;
    float head_offset = rod_length;

    vec4 sdf = sdCapsule(pos, vec3(0, 0, 0), dir * rod_length, rod_radius, color);
    sdf = opU(sdf, opRound(sdCone(pos, dir * head_offset, dir * (head_offset + head_length), head_radius, 0.0, color), 0.01));
    return sdf;
}

vec4 arrow_scale(vec3 pos, vec3 dir, vec3 color) {

    float head_radius = head_length / 2.0;
    float head_offset = rod_length + head_radius;

    vec4 sdf = sdCapsule(pos, vec3(0, 0, 0), dir * rod_length, rod_radius, color);
    sdf = opU(sdf, sdRoundBox(pos - dir * head_offset, vec3(head_radius), 0.01, color));
    return sdf;
}

mat4 rotationMatrix(vec3 axis, float angle) {
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

vec3 opTx( in vec3 p, in mat4 t)
{
    return (inverse(t) * vec4(p, 0.0)).xyz;
}

float gridTextureGradBox( in vec2 p, in vec2 ddx, in vec2 ddy )
{
     // grid ratio
    const float N = 25.0;

	// filter kernel
    vec2 w = max(abs(ddx), abs(ddy)) + 0.01;

	// analytic (box) filtering
    vec2 a = p + 0.5*w;                        
    vec2 b = p - 0.5*w;           
    vec2 i = (floor(a)+min(fract(a)*N,1.0)-
              floor(b)-min(fract(b)*N,1.0))/(N*w);
    //pattern
    return (1.0-i.x)*(1.0-i.y);
}

vec4 map(vec3 pos)
{
    if(solidShading == 1){
        red = vec3(1., 0., 0.);
        green = vec3(0., 1., 0.);
        blue = vec3(0., 0., 1.);
    }


    pos = (rotate(pos, vec3(0.,1.,0.), radians(45.)));
    pos = (rotate(pos, vec3(0.,0.,1.), radians(-35.)));

    vec4 tpos = vec4(pos,1.);
    tpos *= rotMat;
    pos = (tpos.xyz);

    float t = 0.;
    if(state == 1)
        t = 2.;
    else if(state == 2)
        t = 15.;
    
    float arrow_animation_morph = clamp((t - 3.0) * 2.0, 0.0, 1.0);
    float arrow_scale_morph = clamp((t - 1.0) * 2.0, 0.0, 1.0);

    vec4 center_sphere = sdSphere(pos, 0.05, white);

    vec4 pos_arrow = vec4(0.0);
    vec4 scale_arrow = vec4(0.0);

    
    vec3 redClr = red;
    vec3 greenClr = green;
    vec3 blueClr = blue;
    
    if(rHover == 1)
        redClr *= 2.;
    if(gHover == 1)
        greenClr *= 2.;
    if(bHover == 1)
        blueClr *= 2.;

    // arrows
    pos_arrow = arrow(pos, vec3(1.0, 0.0, 0.0), redClr);
    pos_arrow = opU(pos_arrow, arrow(pos, vec3(0.0, 1.0, 0.0), greenClr));
    pos_arrow = opU(pos_arrow, arrow(pos, vec3(0.0, 0.0, 1.0), blueClr));

    scale_arrow = arrow_scale(pos, vec3(1.0, 0.0, 0.0), redClr);
    scale_arrow = opU(scale_arrow, arrow_scale(pos, vec3(0.0, 1.0, 0.0), greenClr));
    scale_arrow = opU(scale_arrow, arrow_scale(pos, vec3(0.0, 0.0, 1.0), blueClr));
    

    redClr = red;
    greenClr = green;
    blueClr = blue;
    
    if(rHoverN == 1)
        redClr *= 2.;
    if(gHoverN == 1)
        greenClr *= 2.;
    if(bHoverN == 1)
        blueClr *= 2.;

    pos_arrow = opU(pos_arrow, arrow(pos, vec3(-1.0, 0.0, 0.0), redClr / 2.));
    pos_arrow = opU(pos_arrow, arrow(pos, vec3(0.0, -1.0, 0.0), greenClr / 2.));
    pos_arrow = opU(pos_arrow, arrow(pos, vec3(0.0, 0.0, -1.0), blueClr / 2.));

    scale_arrow = opU(scale_arrow, arrow_scale(pos, vec3(-1.0, 0.0, 0.0), redClr / 2.));
    scale_arrow = opU(scale_arrow, arrow_scale(pos, vec3(0.0, -1.0, 0.0), greenClr / 2.));
    scale_arrow = opU(scale_arrow, arrow_scale(pos, vec3(0.0, 0.0, -1.0), blueClr / 2.));

    // rotation
    float rotation_smoothing = 0.0;
    float rot_scale = mix(0.0, rod_length + 0.045, 1.0);

    pos = abs(tpos.xyz);

    //red
    vec2 c = vec2(sin(radians(180.0)),cos(radians(180.0)));
    vec4 rotation = sdCappedTorus(pos, c, rot_scale, 0.025, red);
    //green
    mat4 mat = rotationMatrix(vec3(1.0, 0.0, 0.0), radians(90.0));
    rotation = opSmoothUnion(rotation, sdCappedTorus(opTx(pos, mat), c, rot_scale, 0.025, green), rotation_smoothing);
    //blue
    mat = rotationMatrix(vec3(0.0, 1.0, 0.0), radians(90.0));
    rotation = opSmoothUnion(rotation, sdCappedTorus(opTx(pos, mat), c, rot_scale, 0.025, blue), rotation_smoothing);

    vec4 arrows = opU(center_sphere, mix(pos_arrow, scale_arrow, arrow_scale_morph));
    vec4 sdf = mix(arrows, rotation, arrow_animation_morph);
    return arrows;
}

// https://iquilezles.org/articles/normalsSDF
vec3 calcNormal( in vec3 pos )
{
    vec2 e = vec2(1.0,-1.0)*0.5773;
    const float eps = 0.0005;
    return normalize( e.xyy*map( pos + e.xyy*eps ).w + 
					  e.yyx*map( pos + e.yyx*eps ).w + 
					  e.yxy*map( pos + e.yxy*eps ).w + 
					  e.xxx*map( pos + e.xxx*eps ).w );
}

#define AA 1

// Half-Lambert shading
vec3 half_lambert(vec3 l_p, vec3 p, vec3 view, vec3 norm, vec3 col){
    vec3 l = normalize(l_p - p);
    float ldotn = clamp(dot(l, norm), 0.0, 1.0);
    float diffuse = pow((ldotn+1.)/2., 2.);
    return diffuse * col;
}

float intersect_grid( vec3 ro, vec3 rd, out vec3 pos, out vec3 nor )
{
    // raytrace
	float tmin = 10000.0;
	nor = vec3(0.0);
	pos = vec3(0.0);
	
	// raytrace-plane
	float h = (0.01-ro.y)/rd.y;
	if( h>0.0 ) 
	{ 
		tmin = h; 
		nor = vec3(0.0,1.0,0.0); 
		pos = ro + h*rd;
	}


	return tmin;	
}

void main()
{

    float gamma = 2.2;
     // camera movement	
	vec3 ro = vec3(0.8);
    vec3 ta = vec3( 0.0, 0.0, 0.0 );
    // camera matrix
    vec3 ww = normalize( ta - ro );
    vec3 uu = normalize( cross(ww,vec3(0.0,1.0,0.0) ) );
    vec3 vv =          ( cross(uu,ww));
    
    // render
    vec4 tot = vec4(0.0);
    
    #if AA>1
    for( int m=0; m<AA; m++ )
    for( int n=0; n<AA; n++ )
    {
        // pixel coordinates
        vec2 o = vec2(float(m),float(n)) / float(AA) - 0.5;
        vec2 p = TexCoords - 0.5;
        #else    
        vec2 p = TexCoords - 0.5;
        #endif

	    // create view ray
        vec3 rd = normalize( p.x*uu + p.y*vv + 1.5*ww );

        // raymarch
        vec3 obj_col = vec3(0.0);
        const float tmax = 5.0;
        float t = 0.0;
        for( int i=0; i<256; i++ )
        {
            vec3 pos = ro + t*rd;
            vec4 result = map(pos);
            float h = result.w;
            
            if((state == 0 || state == 1) && solidShading == 0)
                tot.a = (1. - result.w) * distance(pos, vec3(0.)) * 2.;
            else
                tot.a = (1. - result.w);

            obj_col = result.xyz;
            if( h<0.0001 || t>tmax ) break;
            t += h;
        }
        
    
        // shading/lighting	
        vec3 col =  pow(black, vec3(gamma));
        bool occluded_grid = false;

        if( t<tmax )
        {
            vec3 pos = ro + t*rd;
            vec3 nor = calcNormal(pos);
            vec3 lig = normalize(vec3(0.6,0.2,0.4));
            vec3 hal = normalize(lig-rd);
            lig = -rd;
            vec3 dif = half_lambert(lig, pos, hal, nor, obj_col);
            
            if(solidShading == 1)
                col = obj_col;
            else
                col = obj_col * dif;

            occluded_grid = true;
        }

        // gamma        
        col = pow(col, vec3(1.0/gamma));
	    tot.rgb += col;
    #if AA>1
    }
    tot.rgb /= float(AA*AA);
    #endif

    tot.r = min(tot.r,1.);
    tot.a = min(tot.a,1.);
    fragColor = tot;
}