#version 400 core

in vec2 TexCoords;
in vec3 fragPos;

out vec4 fragColor;

//!! FROM https://www.shadertoy.com/view/MtS3Dy

/*

Info on curves:
http://research.microsoft.com/en-us/um/people/hoppe/ravg.pdf
http://research.microsoft.com/en-us/um/people/cloop/LoopBlinn05.pdf
http://www.pouet.net/topic.php?which=9119&page=1
http://blog.gludion.com/2009/08/distance-to-quadratic-bezier-curve.html

The top link is where the get_distance_vector comes from.

Thanks also to other bezier curve shadertoys:
https://www.shadertoy.com/view/XsX3zf
https://www.shadertoy.com/view/lts3Df
*/

#define SMOOTH 0.0015
#define EDGE 0.0005

uniform vec2 percScale;

uniform vec2 startPos;
uniform vec2 destPos;
uniform int direction; 
 
#define EPSILON 0.000000001
#define MAX 9999999.
#define PI 3.14159265358979
 
//Find roots using Cardano's method. http://en.wikipedia.org/wiki/Cubic_function#Cardano.27s_method
int findRoots(float a, float b, float c, float d, out float r[3])
{
	vec3 vS = vec3(-1.0,-1.0,-1.0);
	if (abs(a) > EPSILON)
	{
		float z = 1.0/a;
		float d3 = 1.0/3.0;
		float d27 = 1.0/27.0;
		a = b*z;
		b = c*z;
		c = d*z;
		float p = b-a*a*d3;
		float q = a*(2.0*a*a-9.0*b)*d27+c;
		float ppp = p*p*p;
		float D = q*q+4.0*ppp*d27;
		float delta = -a*d3;
		if (D > EPSILON)
		{
			z = sqrt(D);
			float u = (-q+z)*0.5;
			float v = (-q-z)*0.5;
			u = sign(u)*pow(abs(u),d3);
			v = sign(v)*pow(abs(v),d3);
			r[0] = u+v+delta;
			return 1;
		}
		else if (D < -EPSILON)
		{
			float u = sqrt(-p*d3)*2.0;
            float s = -sqrt(-27.0/ppp)*q*0.5;
            if (abs(s) > 0.) {}
			float v = acos(s)*d3;
			r[0] = u*cos(v)+delta;
			r[1] = u*cos(v+2.0*PI*d3)+delta;
			r[2] = u*cos(v+4.0*PI*d3)+delta;
			return 3;
		}		
		else
		{
			q = sign(q)*pow(abs(q)*0.5,d3);
			r[0] = 2.0*-q+delta;
			r[1] = q+delta;
			return 2;
		}
	}
	else
	{
		if (abs(b) <= EPSILON && abs(c) > EPSILON)
		{
			r[0] = -d/c;
			return 1;
		}
		else
		{
			float D = c*c-4.0*b*d;
			float z = 1.0/(2.0*b);
			if (D > EPSILON)
			{
				D = sqrt(D);
				r[0] = (-c-D)*z;
				r[1] = (-c+D)*z;
				return 2;
			}
			else if (D > -EPSILON)
			{
				r[0] = -c*z;
				return 1;
			}
		}
	}
	return 0;
}
 
vec2 getPositionOnBezierCurve(float t, vec2 p0, vec2 p1, vec2 p2) 
{
	float fOneMinusT = 1.0-t;
	vec2 pos = fOneMinusT*fOneMinusT*p0+2.0*t*fOneMinusT*p1+t*t*p2;
	return pos;
}
 
float calculateDistanceToQuadraticBezier(vec2 p, vec2 p0, vec2 p1, vec2 p2) 
{
	vec2 dP0P = p0-p;
	vec2 dP1P0 = p1-p0;
	vec2 sP0P2 = p0+p2-p1*2.0;
	float a = dot(sP0P2,sP0P2);
	float b = dot(dP1P0,sP0P2)*3.0;
	float c = dot(dP1P0,dP1P0)*2.0+dot(dP0P, sP0P2);
	float d = dot(dP0P,dP1P0);
	float r[3];
	int roots = findRoots(a,b,c,d,r);
	float dist = MAX;
    float tmp;
    if (roots > 0 && r[0] >= 0. && r[0] <= 1.) {
        dist = distance(p,getPositionOnBezierCurve(r[0],p0,p1,p2));
        //return r[0] * 50.;
    }
    if (roots > 1 && r[1] >= 0. && r[1] <= 1.) {
        dist = min(dist, distance(p,getPositionOnBezierCurve(r[1],p0,p1,p2)));
    }
    if (roots > 2 && r[2] >= 0. && r[2] <= 1.) {
        dist = min(dist, distance(p,getPositionOnBezierCurve(r[2],p0,p1,p2)));
    }
 	dist = min(dist, min(distance(p, p0), distance(p, p2)));
	return dist;
}
 
//-----------------------------------------------------------------------------
void main()
{	

    
    float aspectRatio = percScale.x / percScale.y; //TODO Update that with node editor scale
    vec2 uv = TexCoords;
    vec2 percent = uv;
    percent.y = 1.-uv.y;
    //percent.y /= aspectRatio;
    
    vec2 A = startPos/100.;
    vec2 C = destPos/100.;
	
	vec2 B = A;
    if(direction == 0)
        B.x -= 0.1;
    else if(direction == 1)
        B.x += 0.1;
    
    float maxX = max(max(A.x, B.x), C.x);
    float minX = min(min(A.x, B.x), C.x);
    float maxY = max(max(A.y, B.y), C.y);
    float minY = min(min(A.y, B.y), C.y);

    if(percent.x > minX && percent.x < maxX && percent.y > minY && percent.y < maxY){
        //if(direction == 0)
        //    B.x -= 0.1;
        //else if(direction == 1)
        //    B.x += 0.1;

        vec3 color = vec3(1.0,1.0,1.0);

        float dist = calculateDistanceToQuadraticBezier(percent, A, B, C);
        if (dist < EDGE + SMOOTH)
        {
            dist = smoothstep(EDGE - SMOOTH, EDGE + SMOOTH, dist);
            color *= vec3(dist);
        }
        
        fragColor = vec4(1) - vec4(color,color.r);
        fragColor.rgb *= vec3(1.5);
    }
    else
        fragColor = vec4(0.);
}

