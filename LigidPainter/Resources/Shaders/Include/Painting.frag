/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    *This file is included to other shader code using painting

    *Include that file using : #pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Painting.frag)

    And don't forget to disable the squiggles for the GLSL 
    (squiggles becomes annoying since the vscode compiler doesn't recognise including other shader files) 
*/

//----------------------- COMBINATION -----------------------  
struct PaintingBuffers{
    sampler2D albedo_txtr;
    sampler2D roughness_txtr;
    sampler2D metallic_txtr;
    sampler2D normal_map_txtr;
    sampler2D height_map_txtr;
    sampler2D ao_txtr;
    sampler2D brush_txtr; // This texture contains the projected brush strokes
};

struct PaintingSmearData{
    float transform_strength;
    float blur_strength;
};

struct ChannelData{
    float roughness_strength;
    float metallic_strength;
    float normal_map_strength;
    float height_map_strength;
    float ao_strength;
};

float channel_data_get_channel_strength(ChannelData channel_data, int channel_index){
    if(channel_index == 0)
        return 1.;
    if(channel_index == 1)
        return channel_data.roughness_strength;
    if(channel_index == 2)
        return channel_data.metallic_strength;
    if(channel_index == 3)
        return channel_data.normal_map_strength;
    if(channel_index == 4)
        return channel_data.height_map_strength;
    if(channel_index == 5)
        return channel_data.ao_strength;

    return 1.;
}

vec4 painting_buffers_get_src_txtr_texture_2D(PaintingBuffers painting_buffers, int channel_index, vec2 uv){
    if(channel_index == 0)
        return texture2D(painting_buffers.albedo_txtr, uv);
    if(channel_index == 1)
        return texture2D(painting_buffers.roughness_txtr, uv);
    if(channel_index == 2)
        return texture2D(painting_buffers.metallic_txtr, uv);
    if(channel_index == 3)
        return texture2D(painting_buffers.normal_map_txtr, uv);
    if(channel_index == 4)
        return texture2D(painting_buffers.height_map_txtr, uv);
    if(channel_index == 5)
        return texture2D(painting_buffers.ao_txtr, uv);

    return texture2D(painting_buffers.albedo_txtr, uv);
}

vec4 painting_buffers_get_src_txtr_texture_lod(PaintingBuffers painting_buffers, int channel_index, vec2 uv, float lod){
    if(channel_index == 0)
        return texture2DLod(painting_buffers.albedo_txtr, uv, lod);
    if(channel_index == 1)
        return texture2DLod(painting_buffers.roughness_txtr, uv, lod);
    if(channel_index == 2)
        return texture2DLod(painting_buffers.metallic_txtr, uv, lod);
    if(channel_index == 3)
        return texture2DLod(painting_buffers.normal_map_txtr, uv, lod);
    if(channel_index == 4)
        return texture2DLod(painting_buffers.height_map_txtr, uv, lod);
    if(channel_index == 5)
        return texture2DLod(painting_buffers.ao_txtr, uv, lod);

    return texture2DLod(painting_buffers.albedo_txtr, uv, lod);
}

struct PaintingData{
    PaintingBuffers painting_buffers;

    PaintingSmearData smear_data;

    ChannelData channel_data;

    int brush_mode; // Drawing 0, softening 1, smearing 2, normal map painting 3, filter painting 4, vector painting 5, bucket painting 6

    bool painting_over_active;
};




//------------------------------------------- SOFTEN BRUSH ------------------------------------------ 

const int LOD = 1,         // gaussian done on MIPmap at scale LOD
          sLOD = 1 << LOD; // tile size = 2^LOD

/**
* Util function for the getSoftenedTexture function
* Calculate the value of a Gaussian function at the given position.
* 
* returns The computed value of the Gaussian function.
*/
float gaussian(
                vec2 i, //The input position in 2D space 
                int range //The range parameter for the Gaussian function
            ) 
{
    // Compute the standard deviation based on the range parameter.
    float sigma = float(range) * 0.25;

    // Calculate the exponent term of the Gaussian function.
    float exponent = -0.5 * dot(i /= sigma, i);

    // Compute the final value of the Gaussian function.
    float value = exp(exponent) / (6.28 * sigma * sigma);

    return value;
}

/**
*    Apply soften brush to the given texture
*
*    Example : 
*
*    vec3 albedo;
*    if(paintTheAlbedoTexture)
*        albedo = getSoftenedTexture(txtr, brushTxtr, TexCoords);
*    else
*        albedo = texture2D(txtr, TexCoords).rgb;
*           
*
*    returns the painted color data with given texture coordinate
**/
vec4 get_soften(
                            PaintingBuffers painting_buffers,
                            ChannelData channel_data,
                            int channel_index,
                            vec2 texCoords
                        )
{ //Intensity from the painted texture
    const float width = 1024;
    int range = 10;
    
    vec4 O = vec4(0); 
    int s = range/sLOD;
    
    // Loop through the subdivisions and accumulate the blurred color
    for (int i = 0; i < s * s; i++) {
        // Compute the offset from the center of the range
        vec2 d = vec2(i % s, i / s) * float(sLOD) - float(range) / 2.0;

        // Compute the contribution using the Gaussian function and textureLod
        O += gaussian(d, range) * painting_buffers_get_src_txtr_texture_lod(painting_buffers, channel_index, texCoords + vec2(1.0 / width) * d, float(LOD));
    }
    
    //Mix the blurred color data with original texture data using the brush texture's alpha value
    return mix(
                painting_buffers_get_src_txtr_texture_2D(painting_buffers, channel_index, texCoords), //Original value
                O.rgba / O.a, //Blurred value
                texture2D(painting_buffers.brush_txtr, texCoords).a * channel_data_get_channel_strength(channel_data, channel_index) //The brush value
            );
}








//------------------------------------------- SMEAR BRUSH ------------------------------------------ 

/**
*    Apply smear brush to the given texture
*
*    Example : 
*
*    vec3 albedo;
*    if(paintTheAlbedoTexture)
*        albedo = getSmearedTexture(txtr, brushTxtr, TexCoords);
*    else
*        albedo = texture2D(txtr, TexCoords).rgb;
*           
*
*    returns the painted color data with given texture coordinate
**/
vec4 get_smear(
                            PaintingBuffers painting_buffers,
                            ChannelData channel_data,
                            PaintingSmearData smear_data,
                            int channel_index,
                            vec2 texCoords
                    )
{
    
    
    float intensity = texture2D(painting_buffers.brush_txtr, texCoords).a/2. * channel_data_get_channel_strength(channel_data, channel_index);
    
    //Get the direction value from the painted texture (which direction the painting made in the current texture coordinates)
    vec2 Direction = texture2D(painting_buffers.brush_txtr, texCoords).rg;

    //Samples (multiple of 2)
    int Samples = 64;
     
    //Get the blurred color (directional)
    vec4 blurredColor = vec4(0.0);  
    
    for(int i = -Samples/2; i <= Samples/2; i++)
    {
        vec2 uv = texCoords - vec2(Direction.x * (float(i) / float(Samples)), Direction.y * (float(i) / float(Samples))) * smear_data.blur_strength * intensity;
        vec2 dest_uv = vec2(uv.x - Direction.x * smear_data.transform_strength, uv.y + Direction.y * smear_data.transform_strength);
        blurredColor += painting_buffers_get_src_txtr_texture_2D(painting_buffers, channel_index, dest_uv) / float(Samples + 1);
    }
    
    //TODO : Write smt better to that returning value

    return blurredColor;
}



//------------------------------------------- NORMAL MAP BRUSH ------------------------------------------ 

vec4 texNormalMap(sampler2D heightMap, in vec2 uv, float str)
{
    vec2 s = vec2(0.00390625); // 1 / 256
    
    float p = texture2D(heightMap, uv).a;
    float h1 = texture2D(heightMap, uv + s * vec2(1.0,0)).a;
    float v1 = texture2D(heightMap, uv + s * vec2(0,1.0)).a;
       
   	return vec4(vec2(p - vec2(h1, v1)) * str + 0.5, 1., texture2D(heightMap, uv).a);
}

vec4 get_normal_map(
                        PaintingBuffers painting_buffers,
                        ChannelData channel_data,
                        int channel_index,
                        vec2 texCoords
                    )
{
    vec4 paintColor = texNormalMap(painting_buffers.brush_txtr, texCoords, channel_data_get_channel_strength(channel_data, channel_index));
    vec4 srcTxtr = painting_buffers_get_src_txtr_texture_2D(painting_buffers, channel_index, texCoords);
    float brushTxtrAlpha = texture(painting_buffers.brush_txtr, texCoords).a; 

    return mix(
                srcTxtr, 
                paintColor, 
                brushTxtrAlpha
            );
}









//------------------------------------------- FILTER DISPLAY BRUSH ------------------------------------------ 

vec4 get_filter_display( 
                        PaintingBuffers painting_buffers,
                        ChannelData channel_data,
                        int channel_index,
                        vec2 texCoords
                    )
{
    float intensity = texture2D(painting_buffers.brush_txtr, texCoords).a;

    //Mix the original color data with painting color using the intensity of the painted texture
    vec4 destColor = vec4(1.) - painting_buffers_get_src_txtr_texture_2D(painting_buffers, channel_index, texCoords);

    return mix  (
                    painting_buffers_get_src_txtr_texture_2D(painting_buffers, channel_index, texCoords),
                    destColor, //paintingColor
                    intensity * channel_data_get_channel_strength(channel_data, channel_index)
                );
}





//------------------------------------------- DRAWING BRUSH ------------------------------------------ 

/**
*    Apply default brush to the given texture
*
*    Example : 
*
*    vec3 albedo;
*    if(paintTheAlbedoTexture)
*        albedo = getPaintedTexture(txtr, brushTxtr, TexCoords, vec3(1,0,0)); //Paint to red
*    else
*        albedo = texture2D(txtr, TexCoords).rgb;
*           
*
*    returns the painted color data with given texture coordinate
**/
vec4 get_draw  ( 
                            PaintingBuffers painting_buffers,
                            ChannelData channel_data,
                            bool painting_over_active,
                            int channel_index,
                            vec2 texCoords
                        )
{

    float brushTxtrAlpha = texture(painting_buffers.brush_txtr, texCoords).a; 
    vec4 paintColor; 
    
    // Albedo
    if(channel_index == 0){
        paintColor = texture2D(painting_buffers.brush_txtr, texCoords);
    }

    // Normal map
    else if(channel_index == 3){
        paintColor = texNormalMap(painting_buffers.brush_txtr, texCoords, channel_data_get_channel_strength(channel_data, channel_index));   
    }

    else{
        if(painting_over_active){
            vec4 s = texture2D(painting_buffers.brush_txtr, texCoords);
            float rs = max(max(s.r,s.g),s.b);
            paintColor = vec4(mix(1.-rs, rs, channel_data_get_channel_strength(channel_data, channel_index)));
        }
        else{
            paintColor = vec4(vec3(channel_data_get_channel_strength(channel_data, channel_index)), 1.);
        }
    }

    vec4 srcTxtr = painting_buffers_get_src_txtr_texture_2D(painting_buffers, channel_index, texCoords);

    return mix(
                srcTxtr, 
                paintColor, 
                brushTxtrAlpha
            );
}





/**
*    Apply a brush to the given texture
*    returns the painted color data with given texture coordinate
**/
vec4 getBrushedTexture (
                            PaintingData paintData, // Painting data
                            int channel_index,
                            vec2 texCoords  //Texture coordinates
                        )
{
    //Apply painting with color
    if(paintData.brush_mode == 0 || paintData.brush_mode == 6)
        return get_draw(paintData.painting_buffers, paintData.channel_data, paintData.painting_over_active, channel_index, texCoords);

    //Apply painting with softening
    if(paintData.brush_mode == 1)
        return get_soften(paintData.painting_buffers, paintData.channel_data, channel_index, texCoords);

    //Apply painting with smearing
    if(paintData.brush_mode == 2)
        return get_smear(paintData.painting_buffers, paintData.channel_data, paintData.smear_data, channel_index, texCoords);
    
    //Apply painting with painting texture color data
    if(paintData.brush_mode == 3)
        return get_normal_map(paintData.painting_buffers, paintData.channel_data, channel_index, texCoords);
    
    //Apply painting with inverted texture color
    if(paintData.brush_mode == 4)
        return get_filter_display(paintData.painting_buffers, paintData.channel_data, channel_index, texCoords);
    
    if(paintData.brush_mode == 5)
        return painting_buffers_get_src_txtr_texture_2D(paintData.painting_buffers, channel_index, texCoords);
    
    //If the brushModeState value is not valid
    return vec4(0);
}
