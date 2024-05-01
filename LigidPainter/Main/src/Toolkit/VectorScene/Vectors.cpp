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

void VectorScene::render_scene(Timer& timer, bool doMouseTracking, bool threeD)
{
    // Rendering vectors
    if(threeD){
        this->render3DVectors(timer, doMouseTracking && !twoD_mode_wrap_checkBox.hover && !this->interaction_panel.hover);
    }
    else{
        this->render2DVectors(timer, doMouseTracking && !twoD_mode_wrap_checkBox.hover && !this->interaction_panel.hover);
    }

    getBox()->bindBuffers();
    ShaderSystem::buttonShader().use();

    // Rendering interaction panel
    interaction_panel.pos.x = panel_library.pos.x + panel_library.scale.x + interaction_panel.scale.x + 1;
    interaction_panel.pos.y = panel_painting_modes.pos.y + panel_painting_modes.scale.y + interaction_panel.scale.y + 1;

    twoD_mode_wrap_checkBox.scale.x = interaction_panel.scale.x; 
    twoD_mode_wrap_checkBox.pos = interaction_panel.pos; 
    twoD_mode_wrap_checkBox.pos.y += interaction_panel.scale.y + twoD_mode_wrap_checkBox.scale.y; 

    this->interaction_panel.render(timer, true); 
    
    if(!threeD)
        twoD_mode_wrap_checkBox.render(timer, true);

    if(this->interaction_panel.sections[0].elements[1].button.clicked){
        this->subdivide_selected_points(threeD);
    }
    if(this->interaction_panel.sections[0].elements[2].button.clicked){
        this->clear_points(threeD);
    }
    if(this->interaction_panel.sections[0].elements[3].button.clicked || shortcuts_CTRL_X()){
        this->delete_selected_points(threeD);
    }
    if(this->interaction_panel.sections[0].elements[4].button.clicked){
        bool success;
        PaintSettings paint_settings = get_paint_settings_using_GUI_data(&success);
        paint_settings.painting_mode = this->interaction_panel.sections[0].elements[0].comboBox.selectedIndex;
        if(success){
            this->apply_strokes(
                                    threeD,
                                    twoD_mode_wrap_checkBox.clickState1,
                                    paint_settings
                                );
        }
    }
}

void VectorScene::delete_selected_points(bool threeD){
    if(!threeD){
        if(!this->isAny2DPointsActive())
            return;

        // registerVectorAction("Selected point deleted", this->strokes_2D);
        while (this->isAny2DPointsActive()){
            for (size_t i = 0; i < this->strokes_2D.size(); i++)
            {
                if(i == 0){
                    if(this->strokes_2D[i].startPoint.active && this->strokes_2D[i].endPoint.active && this->strokes_2D.size() == 1){
                        this->strokes_2D.clear(); 
                        break;
                    }
                    
                    if(this->strokes_2D[i].startPoint.active && this->strokes_2D.size() == 1){
                        this->strokes_2D[i].startPoint.pos = this->strokes_2D[i].endPoint.pos; 
                        this->strokes_2D[i].offsetPoint.pos = this->strokes_2D[i].endPoint.pos; 
                        this->strokes_2D[i].startPoint.active = false; 
                        break;
                    }
                    
                    if(this->strokes_2D[i].endPoint.active && this->strokes_2D.size() == 1){
                        this->strokes_2D[i].endPoint.pos = this->strokes_2D[i].startPoint.pos; 
                        this->strokes_2D[i].offsetPoint.pos = this->strokes_2D[i].startPoint.pos; 
                        this->strokes_2D[i].endPoint.active = false; 
                        break;
                    }

                    if(this->strokes_2D[i].startPoint.active){
                        this->strokes_2D.erase(this->strokes_2D.begin() + i);
                        break;
                    }
                    
                    if(this->strokes_2D[i].endPoint.active && this->strokes_2D.size() > 1){
                        this->strokes_2D[i].endPoint.pos = this->strokes_2D[i + 1].endPoint.pos; 
                        this->strokes_2D[i].endPoint.active = this->strokes_2D[i + 1].endPoint.active; 
                        this->strokes_2D.erase(this->strokes_2D.begin() + i + 1);
                        break;
                    }
                }
                else{
                    if(this->strokes_2D[i].endPoint.active){
                        this->strokes_2D.erase(this->strokes_2D.begin() + i);
                        if(i - 1 < this->strokes_2D.size() && i < this->strokes_2D.size()){
                            this->strokes_2D[i].startPoint.pos = this->strokes_2D[i - 1].endPoint.pos; 
                            this->strokes_2D[i].startPoint.active = false; 
                        }
                        break;
                    }
                }
            }
        }
    }
    else{
        if(!this->isAnyWrappedPointsActive())
            return;
        
        // registerVectorAction("Selected wrapped point deleted", this->strokes_3D);
        while(this->isAnyWrappedPointsActive()){
            for (size_t i = 0; i < this->strokes_3D.size(); i++)
            {
                if(i == 0){

                    if(this->strokes_3D[i].startPoint.active && this->strokes_3D[i].endPoint.active && this->strokes_3D.size() == 1){
                        this->strokes_3D.clear(); 
                        break;
                    }
                    
                    if(this->strokes_3D[i].startPoint.active && this->strokes_3D.size() == 1){
                        this->strokes_3D[i].startPoint = this->strokes_3D[i].endPoint; 
                        break;
                    }
                    
                    if(this->strokes_3D[i].endPoint.active && this->strokes_3D.size() == 1){
                        this->strokes_3D[i].endPoint = this->strokes_3D[i].startPoint; 
                        break;
                    }

                    if(this->strokes_3D[i].startPoint.active){
                        this->strokes_3D.erase(this->strokes_3D.begin() + i);
                        break;
                    }
                    
                    if(this->strokes_3D[i].endPoint.active && this->strokes_3D.size() > 1){
                        this->strokes_3D[i].endPoint = this->strokes_3D[i + 1].endPoint; 
                        this->strokes_3D.erase(this->strokes_3D.begin() + i + 1);
                        break;
                    }
                }
                else{
                    if(this->strokes_3D[i].endPoint.active){
                        this->strokes_3D.erase(this->strokes_3D.begin() + i);
                        if(i - 1 < this->strokes_3D.size() && i < this->strokes_3D.size()){
                            this->strokes_3D[i].startPoint = this->strokes_3D[i - 1].endPoint; 
                            this->strokes_3D[i].startPoint.active = false; 
                        }
                        break;
                    }
                }
            }
        }

        this->update3DVectorBuffers();
    }
}

