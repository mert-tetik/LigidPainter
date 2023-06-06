
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

 #ifndef LGDUIGREETINGDIALOG_HPP
 #define LGDUIGREETINGDIALOG_HPP

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
 #include "GUI/Dialogs/NewProjectDialog.hpp"
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

 class GreetingDialog
 {
 private:
     /* data */
 public:
     Button loadProjectButton;
     Button createProjectButton;
     Button textureDisplayerButton;
     
     Button textButton1;

     Panel bgPanel;

     GreetingDialog(){}

     Box box;

     Shader buttonShader;

    bool active = true;

    GreetingDialog(Context context,glm::vec2 videoScale,ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures){

        this->textButton1 = Button(0,glm::vec2(8,2),colorPalette,buttonShader,"Welcome To The LigidPainter",Texture(),0.f,false);
        this->textButton1.color = glm::vec4(0);
        this->textButton1.pos.x = 40;
        this->textButton1.pos.y = 40;
        this->textButton1.pos.z = 0.9f;
        this->textButton1.textScale = 1.0f;
        
        this->loadProjectButton = Button(0,glm::vec2(4,2),colorPalette,buttonShader,"Load",Texture(),0.f,false);
        this->loadProjectButton.pos.x = 40;
        this->loadProjectButton.pos.y = 60;
        this->loadProjectButton.pos.z = 0.9f;
        
        this->createProjectButton = Button(0,glm::vec2(4,2),colorPalette,buttonShader,"Create",Texture(),0.f,false);
        this->createProjectButton.pos.x = 40;
        this->createProjectButton.pos.y = 55;
        this->createProjectButton.pos.z = 0.9f;
        
        this->textureDisplayerButton = Button(0,glm::vec2(25,25),colorPalette,buttonShader,"",appTextures.greetingDialogImage,0.f,false);
        this->textureDisplayerButton.pos.x = 60;
        this->textureDisplayerButton.pos.y = 50;
        this->textureDisplayerButton.pos.z = 0.9f;
        this->textureDisplayerButton.color = glm::vec4(0);
        this->textureDisplayerButton.color2 = glm::vec4(0);
        this->textureDisplayerButton.outlineColor = glm::vec4(0);
        this->textureDisplayerButton.outlineColor2 = glm::vec4(0);

        this->bgPanel = Panel(buttonShader,colorPalette,{},glm::vec2(20),glm::vec3(50.f,50.f,0.8f),colorPalette.mainColor,colorPalette.thirdColor,false,true,true,true,true,1.f,1.f,{},0.25f);
    }
    
    void render(GLFWwindow* originalWindow,ColorPalette colorPalette,Mouse& mouse,Timer timer,TextRenderer &textRenderer,glm::vec2 videoScale, NewProjectDialog &newProjectDialog){
        bgPanel.render(videoScale,mouse,timer,textRenderer,false);
        loadProjectButton.render(videoScale,mouse,timer,textRenderer,true);
        createProjectButton.render(videoScale,mouse,timer,textRenderer,true);
        
        textButton1.render(videoScale,mouse,timer,textRenderer,false);
        
        if(createProjectButton.hover && mouse.LClick){
            newProjectDialog.active = true;
            this->active = false;
        }

        glDepthFunc(GL_ALWAYS);
        textureDisplayerButton.scale.x = bgPanel.scale.y;
        textureDisplayerButton.scale.y = bgPanel.scale.y;
        textureDisplayerButton.render(videoScale,mouse,timer,textRenderer,false);
        glDepthFunc(GL_LEQUAL);
    }
 };

 #endif