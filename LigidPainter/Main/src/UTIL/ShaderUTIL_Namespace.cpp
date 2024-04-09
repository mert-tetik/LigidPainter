/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <filesystem>
#include <algorithm>

#include "UTIL/Util.hpp"

void ShaderUTIL::set_shader_struct_painting_data(Shader shader, PaintingData painting_data){
    shader.use();
    if(painting_data.painting_buffers.albedo_txtr.ID){
        shader.setInt("painting_data.painting_buffers.albedo_txtr", UTIL::get_texture_slot_index(painting_data.painting_buffers.albedo_txtr_slot)); glActiveTexture(painting_data.painting_buffers.albedo_txtr_slot); glBindTexture(GL_TEXTURE_2D, painting_data.painting_buffers.albedo_txtr.ID);
    }
    if(painting_data.painting_buffers.roughness_txtr.ID){
        shader.setInt("painting_data.painting_buffers.roughness_txtr", UTIL::get_texture_slot_index(painting_data.painting_buffers.roughness_txtr_slot)); glActiveTexture(painting_data.painting_buffers.roughness_txtr_slot); glBindTexture(GL_TEXTURE_2D, painting_data.painting_buffers.roughness_txtr.ID);
    }
    if(painting_data.painting_buffers.metallic_txtr.ID){
        shader.setInt("painting_data.painting_buffers.metallic_txtr", UTIL::get_texture_slot_index(painting_data.painting_buffers.metallic_txtr_slot)); glActiveTexture(painting_data.painting_buffers.metallic_txtr_slot); glBindTexture(GL_TEXTURE_2D, painting_data.painting_buffers.metallic_txtr.ID);
    }
    if(painting_data.painting_buffers.normal_map_txtr.ID){
        shader.setInt("painting_data.painting_buffers.normal_map_txtr", UTIL::get_texture_slot_index(painting_data.painting_buffers.normal_map_txtr_slot)); glActiveTexture(painting_data.painting_buffers.normal_map_txtr_slot); glBindTexture(GL_TEXTURE_2D, painting_data.painting_buffers.normal_map_txtr.ID);
    }
    if(painting_data.painting_buffers.height_map_txtr.ID){
        shader.setInt("painting_data.painting_buffers.height_map_txtr", UTIL::get_texture_slot_index(painting_data.painting_buffers.height_map_txtr_slot)); glActiveTexture(painting_data.painting_buffers.height_map_txtr_slot); glBindTexture(GL_TEXTURE_2D, painting_data.painting_buffers.height_map_txtr.ID);
    }
    if(painting_data.painting_buffers.ao_txtr.ID){
        shader.setInt("painting_data.painting_buffers.ao_txtr", UTIL::get_texture_slot_index(painting_data.painting_buffers.ao_txtr_slot)); glActiveTexture(painting_data.painting_buffers.ao_txtr_slot); glBindTexture(GL_TEXTURE_2D, painting_data.painting_buffers.ao_txtr.ID);
    }
    if(painting_data.painting_buffers.brush_txtr.ID){
        shader.setInt("painting_data.painting_buffers.brush_txtr", UTIL::get_texture_slot_index(painting_data.painting_buffers.brush_txtr_slot)); glActiveTexture(painting_data.painting_buffers.brush_txtr_slot); glBindTexture(GL_TEXTURE_2D, painting_data.painting_buffers.brush_txtr.ID);
    }
    
    shader.setFloat("painting_data.smear_data.transform_strength", painting_data.smear_data.transform_strength);
    shader.setFloat("painting_data.smear_data.blur_strength", painting_data.smear_data.blur_strength);
    
    shader.setFloat("painting_data.channel_data.roughness_strength", painting_data.channel_data.roughness_strength);
    shader.setFloat("painting_data.channel_data.metallic_strength", painting_data.channel_data.metallic_strength);
    shader.setFloat("painting_data.channel_data.normal_map_strength", painting_data.channel_data.normal_map_strength);
    shader.setFloat("painting_data.channel_data.height_map_strength", painting_data.channel_data.height_map_strength);
    shader.setFloat("painting_data.channel_data.ao_strength", painting_data.channel_data.ao_strength);
    
    shader.setInt("painting_data.brush_mode", painting_data.brush_mode); // Drawing 0, softening 1, smearing 2, normal map painting 3, filter painting 4, vector painting 5, bucket painting 6
    
    shader.setInt("painting_data.painting_over_active", painting_data.painting_over_active);
}