void VectorScene::clear_points(bool threeD){
    if(!threeD){
        // registerVectorAction("Vector strokes cleared", this->strokes_2D);
        this->strokes_2D.clear();
    }
    else{
        // registerVectorAction("Wrapped Vector strokes cleared", this->strokes_3D);
        this->strokes_3D.clear();
    }
}

void VectorScene::subdivide_selected_points(bool threeD){
    if(!threeD){
        // registerVectorAction("New point between the selected points", this->strokes_2D);

        for (size_t i = 0; i < this->strokes_2D.size(); i++)
        {
            if(this->strokes_2D[i].endPoint.active && this->strokes_2D[i].startPoint.active){
                glm::vec2 strokeCenter = (this->strokes_2D[i].startPoint.pos + this->strokes_2D[i].endPoint.pos + this->strokes_2D[i].offsetPoint.pos) / 3.0f;
                glm::vec2 offsetPointDistance = strokeCenter - this->strokes_2D[i].offsetPoint.pos;

                VectorStroke newStroke;
                newStroke.startPoint.pos = strokeCenter;
                newStroke.endPoint.pos = this->strokes_2D[i].endPoint.pos;
                newStroke.offsetPoint.pos = newStroke.startPoint.pos - (newStroke.startPoint.pos - newStroke.endPoint.pos) / 2.f - offsetPointDistance;

                this->strokes_2D[i].endPoint.pos  = strokeCenter;

                this->strokes_2D.insert(this->strokes_2D.begin() + i + 1, newStroke);
                i++;
            }
        }
    }
    else{
        // registerVectorAction("New point between the selected wrapped points", this->strokes_3D);

        for (size_t i = 0; i < this->strokes_3D.size(); i++)
        {
            if(this->strokes_3D[i].endPoint.active && this->strokes_3D[i].startPoint.active){
                glm::vec3 strokeCenter = (this->strokes_3D[i].startPoint.pos + this->strokes_3D[i].endPoint.pos) / 2.0f;
                glm::vec3 offsetPointDistance = strokeCenter;

                VectorStroke3D newStroke;
                newStroke.startPoint.pos = strokeCenter;
                newStroke.endPoint = this->strokes_3D[i].endPoint;

                this->strokes_3D[i].endPoint.pos  = strokeCenter;

                this->strokes_3D.insert(this->strokes_3D.begin() + i + 1, newStroke);
                i++;
            }
        }

        this->update3DVectorBuffers();
    }
}

static bool lastVecMovingConditionState = false; 

