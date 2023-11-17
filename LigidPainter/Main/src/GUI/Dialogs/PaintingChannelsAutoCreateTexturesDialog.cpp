
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 Copyright (c) 2022-2023, Mert Tetik

 All rights reserved.

 Official GitHub Link : https:github.com/mert-tetik/LigidPainter
 Official Web Page : https:ligidtools.com/ligidpainter

 ---------------------------------------------------------------------------
 */
 
#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"
#include "LibrarySystem/Library.hpp"
#include "MouseSystem/Mouse.hpp" 
#include "ColorPaletteSystem/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>

#include "tinyfiledialogs.h"

PaintingChannelsAutoCreateTexturesDialog::PaintingChannelsAutoCreateTexturesDialog(){
    //Create the panel
    this->panel = Panel(
        
        {
            {
                Section(
                    Element(Button()),
                    {
                        ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(2,2),  {"256", "512", "1024", "2048", "4096"} , "Resolution", 1.f),
                        CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "Opaque", 1.f),
                        Button(ELEMENT_STYLE_STYLIZED,glm::vec2(2,2), "Create"  , Texture(), 4.f, false)
                    }
                )
            }
        },
        glm::vec2(12.f),
        glm::vec3(50.f,50.f,0.8f),
        ColorPalette::mainColor,
        ColorPalette::thirdColor,
        true,
        true,
        true,
        true,
        true,
        1.f,
        1.f,
        {},
        0.25f,
        false
    );

    panel.sections[0].elements[1].checkBox.clickState1 = true;
}

void PaintingChannelsAutoCreateTexturesDialog::render(Timer timer, std::vector<Section>& paintingChannelsSection){
    
    int textureRes = std::stoi(panel.sections[0].elements[0].comboBox.texts[panel.sections[0].elements[0].comboBox.selectedIndex]);

    dialogControl.updateStart();
    if(panel.sections[0].elements[2].button.clicked){
        for (size_t meshI = 0; meshI < getModel()->meshes.size(); meshI++)
        {
            std::string meshTitle = getModel()->meshes[meshI].materialName;

            for (size_t channelI = 0; channelI < 6; channelI++)
            {
                std::string channelTitle = "";

                if(channelI == 0)
                    channelTitle = "Albedo";
                else if(channelI == 1)
                    channelTitle = "Roughness";
                else if(channelI == 2)
                    channelTitle = "Metallic";
                else if(channelI == 3)
                    channelTitle = "Normal_Map";
                else if(channelI == 4)
                    channelTitle = "Height_Map";
                else if(channelI == 5)
                    channelTitle = "Ambient_Occlusion";

                if(meshI+1 < paintingChannelsSection.size()){
                    if(!paintingChannelsSection[meshI+1].elements[channelI].button.texture.ID){
                        char* pxs = nullptr;
                        
                        if(panel.sections[0].elements[1].checkBox.clickState1){
                            pxs = new char[textureRes * textureRes * 4];
                            for (size_t i = 0; i < textureRes * textureRes; i++)
                            {
                                pxs[i*4+3] = 127;
                            }
                        }

                        Texture texture = Texture(pxs, textureRes, textureRes);
                        texture.title = meshTitle + "_" + channelTitle;

                        if(panel.sections[0].elements[1].checkBox.clickState1)
                            delete[] pxs;
                        
                        paintingChannelsSection[meshI+1].elements[channelI].button.texture = texture;

                        Library::addTexture(texture);
                    }
                }
            }
        }
    }

    //End the dialog
    if((panel.sections[0].elements[2].button.clicked) || getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || (!panel.hover && *Mouse::LClick())){
        this->dialogControl.unActivate();
    }

    //Render the panel
    panel.render(timer,true);

    dialogControl.updateEnd(timer,0.15f);
}