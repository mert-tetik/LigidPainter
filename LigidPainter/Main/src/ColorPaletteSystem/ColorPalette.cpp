/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

Color Pallete of the LigidPainter.

These color values are determining the colors of the graphical user interface & has nothing to do with any other color palette implementations


*/
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ColorPaletteSystem/ColorPalette.hpp"

namespace ColorPalette
{
    glm::vec4 themeColor = glm::vec4(       0.043f      ,0.635f     ,0.823f     ,1.f / 1.2f    );  //LigidPainter's theme color which is cyan-like light blue 
    glm::vec4 mainColor = glm::vec4(        0.26        ,0.26       ,0.26       ,0.9f / 1.2f   );  //That color will be used oftenly
    glm::vec4 secondColor = glm::vec4(      0.16        ,0.16       ,0.16       ,1.f / 1.2f    );  //That too
    glm::vec4 thirdColor = glm::vec4(       0.46        ,0.46       ,0.46       ,1.f / 1.2f    );  //Will be used few times
    glm::vec4 oppositeColor = glm::vec4(    0.8         ,0.8        ,0.8        ,1.f / 1.2f    );  //Will be used for text / icon color etc.

    void newPalette(
                        glm::vec4 themeColor,
                        glm::vec4 mainColor,
                        glm::vec4 secondColor,
                        glm::vec4 thirdColor,
                        glm::vec4 oppositeColor
                    ){
        
        ColorPalette::themeColor = themeColor;
        ColorPalette::mainColor = mainColor;
        ColorPalette::secondColor = secondColor;
        ColorPalette::thirdColor = thirdColor;
        ColorPalette::oppositeColor = oppositeColor;
    }
} 

