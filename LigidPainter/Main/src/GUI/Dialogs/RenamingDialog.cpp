
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

RenamingDialog::RenamingDialog(int){

}

void RenamingDialog::show(Timer& timer, glm::vec2 pos, float width, std::string* title, int max_char_count){
    
    if(title == nullptr)
        return;

    dialogControl.activate();

    this->rename_textbox.pos = glm::vec3(pos, 0.9f);
    this->rename_textbox.scale.x = width * 1.2f;
    this->rename_textbox.text = *title;
    this->rename_textbox.activeChar = title->size() - 1;
    this->rename_textbox.activeChar2 = this->rename_textbox.activeChar;
    this->rename_textbox.color = glm::vec4(0.f);
    this->rename_textbox.color2 = glm::vec4(0.f);
    
    while (!getContext()->window.shouldClose())
    {
        dialogControl.updateStart();

        Button bgBtn = Button(ELEMENT_STYLE_BASIC, this->rename_textbox.scale, "", Texture(), 0.f, false);
        bgBtn.scale.x /= 1.2f;
        bgBtn.pos = this->rename_textbox.pos; 
        bgBtn.color.a = 1.f;
        bgBtn.outlineExtra = false;

        bgBtn.render(timer, false);
        this->rename_textbox.active = true;
        this->rename_textbox.render(timer, true);
        if(this->rename_textbox.text.size() > max_char_count)
            this->rename_textbox.text.pop_back();

        if(
                getContext()->window.isKeyClicked(LIGIDGL_KEY_ESCAPE)|| 
                getContext()->window.isKeyClicked(LIGIDGL_KEY_ENTER)||
                *Mouse::LClick()
            )
        {
            if(!getContext()->window.isKeyClicked(LIGIDGL_KEY_ESCAPE) && this->rename_textbox.text.size()){
                *title = this->rename_textbox.text;
            }

            dialogControl.unActivate();
        }
        
        dialogControl.updateEnd(timer,0.15f);
        if(dialogControl.mixVal == 0.f)
            break;
    }
}