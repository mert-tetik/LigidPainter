/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

//OpenGL & window 
#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

//OpenGL Math Library GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector> //std::vector
#include <string> //std::string
#include <map> //std::map
#include <atomic>

#include "UTIL/Util.hpp"
#include "UTIL/Painting/Painter.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/Painting/Painting_UTIL.hpp"
#include "UTIL/Library/Library.hpp"

#include "3D/Material/Material.hpp"

#include "GUI/Panels.hpp"


Framebuffer projected_painting_FBO;

MirrorSide O_side;
MirrorSide X_side;
MirrorSide Y_side;
MirrorSide XY_side;
MirrorSide Z_side;
MirrorSide XZ_side;
MirrorSide YZ_side;
MirrorSide XYZ_side;

struct PaintedBufferData{
    glm::vec3 clr;
    Texture txtr;
    Texture corresponding_custom_material_channel;
};

std::vector<PaintedBufferData> get_painted_buffers(PaintSettings settings){
    std::vector<PaintedBufferData> result;
    
    for (size_t i = 0; i < 6; i++)
    {
        PaintedBufferData data;
        if(i == 0 && settings.painted_buffers.material_channel_albedo_active){
            data.clr = settings.color_buffer.stroke_albedo_color.getRGB_normalized();
            data.txtr = settings.painted_buffers.material_channel_albedo;
            data.corresponding_custom_material_channel = customMatMesh.albedo;
        }
        if(i == 1 && settings.painted_buffers.material_channel_roughness_active){
            data.clr = glm::vec3(settings.color_buffer.stroke_roughness_color);
            data.txtr = settings.painted_buffers.material_channel_roughness;
            data.corresponding_custom_material_channel = customMatMesh.roughness;
        }
        if(i == 2 && settings.painted_buffers.material_channel_metallic_active){
            data.clr = glm::vec3(settings.color_buffer.stroke_metallic_color);
            data.txtr = settings.painted_buffers.material_channel_metallic;
            data.corresponding_custom_material_channel = customMatMesh.metallic;
        }
        if(i == 3 && settings.painted_buffers.material_channel_normalMap_active){
            data.clr = glm::vec3(settings.color_buffer.stroke_normalMap_color);
            data.txtr = settings.painted_buffers.material_channel_normalMap;
            data.corresponding_custom_material_channel = customMatMesh.normalMap;
        }
        if(i == 4 && settings.painted_buffers.material_channel_heightMap_active){
            data.clr = glm::vec3(settings.color_buffer.stroke_heightMap_color);
            data.txtr = settings.painted_buffers.material_channel_heightMap;
            data.corresponding_custom_material_channel = customMatMesh.heightMap;
        }
        if(i == 5 && settings.painted_buffers.material_channel_ao_active){
            data.clr = glm::vec3(settings.color_buffer.stroke_ao_color);
            data.txtr = settings.painted_buffers.material_channel_ao;
            data.corresponding_custom_material_channel = customMatMesh.ambientOcclusion;
        }
    }

    return result;
}

static int frame_counter = 0;

