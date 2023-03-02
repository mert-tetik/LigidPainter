#version 330 core

out vec4 fragColor;

in vec2 TexCoords;

uniform float point1D[10];
uniform vec3 color1D[10];

vec3 getGradient(vec2 uv)
{
    vec3 clr1 = vec3(0,0,0);
    vec3 clr2 = vec3(0,0,0);
    
    float co1 = 1.;
    float co2 = 0.;
    
    float minVal = 1.;
    vec3 minI = vec3(0,1,0);
    float maxVal = 0.;
    vec3 maxI = vec3(0,1,0);
    for(int i = 0; i < 10;i++){
        if(point1D[i] < 2. && point1D[i] > 0.01){
            if(minVal > point1D[i]){
                minVal = point1D[i];
                minI = color1D[i]/255.;
            }
            if(maxVal < point1D[i]){
                maxVal = point1D[i];
                maxI = color1D[i]/255.;
            }
            if(co1 > point1D[i] && uv.y <= point1D[i]){//Smaller
                co1 = point1D[i];
                clr1 = color1D[i]/255.;
        
            }
            if(co2 < point1D[i] && uv.y >= point1D[i]){//Bigger
                co2 = point1D[i];        
                clr2 = color1D[i]/255.;
            }
        }
    }
    //Causes visual glitch
    if(uv.y < minVal)
        return minI;
    if(uv.y > maxVal)
        return maxI;

    float coDiff = co2-co1;
    return mix(clr1,clr2,(uv.y-co1)/coDiff);
}
void main()
{
    vec2 uv = TexCoords;
    
    vec3 grad = getGradient(uv);

    // Output to screen
    fragColor = vec4(grad,1.0);
}