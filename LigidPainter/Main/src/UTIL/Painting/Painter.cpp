/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

TODO : Update color buffer resolutions
TODO : Refresh painting buffers

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
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/Painting/Painter.hpp"
#include "UTIL/Painting/Painting_UTIL.hpp"
#include "UTIL/Library/Library.hpp"

#include "3D/Material/Material.hpp"

#include "GUI/Panels.hpp"

extern bool updateThePreRenderedPanels;

Framebuffer painting_projected_painting_FBO;

MirrorSide O_side;
MirrorSide X_side;
MirrorSide Y_side;
MirrorSide XY_side;
MirrorSide Z_side;
MirrorSide XZ_side;
MirrorSide YZ_side;
MirrorSide XYZ_side;

static int frame_counter = 0;

static ThreeDPoint last_threeD_point;
static Texture one_px_txtr;

#define RETURN_FROM_PAINTING glEnable(GL_BLEND);\
                             return;


void painting_paint_buffers(PaintSettings settings, bool first_frame, bool last_frame){
    
    glDisable(GL_BLEND);

    if(first_frame){
        Debugger::block("Painting : First frame: Update buffers"); // Start
        frame_counter = 0;
        settings.painting_over_data.texture_field_scene->update_painting_over_texture(settings.point.use_3D);
        Debugger::block("Painting : First frame: Update buffers"); // End
    }

    Brush stroke_brush;
    GET_DATA_FROM_PAINT_MODE(stroke_brush, stroke_brush);

    std::vector<MirrorSide*> mirrorSides = painting_get_selected_mirror_sides(settings.mirror_settings.X, settings.mirror_settings.Y, settings.mirror_settings.Z);
    
    if(settings.painting_mode == 6){
        Debugger::block("Painting : Bucket painting"); // Start
        bucket_paint_texture(painting_projected_painting_FBO.colorBuffer, Color(settings.color_buffer.stroke_albedo_color), stroke_brush.properties.opacity);
        Debugger::block("Painting : Bucket painting"); // End
        goto UPDATE_TEXTURE;
    } 

    if(settings.point.use_3D){
        for (MirrorSide* mirrorSide : mirrorSides)
        {
            glm::vec3 offset = glm::vec3(
                                            settings.mirror_settings.X_offset,
                                            settings.mirror_settings.Y_offset,
                                            settings.mirror_settings.Z_offset
                                        );

            offset *= glm::max(mirrorSide->effectAxis, 0.f);

            if(first_frame && mirrorSide->effectAxis == O_side.effectAxis)
                last_threeD_point = settings.point.point_3D;
            
            ThreeDPoint des_point = ThreeDPoint(glm::vec3(settings.point.point_3D.pos * (mirrorSide->effectAxis * -1.f) - offset * 2.f), glm::vec3(settings.point.point_3D.normal * (mirrorSide->effectAxis * -1.f)));
            ThreeDPoint last_des_point = ThreeDPoint(glm::vec3(last_threeD_point.pos * (mirrorSide->effectAxis * -1.f) - offset * 2.f), glm::vec3(last_threeD_point.normal * (mirrorSide->effectAxis * -1.f)));

            Debugger::block("Painting : Process 3D point"); // Start
            std::vector<glm::vec2> strokes;
            Camera cam;
            process_3D_point(des_point, last_des_point, *mirrorSide, &cam, &strokes, settings.vertex_buffer.model_mesh, first_frame, stroke_brush.properties.spacing);
            Debugger::block("Painting : Process 3D point"); // End
            
            if(first_frame && mirrorSide->effectAxis == O_side.effectAxis)
                last_threeD_point = settings.point.point_3D;
        
            Debugger::block("Painting : 3D Paint : Paint window"); // Start
            // Perform window painting
            window_paint(&mirrorSide->paintingBuffers.window_painting_texture, strokes, stroke_brush, frame_counter, settings.painting_mode == 2);
            Debugger::block("Painting : 3D Paint : Paint window"); // End

            Debugger::block("Painting : 3D Paint : Update depth texture"); // Start
            update_depth_texture(mirrorSide->paintingBuffers.depth_texture, cam, settings.vertex_buffer.model_mesh);
            Debugger::block("Painting : 3D Paint : Update depth texture"); // End

            Debugger::block("Painting : 3D Paint : Project window painting texture"); // Start
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
            Debugger::block("Painting : 3D Paint : Project window painting texture"); // End
        }
    }
    else{
        Debugger::block("Painting : Process 2D point"); // Start
        std::vector<glm::vec2> strokes;
        process_2D_point(settings.point.point_2D, &strokes, first_frame, stroke_brush.properties.spacing);
        Debugger::block("Painting : Process 2D point"); // End

        Debugger::block("Painting : 2D Paint : Window painting"); // Start
        // Perform window painting
        window_paint(&O_side.paintingBuffers.window_painting_texture, strokes, stroke_brush, frame_counter, settings.painting_mode == 2);
        Debugger::block("Painting : 2D Paint : Window painting"); // End
        
        for (MirrorSide* mirrorSide : mirrorSides){
            Camera cam = getScene()->camera;
            
            if(mirrorSide->effectAxis != O_side.effectAxis){
                cam = twoD_painting_calculate_camera(settings, *mirrorSide);

                Debugger::block("Painting : 2D Paint : Generate mirrored window painting texture"); // Start
                twoD_painting_generate_mirrored_window_painting_texture(mirrorSide);                
                Debugger::block("Painting : 2D Paint : Generate mirrored window painting texture"); // End
            }

            if(first_frame){
                Debugger::block("Painting : 2D Paint : Update depth texture"); // Start
                update_depth_texture(mirrorSide->paintingBuffers.depth_texture, cam, settings.vertex_buffer.model_mesh);
                Debugger::block("Painting : 2D Paint : Update depth texture"); // End
            }

                Texture* projected_painting_texture = &mirrorSide->paintingBuffers.projected_painting_texture_low; 
                if(last_frame){
                    projected_painting_texture = &mirrorSide->paintingBuffers.projected_painting_texture; 
                }

            Debugger::block("Painting : 2D Paint : Project window painting texture"); // Start
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
            Debugger::block("Painting : 2D Paint : Project window painting texture"); // End
        }
    }
    
    Debugger::block("Painting : Generate projected painting texture"); // Start
    generate_projected_painting_texture(&painting_projected_painting_FBO, settings.mirror_settings.X, settings.mirror_settings.Y, settings.mirror_settings.Z, !last_frame && !settings.point.use_3D);
    Debugger::block("Painting : Generate projected painting texture"); // End
        
    frame_counter++;

    UPDATE_TEXTURE:

    if(last_frame){
        if(twoD_painting_mode && panel_displaying_modes.selectedElement != 2){
            LGDLOG::start << "ERROR : Painting : Invalid displaying mode for the 2D painting" << LGDLOG::end;
            RETURN_FROM_PAINTING;
        }

        if(button_mesh_selection.selectedMeshI >= getScene()->model->meshes.size()){
            LGDLOG::start << "ERROR : Painting : Invalid selected mesh" << LGDLOG::end;
            RETURN_FROM_PAINTING;
        }
        
        if(settings.color_buffer.use_custom_material){
            Debugger::block("Painting : Update custom material mesh"); // Start
            update_custom_material_mesh(settings.color_buffer, settings.vertex_buffer.model_mesh, (settings.painted_buffers.material_painting) ? settings.painted_buffers.material_channel_albedo.getResolution() : settings.painted_buffers.solid_painted_texture.getResolution()); 
            Debugger::block("Painting : Update custom material mesh"); // End
        }

        register_history_actions(settings.painting_mode, settings.painted_buffers);

        if(settings.painting_mode == 4){
            for (PaintedBufferData painted_buffer : get_painted_buffers(settings)){
                char clrPx[4] = {painted_buffer.clr.r * 127, painted_buffer.clr.g * 127, painted_buffer.clr.b * 127, 1.};
                one_px_txtr.update(clrPx, 1, 1);
                button_painting_filter_mode_filter.filter.applyFilter(painted_buffer.txtr.ID, painting_projected_painting_FBO.colorBuffer, one_px_txtr);
            }
        }
        else{
            for (PaintedBufferData painted_buffer : get_painted_buffers(settings))
            {
                if(settings.color_buffer.use_custom_material){
                    Debugger::block("Painting : Applying custom mat"); // Start
                    painted_buffer.txtr.mix(painted_buffer.corresponding_custom_material_channel, painting_projected_painting_FBO.colorBuffer, true, false, false);
                    painted_buffer.txtr.removeSeams(*getScene()->get_selected_mesh(), painted_buffer.txtr.getResolution());
                    Debugger::block("Painting : Applying custom mat"); // End
                }
                else{
                    Debugger::block("Painting : Update the texture"); // Start
                    updateTheTexture(painted_buffer.txtr, painted_buffer.channel_index, settings, painting_projected_painting_FBO);
                    Debugger::block("Painting : Update the texture"); // End

                    for (size_t i = 0; i < Library::getTextureArraySize(); i++)
                    {
                        if(painted_buffer.txtr.ID == Library::getTexture(i)->ID){
                            Library::getTexture(i)->copyDataToTheCopyContext();
                            projectUpdatingThreadElements.updateTextures = true;
                        }
                    }
                }
            }
        }

        refresh_buffers(&painting_projected_painting_FBO);

        updateThePreRenderedPanels = true;
    }

    RETURN_FROM_PAINTING;
}