void VectorScene::render2DVectors(Timer& timer, bool doMouseTracking){
    
    // Set the cursor to ink pen if the current cursor is the default one 
    if(doMouseTracking && Mouse::activeCursor()->cursorType == Mouse::defaultCursor()->cursorType && doMouseTracking)
        Mouse::setCursor(*Mouse::inkPenCursor());

    getBox()->bindBuffers();

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
        // registerVectorAction("Point moved", this->strokes_2D);

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
    
    getScene()->get_selected_mesh()->Draw();

    GL::releaseBoundTextures("VectorScene : render3DVectors");

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
        // registerVectorAction("Wrapped point moved", this->strokes_3D);

    lastVecMovingConditionState = anyPointMovingCondition;

    // Clear the depth buffer
    glClear(GL_DEPTH_BUFFER_BIT);
    
    // Add new point
    if(*Mouse::LClick() && clickedPointI == -1 && doMouseTracking){
        this->addNew3DVector();
        this->update3DVectorBuffers();
    }

    getBox()->bindBuffers();
    ShaderSystem::buttonShader().use();
    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();
}

void VectorScene::addNew2DVector(){
    VectorStroke vecStroke;
    if(!this->strokes_2D.size()){
        // registerVectorAction("First point created", this->strokes_2D);
        vecStroke.startPoint.pos = *Mouse::cursorPos() / *Settings::videoScale() * 100.f; 
        vecStroke.endPoint.pos = vecStroke.startPoint.pos;
        vecStroke.offsetPoint.pos = vecStroke.startPoint.pos;
        this->strokes_2D.push_back(vecStroke);
    }
    else{
        if(this->strokes_2D[this->strokes_2D.size() - 1].endPoint.pos == this->strokes_2D[this->strokes_2D.size() - 1].startPoint.pos){
            // registerVectorAction("New point", this->strokes_2D);
            this->strokes_2D[this->strokes_2D.size() - 1].endPoint.pos = *Mouse::cursorPos() / *Settings::videoScale() * 100.f;
            this->strokes_2D[this->strokes_2D.size() - 1].offsetPoint.pos = this->strokes_2D[this->strokes_2D.size() - 1].startPoint.pos - (this->strokes_2D[this->strokes_2D.size() - 1].startPoint.pos - this->strokes_2D[this->strokes_2D.size() - 1].endPoint.pos) / 2.f;
            this->strokes_2D[this->strokes_2D.size() - 1].offsetPoint.pos += 0.001f; // Vectors can't be rendered if the offset point alligns perfectly :(
        }
        else{
            // registerVectorAction("New point", this->strokes_2D);
            vecStroke.startPoint.pos = this->strokes_2D[this->strokes_2D.size() - 1].endPoint.pos; 
            vecStroke.endPoint.pos = *Mouse::cursorPos() / *Settings::videoScale() * 100.f;
            vecStroke.offsetPoint.pos = vecStroke.startPoint.pos - (vecStroke.startPoint.pos - vecStroke.endPoint.pos) /2.f;
            vecStroke.offsetPoint.pos += 0.001f; // Vectors can't be rendered if the offset point alligns perfectly :(
            this->strokes_2D.push_back(vecStroke);
        }
    }
}

void VectorScene::addNew3DVector(){

    // Point on top of the cursor is not valid 
    if(getScene()->get_selected_mesh()->getCurrentPosNormalDataOverCursor().pos == glm::vec3(-1000.f))
        return;

    VectorStroke3D vecStroke;
    if(!this->strokes_3D.size()){
        // registerVectorAction("First wrapped point created", this->strokes_3D);
        vecStroke.startPoint = getScene()->get_selected_mesh()->getCurrentPosNormalDataOverCursor(); 
        vecStroke.endPoint = vecStroke.startPoint;
        this->strokes_3D.push_back(vecStroke);
    }
    else{
        if(this->strokes_3D[this->strokes_3D.size() - 1].endPoint.pos == this->strokes_3D[this->strokes_3D.size() - 1].startPoint.pos){
            // registerVectorAction("New wrapped point", this->strokes_3D);
            this->strokes_3D[this->strokes_3D.size() - 1].endPoint = getScene()->get_selected_mesh()->getCurrentPosNormalDataOverCursor();
        }
        else{
            // registerVectorAction("New wrapped point", this->strokes_3D);
            vecStroke.startPoint = this->strokes_3D[this->strokes_3D.size() - 1].endPoint; 
            vecStroke.endPoint = getScene()->get_selected_mesh()->getCurrentPosNormalDataOverCursor();
            this->strokes_3D.push_back(vecStroke);
        }
    }
}

void VectorScene::update3DVectorBuffers(){
    for (size_t i = 0; i < this->strokes_3D.size(); i++)
    {
        this->strokes_3D[i].updateLinePoints();
    }
}

