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

#include "UTIL/Painting/Painter.hpp"

#include "GUI/Dialogs.hpp"

#include "Toolkit/VectorScene/VectorScene.hpp"

#include <string>
#include <vector>

void VectorScene::subdivide_selected_points(bool threeD){
    if(!threeD){
        dialog_log.registerVectorAction("New point between the selected points", this);

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
        dialog_log.registerVectorAction("New point between the selected wrapped points", this);

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