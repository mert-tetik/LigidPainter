/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "UTIL/GL/GL.hpp"
#include "UTIL/Painting/Painter.hpp"

#include "3D/ThreeD.hpp"

#include "Toolkit/Layers/Layers.hpp"

#include "GUI/GUI.hpp"
#include "GUI/Panels.hpp"

#include <string>
#include <vector>

Panel panel_twoD_painting;
Box twoD_painting_box;
bool twoD_painting_mode = false;

/*! Scene transform data */
static float scroll = 2.f;
static glm::vec2 pos = glm::vec2(0.f);

static void render_barriers();
static void transform_scene();

void panel_twoD_painting_render(Timer& timer, bool doMouseTracking){
    

    if(panel_displaying_modes.selectedElement == 2){
        transform_scene();
        
        Settings::defaultFramebuffer()->FBO.bind();
        Settings::defaultFramebuffer()->setViewport();

        //Render the 2D painting panel
        panel_twoD_painting.sections[0].elements[0].button.text = "";
        panel_twoD_painting.render(timer,false);
        if(panel_twoD_painting.resizingDone){
            panels_transform();
        }

        render_barriers();

        ShaderSystem::twoDPaintingModeAreaShader().use();

        ShaderUTIL::set_shader_struct_painting_data(
                                                    ShaderSystem::twoDPaintingModeAreaShader(), 
                                                    ShaderUTIL::PaintingData(
                                                                                ShaderUTIL::PaintingData::PaintingBuffers(
                                                                                                                            GL_TEXTURE5,
                                                                                                                            (getScene()->get_selected_mesh()->face_selection_data.meshMask.ID) ? getScene()->get_selected_mesh()->face_selection_data.meshMask : appTextures.black,
                                                                                                                            0,
                                                                                                                            0,
                                                                                                                            0,
                                                                                                                            0,
                                                                                                                            painting_projected_painting_FBO.colorBuffer
                                                                                                                        ),   
                                                                                ShaderUTIL::PaintingData::PaintingSmearData(
                                                                                                                                panel_smear_painting_properties.sections[0].elements[0].rangeBar.value,
                                                                                                                                panel_smear_painting_properties.sections[0].elements[1].rangeBar.value
                                                                                                                            ),   
                                                                                ShaderUTIL::PaintingData::ChannelData(
                                                                                                                        1.f,
                                                                                                                        1.f,
                                                                                                                        1.f,
                                                                                                                        1.f,
                                                                                                                        1.f
                                                                                                                    ),
                                                                                panel_painting_modes.selectedElement,
                                                                                checkComboList_painting_over.panel.sections[0].elements[0].checkBox.clickState1                            
                                                                            )
                                                );


        ShaderSystem::twoDPaintingModeAreaShader().setInt("channel_index", 0);
        
        //*Vertex
        ShaderSystem::twoDPaintingModeAreaShader().setMat4("projection", getContext()->ortho_projection);
        ShaderSystem::twoDPaintingModeAreaShader().setMat4("view", glm::mat4(1.));
        ShaderSystem::twoDPaintingModeAreaShader().setMat4("modelMatrix", glm::mat4(1.));
        
        twoD_painting_box.draw("2D Painting scene : painted texture");

        GL::releaseBoundTextures("panel_twoD_painting_render");
        ShaderUTIL::release_bound_shader();
    }
    else{
        //Render the 2D painting panel
        panel_twoD_painting.sections[0].elements[0].button.text = "2D Painting can't be displayed in the current displaying mode";
        panel_twoD_painting.render(timer, false);
        if(panel_twoD_painting.resizingDone){
            panels_transform();
        }
    }
    
    glClear(GL_DEPTH_BUFFER_BIT);
}

