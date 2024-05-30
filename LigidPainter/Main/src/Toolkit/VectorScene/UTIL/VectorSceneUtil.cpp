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

void VectorScene::update3DVectorBuffers(){
    for (size_t i = 0; i < this->strokes_3D.size(); i++)
    {
        this->strokes_3D[i].updateLinePoints();
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

void VectorScene::clear_points(bool threeD){
    if(!threeD){
        dialog_log.registerVectorAction("Vector strokes cleared", this);
        this->strokes_2D.clear();
    }
    else{
        dialog_log.registerVectorAction("Wrapped Vector strokes cleared", this);
        this->strokes_3D.clear();
    }
}