/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    This Cpp file contains the functions related to whole structure of vectors
    See the ../VectorStroke folder for single vector specific definitions

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

void Painter::deleteSelectedVectorPoints(){
    if(!this->wrapMode){
        registerVectorAction("Selected point deleted", this->vectorStrokes);
        for (size_t i = 0; i < this->vectorStrokes.size(); i++)
        {
            if(i == 0){
                if(this->vectorStrokes[i].startPointClicked){
                    this->vectorStrokes.erase(this->vectorStrokes.begin() + i);
                    break;
                }
                else if(this->vectorStrokes[i].endPointClicked && this->vectorStrokes.size() > 1){
                    this->vectorStrokes[i].endPos = this->vectorStrokes[i + 1].endPos; 
                    this->vectorStrokes.erase(this->vectorStrokes.begin() + i + 1);
                    break;
                }
            }
            else{
                if(this->vectorStrokes[i].endPointClicked){
                    this->vectorStrokes.erase(this->vectorStrokes.begin() + i);
                    if(i - 1 < this->vectorStrokes.size() && i < this->vectorStrokes.size())
                        this->vectorStrokes[i].startPos = this->vectorStrokes[i - 1].endPos; 
                    break;
                }
            }
        }
    }
    else{
        registerVectorAction("Selected wrapped point deleted", this->vectorStrokes3D);
        for (size_t i = 0; i < this->vectorStrokes3D.size(); i++)
        {
            if(i == 0){
                if(this->vectorStrokes3D[i].startPoint.active){
                    this->vectorStrokes3D.erase(this->vectorStrokes3D.begin() + i);
                    break;
                }
                else if(this->vectorStrokes3D[i].endPoint.active && this->vectorStrokes3D.size() > 1){
                    this->vectorStrokes3D[i].endPoint = this->vectorStrokes3D[i + 1].endPoint; 
                    this->vectorStrokes3D.erase(this->vectorStrokes3D.begin() + i + 1);
                    break;
                }
            }
            else{
                if(this->vectorStrokes3D[i].endPoint.active){
                    this->vectorStrokes3D.erase(this->vectorStrokes3D.begin() + i);
                    if(i - 1 < this->vectorStrokes3D.size() && i < this->vectorStrokes3D.size())
                        this->vectorStrokes3D[i].startPoint = this->vectorStrokes3D[i - 1].endPoint; 
                    break;
                }
            }
        }

        this->update3DVectorBuffers();
    }
}

void Painter::clearSelectedVectorPoints(){
    if(!this->wrapMode){
        registerVectorAction("Vector strokes cleared", this->vectorStrokes);
        this->vectorStrokes.clear();
    }
    else{
        registerVectorAction("Wrapped Vector strokes cleared", this->vectorStrokes3D);
        this->vectorStrokes3D.clear();
    }
}

void Painter::subdivideSelectedPoints(){
    if(!this->wrapMode){
        registerVectorAction("New point between the selected points", this->vectorStrokes);

        for (size_t i = 0; i < this->vectorStrokes.size(); i++)
        {
            if(this->vectorStrokes[i].endPointClicked && this->vectorStrokes[i].startPointClicked){
                glm::vec2 strokeCenter = (this->vectorStrokes[i].startPos + this->vectorStrokes[i].endPos + this->vectorStrokes[i].offsetPos) / 3.0f;
                glm::vec2 offsetPointDistance = strokeCenter - this->vectorStrokes[i].offsetPos;

                VectorStroke newStroke;
                newStroke.startPos = strokeCenter;
                newStroke.endPos = this->vectorStrokes[i].endPos;
                newStroke.offsetPos = newStroke.startPos - (newStroke.startPos - newStroke.endPos) / 2.f - offsetPointDistance;

                this->vectorStrokes[i].endPos  = strokeCenter;

                this->vectorStrokes.insert(this->vectorStrokes.begin() + i + 1, newStroke);
                i++;
            }
        }
    }
    else{
        registerVectorAction("New point between the selected wrapped points", this->vectorStrokes3D);

        for (size_t i = 0; i < this->vectorStrokes3D.size(); i++)
        {
            if(this->vectorStrokes3D[i].endPoint.active && this->vectorStrokes3D[i].startPoint.active){
                glm::vec3 strokeCenter = (this->vectorStrokes3D[i].startPoint.pos + this->vectorStrokes3D[i].endPoint.pos) / 2.0f;
                glm::vec3 offsetPointDistance = strokeCenter;

                VectorStroke3D newStroke;
                newStroke.startPoint.pos = strokeCenter;
                newStroke.endPoint = this->vectorStrokes3D[i].endPoint;

                this->vectorStrokes3D[i].endPoint.pos  = strokeCenter;

                this->vectorStrokes3D.insert(this->vectorStrokes3D.begin() + i + 1, newStroke);
                i++;
            }
        }

        this->update3DVectorBuffers();
    }
}

