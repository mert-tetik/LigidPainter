#version 400 core

//Painting
#pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Painting.frag)


//Functions related to PBR 
#pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Physics_Math.frag)

struct VertexData{
    vec2 TexCoords;
    vec3 Normal;
    vec3 Pos;
    vec3 Tangent;
    vec3 Bitangent;
    vec4 ProjectedPos;
};

//Vertex data : pos , normal , texture coordinates , tangent , bitangent , projected position
in VertexData vertexData;

//Position of the camera
uniform vec3 viewPos;

//Used for ambient
//TODO : Don't use the skybox use prefilterMap instead & use blury
uniform samplerCube skybox;

//Used for reflection
uniform samplerCube prefilterMap;

//Material channels
uniform sampler2D albedoTxtr; //Albedo
uniform sampler2D roughnessTxtr; //Roughness
uniform sampler2D metallicTxtr; //Metallic
uniform sampler2D normalMapTxtr; //Normal Map
uniform sampler2D ambientOcclusionTxtr; //Ambient occlusion (ao)

//Contains the brush strokes
uniform sampler2D paintingTexture;

//3D Model rendered with depth shader (to compare depth)
uniform sampler2D depthTexture;

//0 = paint the albedo
//1 = paint the roughness
//2 = paint the metallic 
//3 = paint the normal map
//4 = paint the height map
//5 = paint the ambient Occlusion
uniform int paintedTxtrStateIndex;

//0 = paint
//1 = soften
//2 = smear
uniform int brushModeState; 

//Return only the solid texture instead of the pbr 
uniform int returnSingleTxtr;

//Selected opacity for painting
uniform float paintingOpacity;

//Selected color for painting
uniform vec3 paintingColor;

//Fragment shader output
out vec4 fragColor;

vec3 getPBR(){
    
    vec4 brushTxtr = getBrushValue(paintingTexture, depthTexture, vertexData.ProjectedPos, paintingOpacity);

    vec3 albedo;
    float roughness;
    float metallic;
    vec3 normal;
    float ao;
    
    if(paintedTxtrStateIndex == 0){
        albedo = getBrushedTexture(albedoTxtr,brushTxtr,vertexData.TexCoords, paintingColor, brushModeState).rgb;
        if(returnSingleTxtr == 1)
            return albedo;
    }
    else
        albedo = texture(albedoTxtr,vertexData.TexCoords).rgb;
    
    if(paintedTxtrStateIndex == 1){
        roughness = getBrushedTexture(roughnessTxtr,brushTxtr,vertexData.TexCoords, paintingColor, brushModeState).r;
        if(returnSingleTxtr == 1)
            return roughness;
    }
    else
        roughness = texture(roughnessTxtr,vertexData.TexCoords).r;
    
    if(paintedTxtrStateIndex == 2){
        metallic = getBrushedTexture(metallicTxtr,brushTxtr,vertexData.TexCoords, paintingColor, brushModeState).r;
        if(returnSingleTxtr == 1)
            return metallic;
    }
    else
        metallic = texture(metallicTxtr,vertexData.TexCoords).r;

    if(paintedTxtrStateIndex == 3){
        normal = getBrushedTexture(normalMapTxtr,brushTxtr,vertexData.TexCoords, paintingColor, brushModeState).rgb;
        if(returnSingleTxtr == 1)
            return normal;
    }
    else
        normal = texture(normalMapTxtr,vertexData.TexCoords).rgb;
    
    if(paintedTxtrStateIndex == 5){
        ao = getBrushedTexture(ambientOcclusionTxtr,brushTxtr,vertexData.TexCoords, paintingColor, brushModeState).r;
        if(returnSingleTxtr == 1)
            return ao;
    }
    else
        ao = texture(ambientOcclusionTxtr,vertexData.TexCoords).r;
    
    

    vec3 T = normalize(vec3(vec4(vertexData.Tangent, 0.0)));
    vec3 aN = normalize(vec3(vec4(vertexData.Normal, 0.0)));
    T = normalize(T - dot(T, aN) * aN);
    vec3 B = cross(aN, T);
    
    mat3 TBN = transpose(mat3(T, B, aN));    
    
    vec3 tangentViewPos  = TBN * viewPos;
    vec3 tangentPosModel  = TBN * vertexData.Pos;

    vec3 N = normal;

    N = normalize(N * 2.0 - 1.0);  // this normal is in tangent space


   
   vec3 lightPos1 = vec3(-10.0,  10.0, 10.0);
   vec3 lightPos2 = vec3( 10.0,  10.0, 10.0);
   vec3 lightPos3 = vec3(-10.0, -10.0, 10.0);
   vec3 lightPos4 = vec3( 10.0, -10.0, 10.0);

   vec3 lightColorX = vec3(300.0f, 300.0f, 300.0f);


    vec3 V = normalize(tangentViewPos - tangentPosModel);
    
    vec3 R = reflect(-normalize(viewPos - vertexData.Pos), vertexData.Normal); 

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 0; ++i) 
    {
      vec3 lightPosX;

      if(i == 0)
         lightPosX = lightPos1;
      if(i == 1)
         lightPosX = lightPos2;
      if(i == 2)
         lightPosX = lightPos3;
      if(i == 3)
         lightPosX = lightPos4;

         vec3 tangentLightPos = TBN * lightPosX;

        // calculate per-light radiance
        vec3 L = normalize(tangentLightPos - tangentPosModel);
        vec3 H = normalize(V + L);
        float distance = length(tangentLightPos - tangentPosModel);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColorX * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);    
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);        
        
        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        
         // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	                
            
        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL; // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   
    
    // ambient lighting (we now use IBL as the ambient term)
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  
    
    vec3 irradiance = texture(skybox, N).rgb ;
    vec3 diffuse      = albedo;
    
    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
    //vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F);

    vec3 ambient = (kD * diffuse + specular) * ao;

   vec3 result = ambient + Lo;

   result = result / (result + vec3(1.0));

   result = pow(result,vec3(1.0/2.2));
   
   return result;
}

void main() {
    fragColor = vec4(getPBR(),1.);
}