bool painting_paintable_condition()
{
    return (!panels_any_hovered() || (panel_twoD_painting.hover && twoD_painting_mode)) && 
            (panel_displaying_modes.selectedElement == 1 || panel_displaying_modes.selectedElement == 2) && panel_painting_modes.selectedElement != 5 &&
            !checkComboList_painting_over.panel.sections[0].elements[1].checkBox.clickState1 &&
            !getScene()->get_selected_mesh()->face_selection_data.editMode &&
            !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) &&
            !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_ALT);
}

bool painting_paint_condition(){
    return painting_paintable_condition() && *Mouse::LPressed();
}

std::vector<MirrorSide*> painting_get_selected_mirror_sides(bool mirror_X, bool mirror_Y, bool mirror_Z){
    std::vector<MirrorSide*> mirrorSides;
    
    bool mirror_XY = mirror_X && mirror_Y;     
    bool mirror_XZ = mirror_X && mirror_Z; 
    bool mirror_YZ = mirror_Y && mirror_Z; 
    bool mirror_XYZ = mirror_X && mirror_Y && mirror_Z; 
    
    if(true)
        mirrorSides.push_back(&O_side); // 0
    if(mirror_X)
        mirrorSides.push_back(&X_side); // 1
    if(mirror_Y)
        mirrorSides.push_back(&Y_side); // 2
    if(mirror_XY)
        mirrorSides.push_back(&XY_side); // 3
    if(mirror_Z)
        mirrorSides.push_back(&Z_side); // 4
    if(mirror_XZ)
        mirrorSides.push_back(&XZ_side); // 5
    if(mirror_YZ)
        mirrorSides.push_back(&YZ_side); // 6
    if(mirror_XYZ)
        mirrorSides.push_back(&XYZ_side); // 7

    return mirrorSides;
}