static bool lastVecMovingConditionState = false; 

void Painter::render2DVectors(Timer& timer, bool doMouseTracking){
    
    // Set the cursor to ink pen if the current cursor is the default one 
    if(doMouseTracking && Mouse::activeCursor()->cursorType == Mouse::defaultCursor()->cursorType && doMouseTracking)
        Mouse::setCursor(*Mouse::inkPenCursor());

    // Render the vectors
    bool anyPointMovingCondition = false;
    for (int i = this->vectorStrokes.size() - 1; i >= 0; i--)
    {
        this->vectorStrokes[i].draw(timer, 0.0005f, !doMouseTracking, this->vectorStrokes, i);

        VectorStroke offsetStrokeEnd = VectorStroke(this->vectorStrokes[i].endPos, this->vectorStrokes[i].offsetPos, this->vectorStrokes[i].offsetPos); 
        VectorStroke offsetStrokeStart = VectorStroke(this->vectorStrokes[i].startPos, this->vectorStrokes[i].offsetPos, this->vectorStrokes[i].offsetPos); 
    
        offsetStrokeEnd.draw(timer, 0.0001f, !doMouseTracking, this->vectorStrokes, i);
        offsetStrokeStart.draw(timer, 0.0001f, !doMouseTracking, this->vectorStrokes, i);
    
        if(this->vectorStrokes[i].startPointPressed)
            anyPointMovingCondition = true;
        if(this->vectorStrokes[i].endPointPressed)
            anyPointMovingCondition = true;
        if(this->vectorStrokes[i].offsetPointPressed)
            anyPointMovingCondition = true;
    }

    if(anyPointMovingCondition && !lastVecMovingConditionState)
        registerVectorAction("Point moved", this->vectorStrokes);

    lastVecMovingConditionState = anyPointMovingCondition;

    // Clear the depth buffer after rendering the vectors
    glClear(GL_DEPTH_BUFFER_BIT);
    
    // Check if any points hovered
    bool anyVectorPointHover = false;
    for (size_t i = 0; i < this->vectorStrokes.size(); i++){
        if(this->vectorStrokes[i].endPointHover || this->vectorStrokes[i].startPointHover || this->vectorStrokes[i].offsetPointHover)
            anyVectorPointHover = true;
    }

    if(*Mouse::LClick() && doMouseTracking && !anyVectorPointHover){
        this->addNew2DVector();
    }
}

