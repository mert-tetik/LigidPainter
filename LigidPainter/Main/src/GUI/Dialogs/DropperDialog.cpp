
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 (c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
 Official Web Page : https:ligidtools.com/ligidpainter

 ---------------------------------------------------------------------------
 */

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "3D/ThreeD.hpp" 
#include "GUI/GUI.hpp" 
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

DropperDialog::DropperDialog(int){

}

void DropperDialog::show(Timer& timer, Color* color){
    
    dialogControl.activate();

    while (!getContext()->window.shouldClose())
    {
        dialogControl.updateStart();

        // Dropper active pick color
        glm::vec4 pxColor;
        // Read the cursor position from the default frame buffer
        glReadPixels(Mouse::cursorPos()->x, getContext()->windowScale.y - Mouse::cursorPos()->y, 1, 1, GL_RGBA, GL_FLOAT, &pxColor);
        // Make sure that hovered pixel color is opaque
        pxColor.a = 1.f;

        Button colorDisplayer = Button(ELEMENT_STYLE_STYLIZED, glm::vec2(5.f / (Settings::videoScale()->x / Settings::videoScale()->y), 5.f), "", Texture(), 0.f, false);
        colorDisplayer.color = pxColor;
        colorDisplayer.pos = glm::vec3(
                                            Mouse::cursorPos()->x / Settings::videoScale()->x * 100.f,
                                            Mouse::cursorPos()->y / Settings::videoScale()->y * 100.f,
                                            0.7f
                                        );
        colorDisplayer.render(timer, false);

        if(
            getContext()->window.isKeyClicked(LIGIDGL_KEY_ENTER)||
            *Mouse::LClick()
        )
        {
            // Assign the selected color
            color->loadRGB_normalized(pxColor);
        }

        if(
                getContext()->window.isKeyClicked(LIGIDGL_KEY_ESCAPE)|| 
                getContext()->window.isKeyClicked(LIGIDGL_KEY_ENTER)||
                *Mouse::LClick()
            )
        {
            dialogControl.unActivate();
        }
        
        dialogControl.updateEnd(timer,0.15f);
        if(dialogControl.mixVal == 0.f)
            break;
    }
    
}