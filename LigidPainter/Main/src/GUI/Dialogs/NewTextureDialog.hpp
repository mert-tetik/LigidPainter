
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

 #ifndef LGDUINEWTEXTUREDIALOG_HPP
 #define LGDUINEWTEXTUREDIALOG_HPP

 #include<glad/glad.h>
 #include<GLFW/glfw3.h>

 #include <glm/glm.hpp>
 #include <glm/gtc/matrix_transform.hpp>

 #include "assimp/Importer.hpp"
 #include "assimp/scene.h"
 #include "assimp/postprocess.h"

 #include "Shader.hpp"
 #include "Box.hpp"
 #include "Renderer.h"
 #include "Util.hpp"

 #include "GUI/UI.hpp"
 #include "GUI/Button.hpp"
 #include "Mouse.hpp"
 #include "Timer.hpp"


 #include <glm/gtc/type_ptr.hpp>

 #include <string>
 #include <fstream>
 #include <sstream>
 #include <iostream>
 #include <map>
 #include <vector>
 #include <cstdlib>

 #include "Color.hpp"

 class NewTextureDialog
 {
 private:
    Context context;
 public:
     Panel panel;

     NewTextureDialog(){}

     Shader buttonShader;

     bool active = false;


    NewTextureDialog(Context context,glm::vec2 videoScale,ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures){
        this->context = context;

        this->panel = Panel(buttonShader,colorPalette,{
            {
                Section(
                    Element(Button()),
                    {
                        Element(Button(2,glm::vec2(2,2),colorPalette,buttonShader, "Color"  , Texture(), 1.f, false)),
                        Element(TextBox(0,glm::vec2(4,2),colorPalette,buttonShader,"NewTexture",1.f,false),context.window),
                        Element(Button(0,glm::vec2(2,2),colorPalette,buttonShader, "Create"  , Texture(), 4.f, true))
                    }
                )
            }
        },glm::vec2(9.f),glm::vec3(50.f,50.f,0.8f),colorPalette.mainColor,colorPalette.thirdColor,true,true,true,true,true,1.f,1.f,{},0.25f);

        panel.sections[0].elements[0].button.color = glm::vec4(0,0,0,1);
    }
    
    void render(GLFWwindow* originalWindow,ColorPalette colorPalette,Mouse& mouse,Timer timer,TextRenderer &textRenderer,Library &library,glm::vec2 videoScale,int textureRes){
        panel.render(videoScale,mouse,timer,textRenderer,true);

        //Invert the text color
        panel.sections[0].elements[0].button.textColor = glm::vec4(glm::vec3(1.) - glm::vec3(panel.sections[0].elements[0].button.color),1);

        //Clicked to the color button
        if(panel.sections[0].elements[0].button.hover && mouse.LClick){
            unsigned char defRGB[4] = {0, 0, 0, 0}; // Black color (RGB = 0, 0, 0), alpha = 0
            const char* hex0Val = "#000000";
            auto check = tinyfd_colorChooser("Select a color",hex0Val,defRGB,defRGB);

            Color clr;
            if(check)
                panel.sections[0].elements[0].button.color = glm::vec4(clr.hexToRgb(check)/glm::vec3(255.f),1.f);
        }

        //Clicked to the create button
        if(panel.sections[0].elements[2].button.hover && mouse.LClick){
            Texture txtr;
            txtr.title = panel.sections[0].elements[1].textBox.text;
            //Create the texture
            std::vector<GLubyte> colorData(textureRes * textureRes * 4, 0); // RGBA format
            for (int i = 0; i < textureRes * textureRes; ++i) {
                colorData[i * 4] = panel.sections[0].elements[0].button.color.r * 255.f;     // Red component
                colorData[i * 4 + 1] = panel.sections[0].elements[0].button.color.g * 255.f;   // Green component
                colorData[i * 4 + 2] = panel.sections[0].elements[0].button.color.b * 255.f;   // Blue component
                colorData[i * 4 + 3] = 255; // Alpha component
            }
            glActiveTexture(GL_TEXTURE0);
            glGenTextures(1,&txtr.ID);
            glBindTexture(GL_TEXTURE_2D,txtr.ID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

		    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureRes, textureRes, 0, GL_RGBA, GL_UNSIGNED_BYTE, &colorData[0]);
            glGenerateMipmap(GL_TEXTURE_2D);

            library.textures.push_back(txtr);

        }
        
        //End the dialog
        if((panel.sections[0].elements[2].button.hover && mouse.LClick) || glfwGetKey(context.window,GLFW_KEY_ESCAPE) == GLFW_PRESS || (!panel.hover && mouse.LClick)){
            panel.sections[0].elements[0].button.color = glm::vec4(0,0,0,1);
            panel.sections[0].elements[1].textBox.text = "NewTexture";
            active = false;
        }
    }
 };

 #endif