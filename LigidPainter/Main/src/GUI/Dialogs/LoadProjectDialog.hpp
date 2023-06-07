
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
     
     Button textBtn1;
     Button textBtn2;
     Button textBtn3;
     Button textBtn4;

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
        this->loadButton.pos.x = 30;
        this->loadButton.pos.y = 35;
        this->loadButton.pos.z = 0.9f;

        this->textBtn1 = Button(0,glm::vec2(8,2),colorPalette,buttonShader,"Load a *.ligid file",Texture(),0.f,false);
        this->textBtn1.color = glm::vec4(0);
        this->textBtn1.pos.x = 30;
        this->textBtn1.pos.y = 30;
        this->textBtn1.pos.z = 0.9f;
        this->textBtn1.textScale = 1.0f;
        
        this->textBtn2 = Button(0,glm::vec2(8,2),colorPalette,buttonShader,"The *.ligid file is a binary file located",Texture(),0.f,false);
        this->textBtn2.color = glm::vec4(0);
        this->textBtn2.pos.x = 30;
        this->textBtn2.pos.y = 45;
        this->textBtn2.pos.z = 0.9f;
        this->textBtn2.textScale = 0.7f;
        
        this->textBtn3 = Button(0,glm::vec2(8,2),colorPalette,buttonShader,"in a project folder capable of contain project data.",Texture(),0.f,false);
        this->textBtn3.color = glm::vec4(0);
        this->textBtn3.pos.x = 30;
        this->textBtn3.pos.y = 48;
        this->textBtn3.pos.z = 0.9f;
        this->textBtn3.textScale = 0.7f;
        
        this->textBtn4 = Button(0,glm::vec2(8,2),colorPalette,buttonShader,"Please create a project folder if you don't have access to a project folder",Texture(),0.f,false);
        this->textBtn4.color = glm::vec4(0);
        this->textBtn4.pos.x = 30;
        this->textBtn4.pos.y = 54;
        this->textBtn4.pos.z = 0.9f;
        this->textBtn4.textScale = 0.5f;
    }
    

    void render(GLFWwindow* originalWindow,ColorPalette colorPalette,Mouse& mouse,Timer timer,TextRenderer &textRenderer,glm::vec2 videoScale,Project &project,bool &greetingDialogActive,Library &library,Shaders shaders,Model &model){
        bgPanel.render(videoScale,mouse,timer,textRenderer,true);
        loadButton.render(videoScale,mouse,timer,textRenderer,true);

        textBtn1.render(videoScale,mouse,timer,textRenderer,false);
        textBtn2.render(videoScale,mouse,timer,textRenderer,false);
        textBtn3.render(videoScale,mouse,timer,textRenderer,false);
        textBtn4.render(videoScale,mouse,timer,textRenderer,false);

        if(loadButton.hover && mouse.LClick){
            //Select 3D Model
            char const* lFilterPatterns[1] = { "*.ligid" };
	        char* test = tinyfd_openFileDialog("Select a ligid file","",1, lFilterPatterns,"",false);
            
            if(test){
                if(project.loadProject(test,library,shaders,model))
                    this->active = false;
                else{
                    const char* title = "Warning";
                    const char* message = "Error while reading the *.ligid file! Detailed error message is printed to the terminal.";
                    const char* icon = "warning";
                    const char* button = "Ok";
                    tinyfd_messageBox(title, message, button, icon, 1);
                }
            }
            
        }
        
        int counter = 0;
        
        Section projectSection;
        projectsPanel.sections.clear();

        for (const auto& entry : std::filesystem::directory_iterator("./Projects")) {
            std::string projectPath = entry.path().string();

            Button btn = Button(2,glm::vec2(4,2),colorPalette,buttonShader,projectPath,Texture(),0.f,false);
            btn.scale.x = projectsPanel.scale.x;

            std::string ligidFilePath = project.locateLigidFileInFolder(projectPath);

            if(ligidFilePath.size()){
                time_t creationDate;
                time_t lastOpeningDate;
                project.returnLigidFileData(ligidFilePath,creationDate,lastOpeningDate);
                
                std::string creationDateStr = (std::string)std::ctime(&creationDate);
                std::string lastOpeningDateStr = (std::string)std::ctime(&lastOpeningDate);
                
                creationDateStr.pop_back(); //Remove suspecious '\n' character

                //if (projectPath.length() >= 50) {
                //    projectPath = projectPath.substr(0, 49);
                //}

                //btn.text = projectPath;

                //for (size_t i = 0; i < 50 - projectPath.size(); i++)
                //{
                //    btn.text += " ";
                //}
                //
                //btn.text += creationDateStr + "                          " + lastOpeningDateStr;
                
                projectSection.elements.push_back(btn);
                
                counter++;
            }
        }

        projectsPanel.sections.push_back(projectSection);
        projectsPanel.render(videoScale,mouse,timer,textRenderer,true);

        for (size_t i = 0; i < projectsPanel.sections[0].elements.size(); i++)
        {
            if(projectsPanel.sections[0].elements[i].button.hover && mouse.LClick){
                
                std::string ligidFilePath = project.locateLigidFileInFolder(projectsPanel.sections[0].elements[i].button.text);

                if(project.loadProject(ligidFilePath,library,shaders,model))
                    this->active = false;
                else{
                    const char* title = "Warning";
                    const char* message = "Error while reading the *.ligid file! Detailed error message is printed to the terminal.";
                    const char* icon = "warning";
                    const char* button = "Ok";

                    tinyfd_messageBox(title, message, button, icon, 1);
                }
            }
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