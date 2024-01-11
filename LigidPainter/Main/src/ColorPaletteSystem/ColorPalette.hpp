/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

All rights reserved.

Official GitHub Link : https:github.com/mert-tetik/LigidPainter
Official Web Page : https:ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef COLORPALETTE_HPP
#define COLORPALETTE_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ColorPalette
{
    extern glm::vec4 themeColor;
    extern glm::vec4 mainColor;
    extern glm::vec4 secondColor;
    extern glm::vec4 thirdColor;
    extern glm::vec4 oppositeColor;

    void newPalette(glm::vec4 themeColor,glm::vec4 mainColor,glm::vec4 secondColor,glm::vec4 thirdColor,glm::vec4 oppositeColor);
};

#endif //COLORPALETTE_HPP