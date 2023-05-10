#version 330 core


out vec4 fragColor;
in vec2 TexCoords;


#define SRGB_TO_LINEAR(c) pow((c), vec3(2.2))
#define LINEAR_TO_SRGB(c) pow((c), vec3(1.0 / 2.2))
#define SRGB(r, g, b) SRGB_TO_LINEAR(vec3(float(r), float(g), float(b)) / 255.0)

const vec3 COLOR0 = SRGB(255, 255, 255);
const vec3 COLOR1 = SRGB(0, 166, 201);

// Gradient noise from Jorge Jimenez's presentation:
// http://www.iryoku.com/next-generation-post-processing-in-call-of-duty-advanced-warfare
float gradientNoise(in vec2 uv)
{
    const vec3 magic = vec3(0.06711056, 0.00583715, 52.9829189);
    return fract(magic.z * fract(dot(uv, magic.xy)));
}

void main()
{
    vec2 a; // First gradient point.
    vec2 b; // Second gradient point.
    a = abs(vec2(0));
    b = vec2(3);

    // Calculate interpolation factor with vector projection.
    vec2 ba = b - a;
    float t = dot(TexCoords - a, ba) / dot(ba, ba);
    // Saturate and apply smoothstep to the factor.
    t = smoothstep(0.0, 1.0, clamp(t, 0.0, 1.0));
    // Interpolate.
    vec3 color = mix(COLOR0, COLOR1, t);

    // Convert color from linear to sRGB color space (=gamma encode).
    color = LINEAR_TO_SRGB(color);

    // Add gradient noise to reduce banding.
    color += (1.0/255.0) * gradientNoise(TexCoords) - (0.5/255.0);

    fragColor = vec4(color, 1.0);
}