void paint_buffers(PaintSettings settings, bool first_frame, bool last_frame){
    
    if(first_frame){
        frame_counter = 0;
        settings.painting_over_data.texture_field_scene->update_painting_over_texture(settings.point.use_3D);
    }

    Brush stroke_brush;
    GET_DATA_FROM_PAINT_MODE(stroke_brush, stroke_brush);

    if(settings.painting_mode == 5){
        bucket_paint_texture(projected_painting_FBO.colorBuffer, Color(settings.color_buffer.stroke_albedo_color), stroke_brush.properties.opacity);
        return;
    } 

    std::vector<MirrorSide*> mirrorSides = get_selected_mirror_sides(settings.mirror_settings.X, settings.mirror_settings.Y, settings.mirror_settings.Z);

    if(settings.point.use_3D){
        for (MirrorSide* mirrorSide : mirrorSides)
        {
            glm::vec3 offset = glm::vec3(
                                            settings.mirror_settings.X_offset,
                                            settings.mirror_settings.Y_offset,
                                            settings.mirror_settings.Z_offset
                                        );

            offset *= glm::max(mirrorSide->effectAxis, 0.f);

            ThreeDPoint des_point = ThreeDPoint(glm::vec3(settings.point.point_3D.pos * mirrorSide->effectAxis - offset * 2.f), glm::vec3(settings.point.point_3D.normal * mirrorSide->effectAxis));

            std::vector<glm::vec2> strokes;
            Camera cam;
            process_3D_point(des_point, &cam, &strokes, settings.vertex_buffer.model_mesh, first_frame, stroke_brush.properties.spacing);
        
            // Perform window painting
            window_paint(&mirrorSide->paintingBuffers.window_painting_texture, strokes, stroke_brush, frame_counter, settings.painting_mode == 2);

            update_depth_texture(mirrorSide->paintingBuffers.depth_texture, cam, settings.vertex_buffer.model_mesh);

            project_window_painting_texture(
                                                &mirrorSide->paintingBuffers.projected_painting_texture, 
                                                mirrorSide->paintingBuffers.window_painting_texture, 
                                                mirrorSide->paintingBuffers.depth_texture.ID, 
                                                cam,
                                                settings.vertex_buffer,
                                                settings.painting_over_data,
                                                settings.color_buffer.stroke_albedo_color.getRGB_normalized(),
                                                settings.painting_mode, 
                                                stroke_brush.properties.opacity, 
                                                true
                                            );
        }
    }
    else{
        std::vector<glm::vec2> strokes;
        process_2D_point(settings.point.point_2D, &strokes, first_frame, stroke_brush.properties.spacing);
        
        // Perform window painting
        window_paint(&O_side.paintingBuffers.window_painting_texture, strokes, stroke_brush, frame_counter, settings.painting_mode == 2);
        
        for (MirrorSide* mirrorSide : mirrorSides){
            Camera cam = getScene()->camera;
            
            if(mirrorSide->effectAxis != O_side.effectAxis){
                cam = twoD_painting_calculate_camera(settings, *mirrorSide);

                twoD_painting_generate_mirrored_window_painting_texture(mirrorSide);                
            }

            if(first_frame)
                update_depth_texture(mirrorSide->paintingBuffers.depth_texture, cam, settings.vertex_buffer.model_mesh);

            Texture* projected_painting_texture = &mirrorSide->paintingBuffers.projected_painting_texture_low; 
            if(last_frame)
                projected_painting_texture = &mirrorSide->paintingBuffers.projected_painting_texture; 

            project_window_painting_texture(
                                                projected_painting_texture, 
                                                mirrorSide->paintingBuffers.window_painting_texture, 
                                                mirrorSide->paintingBuffers.depth_texture.ID, 
                                                cam,
                                                settings.vertex_buffer,
                                                settings.painting_over_data,
                                                settings.color_buffer.stroke_albedo_color.getRGB_normalized(),
                                                settings.painting_mode, 
                                                stroke_brush.properties.opacity, 
                                                false
                                            );
        }
    }
    
    generate_projected_painting_texture(&projected_painting_FBO, settings.mirror_settings.X, settings.mirror_settings.Y, settings.mirror_settings.Z, !last_frame && !settings.point.use_3D);
        
    frame_counter++;

    if(last_frame){
        if(twoD_painting_mode && panel_displaying_modes.selectedElement != 2){
            LGDLOG::start << "ERROR : Painting : Invalid displaying mode for the 2D painting" << LGDLOG::end;
            return;
        }

        if(button_mesh_selection.selectedMeshI >= getScene()->model->meshes.size()){
            LGDLOG::start << "ERROR : Painting : Invalid selected mesh" << LGDLOG::end;
            return;
        }
        
        if(settings.color_buffer.use_custom_material){
           update_custom_material_mesh(settings.color_buffer, settings.vertex_buffer, (settings.painted_buffers.material_painting) ? settings.painted_buffers.material_channel_albedo.getResolution() : settings.painted_buffers.solid_painted_texture.getResolution()); 
        }

        register_history_actions(settings.painting_mode, settings.painted_buffers);

        if(settings.painting_mode == 4){
            //button_painting_filter_mode_filter.filter.applyFilter(panel_library_selected_texture.ID, this->projectedPaintingTexture, 0);
        }
        else{
            for (PaintedBufferData painted_buffer : get_painted_buffers(settings))
            {
                updateTheTexture(painted_buffer.txtr, paintingMode, painted_buffer.clr, i);
                
                /*
                txtr.mix(customMatTxtr, projectedPaintingTexture, true, false, false);
                txtr.removeSeams(*getScene()->get_selected_mesh(), txtr.getResolution());
                */

                for (size_t i = 0; i < Library::getTextureArraySize(); i++)
                {
                    if(painted_buffer.txtr.ID == Library::getTexture(i)->ID){
                        Library::getTexture(i)->copyDataToTheCopyContext();
                        projectUpdatingThreadElements.updateTextures = true;
                    }
                }
            }
        }

        updateThePreRenderedPanels = true;
    }
}
