/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef THREED_LINE_HPP
#define THREED_LINE_HPP

#include <glm/glm.hpp>

struct Line3D{
    glm::vec3 startPoint = glm::vec3(0.f);
    glm::vec3 endPoint = glm::vec3(0.f);
    
    Line3D(){}
    Line3D(glm::vec3 startPoint, glm::vec3 endPoint){
        this->startPoint = startPoint; 
        this->endPoint = endPoint; 
    }
};

/*! @brief Renders a 3D line from start point to end point *efficiently* */
void render_line(Line3D line, std::vector<glm::vec3> pos_offsets, Shader shader);

#endif