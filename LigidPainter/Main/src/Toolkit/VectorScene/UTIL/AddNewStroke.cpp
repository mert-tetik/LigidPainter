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

#include "3D/ThreeD.hpp"

#include "GUI/Dialogs.hpp"

#include "Toolkit/VectorScene/VectorScene.hpp"

#include <string>
#include <vector>


void VectorScene::addNew2DVector(){
    VectorStroke vecStroke;
    if(!this->strokes_2D.size()){
        dialog_log.registerVectorAction("First point created", this);
        vecStroke.startPoint.pos = *Mouse::cursorPos() / *Settings::videoScale() * 100.f; 
        vecStroke.endPoint.pos = vecStroke.startPoint.pos;
        vecStroke.offsetPoint.pos = vecStroke.startPoint.pos;
        this->strokes_2D.push_back(vecStroke);
    }
    else{
        if(this->strokes_2D[this->strokes_2D.size() - 1].endPoint.pos == this->strokes_2D[this->strokes_2D.size() - 1].startPoint.pos){
            dialog_log.registerVectorAction("New point", this);
            this->strokes_2D[this->strokes_2D.size() - 1].endPoint.pos = *Mouse::cursorPos() / *Settings::videoScale() * 100.f;
            this->strokes_2D[this->strokes_2D.size() - 1].offsetPoint.pos = this->strokes_2D[this->strokes_2D.size() - 1].startPoint.pos - (this->strokes_2D[this->strokes_2D.size() - 1].startPoint.pos - this->strokes_2D[this->strokes_2D.size() - 1].endPoint.pos) / 2.f;
            this->strokes_2D[this->strokes_2D.size() - 1].offsetPoint.pos += 0.001f; // Vectors can't be rendered if the offset point alligns perfectly :(
            this->strokes_2D[this->strokes_2D.size() - 1].endPoint.active = true;
            this->strokes_2D[this->strokes_2D.size() - 1].endPoint.canMove = true;
        }
        else{
            dialog_log.registerVectorAction("New point", this);
            vecStroke.startPoint.pos = this->strokes_2D[this->strokes_2D.size() - 1].endPoint.pos; 
            vecStroke.endPoint.pos = *Mouse::cursorPos() / *Settings::videoScale() * 100.f;
            vecStroke.offsetPoint.pos = vecStroke.startPoint.pos - (vecStroke.startPoint.pos - vecStroke.endPoint.pos) /2.f;
            vecStroke.offsetPoint.pos += 0.001f; // Vectors can't be rendered if the offset point alligns perfectly :(
            vecStroke.endPoint.active = true;
            vecStroke.endPoint.canMove = true;
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
        dialog_log.registerVectorAction("First wrapped point created", this);
        vecStroke.startPoint = getScene()->get_selected_mesh()->getCurrentPosNormalDataOverCursor(); 
        vecStroke.endPoint = vecStroke.startPoint;
        this->strokes_3D.push_back(vecStroke);
    }
    else{
        if(this->strokes_3D[this->strokes_3D.size() - 1].endPoint.pos == this->strokes_3D[this->strokes_3D.size() - 1].startPoint.pos){
            dialog_log.registerVectorAction("New wrapped point", this);
            this->strokes_3D[this->strokes_3D.size() - 1].endPoint = getScene()->get_selected_mesh()->getCurrentPosNormalDataOverCursor();
        }
        else{
            dialog_log.registerVectorAction("New wrapped point", this);
            vecStroke.startPoint = this->strokes_3D[this->strokes_3D.size() - 1].endPoint; 
            vecStroke.endPoint = getScene()->get_selected_mesh()->getCurrentPosNormalDataOverCursor();
            this->strokes_3D.push_back(vecStroke);
        }
    }
}