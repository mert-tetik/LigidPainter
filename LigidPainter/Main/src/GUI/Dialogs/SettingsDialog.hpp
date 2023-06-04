
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

 #ifndef LGDUISETTINGSDIALOG_HPP
 #define LGDUISETTINGSDIALOG_HPP

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

 class SettingsDialog
 {
 private:
    Context context;
 public:
    Panel panel;

    SettingsDialog(){}

    Shader buttonShader;

    bool active = false;
    bool firstActivation = false;

    SettingsDialog(Context context,glm::vec2 videoScale,ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures){
        this->context = context;

        this->panel = Panel(buttonShader,colorPalette,{
            {
                Section(
                    Element(Button()),
                    {
                        Element(ComboBox(0,glm::vec2(4,2),colorPalette,buttonShader,
                        {
                            "256",
                            "512",
                            "1024",
                            "2048",
                            "4096"
                        },4.f)),
                        Element(CheckBox(0,glm::vec2(2,2),colorPalette,buttonShader, "VSync"  , 1.f)),
                    }
                )
            }
        },glm::vec2(9.f),glm::vec3(50.f,50.f,0.8f),colorPalette.mainColor,colorPalette.thirdColor,true,true,true,true,true,1.f,1.f,{},0.25f);
    }
    
    void render(GLFWwindow* originalWindow,ColorPalette colorPalette,Mouse& mouse,Timer timer,TextRenderer &textRenderer,Library &library,glm::vec2 videoScale,int &textureRes,bool &VSync){
        
        //Set the combo box selected index as the textureRes
        int txtrRes = 256;
        for (size_t i = 0; i < panel.sections[0].elements[0].comboBox.texts.size(); i++)
        {
            if(textureRes == txtrRes)
                panel.sections[0].elements[0].comboBox.selectedIndex = i;
            
            txtrRes*=2;
        }
        
        
        VSync = panel.sections[0].elements[1].checkBox.clickState1;

        
        panel.render(videoScale,mouse,timer,textRenderer,true);

        //End the dialog
        if((glfwGetKey(context.window,GLFW_KEY_ESCAPE) == GLFW_PRESS || (!panel.hover && mouse.LClick)) && !firstActivation){
            active = false;
        }
        firstActivation = false;

        for (size_t i = 0; i < panel.sections[0].elements[0].comboBox.hover.size(); i++)
        {
            //If pressed to any of the combo box element
            if(panel.sections[0].elements[0].comboBox.hover[i] && mouse.LClick){
                textureRes = stoi(panel.sections[0].elements[0].comboBox.texts[panel.sections[0].elements[0].comboBox.selectedIndex]);
            }
        }
        
    }

    void activate(){
        active = true;
        firstActivation = true;
    }
 };

 #endif