/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Render all the GUI elements (Panels, Dialogs & stuff)

*/


#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer.h"
#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "Toolkit/Layers/Layers.hpp"
#include "GUI/Panels.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

// Defined in the RenderPanel.cpp
extern bool updateThePreRenderedPanels;

/* -- Forward declerations -- */

static void renderBrushCursor(float radius);

std::vector<ThreeDPoint> threedPoints;

void UI::render(Timer &timer,Project &project, Painter &painter, Skybox &skybox){
    
    Debugger::block("GUI : Start"); // Start
    
    //Set pass less or equal
    glDepthFunc(GL_LEQUAL);

    //Give projection to the curve shader        
    ShaderSystem::vectoralCurve().use();
    ShaderSystem::vectoralCurve().setMat4("projection", getScene()->gui_projection); 
    
    //Use the related shader 
    ShaderSystem::buttonShader().use();
    ShaderSystem::buttonShader().setVec2("properties.txtrScale", glm::vec2(1.f));
    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.f);

    //Set the ortho projection     
    ShaderSystem::buttonShader().setMat4("projection", getScene()->gui_projection); 

    Debugger::block("GUI : Start"); // End
    
    Debugger::block("GUI : Complete Panels"); // Start

    //Render the panels
    renderPanels(timer, painter, project);

    updateThePreRenderedPanels = false;

    Debugger::block("GUI : Complete Panels"); // End

    //Render the brush cursor
    if(
            !panels_any_hovered() && 
            (painter.selectedDisplayingModeIndex == 1 || painter.selectedDisplayingModeIndex == 2) && painter.selectedPaintingModeIndex != 5 && painter.selectedPaintingModeIndex != 6 &&
            !painter.paintingoverTextureEditorMode &&
            !painter.faceSelection.editMode &&
            !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) &&
            !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_ALT) && 
            !painter.wrapMode
        )
    {
        renderBrushCursor(painter.brushProperties.radius);
    }
    else if(painter.faceSelection.editMode && painter.faceSelection.selectionModeIndex == 0 && !panels_any_hovered()){
        renderBrushCursor(painter.faceSelection.radius / Settings::videoScale()->x);
    }
    else{
        getContext()->window.setCursorVisibility(true);
    }
}



//UTILITY FUNCTIONS

static void renderBrushCursor(float radius){
    /* Use the circle shader */
    ShaderSystem::circleShader().use();

    /* Set the transform data & the projection */
    ShaderSystem::circleShader().setMat4("projection", getScene()->gui_projection);
    ShaderSystem::circleShader().setVec3("pos", glm::vec3(Mouse::cursorPos()->x, Mouse::cursorPos()->y, 1));
    ShaderSystem::circleShader().setVec2("scale", glm::vec2(radius * (Settings::videoScale()->x)));

    /* Hide the cursor */
    getContext()->window.setCursorVisibility(false);

    /* Render the circle s*/
    LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Rendering 2D brush cursor");

    /* Clear the depth buffer of the current framebuffers*/
    glClear(GL_DEPTH_BUFFER_BIT);

    /* Use the button shader back */
    ShaderSystem::buttonShader().use();
}

std::string __faceSelectionActiveMesh = "";
int __faceSelectionActiveObjIndex = 0;

static void renderDotsToWindow(){
    ShaderSystem::dotsShader().use();

    ShaderSystem::dotsShader().setMat4("projection", getScene()->gui_projection);
    ShaderSystem::dotsShader().setVec3("pos", glm::vec3(getContext()->windowScale / glm::ivec2(2), 0.5f));
    ShaderSystem::dotsShader().setVec2("scale", getContext()->windowScale / glm::ivec2(2));

    ShaderSystem::dotsShader().setVec2("dotPos", glm::vec2(0.f));
    ShaderSystem::dotsShader().setFloat("scroll", 1.f);

    /* Render the dots */
    LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Rendering dots to window");

    /* Clear the depth buffer of the current framebuffers*/
    glClear(GL_DEPTH_BUFFER_BIT);

    ShaderSystem::buttonShader().use(); 
}

