
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

DisplayTextureDialog::DisplayTextureDialog(int){
    
    //Create the panel
    this->panel = Panel(
                            
                            {
                                Section(
                                    Element(Button()),
                                    {
                                    }
                                )
                            },
                            glm::vec2(35.f),
                            glm::vec3(50.f,50.f,0.8f),
                            ColorPalette::mainColor,
                            ColorPalette::thirdColor,
                            true,
                            true,
                            true,
                            true,
                            true,
                            1.f,
                            1,
                            {},
                            0.25f,
                            false
                        );

    this->threeD_display_btn = Button(ELEMENT_STYLE_SOLID, glm::vec2(1,2.f), "", Texture(nullptr, 1024, 1024), 0.f, false);
    this->twoD_display_btn = Button(ELEMENT_STYLE_SOLID, glm::vec2(1,2.f), "", Texture(), 0.f, false);

    this->displayingCam.cameraPos = glm::vec3(0,0,-3.5f);
    this->displayingCam.radius = 3.5f;

    this->displayingFBO = Framebuffer(Texture(nullptr, 512, 512), GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, glm::ivec2(512)), "Material displayer dialog - displaying");
}

void DisplayTextureDialog::show(Timer& timer, Texture texture){
    
    this->dialogControl.activate();

    while (!getContext()->window.shouldClose())
    {
        this->dialogControl.updateStart();

        if(this->threeD_display_btn.hover){
            this->displayingCam.interaction(*Mouse::mouseScroll(), *Mouse::mouseOffset());
            this->displayingCam.posShortcutInteraction(true);
        }

        //Render the panel
        this->panel.render(timer,true);
        
        threeD_display_btn.scale = glm::vec2(this->panel.scale.x / 2.f, this->panel.scale.y);
        threeD_display_btn.pos = glm::vec3(this->panel.pos.x - this->panel.scale.x + threeD_display_btn.scale.x, this->panel.pos.y, this->panel.pos.z);
        threeD_display_btn.render(timer, true);

        if(threeD_display_btn.hover)
            this->threeD_display_btn.texture.render_mesh(*getScene()->get_selected_mesh(), MaterialChannels(texture, appTextures.white, appTextures.black, appTextures.white, appTextures.white, appTextures.white), this->displayingCam);

        ShaderSystem::buttonShader().use();
        getBox()->bindBuffers();

        twoD_display_btn.scale = glm::vec2(this->panel.scale.x / 2.f, this->panel.scale.y);
        twoD_display_btn.pos = glm::vec3(this->panel.pos.x + this->panel.scale.x - twoD_display_btn.scale.x, this->panel.pos.y, this->panel.pos.z);
        twoD_display_btn.texture = texture;
        twoD_display_btn.render(timer, true);
        
        //Close the dialog
        if(getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || (!this->panel.hover && *Mouse::LClick())){
            if(!this->dialogControl.firstFrameActivated){
                this->dialogControl.unActivate();
            }
        }
        
        dialogControl.updateEnd(timer, 0.15f);

        if(dialogControl.mixVal == 0.f)
            break;
    }
}