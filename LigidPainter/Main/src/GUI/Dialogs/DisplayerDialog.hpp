
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

 #ifndef LGDUIDISPLAYERDIALOGHPP
 #define LGDUIDISPLAYERDIALOGHPP

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

 class DisplayerDialog
 {
 private:
    Context context;
 public:
    Panel panel;

    DisplayerDialog(){}

    Shader buttonShader;

    bool active = false;
    bool firstActivation = false;

    DisplayerDialog(Context context,glm::vec2 videoScale,ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures){
        this->context = context;

        this->panel = Panel(buttonShader,colorPalette,{
            {
                Section(
                    Element(Button()),
                    {
                        Element(Button(1,glm::vec2(2,4),colorPalette,buttonShader, "Texture"  , appTextures.greetingDialogImage, 1.f,true)),
                        Element(RangeBar(0,glm::vec2(2,1),colorPalette,buttonShader, "Rotation"  , Texture(), 1.f,0.f,360.f,0.f)), 
                        Element(RangeBar(0,glm::vec2(2,1),colorPalette,buttonShader, "Blur"  , Texture(), 1.f,0.f,100.f,0.f)), 
                        
                        Element(RangeBar(0,glm::vec2(2,1),colorPalette,buttonShader, "Opacity"  , Texture(), 1.f,0.f,100.f,0.f)), 
                        Element(Button(2,glm::vec2(2,2),colorPalette,buttonShader, "Color"  , Texture(), 1.f, false)),
                    }
                )
            }
        },glm::vec2(15.f),glm::vec3(50.f,50.f,0.8f),colorPalette.mainColor,colorPalette.thirdColor,true,true,true,true,true,1.f,1.f,{},0.25f);
    }
    
    void render(GLFWwindow* originalWindow,ColorPalette colorPalette,Mouse& mouse,Timer timer,TextRenderer &textRenderer,Library &library,glm::vec2 videoScale,Skybox &skybox){
        panel.render(videoScale,mouse,timer,textRenderer,true);

        panel.sections[0].elements[0].button.color = glm::vec4(skybox.bgColor,1);

        if(panel.sections[0].elements[0].button.hover && mouse.LClick){
            unsigned char defRGB[4] = {0, 0, 0, 0}; // Black color (RGB = 0, 0, 0), alpha = 0
            const char* hex0Val = "#000000";
            auto check = tinyfd_colorChooser("Select a color",hex0Val,defRGB,defRGB);

            Color clr;
            if(check)
                skybox.bgColor = clr.hexToRgb(check)/glm::vec3(255.f);
        }

        skybox.transformMatrix = glm::mat4(1);
        skybox.transformMatrix = glm::rotate(skybox.transformMatrix,glm::radians(panel.sections[0].elements[1].rangeBar.value),glm::vec3(0,1,0));
    
        skybox.lod = panel.sections[0].elements[2].rangeBar.value/25.f;
        
        skybox.opacity = panel.sections[0].elements[3].rangeBar.value/100.f;
    }

    void activate(){
        active = true;
        firstActivation = true;
    }
 };

 #endif