/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "UTIL/Util.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "UTIL/GL/GL.hpp"
#include "UTIL/Painting/Painter.hpp"

#include "GUI/GUI.hpp"
#include "GUI/Panels.hpp"

#include "3D/ThreeD.hpp"

#include "Toolkit/VectorScene/VectorScene.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

VectorScene::VectorScene()
{

}

VectorScene::VectorScene(std::vector<VectorStroke> strokes_2D, std::vector<VectorStroke3D> strokes_3D)
{
    this->strokes_2D = strokes_2D; 
    this->strokes_3D = strokes_3D; 
}


void VectorScene::render_scene(Timer& timer, bool doMouseTracking, bool threeD, bool enable_stroke)
{
    // Rendering vectors
    if(threeD){
        this->render3DVectors(timer, doMouseTracking);
    }
    else{
        this->render2DVectors(timer, doMouseTracking);
    }

    glClear(GL_DEPTH_BUFFER_BIT);

    bool any_3D_point_hovered = false;
    for(VectorStroke3D stroke : this->strokes_3D){
        if(stroke.startPoint.is_hovered(0.02f) || stroke.endPoint.is_hovered(0.02f))
            any_3D_point_hovered = true;

    }
    bool any_2D_point_hovered = false;
    for(VectorStroke stroke : this->strokes_2D){
        if(stroke.startPoint.hover || stroke.endPoint.hover || stroke.offsetPoint.hover)
            any_2D_point_hovered = true;
    }

    std::string new_point_between_selected_txt = "New Point Between Selected";
    std::string stroke_txt = "Stroke";
    std::string delete_selected_txt = "Delete Selected (CTRL + X)";
    std::string clear_all_txt = "Clear All";

    std::vector<std::string> context_menu_actions = {
                                                        new_point_between_selected_txt,                                                        
                                                        stroke_txt,                                                        
                                                        delete_selected_txt,                                                        
                                                        clear_all_txt                                       
                                                    };

    if(!enable_stroke){
        context_menu_actions.erase(context_menu_actions.begin() + 1);
    }

    int result = -1;
    if(!panels_any_hovered() && (any_3D_point_hovered || !threeD) && (any_2D_point_hovered || threeD) && *Mouse::RClick()){
        glDisable(GL_DEPTH_TEST);
        
        result = show_context_menu(timer, context_menu_actions);
        
        glEnable(GL_DEPTH_TEST);
    }

    if(result != -1){
        if(context_menu_actions[result] == new_point_between_selected_txt){
            this->subdivide_selected_points(threeD);
        }
        if(context_menu_actions[result] == stroke_txt){
            this->stroke_dialog.show_stroke_dialog(threeD, this);
        }
        if(context_menu_actions[result] == delete_selected_txt || shortcuts_CTRL_X()){
            this->delete_selected_points(threeD);
        }
        if(context_menu_actions[result] == clear_all_txt){
            this->clear_points(threeD);
        }
    }
}

static bool lastVecMovingConditionState = false; 

void VectorScene::render2DVectors(Timer& timer, bool doMouseTracking){
    
    // Set the cursor to ink pen if the current cursor is the default one 
    if(doMouseTracking && Mouse::activeCursor()->cursorType == Mouse::defaultCursor()->cursorType && doMouseTracking)
        Mouse::setCursor(*Mouse::inkPenCursor());

    // Render the vectors
    bool anyPointMovingCondition = false;
    for (int i = this->strokes_2D.size() - 1; i >= 0; i--)
    {
        this->strokes_2D[i].draw(timer, 0.0005f, doMouseTracking, this->strokes_2D, i);
    
        if(this->strokes_2D[i].startPoint.canMove)
            anyPointMovingCondition = true;
        if(this->strokes_2D[i].startPoint.canMove)
            anyPointMovingCondition = true;
        if(this->strokes_2D[i].startPoint.canMove)
            anyPointMovingCondition = true;
    }

    if(anyPointMovingCondition && !lastVecMovingConditionState)
        dialog_log.registerVectorAction("Point moved", this);

    lastVecMovingConditionState = anyPointMovingCondition;

    // Clear the depth buffer after rendering the vectors
    glClear(GL_DEPTH_BUFFER_BIT);
    
    // Check if any points hovered
    bool anyVectorPointHover = false;
    for (size_t i = 0; i < this->strokes_2D.size(); i++){
        if(this->strokes_2D[i].endPoint.hover || this->strokes_2D[i].startPoint.hover || this->strokes_2D[i].offsetPoint.hover)
            anyVectorPointHover = true;
    }

    if(*Mouse::LClick() && doMouseTracking && !anyVectorPointHover){
        this->addNew2DVector();
    }
}

void VectorScene::render3DVectors(Timer& timer, bool doMouseTracking){
    
    // Set the cursor to ink pen if the current cursor is the default one 
    if(doMouseTracking && Mouse::activeCursor()->cursorType == Mouse::defaultCursor()->cursorType)
        Mouse::setCursor(*Mouse::inkPenCursor());

    ShaderSystem::alphaZero3D().use();
    ShaderSystem::alphaZero3D().setMat4("view", getScene()->camera.viewMatrix);
    ShaderSystem::alphaZero3D().setMat4("projection", getScene()->projectionMatrix);
    ShaderSystem::alphaZero3D().setMat4("modelMatrix", getScene()->transformMatrix);
    
    ShaderUTIL::set_shader_struct_face_selection_data(ShaderSystem::alphaZero3D(), *getScene()->get_selected_mesh());
    
    getScene()->get_selected_mesh()->Draw("VectorScene::render3DVectors");

    GL::releaseBoundTextures("VectorScene : render3DVectors");
    ShaderUTIL::release_bound_shader();

    // Render all the vectors
    int clickedPointI = -1; 
    bool anyPointMovingCondition = false;
    for (size_t i = 0; i < this->strokes_3D.size(); i++)
    {
        if(this->strokes_3D[i].draw(timer, 0.0005f, doMouseTracking, this->strokes_3D, i))
            clickedPointI = i;
    
        if(this->strokes_3D[i].startPoint.areMovingConditionsSet(true))
            anyPointMovingCondition = true;
        
        if(this->strokes_3D[i].endPoint.areMovingConditionsSet(true))
            anyPointMovingCondition = true;
    }

    if(anyPointMovingCondition && !lastVecMovingConditionState)
        dialog_log.registerVectorAction("Wrapped point moved", this);

    lastVecMovingConditionState = anyPointMovingCondition;

    // Clear the depth buffer
    glClear(GL_DEPTH_BUFFER_BIT);
    
    // Add new point
    if(*Mouse::LClick() && clickedPointI == -1 && doMouseTracking){
        this->addNew3DVector();
        this->update3DVectorBuffers();
    }

    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();
}
