//!! IS NOT USED 

//!! SAMPLE FILE 

#version 400 core

in vec2 TexCoords;
out vec4 fragColor;

uniform int renderHSCircle;

vec3 hsv2rgb(vec3 hsv)
{
    float hue = hsv.x;
    float saturation = hsv.y;
    float value = hsv.z;
    
    float chroma = value * saturation;
    float huePrime = hue * 6.0;
    float x = chroma * (1.0 - abs(mod(huePrime, 2.0) - 1.0));
    float m = value - chroma;
    
    vec3 rgb;
    
    if (huePrime < 1.0)
        rgb = vec3(chroma, x, 0.0);
    else if (huePrime < 2.0)
        rgb = vec3(x, chroma, 0.0);
    else if (huePrime < 3.0)
        rgb = vec3(0.0, chroma, x);
    else if (huePrime < 4.0)
        rgb = vec3(0.0, x, chroma);
    else if (huePrime < 5.0)
        rgb = vec3(x, 0.0, chroma);
    else
        rgb = vec3(chroma, 0.0, x);
    
    return rgb + vec3(m);
}

void calcHSCircle()
{
    // Define the number of samples for supersampling
    const int numSamples = 8;
    
    // Initialize the accumulated color
    vec3 accumulatedColor = vec3(0.0);
    
    // Convert the fragment coordinate to UV coordinates
    vec2 uv = TexCoords;
    
    // Calculate the center and radius of the circle
    vec2 center = vec2(0.5);
    float radius = 0.4;
    
    // Iterate over the samples
    for (int i = 0; i < numSamples; i++) {
        // Calculate the offset for the current sample
        float offset = float(i) / float(numSamples - 1) - 0.5;
        
        // Calculate the UV coordinate for the current sample
        vec2 sampleUV = uv + offset / vec2(1080,1080);
        
        // Calculate the distance from the sample's UV coordinate to the center of the circle
        float distance = length(sampleUV - center);
        
        // Check if the current sample is within the circle
        if (distance < radius) {
            // Calculate the hue based on the sample's position within the circle
            float hue = (atan(sampleUV.y - center.y, sampleUV.x - center.x) + 3.1416) / (2.0 * 3.1416);
            
            // Calculate the saturation and value based on the sample's position within the circle
            float saturation = distance / radius;
            float value = 1.0;
            
            // Convert the HSV values to RGB
            vec3 color = hsv2rgb(vec3(hue, saturation, value));
            
            // Accumulate the color
            accumulatedColor += color;
        }
    }
    
    // Average the accumulated color
    vec3 averagedColor = accumulatedColor / float(numSamples);
    
    // Output the final color
    float alpha = max(max(averagedColor.r,averagedColor.g),averagedColor.b);

    fragColor = vec4(averagedColor, alpha);
}

void calcValBar(){
    fragColor = vec4(vec3(TexCoords.y),1);
}

void main(){
    if(renderHSCircle == 1)
        calcHSCircle();
    else
        calcValBar();
}