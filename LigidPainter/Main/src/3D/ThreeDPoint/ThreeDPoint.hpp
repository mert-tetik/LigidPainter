/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef THREED_THREED_POINT_HPP
#define THREED_THREED_POINT_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <string>

#include "UTIL/ColorPalette/ColorPalette.hpp"

class Timer;

struct ThreeDPoint{
    glm::vec3 pos = glm::vec3(0.f);
    glm::vec3 normal = glm::vec3(0.f);

    glm::vec4 color = glm::vec4(1.f);
    glm::vec4 colorActive = ColorPalette::themeColor;

    bool clickState1 = false;
    
    bool active = false;
    bool moving = false;

    int detailI = 0;

    ThreeDPoint(){}
    ThreeDPoint(glm::vec3 pos){
        this->pos = pos;
    }
    ThreeDPoint(glm::vec3 pos, glm::vec3 normal){
        this->pos = pos;
        this->normal = normal;
    }

    /*! @return true if clicked to the point*/
    bool render(Timer& timer, bool doMouseTracking, bool stencilTest, float radius, bool canMove);

    /*! @return true if the moving conditions of the point is set*/
    bool areMovingConditionsSet(bool canMove);

    /*! @return true if the point is hovered by mouse*/
    bool is_hovered(float radius);
};

#endif