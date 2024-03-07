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
        if(!this->isAny2DPointsActive())
            return;

        registerVectorAction("Selected point deleted", this->vectorStrokes);
        while (this->isAny2DPointsActive()){
            for (size_t i = 0; i < this->vectorStrokes.size(); i++)
            {
                if(i == 0){
                    if(this->vectorStrokes[i].startPoint.active && this->vectorStrokes[i].endPoint.active && this->vectorStrokes.size() == 1){
                        this->vectorStrokes.clear(); 
                        break;
                    }
                    
                    if(this->vectorStrokes[i].startPoint.active && this->vectorStrokes.size() == 1){
                        this->vectorStrokes[i].startPoint.pos = this->vectorStrokes[i].endPoint.pos; 
                        this->vectorStrokes[i].offsetPoint.pos = this->vectorStrokes[i].endPoint.pos; 
                        this->vectorStrokes[i].startPoint.active = false; 
                        break;
                    }
                    
                    if(this->vectorStrokes[i].endPoint.active && this->vectorStrokes.size() == 1){
                        this->vectorStrokes[i].endPoint.pos = this->vectorStrokes[i].startPoint.pos; 
                        this->vectorStrokes[i].offsetPoint.pos = this->vectorStrokes[i].startPoint.pos; 
                        this->vectorStrokes[i].endPoint.active = false; 
                        break;
                    }

                    if(this->vectorStrokes[i].startPoint.active){
                        this->vectorStrokes.erase(this->vectorStrokes.begin() + i);
                        break;
                    }
                    
                    if(this->vectorStrokes[i].endPoint.active && this->vectorStrokes.size() > 1){
                        this->vectorStrokes[i].endPoint.pos = this->vectorStrokes[i + 1].endPoint.pos; 
                        this->vectorStrokes[i].endPoint.active = this->vectorStrokes[i + 1].endPoint.active; 
                        this->vectorStrokes.erase(this->vectorStrokes.begin() + i + 1);
                        break;
                    }
                }
                else{
                    if(this->vectorStrokes[i].endPoint.active){
                        this->vectorStrokes.erase(this->vectorStrokes.begin() + i);
                        if(i - 1 < this->vectorStrokes.size() && i < this->vectorStrokes.size()){
                            this->vectorStrokes[i].startPoint.pos = this->vectorStrokes[i - 1].endPoint.pos; 
                            this->vectorStrokes[i].startPoint.active = false; 
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
        
        registerVectorAction("Selected wrapped point deleted", this->vectorStrokes3D);
        while(this->isAnyWrappedPointsActive()){
            for (size_t i = 0; i < this->vectorStrokes3D.size(); i++)
            {
                if(i == 0){

                    if(this->vectorStrokes3D[i].startPoint.active && this->vectorStrokes3D[i].endPoint.active && this->vectorStrokes3D.size() == 1){
                        this->vectorStrokes3D.clear(); 
                        break;
                    }
                    
                    if(this->vectorStrokes3D[i].startPoint.active && this->vectorStrokes3D.size() == 1){
                        this->vectorStrokes3D[i].startPoint = this->vectorStrokes3D[i].endPoint; 
                        break;
                    }
                    
                    if(this->vectorStrokes3D[i].endPoint.active && this->vectorStrokes3D.size() == 1){
                        this->vectorStrokes3D[i].endPoint = this->vectorStrokes3D[i].startPoint; 
                        break;
                    }

                    if(this->vectorStrokes3D[i].startPoint.active){
                        this->vectorStrokes3D.erase(this->vectorStrokes3D.begin() + i);
                        break;
                    }
                    
                    if(this->vectorStrokes3D[i].endPoint.active && this->vectorStrokes3D.size() > 1){
                        this->vectorStrokes3D[i].endPoint = this->vectorStrokes3D[i + 1].endPoint; 
                        this->vectorStrokes3D.erase(this->vectorStrokes3D.begin() + i + 1);
                        break;
                    }
                }
                else{
                    if(this->vectorStrokes3D[i].endPoint.active){
                        this->vectorStrokes3D.erase(this->vectorStrokes3D.begin() + i);
                        if(i - 1 < this->vectorStrokes3D.size() && i < this->vectorStrokes3D.size()){
                            this->vectorStrokes3D[i].startPoint = this->vectorStrokes3D[i - 1].endPoint; 
                            this->vectorStrokes3D[i].startPoint.active = false; 
                        }
                        break;
                    }
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
            if(this->vectorStrokes[i].endPoint.active && this->vectorStrokes[i].startPoint.active){
                glm::vec2 strokeCenter = (this->vectorStrokes[i].startPoint.pos + this->vectorStrokes[i].endPoint.pos + this->vectorStrokes[i].offsetPoint.pos) / 3.0f;
                glm::vec2 offsetPointDistance = strokeCenter - this->vectorStrokes[i].offsetPoint.pos;

                VectorStroke newStroke;
                newStroke.startPoint.pos = strokeCenter;
                newStroke.endPoint.pos = this->vectorStrokes[i].endPoint.pos;
                newStroke.offsetPoint.pos = newStroke.startPoint.pos - (newStroke.startPoint.pos - newStroke.endPoint.pos) / 2.f - offsetPointDistance;

                this->vectorStrokes[i].endPoint.pos  = strokeCenter;

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
        this->vectorStrokes[i].draw(timer, 0.0005f, doMouseTracking, this->vectorStrokes, i);
    
        if(this->vectorStrokes[i].startPoint.canMove)
            anyPointMovingCondition = true;
        if(this->vectorStrokes[i].startPoint.canMove)
            anyPointMovingCondition = true;
        if(this->vectorStrokes[i].startPoint.canMove)
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
        if(this->vectorStrokes[i].endPoint.hover || this->vectorStrokes[i].startPoint.hover || this->vectorStrokes[i].offsetPoint.hover)
            anyVectorPointHover = true;
    }

    if(*Mouse::LClick() && doMouseTracking && !anyVectorPointHover){
        this->addNew2DVector();
    }
}

void Painter::render3DVectors(Timer& timer, bool doMouseTracking, std::vector<VectorStroke3D>& strokes){
    
    // Set the cursor to ink pen if the current cursor is the default one 
    if(doMouseTracking && Mouse::activeCursor()->cursorType == Mouse::defaultCursor()->cursorType)
        Mouse::setCursor(*Mouse::inkPenCursor());

    ShaderSystem::alphaZero3D().use();
    ShaderSystem::alphaZero3D().setMat4("view", getScene()->viewMatrix);
    ShaderSystem::alphaZero3D().setMat4("projection", getScene()->projectionMatrix);
    ShaderSystem::alphaZero3D().setMat4("modelMatrix", getScene()->transformMatrix);

    ShaderSystem::alphaZero3D().setInt("usingMeshSelection", this->faceSelection.activated);
    ShaderSystem::alphaZero3D().setInt("hideUnselected", this->faceSelection.hideUnselected);
    ShaderSystem::alphaZero3D().setInt("selectedPrimitiveIDS", 0);
    ShaderSystem::alphaZero3D().setInt("meshMask", 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->faceSelection.selectedFaces.ID);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->faceSelection.meshMask.ID);
    
    if(this->selectedMeshIndex < getModel()->meshes.size()){
        ShaderSystem::alphaZero3D().setInt("primitiveCount", getModel()->meshes[this->selectedMeshIndex].indices.size() / 3);
        getModel()->meshes[this->selectedMeshIndex].Draw(false);
    }

    // Render all the vectors
    int clickedPointI = -1; 
    bool anyPointMovingCondition = false;
    for (size_t i = 0; i < strokes.size(); i++)
    {
        if(strokes[i].draw(timer, 0.0005f, doMouseTracking, strokes, i, *this))
            clickedPointI = i;
    
        if(strokes[i].startPoint.areMovingConditionsSet(true))
            anyPointMovingCondition = true;
        
        if(strokes[i].endPoint.areMovingConditionsSet(true))
            anyPointMovingCondition = true;
    }

    if(anyPointMovingCondition && !lastVecMovingConditionState)
        registerVectorAction("Wrapped point moved", strokes);

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
        vecStroke.startPoint.pos = *Mouse::cursorPos() / *Settings::videoScale() * 100.f; 
        vecStroke.endPoint.pos = vecStroke.startPoint.pos;
        vecStroke.offsetPoint.pos = vecStroke.startPoint.pos;
        this->vectorStrokes.push_back(vecStroke);
    }
    else{
        if(this->vectorStrokes[this->vectorStrokes.size() - 1].endPoint.pos == this->vectorStrokes[this->vectorStrokes.size() - 1].startPoint.pos){
            registerVectorAction("New point", this->vectorStrokes);
            this->vectorStrokes[this->vectorStrokes.size() - 1].endPoint.pos = *Mouse::cursorPos() / *Settings::videoScale() * 100.f;
            this->vectorStrokes[this->vectorStrokes.size() - 1].offsetPoint.pos = this->vectorStrokes[this->vectorStrokes.size() - 1].startPoint.pos - (this->vectorStrokes[this->vectorStrokes.size() - 1].startPoint.pos - this->vectorStrokes[this->vectorStrokes.size() - 1].endPoint.pos) / 2.f;
            this->vectorStrokes[this->vectorStrokes.size() - 1].offsetPoint.pos += 0.001f; // Vectors can't be rendered if the offset point alligns perfectly :(
        }
        else{
            registerVectorAction("New point", this->vectorStrokes);
            vecStroke.startPoint.pos = this->vectorStrokes[this->vectorStrokes.size() - 1].endPoint.pos; 
            vecStroke.endPoint.pos = *Mouse::cursorPos() / *Settings::videoScale() * 100.f;
            vecStroke.offsetPoint.pos = vecStroke.startPoint.pos - (vecStroke.startPoint.pos - vecStroke.endPoint.pos) /2.f;
            vecStroke.offsetPoint.pos += 0.001f; // Vectors can't be rendered if the offset point alligns perfectly :(
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

void Painter::applyVectorStrokes(
                                    std::vector<VectorStroke> vectorStrokes, 
                                    Panel& twoDPaintingPanel, 
                                    glm::mat4 windowOrtho, 
                                    int paintingMode, 
                                    Filter filterBtnFilter, 
                                    Box twoDPaintingBox, 
                                    Material& paintingCustomMat,
                                    std::vector<TextureField> textureFields,
                                    bool twoDWrap
                                )
{
    if(this->wrapMode && this->selectedPaintingModeIndex == 5){
        for (size_t strokeI = 0; strokeI < this->vectorStrokes3D.size(); strokeI++)
        {
            for (size_t vertI = 0; vertI < this->vectorStrokes3D[strokeI].lineVertices.size(); vertI++){
                if(vertI % (int)std::ceil((this->brushProperties.spacing + 1.f) / 20.f) == 0){
                    this->doPaint(
                                    ThreeDPoint(this->vectorStrokes3D[strokeI].lineVertices[vertI].Position, this->vectorStrokes3D[strokeI].lineVertices[vertI].Normal), 
                                    vertI == 0, 
                                    paintingMode, 
                                    true, 
                                    twoDPaintingBox, 
                                    textureFields
                                );
                }
            }
            
            this->updateTexture(twoDPaintingPanel, windowOrtho, paintingMode, filterBtnFilter, twoDPaintingBox, paintingCustomMat);

            this->refreshPainting();
        }


        ShaderSystem::buttonShader().use();
        Settings::defaultFramebuffer()->FBO.bind();
        Settings::defaultFramebuffer()->setViewport();
    }
    else{

        std::vector<glm::vec2> strokePositions;

        for (size_t vecI = 0; vecI < vectorStrokes.size(); vecI++)
        {
            float distance = glm::distance(vectorStrokes[vecI].startPoint.pos, vectorStrokes[vecI].offsetPoint.pos) + glm::distance(vectorStrokes[vecI].endPoint.pos, vectorStrokes[vecI].offsetPoint.pos);
            unsigned int quality = (unsigned int)(distance / 2.f);

            std::vector<glm::vec2> points = calculateBezierCurve(
                                                                    vectorStrokes[vecI].startPoint.pos / 100.f * *Settings::videoScale(), 
                                                                    vectorStrokes[vecI].endPoint.pos / 100.f * *Settings::videoScale(), 
                                                                    vectorStrokes[vecI].offsetPoint.pos / 100.f * *Settings::videoScale(), 
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
                this->doPaint(subVector[i], twoDWrap, i == 0 || (i == subVector.size() - 1 && this->brushProperties.spacing > 5.f), paintingMode, true, twoDPaintingBox, textureFields);
            }
        }

        this->updateTexture(twoDPaintingPanel, windowOrtho, paintingMode, filterBtnFilter, twoDPaintingBox, paintingCustomMat);

        this->refreshPainting();

        ShaderSystem::buttonShader().use();
        Settings::defaultFramebuffer()->FBO.bind();
        Settings::defaultFramebuffer()->setViewport();
    }
}

bool Painter::isAny2DPointsActive(){
    for (size_t i = 0; i < this->vectorStrokes.size(); i++)
    {
        if(this->vectorStrokes[i].startPoint.active)
            return true;
        if(this->vectorStrokes[i].endPoint.active)
            return true;
    }
    
    return false;
}

bool Painter::isAnyWrappedPointsActive(){
    for (size_t i = 0; i < this->vectorStrokes3D.size(); i++)
    {
        if(this->vectorStrokes3D[i].startPoint.active)
            return true;
        if(this->vectorStrokes3D[i].endPoint.active)
            return true;
    }
    
    return false;
}