#define INIT_MIRROR_SIDE(mirror_side, axis) mirror_side.paintingBuffers.depth_texture  = Texture(nullptr, 1024, 1024, GL_LINEAR, GL_RGBA, GL_RGBA32F);\
                                            mirror_side.paintingBuffers.window_painting_texture  = Texture(nullptr, 1024, 1024);\
                                            mirror_side.paintingBuffers.projected_painting_texture  = Texture(nullptr, 1024, 1024, GL_LINEAR);\
                                            mirror_side.paintingBuffers.projected_painting_texture_low  = Texture(nullptr, 512, 512, GL_LINEAR, GL_RGBA, GL_RGBA16F);\
                                            mirror_side.effectAxis = axis;

void painting_init_buffers(){
    one_px_txtr = Texture(nullptr, 1, 1, GL_NEAREST);

    painting_projected_painting_FBO = Framebuffer(Texture(nullptr, 1024, 1024), GL_TEXTURE_2D, "projected_painting_FBO");
    
    INIT_MIRROR_SIDE(O_side, glm::vec3(-1.f, -1.f, -1.f))
    INIT_MIRROR_SIDE(X_side, glm::vec3(1.f, -1.f, -1.f));
    INIT_MIRROR_SIDE(Y_side, glm::vec3(-1.f, 1.f, -1.f));
    INIT_MIRROR_SIDE(XY_side, glm::vec3(1.f, 1.f, -1.f));
    INIT_MIRROR_SIDE(Z_side, glm::vec3(-1.f, -1.f, 1.f));
    INIT_MIRROR_SIDE(XZ_side, glm::vec3(1.f, -1.f, 1.f));
    INIT_MIRROR_SIDE(YZ_side, glm::vec3(-1.f, 1.f, 1.f));
    INIT_MIRROR_SIDE(XYZ_side, glm::vec3(1.f, 1.f, 1.f));
}