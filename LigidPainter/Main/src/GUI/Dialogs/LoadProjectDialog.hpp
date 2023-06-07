
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

 #ifndef LGDUILOADPROJECTDIALOG_HPP
 #define LGDUILOADPROJECTDIALOG_HPP

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

 class LoadProjectDialog
 {
 private:
     /* data */
 public:
     Panel bgPanel;
     Panel projectsPanel;
     Button loadButton;

     LoadProjectDialog(){}

     Box box;

     Shader buttonShader;

    bool active = false;

    LoadProjectDialog(Context context,glm::vec2 videoScale,ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures){
        this->buttonShader = buttonShader;

        this->bgPanel = Panel(
                                buttonShader,
                                colorPalette,
                                {
                                    Section(
                                        Element(Button()),
                                        {
                                            Element(Button(0,glm::vec2(4,2),colorPalette,buttonShader,"Load Project",Texture(),4.f,false)), 
                                        }
                                    )
                                },
                                glm::vec2(30,30),
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
        this->projectsPanel = Panel(
                                buttonShader,
                                colorPalette,
                                {
                                },
                                glm::vec2(20,25),
                                glm::vec3(60.f,50.f,0.8f),
                                colorPalette.secondColor,
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

        //Panel's navigation (kinda) bar
        this->bgPanel.sections[0].elements[0].button.color = colorPalette.secondColor;
        this->bgPanel.sections[0].elements[0].button.color2 = colorPalette.thirdColor;
        this->bgPanel.sections[0].elements[0].button.outlineColor = colorPalette.thirdColor;
        this->bgPanel.sections[0].elements[0].button.outlineColor2 = colorPalette.thirdColor;

        //The load a project button
        this->loadButton = Button(0,glm::vec2(4,2),colorPalette,buttonShader,"Load",Texture(),0.f,false);
        this->loadButton.pos.x = 40;
        this->loadButton.pos.y = 60;
        this->loadButton.pos.z = 0.9f;
    }
    

    void render(GLFWwindow* originalWindow,ColorPalette colorPalette,Mouse& mouse,Timer timer,TextRenderer &textRenderer,glm::vec2 videoScale,Project &project,bool &greetingDialogActive,Library &library,Shaders shaders,Model &model){
        bgPanel.render(videoScale,mouse,timer,textRenderer,true);
        projectsPanel.render(videoScale,mouse,timer,textRenderer,true);
        
        int counter = 0;
        for (const auto& entry : std::filesystem::directory_iterator("./Projects")) {
            std::string projectPath = entry.path().string();

            Button btn = Button(0,glm::vec2(4,2),colorPalette,buttonShader,projectPath,Texture(),4.f,false);
            btn.scale.x = projectsPanel.scale.x;
            btn.pos =  projectsPanel.pos;
            btn.pos.y -= projectsPanel.scale.y;
            btn.pos.y += btn.scale.y;

            btn.pos.y += btn.scale.y * 2 * counter;

            btn.render(videoScale,mouse,timer,textRenderer,true);
        
            counter++;
        }

        if(loadButton.hover && mouse.LClick){
            //project.loadProject(project.ligidFilePath,library,shaders,model);
            //this->active = false;
        }

        if(glfwGetKey(originalWindow,GLFW_KEY_ESCAPE) == GLFW_PRESS || bgPanel.sections[0].elements[0].button.hover && mouse.LDoubleClick){
            greetingDialogActive = true;
            this->active = false;
        }
    }
 };

 #endif