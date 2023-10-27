
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

NewTextureDialog::NewTextureDialog(){
    //Create the panel
    this->panel = Panel(
        
        {
            {
                Section(
                    Element(Button()),
                    {
                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Color"  , Texture(), 1.f, false)),
                        Element(TextBox(ELEMENT_STYLE_BASIC, glm::vec2(4,2),"NewTexture",1.f,false)),
                        Element(ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(2,2),  {"256", "512", "1024", "2048", "4096"} , "Resolution", 1.f)),
                        Element(Button(ELEMENT_STYLE_STYLIZED,glm::vec2(2,2), "Create"  , Texture(), 4.f, true)),
                        Element(Button(ELEMENT_STYLE_STYLIZED,glm::vec2(3,2), "Create With The Texture Selection Dialog"  , Texture(), 1.f, true))
                    }
                )
            }
        },
        glm::vec2(16.f),
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
    
    //Change the color button's color to black
    panel.sections[0].elements[0].button.color = glm::vec4(0,0,0,1);
    panel.sections[0].elements[0].button.solidColor = true;
}

bool __newTxtrDialog_last_texture_selection_dialog_state = false;

void NewTextureDialog::render(Timer timer){
    
    int textureRes = std::stoi(panel.sections[0].elements[2].comboBox.texts[panel.sections[0].elements[2].comboBox.selectedIndex]);

    dialogControl.updateStart();
    
    //Show the color picker dialog if clicked to the color button
    if(panel.sections[0].elements[0].button.clicked){
        unsigned char defRGB[4] = {0, 0, 0, 0}; // Black color (RGB = 0, 0, 0), alpha = 0
        Color clrObj;
        clrObj.loadRGB_normalized(glm::vec3(panel.sections[0].elements[0].button.color.r, panel.sections[0].elements[0].button.color.g, panel.sections[0].elements[0].button.color.b));
        const char* hex0Val = clrObj.getHEX().c_str();
        auto check = tinyfd_colorChooser("Select a color",hex0Val,defRGB,defRGB);

        if(check){
            Color clr(check);
            panel.sections[0].elements[0].button.color = glm::vec4(clr.getRGB_normalized(),1.f);
        }
    }

    //Clicked to the create button
    if(panel.sections[0].elements[4].button.clicked && !panel.sections[0].elements[2].comboBox.pressed){
        
        //Create the texture class
        Texture txtr;

        //Set the text of the texture as the title textbox's text
        txtr.title = panel.sections[0].elements[1].textBox.text;
        
        showTextureSelectionDialog(txtr, textureRes, true);

        //Send the created texture to the library
        if(txtr.ID){
            panel.sections[0].elements[0].button.color = glm::vec4(0,0,0,1);
            panel.sections[0].elements[1].textBox.text = "NewTexture";
            dialogControl.unActivate();
            Library::addTexture(txtr);
        }
    }
    
    if(panel.sections[0].elements[3].button.clicked && !panel.sections[0].elements[2].comboBox.pressed){
        
        //Create the texture class
        Texture txtr;

        //Set the text of the texture as the title textbox's text
        txtr.title = panel.sections[0].elements[1].textBox.text;
        
        //Pixels of the texture
        std::vector<GLubyte> colorData(textureRes * textureRes * 4, 0); // RGBA format
        
        //Fill the pixels of the texture array with the selected color value 
        for (int i = 0; i < textureRes * textureRes; ++i) {
            colorData[i * 4] = panel.sections[0].elements[0].button.color.r * 255.f;     // Red component
            colorData[i * 4 + 1] = panel.sections[0].elements[0].button.color.g * 255.f;   // Green component
            colorData[i * 4 + 2] = panel.sections[0].elements[0].button.color.b * 255.f;   // Blue component
            colorData[i * 4 + 3] = 255; // Alpha component
        }
        
        //Generate the texture
        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1,&txtr.ID);
        glBindTexture(GL_TEXTURE_2D,txtr.ID);

        //Texture params
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
	    
        //Write the texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureRes, textureRes, 0, GL_RGBA, GL_UNSIGNED_BYTE, &colorData[0]);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        //Send the created texture to the library
        Library::addTexture(txtr);
    }

    //End the dialog
    if((panel.sections[0].elements[3].button.clicked) || getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || (!panel.hover && *Mouse::LClick())){
        if(!(panel.sections[0].elements[4].button.clicked)){
            if(!panel.sections[0].elements[2].comboBox.pressed){
                panel.sections[0].elements[0].button.color = glm::vec4(0,0,0,1);
                panel.sections[0].elements[1].textBox.text = "NewTexture";
                dialogControl.unActivate();
            }
        }
    }

    __newTxtrDialog_last_texture_selection_dialog_state = panel.sections[0].elements[4].button.clicked;

    //Render the panel
    panel.render(timer,true);
    
    //Invert the text color of the color button
    panel.sections[0].elements[0].button.textColor = glm::vec4(glm::vec3(1.) - glm::vec3(panel.sections[0].elements[0].button.color),1);

    dialogControl.updateEnd(timer,0.15f);
}