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
#include "UTIL/GL/GL.hpp"

#include "3D/Mesh/Mesh.hpp"

void ShaderUTIL::set_shader_struct_face_selection_data(Shader shader, Mesh& mesh){
    shader.use();
    if(mesh.face_selection_data.selectedFaces.ID && mesh.face_selection_data.meshMask.ID){
        shader.setFloat("face_selection_data.prim_txtr_res", mesh.face_selection_data.selectedFaces.getResolution().x);
        shader.setInt("face_selection_data.meshSelectionEditing", mesh.face_selection_data.editMode);
        shader.setInt("face_selection_data.hideUnselected", mesh.face_selection_data.hideUnselected);
        shader.setInt("face_selection_data.usingMeshSelection", mesh.face_selection_data.activated);
        shader.setInt("face_selection_data.selectedPrimitiveIDS", GL::bindTexture_2D(mesh.face_selection_data.selectedFaces.ID, "ShaderUTIL::set_shader_struct_face_selection_data"));
        shader.setInt("face_selection_data.meshMask", GL::bindTexture_2D(mesh.face_selection_data.meshMask.ID, "ShaderUTIL::set_shader_struct_face_selection_data"));
        shader.setInt("face_selection_data.primitiveCount", mesh.face_selection_data.selectedPrimitiveIDs.size());
    }
    else{
        shader.setInt("face_selection_data.usingMeshSelection", false);
    }
}

void ShaderUTIL::set_shader_struct_painting_data(Shader shader, PaintingData painting_data){
    shader.use();

    shader.setInt("painting_data.painting_buffers.albedo_txtr", GL::bindTexture_2D(painting_data.painting_buffers.albedo_txtr.ID, "ShaderUTIL::set_shader_struct_painting_data"));
    shader.setInt("painting_data.painting_buffers.roughness_txtr", GL::bindTexture_2D(painting_data.painting_buffers.roughness_txtr.ID, "ShaderUTIL::set_shader_struct_painting_data"));
    shader.setInt("painting_data.painting_buffers.metallic_txtr", GL::bindTexture_2D(painting_data.painting_buffers.metallic_txtr.ID, "ShaderUTIL::set_shader_struct_painting_data"));
    shader.setInt("painting_data.painting_buffers.normal_map_txtr", GL::bindTexture_2D(painting_data.painting_buffers.normal_map_txtr.ID, "ShaderUTIL::set_shader_struct_painting_data"));
    shader.setInt("painting_data.painting_buffers.height_map_txtr", GL::bindTexture_2D(painting_data.painting_buffers.height_map_txtr.ID, "ShaderUTIL::set_shader_struct_painting_data"));
    shader.setInt("painting_data.painting_buffers.ao_txtr", GL::bindTexture_2D(painting_data.painting_buffers.ao_txtr.ID, "ShaderUTIL::set_shader_struct_painting_data"));
    shader.setInt("painting_data.painting_buffers.brush_txtr", GL::bindTexture_2D(painting_data.painting_buffers.brush_txtr.ID, "ShaderUTIL::set_shader_struct_painting_data"));
    
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

Shader ShaderUTIL::get_bound_shader(){
    int currentProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);

    Shader bound_shader;
    bound_shader.ID = currentProgram;
    return bound_shader;
}