void Painter::render3DVectors(Timer& timer, bool doMouseTracking){
    
    // Set the cursor to ink pen if the current cursor is the default one 
    if(doMouseTracking && Mouse::activeCursor()->cursorType == Mouse::defaultCursor()->cursorType)
        Mouse::setCursor(*Mouse::inkPenCursor());

    // Render all the vectors
    int clickedPointI = -1; 
    bool anyPointMovingCondition = false;
    for (size_t i = 0; i < this->vectorStrokes3D.size(); i++)
    {
        if(this->vectorStrokes3D[i].draw(timer, 0.0005f, doMouseTracking, this->vectorStrokes3D, i, *this))
            clickedPointI = i;
    
        if(this->vectorStrokes3D[i].startPoint.areMovingConditionsSet(true))
            anyPointMovingCondition = true;
        
        if(this->vectorStrokes3D[i].endPoint.areMovingConditionsSet(true))
            anyPointMovingCondition = true;
    }

    if(anyPointMovingCondition && !lastVecMovingConditionState)
        registerVectorAction("Wrapped point moved", this->vectorStrokes3D);

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

void Painter::addNew2DVector(){
    VectorStroke vecStroke;
    if(!this->vectorStrokes.size()){
        registerVectorAction("First point created", this->vectorStrokes);
        vecStroke.startPos = *Mouse::cursorPos() / *Settings::videoScale() * 100.f; 
        vecStroke.endPos = vecStroke.startPos;
        vecStroke.offsetPos = vecStroke.startPos;
        this->vectorStrokes.push_back(vecStroke);
    }
    else{
        if(this->vectorStrokes[this->vectorStrokes.size() - 1].endPos == this->vectorStrokes[this->vectorStrokes.size() - 1].startPos){
            registerVectorAction("New point", this->vectorStrokes);
            this->vectorStrokes[this->vectorStrokes.size() - 1].endPos = *Mouse::cursorPos() / *Settings::videoScale() * 100.f;
            this->vectorStrokes[this->vectorStrokes.size() - 1].offsetPos = this->vectorStrokes[this->vectorStrokes.size() - 1].startPos - (this->vectorStrokes[this->vectorStrokes.size() - 1].startPos - this->vectorStrokes[this->vectorStrokes.size() - 1].endPos) / 2.f;
        }
        else{
            registerVectorAction("New point", this->vectorStrokes);
            vecStroke.startPos = this->vectorStrokes[this->vectorStrokes.size() - 1].endPos; 
            vecStroke.endPos = *Mouse::cursorPos() / *Settings::videoScale() * 100.f;
            vecStroke.offsetPos = vecStroke.startPos - (vecStroke.startPos - vecStroke.endPos) /2.f;
            this->vectorStrokes.push_back(vecStroke);
        }
    }
}

void Painter::addNew3DVector(){

    // Point on top of the cursor is not valid 
    if(this->getCurrentPosNormalDataOverCursor().pos == glm::vec3(-1000.f))
        return;

    VectorStroke3D vecStroke;
    if(!this->vectorStrokes3D.size()){
        registerVectorAction("First wrapped point created", this->vectorStrokes3D);
        vecStroke.startPoint = this->getCurrentPosNormalDataOverCursor(); 
        vecStroke.endPoint = vecStroke.startPoint;
        this->vectorStrokes3D.push_back(vecStroke);
    }
    else{
        if(this->vectorStrokes3D[this->vectorStrokes3D.size() - 1].endPoint.pos == this->vectorStrokes3D[this->vectorStrokes3D.size() - 1].startPoint.pos){
            registerVectorAction("New wrapped point", this->vectorStrokes3D);
            this->vectorStrokes3D[this->vectorStrokes3D.size() - 1].endPoint = this->getCurrentPosNormalDataOverCursor();
        }
        else{
            registerVectorAction("New wrapped point", this->vectorStrokes3D);
            vecStroke.startPoint = this->vectorStrokes3D[this->vectorStrokes3D.size() - 1].endPoint; 
            vecStroke.endPoint = this->getCurrentPosNormalDataOverCursor();
            this->vectorStrokes3D.push_back(vecStroke);
        }
    }
}

void Painter::update3DVectorBuffers(){
    for (size_t i = 0; i < this->vectorStrokes3D.size(); i++)
    {
        this->vectorStrokes3D[i].updateLinePoints(*this);
    }
}