static void render_barriers(){
    ShaderSystem::color2d().use();

    //Bottom Barrier
    ShaderSystem::color2d().setMat4("projection", getContext()->ortho_projection);
    ShaderSystem::color2d().setVec3("pos", glm::vec3(panel_twoD_painting.resultPos.x,   panel_twoD_painting.resultPos.y + panel_twoD_painting.resultScale.y + 5000, 1.f)); 
    ShaderSystem::color2d().setVec2("scale", glm::vec2(5000));
    ShaderSystem::color2d().setVec4("color" , glm::vec4(0.f)); 
    getBox()->draw("2D Painting panel : Barrier : Bottom");

    //Top Barrier
    ShaderSystem::color2d().setVec3("pos", glm::vec3(panel_twoD_painting.resultPos.x,   panel_twoD_painting.resultPos.y - panel_twoD_painting.resultScale.y - 5000,   1.f));
    ShaderSystem::color2d().setVec2("scale", glm::vec2(5000));
    getBox()->draw("2D Painting panel : Barrier : Top");
    
    //Left Barrier
    ShaderSystem::color2d().setVec3("pos", glm::vec3(panel_twoD_painting.resultPos.x  - panel_twoD_painting.resultScale.x  - 5000,   panel_twoD_painting.resultPos.y,   1.f));
    ShaderSystem::color2d().setVec2("scale", glm::vec2(5000));
    getBox()->draw("2D Painting panel : Barrier : Left");
    
    //Right Barrier
    ShaderSystem::color2d().setVec3("pos", glm::vec3(panel_twoD_painting.resultPos.x  + panel_twoD_painting.resultScale.x  + 5000,   panel_twoD_painting.resultPos.y,   1.f));
    ShaderSystem::color2d().setVec2("scale", glm::vec2(5000));
    getBox()->draw("2D Painting panel : Barrier : Right");

    ShaderUTIL::release_bound_shader();
}

static void transform_scene(){
    glm::vec2 destScale = glm::vec2(glm::vec2(panel_library_selected_texture.getResolution()));
    glm::vec2 prevScale = destScale * scroll;
    float scrVal = *Mouse::mouseScroll() / Settings::videoScale()->y * 4.f;
    if(!panel_twoD_painting.hover)
        scrVal = 0.f;

    //Calculate the scroll value
    if(scroll > 1.f)
        scroll += scrVal;
    else{
        if(scrVal < 0)
            scroll -= scroll / 4.f;
        if(scrVal > 0)
            scroll += scroll / 4.f;
    } 
    if(scroll < 0.02)
        scroll = 0.02;

    bool posChanged = false;
    //Calculate the position value        
    if(*Mouse::MPressed() && panel_twoD_painting.hover){
        pos += *Mouse::mouseOffset();
        posChanged = true;
    }

    //(Zoom into the cursor)
    glm::vec2 scaleGap = prevScale - destScale * scroll;
    pos += (scaleGap) * (((*Mouse::cursorPos() - glm::vec2(panel_twoD_painting.sections[0].elements[0].button.resultPos.x + pos.x, panel_twoD_painting.sections[0].elements[0].button.resultPos.y + pos.y)) + 0.00001f) / (prevScale + 0.00001f));

    if(pos.x > destScale.x * scroll)
        pos.x = destScale.x * scroll;
    if(pos.x < -destScale.x * scroll)
        pos.x = -destScale.x * scroll;
    
    if(pos.y > destScale.y * scroll)
        pos.y = destScale.y * scroll;
    if(pos.y < -destScale.y * scroll)
        pos.y = -destScale.y * scroll;

    glm::vec3 boxPos = glm::vec3(
                                    panel_twoD_painting.sections[0].elements[0].button.resultPos.x + pos.x,
                                    panel_twoD_painting.sections[0].elements[0].button.resultPos.y + pos.y,
                                    panel_twoD_painting.sections[0].elements[0].button.resultPos.z
                                );
    
    glm::vec2 boxScale = glm::vec2(destScale * scroll);
    
    // If scene transformed update the 2D square buffers
    if(twoD_painting_box.customPos != boxPos || twoD_painting_box.customScale != boxScale){
        twoD_painting_box.customMeshUpdate(boxPos, boxScale);
    }
}