/* 
    Bezier curve function
    
    B(t) = (1 - t)^2 * start + 2 * (1 - t) * t * offset + t^2 * end
*/
static glm::vec2 bezier(float t, glm::vec2 start, glm::vec2 end, glm::vec2 offset){
    return glm::pow((glm::vec2(1.f) - glm::vec2(t)), glm::vec2(2)) * start + glm::vec2(2) * (glm::vec2(1) - glm::vec2(t)) * glm::vec2(t) * offset + glm::pow(glm::vec2(t), glm::vec2(2)) * end;
}

static std::vector<glm::vec2> calculateBezierCurve(glm::vec2 start, glm::vec2 end, glm::vec2 direction, int frequency){
    
    std::vector<glm::vec2> points;

    for (size_t i = 0; i < frequency; i++)
    {
        points.push_back(bezier((float)i / ((float)frequency - 1.f), start, end, direction));
    }

    return points;
}

void VectorScene::apply_strokes(
                                    bool threeD,
                                    bool twoDWrap,
                                    PaintSettings paint_settings
                                )
{
    if(threeD){
        for (VectorStroke3D stroke3D : this->strokes_3D)
        {
            int stroke_pos_i = 0;
            for (VertexUTIL stroke_pos : stroke3D.lineVertices){
                if(stroke_pos_i % (int)std::ceil((paint_settings.stroke_brush.properties.spacing + 1.f) / 20.f) == 0){
                    paint_settings.point.use_3D = true;
                    paint_settings.point.point_3D = ThreeDPoint(stroke_pos.Position, stroke_pos.Normal);
                    painting_paint_buffers(paint_settings, stroke_pos_i == 0, stroke_pos_i == stroke3D.lineVertices.size() - 1);
                }

                stroke_pos_i++;
            }
        }
    }
    else{
        std::vector<glm::vec2> strokePositions;

        for (size_t vecI = 0; vecI < strokes_2D.size(); vecI++)
        {
            float distance = glm::distance(strokes_2D[vecI].startPoint.pos, strokes_2D[vecI].offsetPoint.pos) + glm::distance(strokes_2D[vecI].endPoint.pos, strokes_2D[vecI].offsetPoint.pos);
            unsigned int quality = (unsigned int)(distance / 2.f);

            std::vector<glm::vec2> points = calculateBezierCurve(
                                                                    strokes_2D[vecI].startPoint.pos / 100.f * *Settings::videoScale(), 
                                                                    strokes_2D[vecI].endPoint.pos / 100.f * *Settings::videoScale(), 
                                                                    strokes_2D[vecI].offsetPoint.pos / 100.f * *Settings::videoScale(), 
                                                                    quality
                                                                );

            // Add the points of a single vector to the total stroke positions
            for (size_t pointI = 0; pointI < points.size(); pointI++)
            {
                strokePositions.push_back(points[pointI]);
            }
        }
        
        const int maxStrokeSize = 50;

        // Calculate how many subvectors you'll need
        const int numSubvectors = (strokePositions.size() + maxStrokeSize - 1) / maxStrokeSize;

        // Loop through and process the subvectors
        for (int i = 0; i < numSubvectors; ++i) {
            
            // Calculate the start and end indices for each subvector
            int startIdx = i * maxStrokeSize;
            int endIdx = std::min((i + 1) * maxStrokeSize, static_cast<int>(strokePositions.size()));

            // Create a subvector from the original vector
            std::vector<glm::vec2> subVector(strokePositions.begin() + startIdx, strokePositions.begin() + endIdx);

            for (size_t i = 0; i < subVector.size(); i++)
            {
                paint_settings.point.use_3D = false;
                paint_settings.point.point_2D = subVector[i];
                painting_paint_buffers(paint_settings, i == 0, i == subVector.size() - 1);
            }
        }

        ShaderSystem::buttonShader().use();
        Settings::defaultFramebuffer()->FBO.bind();
        Settings::defaultFramebuffer()->setViewport();
    }
}

bool VectorScene::isAny2DPointsActive(){
    for (size_t i = 0; i < this->strokes_2D.size(); i++)
    {
        if(this->strokes_2D[i].startPoint.active)
            return true;
        if(this->strokes_2D[i].endPoint.active)
            return true;
    }
    
    return false;
}

bool VectorScene::isAnyWrappedPointsActive(){
    for (size_t i = 0; i < this->strokes_3D.size(); i++)
    {
        if(this->strokes_3D[i].startPoint.active)
            return true;
        if(this->strokes_3D[i].endPoint.active)
            return true;
    }
    
    return false;
    
}