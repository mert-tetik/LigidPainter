
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 Copyright (c) 2022-2023, LigidTools 

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

#include <string>
#include <iostream>
#include <vector>

#include "tinyfiledialogs.h"

NewTextureDialog::NewTextureDialog(){}
NewTextureDialog::NewTextureDialog(Context context,glm::vec2 videoScale,ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures){
    this->context = context;
    this->buttonShader = buttonShader;
    
    //Create the panel
    this->panel = Panel(
        buttonShader,
        colorPalette,
        {
            {
                Section(
                    Element(Button()),
                    {
                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2),colorPalette,buttonShader, "Color"  , Texture(), 1.f, false)),
                        Element(TextBox(0,glm::vec2(4,2),colorPalette,buttonShader,"NewTexture",1.f,false),context.window),
                        Element(Button(ELEMENT_STYLE_STYLIZED,glm::vec2(2,2),colorPalette,buttonShader, "Create"  , Texture(), 4.f, true))
                    }
                )
            }
        },
        glm::vec2(9.f),
        glm::vec3(50.f,50.f,0.8f),
        colorPalette.mainColor,
        colorPalette.thirdColor,
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
}

void NewTextureDialog::render(LigidWindow originalWindow,ColorPalette colorPalette,Mouse& mouse,Timer timer,TextRenderer &textRenderer,Library &library,glm::vec2 videoScale,int textureRes){
    
    dialogControl.updateStart(buttonShader);

    //Render the panel
    panel.render(videoScale,mouse,timer,textRenderer,true);
    
    //Invert the text color of the color button
    panel.sections[0].elements[0].button.textColor = glm::vec4(glm::vec3(1.) - glm::vec3(panel.sections[0].elements[0].button.color),1);
    
    //Show the color picker dialog if clicked to the color button
    if(panel.sections[0].elements[0].button.hover && mouse.LClick){
        unsigned char defRGB[4] = {0, 0, 0, 0}; // Black color (RGB = 0, 0, 0), alpha = 0
        const char* hex0Val = "#000000";
        auto check = tinyfd_colorChooser("Select a color",hex0Val,defRGB,defRGB);
        Color clr(check);

        if(check)
            panel.sections[0].elements[0].button.color = glm::vec4(clr.getRGB_normalized(),1.f);
    }

    //Clicked to the create button
    if(panel.sections[0].elements[2].button.hover && mouse.LClick){
        
        //Create the texture class
        Texture txtr;

        //Set the text of the texture as the title textbox's text
        txtr.title = panel.sections[0].elements[1].textBox.text;
        
        //Create the texture
        
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	    
        //Write the texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureRes, textureRes, 0, GL_RGBA, GL_UNSIGNED_BYTE, &colorData[0]);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        //Send the created texture to the library
        library.addTexture(txtr);
    }
    
    //End the dialog
    if((panel.sections[0].elements[2].button.hover && mouse.LClick) || context.window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || (!panel.hover && mouse.LClick)){
        panel.sections[0].elements[0].button.color = glm::vec4(0,0,0,1);
        panel.sections[0].elements[1].textBox.text = "NewTexture";
        dialogControl.unActivate();
    }

    dialogControl.updateEnd(timer,buttonShader,0.15f);
}