void UI::renderPanels(Timer &timer, Painter &painter, Project& project){
    
    Debugger::block("GUI : Panels : Start"); // Start
    Debugger::block("GUI : Panels : Start"); // End

    if(painter.paintingoverTextureEditorMode){
        Debugger::block("GUI : Rendering dots for the painting over texture fields editor mode"); // Start
        renderDotsToWindow();
        Debugger::block("GUI : Rendering dots for the painting over texture fields editor mode"); // End
    }

    if(painter.usePaintingOver){
        Debugger::block("GUI : Texture fields"); // Start
        getTextureFieldScene()->render(timer, painter, !panels_any_hovered(), checkBox_wrap_mode.clickState1);
        Debugger::block("GUI : Texture fields"); // End
    }
    
    if(painter.selectedPaintingModeIndex == 5 && painter.selectedDisplayingModeIndex != 0 && !painter.paintingoverTextureEditorMode){
        Debugger::block("GUI : Vectors"); // Start
        getVectorScene()->render_scene(timer, !panels_any_hovered(), checkBox_wrap_mode.clickState1);
        Debugger::block("GUI : Vectors"); // End
    }
    
    Debugger::block("GUI : Panels : Rest"); // Start

    bool straightLinePaintingCondition = painter.selectedDisplayingModeIndex != 0 && painter.selectedPaintingModeIndex != 5 && (getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) || getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_ALT)) && *Mouse::LPressed(); 
    if(straightLinePaintingCondition && !painter.faceSelection.editMode)
    {
        if(!prevStraightLinePaintingCondition){
            straightLinePaintingStartPos = *Mouse::cursorPos() / *Settings::videoScale() * 100.f;
            glm::vec2 cursorOffset = *Mouse::mouseOffset();
            if(cursorOffset.x || cursorOffset.y)
                cursorOffset = glm::normalize(cursorOffset);
            straightLinePaintingDirectionPos = straightLinePaintingStartPos + cursorOffset * 5.f; 
        }

        glm::vec2 destPos = *Mouse::cursorPos() / *Settings::videoScale() * 100.f;
        
        if(glm::distance(straightLinePaintingDirectionPos, straightLinePaintingStartPos) < 10.f){
            straightLinePaintingDirectionPos = destPos; 
        }

        //ABS
        ShaderSystem::vectoralCurve().use();
        
        ShaderSystem::vectoralCurve().setVec3("pos", glm::vec3(Settings::videoScale()->x/2.f, Settings::videoScale()->y/2.f, 0.9f));
        ShaderSystem::vectoralCurve().setVec2("scale", glm::vec2(Settings::videoScale()->x/2.f, Settings::videoScale()->y/2.f));
        
        if(getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_ALT))
            ShaderSystem::vectoralCurve().setVec2("direction", straightLinePaintingDirectionPos);
        else
            ShaderSystem::vectoralCurve().setVec2("direction", destPos);
        
        ShaderSystem::vectoralCurve().setVec2("startPos", straightLinePaintingStartPos);
        ShaderSystem::vectoralCurve().setVec2("destPos", destPos);
        
        ShaderSystem::vectoralCurve().setVec2("percScale", *Settings::videoScale());
        ShaderSystem::vectoralCurve().setInt("lineCapturingMode", 0);
        
        ShaderSystem::vectoralCurve().setFloat("EDGE", 0.0005f);

        LigidGL::makeDrawCall(GL_TRIANGLES, 0 , 6, "Straight line painting vector");
        
        ShaderSystem::buttonShader().use();
    }
    else if(prevStraightLinePaintingCondition && !painter.faceSelection.editMode){
    /*std::vector<VectorStroke> strokeArray;
    strokeArray.push_back(VectorStroke(straightLinePaintingStartPos, *Mouse::cursorPos() / *Settings::videoScale() * 100.f, straightLinePaintingDirectionPos));
    painter.applyVectorStrokes(
                                strokeArray, 
                                painter.selectedPaintingModeIndex, 
                                paintingColorCheckComboList.panel.sections[0].elements[14].button.material,
                                painter.wrapMode
                            );*/
    }
    
    prevStraightLinePaintingCondition = straightLinePaintingCondition;

    Debugger::block("GUI : Panels : Rest"); // End
}
