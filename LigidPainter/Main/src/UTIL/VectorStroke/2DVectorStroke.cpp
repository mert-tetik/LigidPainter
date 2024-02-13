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
#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "MouseSystem/Mouse.hpp"
#include "SettingsSystem/Settings.hpp"
#include "LibrarySystem/Library.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

bool __anyVectoralPointMoving = false;
int __anyVectoralPointMovingType = 0;
int __anyVectoralPointMovingIndex = 0;

void VectorStroke::draw(Timer& timer, float edge, bool sceneState, std::vector<VectorStroke>& strokes, int curI){
    glm::vec2 cursPos = *Mouse::cursorPos() / *Settings::videoScale() * 100.f;
    const glm::vec2 dotScale = glm::vec2(10.);
    this->endPointHover = Mouse::isMouseHover(dotScale, this->endPos / 100.f * *Settings::videoScale());
    this->startPointHover = Mouse::isMouseHover(dotScale, this->startPos / 100.f * *Settings::videoScale());
    this->offsetPointHover = Mouse::isMouseHover(dotScale, this->offsetPos / 100.f * *Settings::videoScale());

    if(*Mouse::LClick()){
        if(endPointHover){
            if(this->startPos != this->endPos){
                this->endPointPressed = true;
            }
            this->endPointClicked = !this->endPointClicked;
        }
        else if(!sceneState && !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT))
            this->endPointClicked = false;
        if(startPointHover){
            if(this->startPos != this->endPos){
                this->startPointPressed = true;
            }
            this->startPointClicked = !this->startPointClicked;
        }
        else if(!sceneState && !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT))
            this->startPointClicked = false;
        if(offsetPointHover){
            if(this->startPos != this->endPos){
                this->offsetPointPressed = true;
            }
            this->offsetPointClicked = !this->offsetPointClicked;
        }
        else if(!sceneState && !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT))
            this->offsetPointClicked = false;
    }

    if(!*Mouse::LPressed()){
        this->endPointPressed = false;
        this->startPointPressed = false;
        this->offsetPointPressed = false;
    }

    if(Mouse::mouseOffset()->x || Mouse::mouseOffset()->y){
        if(this->endPointPressed && !sceneState)
            this->endPointClicked = false;
        if(this->startPointPressed && !sceneState)
            this->startPointClicked = false;
        if(this->offsetPointPressed && !sceneState)
            this->offsetPointClicked = false;
    }

    if(this->endPointPressed){
        if(!__anyVectoralPointMoving || (__anyVectoralPointMovingIndex == curI + 1 && __anyVectoralPointMovingType == 1)){
            this->endPos += *Mouse::mouseOffset()  / *Settings::videoScale() * 100.f; 
            __anyVectoralPointMovingType = 0;
            __anyVectoralPointMovingIndex = curI;
        }
    
        __anyVectoralPointMoving = true;
    }

    if(this->startPointPressed){
        if(!__anyVectoralPointMoving || (__anyVectoralPointMovingIndex == curI - 1 && __anyVectoralPointMovingType == 0)){
            this->startPos += *Mouse::mouseOffset()  / *Settings::videoScale() * 100.f; 
            __anyVectoralPointMovingType = 1;
            __anyVectoralPointMovingIndex = curI;
        }
        
        __anyVectoralPointMoving = true;
    }

    if(this->offsetPointPressed){
        if(!__anyVectoralPointMoving){
            this->offsetPos += *Mouse::mouseOffset()  / *Settings::videoScale() * 100.f; 
            __anyVectoralPointMovingType = 2;
            __anyVectoralPointMovingIndex = curI;
        }
        
        __anyVectoralPointMoving = true;
    }
        

    //ABS
    ShaderSystem::vectoralCurve().use();
    
    ShaderSystem::vectoralCurve().setVec3("pos", glm::vec3(Settings::videoScale()->x/2.f, Settings::videoScale()->y/2.f, 0.9f));
    ShaderSystem::vectoralCurve().setVec2("scale", glm::vec2(Settings::videoScale()->x/2.f, Settings::videoScale()->y/2.f));
    
    ShaderSystem::vectoralCurve().setVec2("direction", this->offsetPos);
    
    ShaderSystem::vectoralCurve().setVec2("startPos", this->startPos);
    ShaderSystem::vectoralCurve().setVec2("destPos", this->endPos);
    ShaderSystem::vectoralCurve().setVec2("percScale", *Settings::videoScale());
    ShaderSystem::vectoralCurve().setInt("lineCapturingMode", 0);
    
    ShaderSystem::vectoralCurve().setFloat("EDGE", edge);

    glm::vec3 endPointColor = glm::vec3(1.f, 1.f, 1.f);

    if(endPointHover || endPointPressed)
        endPointColor = ColorPalette::themeColor;
    else if(endPointClicked)
        endPointColor = glm::vec3(0.f,0.f,1.f);

    LigidGL::makeDrawCall(GL_TRIANGLES, 0 , 6, "VectorStroke::draw : Rendering the line");
    
    ShaderSystem::buttonShader().use();

    Button endPointBtn = Button(0, glm::vec2(1.f), "", Texture(), 0.f, true);
    endPointBtn.hoverMixVal = endPointHoverMixVal;
    endPointBtn.clickedMixVal = endPointClickedMixVal;
    endPointBtn.hover = endPointHover;
    endPointBtn.clickState1 = endPointClicked;
    endPointBtn.radius = 0.3f;
    endPointBtn.scale = glm::vec2(1.f, Settings::videoScale()->x / Settings::videoScale()->y) / 2.f;
    endPointBtn.pos = glm::vec3(this->endPos, 0.9f);

    Button startPointBtn = endPointBtn;
    startPointBtn.pos = glm::vec3(this->startPos, 0.9f);
    startPointBtn.clickState1 = startPointClicked;
    startPointBtn.hover = startPointHover;
    startPointBtn.clickedMixVal = startPointClickedMixVal;
    startPointBtn.hoverMixVal = startPointHoverMixVal;
    
    if(edge == 0.0001f){
        endPointBtn.scale /= 2.;
        endPointBtn.radius = 0.2;
    }
    endPointBtn.render(timer, true);
    if(edge != 0.0001f)
        startPointBtn.render(timer, true);

    if(startPointHover || startPointPressed || endPointHover || endPointPressed || offsetPointHover || offsetPointPressed){
        Mouse::setCursor(*Mouse::pointerCursor());
    }

    endPointHoverMixVal = endPointBtn.hoverMixVal;
    endPointClickedMixVal = endPointBtn.clickedMixVal;
    startPointHoverMixVal = startPointBtn.hoverMixVal;
    startPointClickedMixVal = startPointBtn.clickedMixVal;

    if(curI == 0)
        __anyVectoralPointMoving = false;
}