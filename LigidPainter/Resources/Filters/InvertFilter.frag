#ligidFilter

// Predefined values :
// texCoords (frag coordinate vec2)
// txtr (sampler2D)
// fragColor (out vec4) 

void filterMain(){
    fragColor = vec4(vec3(1.) - texture(txtr, texCoords).rgb, 1.);
}