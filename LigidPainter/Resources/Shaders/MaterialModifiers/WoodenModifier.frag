
/* Perlin Properties */
int maxOctaves = 8;
float persistance = 0.5;

float noiseOffset = 1.;

float fbmFrequency = 1.0;

float musgraveNoise = 1.;

float seed = 3000.;

float hash(float n) {
    return fract(sin(n) * seed);
}

float noise(vec3 p) {
    vec3 i = floor(p);
    vec3 f = fract(p);
    
    vec3 u = f * f * noiseOffset;
    
    float n000 = hash(dot(i, vec3(1.0, 1.0, 1.0)));
    float n001 = hash(dot(i + vec3(0.0, 0.0, 1.0), vec3(1.0, 1.0, 1.0)));
    float n010 = hash(dot(i + vec3(0.0, 1.0, 0.0), vec3(1.0, 1.0, 1.0)));
    float n011 = hash(dot(i + vec3(0.0, 1.0, 1.0), vec3(1.0, 1.0, 1.0)));
    float n100 = hash(dot(i + vec3(1.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0)));
    float n101 = hash(dot(i + vec3(1.0, 0.0, 1.0), vec3(1.0, 1.0, 1.0)));
    float n110 = hash(dot(i + vec3(1.0, 1.0, 0.0), vec3(1.0, 1.0, 1.0)));
    float n111 = hash(dot(i + vec3(1.0, 1.0, 1.0), vec3(1.0, 1.0, 1.0)));
    
    return mix(
        mix(mix(n000, n100, u.x), mix(n001, n101, u.x), u.y),
        mix(mix(n010, n110, u.x), mix(n011, n111, u.x), u.y),
        u.z
    );
}

float fbm(vec3 p, int octaves, float roughness) {
    
    float freq = fbmFrequency;
    
    float amplitude = 1.0;
    
    float total = 0.0;
    float maxTotal = 0.0;
    
    for (int i = 0; i < octaves; ++i) {
        total += amplitude * noise(p * freq);
        maxTotal += amplitude;
        
        freq *= 2.0;
        amplitude *= roughness;
    }
    
    return total / maxTotal;
}

float fbmx(vec3 p) {
    vec3 noise = vec3(noise(p + vec3(0.)), noise(p + vec3(1.)), noise(p + vec3(2.)));
    p += noise;
    return fbm(p, maxOctaves, persistance);
}

float musgrave(vec3 p, float octaves, float dimension, float lacunarity) {
    float sum = 0.0;
    float amp = 1.0;
    float m = pow(lacunarity, -dimension);
    
    while (octaves-- > 0.0) {
        float n = noise(p) * 2.0 - 1.0;
        sum += n * amp;
        amp *= m * musgraveNoise;
        p *= lacunarity;
    }
    
    return sum;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = fragCoord;

    float val = musgrave(fbmx(vec3(fragCoord,0.)) * vec3(8., 1., 1.), 8., 0., 2.5);

    fragColor = vec4(vec3(val),1.0);
}