
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

 #ifndef LGDUINEWPROJECTDIALOG_HPP
 #define LGDUINEWPROJECTDIALOG_HPP

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
 
 #include "Project.hpp"

 #include <glm/gtc/type_ptr.hpp>

 #include <string>
 #include <fstream>
 #include <sstream>
 #include <iostream>
 #include <map>
 #include <vector>
 #include <cstdlib>

 class NewProjectDialog
 {
 private:
     /* data */
 public:
     Panel panel;

     NewProjectDialog(){}

     Box box;

     Shader buttonShader;

    

    NewProjectDialog(Context context,glm::vec2 videoScale,ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures){
        this->buttonShader = buttonShader;

        this->panel = Panel(
                                buttonShader,
                                colorPalette,
                                {
                                    Section(
                                        Element(Button()),
                                        {
                                            Element(Button(0,glm::vec2(4,2),colorPalette,buttonShader,"",Texture(),4.f,false)), //Empty
                                            Element(TextBox(0,glm::vec2(4,2),colorPalette,buttonShader,"MyProject",4.f,false)),
                                            Element(TextBox(0,glm::vec2(4,2),colorPalette,buttonShader,"Path",4.f,true)),
                                            
                                            Element(ComboBox(0,glm::vec2(4,2),colorPalette,buttonShader,
                                            {
                                                "256",
                                                "512",
                                                "1024",
                                                "2048",
                                                "4096"
                                            },4.f)),

                                            Element(CheckBox(0,glm::vec2(4,2),colorPalette,buttonShader,"Large",4.f)),
                                            Element(CheckBox(0,glm::vec2(4,2),colorPalette,buttonShader,"Medium",0.f)),
                                            Element(CheckBox(0,glm::vec2(4,2),colorPalette,buttonShader,"Small",0.f)),
                                            Element(Button(0,glm::vec2(4,2),colorPalette,buttonShader,"3D Model",Texture(),4.f,false)),
                                            Element(Button(0,glm::vec2(4,2),colorPalette,buttonShader,"Create",Texture(),4.f,false))
                                        }
                                    )
                                },
                                glm::vec2(20,40),
                                glm::vec3(50.f,50.f,0.8f),
                                colorPalette.mainColor,
                                colorPalette.thirdColor,
                                true,
                                true,
                                true,
                                true,
                                true,
                                1.f,
                                1,
                                {},
                                0.25f
                            );

        this->panel.sections[0].elements[0].button.color = glm::vec4(0);
        this->panel.sections[0].elements[0].button.color2 = glm::vec4(0);
    }
    

    void render(GLFWwindow* originalWindow,ColorPalette colorPalette,Mouse& mouse,Timer timer,TextRenderer &textRenderer,glm::vec2 videoScale,Project &project){
        panel.render(videoScale,mouse,timer,textRenderer,true);

        if(panel.sections[0].elements[panel.sections[0].elements.size()-1].button.hover && mouse.LClick){
            project.createProject(panel.sections[0].elements[2].textBox.text,panel.sections[0].elements[1].textBox.text);
        }
    }
 };

 #endif