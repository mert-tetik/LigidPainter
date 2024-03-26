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

#include "3D/Material/Material.hpp"

#include "GUI/Panels.hpp"

#include "UTIL/Painting/Painting_UTIL.hpp"

Framebuffer projected_painting_FBO;

MirrorSide O_side;
MirrorSide X_side;
MirrorSide Y_side;
MirrorSide XY_side;
MirrorSide Z_side;
MirrorSide XZ_side;
MirrorSide YZ_side;
MirrorSide XYZ_side;

static int frame_counter = 0;

void paint_buffers(PaintSettings settings, bool first_frame, bool last_frame){
    
    if(first_frame){
        frame_counter = 0;
        getTextureFieldScene()->update_painting_over_texture(wrapMode, *this);
    }

    Brush stroke_brush;
    GET_DATA_FROM_PAINT_MODE(stroke_brush, stroke_brush);

    if(settings.painting_mode == 5){
        bucket_paint_texture(projected_painting_FBO.colorBuffer, Color(settings.bucket_mode.bucket_color_buffer.stroke_albedo_color), stroke_brush.properties.opacity);
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
            process_3D_point(des_point, &cam, &strokes, first_frame);
        
            // Perform window painting
            window_paint(&mirrorSide->paintingBuffers.window_painting_texture, strokes, stroke_brush, frame_counter);

            update_depth_texture(mirrorSide->paintingBuffers.depth_texture, cam, *settings.vertex_buffer.model_mesh);

            project_window_painting_texture(
                                                &mirrorSide->paintingBuffers.projected_painting_texture, 
                                                mirrorSide->paintingBuffers.window_painting_texture, 
                                                mirrorSide->paintingBuffers.depth_texture.ID, 
                                                cam,
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
        window_paint(&O_side.paintingBuffers.window_painting_texture, strokes, stroke_brush, frame_counter);
        
        for (MirrorSide* mirrorSide : mirrorSides){
            Camera cam = getScene()->camera;
            
            if(mirrorSide->effectAxis != O_side.effectAxis){
                cam = twoD_painting_calculate_camera(settings, *mirrorSide);

                twoD_painting_generate_mirrored_window_painting_texture(mirrorSide);                
            }

            if(first_frame)
                update_depth_texture(mirrorSide->paintingBuffers.depth_texture, cam, *settings.vertex_buffer.model_mesh);

            Texture* projected_painting_texture = &mirrorSide->paintingBuffers.projected_painting_texture_low; 
            if(last_frame)
                projected_painting_texture = &mirrorSide->paintingBuffers.projected_painting_texture; 

            project_window_painting_texture(
                                                projected_painting_texture, 
                                                mirrorSide->paintingBuffers.window_painting_texture, 
                                                mirrorSide->paintingBuffers.depth_texture.ID, 
                                                cam,
                                                settings.painting_mode, 
                                                stroke_brush.properties.opacity, 
                                                false
                                            );
        }
    }
    
    generate_projected_painting_texture(&projected_painting_FBO, settings.mirror_settings.X, settings.mirror_settings.Y, settings.mirror_settings.Z, !last_frame && !settings.point.use_3D);
        
    frame_counter++;
}
