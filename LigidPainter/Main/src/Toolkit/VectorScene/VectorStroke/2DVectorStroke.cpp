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
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

void VectorPoint2D::render(Timer& timer, bool doMouseTracking, float scaleDivider, glm::vec4 color){
    
    const float z_axis_val = 0.9f;
    const glm::vec2 point_track_scale = glm::vec2(10.);
    
    // Check if the point was hovered by the cursor
    this->hover = Mouse::isMouseHover(point_track_scale, this->pos / 100.f * *Settings::videoScale());
    if(!doMouseTracking)
        this->hover = false;

    // Activate the point if CTRL + A 
    if(shortcuts_CTRL_A())
        this->active = true;

    // If mouse left button clicked
    if((*Mouse::LClick() || *Mouse::RClick()) && doMouseTracking){
        // If clicked to the point
        if(this->hover){
            this->active = !this->active;
            this->canMove = true;
        }
        // If not clicked to the point
        else if(!getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT))
            this->active = false;
    }

    // Prevent moving if mouse left button is not pressed
    if(!*Mouse::LPressed() || !this->active){
        this->canMove = false;
    }

    // Move the point if pressed to 'G'
    if(getContext()->window.isKeyPressed(LIGIDGL_KEY_G) && this->active){
        this->canMove = true;
    }
    
    // Move the point if it can move
    if(this->canMove){
        this->pos += *Mouse::mouseOffset()  / *Settings::videoScale() * 100.f; 
    }
    
    // Create the button
    Button btn = Button(0, glm::vec2(1.f), "", Texture(), 0.f, true);
    btn.hoverMixVal = this->hoverMixVal;
    btn.clickedMixVal = this->activeMixVal;
    btn.hover = this->hover;
    btn.clickState1 = this->active;
    btn.scale = glm::vec2(1.f, Settings::videoScale()->x / Settings::videoScale()->y) / 2.f;
    btn.scale /= scaleDivider;
    btn.radius = btn.scale.x / 2.f;
    btn.pos = glm::vec3(this->pos, z_axis_val);
    btn.color = color;

    // Render the button
    btn.render(timer, doMouseTracking);

    this->hoverMixVal = btn.hoverMixVal;
    this->activeMixVal = btn.clickedMixVal;
}

void VectorStroke::renderCurve(float edge, glm::vec2 start, glm::vec2 dest, glm::vec2 offset){
    ShaderSystem::vectoralCurve().use();
    ShaderSystem::vectoralCurve().setMat4("projection", getContext()->ortho_projection); 
    
    ShaderSystem::vectoralCurve().setVec3("pos", glm::vec3(Settings::videoScale()->x/2.f, Settings::videoScale()->y/2.f, 0.9f));
    ShaderSystem::vectoralCurve().setVec2("scale", glm::vec2(Settings::videoScale()->x/2.f, Settings::videoScale()->y/2.f));
    
    ShaderSystem::vectoralCurve().setVec2("startPos", start);
    ShaderSystem::vectoralCurve().setVec2("destPos", dest);
    ShaderSystem::vectoralCurve().setVec2("direction", offset);
    
    ShaderSystem::vectoralCurve().setVec2("percScale", *Settings::videoScale());
    ShaderSystem::vectoralCurve().setInt("lineCapturingMode", 0);
    ShaderSystem::vectoralCurve().setFloat("EDGE", edge);
    
    getBox()->draw("VectorStroke::draw : Rendering the curve");

    ShaderUTIL::release_bound_shader();

    glClear(GL_DEPTH_BUFFER_BIT);

}

static bool __anyVectoralPointMoving = false;
static int __anyVectoralPointMovingType = 0;
static int __anyVectoralPointMovingIndex = 0;

void VectorStroke::draw(Timer& timer, float edge, bool doMouseTracking, std::vector<VectorStroke>& strokes, int curI){
    
    // Rendering the main curve
    this->renderCurve(edge, this->startPoint.pos, this->endPoint.pos, this->offsetPoint.pos);
    // Rendering the offset curves
    this->renderCurve(edge / 5.f, this->startPoint.pos, this->endPoint.pos, this->offsetPoint.pos);
    this->renderCurve(edge / 5.f, this->startPoint.pos, this->offsetPoint.pos, this->offsetPoint.pos);
    this->renderCurve(edge / 5.f, this->endPoint.pos, this->offsetPoint.pos, this->offsetPoint.pos);

    // Rendering the points
    glm::vec4 endClr = glm::vec4(1.f);
    glm::vec4 startClr = glm::vec4(1.f);

    if(curI == strokes.size() - 1){
        endClr = glm::vec4(148.f / 255.f, 194.f / 255.f, 132.f / 255.f, 1.f);
    }
    if(curI == 0){
        startClr = glm::vec4(194.f / 255.f, 132.f / 255.f, 177.f / 255.f, 1.f);
    }

    this->startPoint.render(timer, doMouseTracking, 1.f, startClr);
    if(curI == strokes.size() - 1){
        this->endPoint.render(timer, doMouseTracking, 1.f, endClr);
    }
    else{
        this->endPoint = strokes[curI + 1].startPoint;
    }
    this->offsetPoint.render(timer, doMouseTracking, 2.f, glm::vec4(1.f));
}