
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

#include <string>
#include <iostream>
#include <vector>

GreetingDialog::GreetingDialog(){}

GreetingDialog::GreetingDialog(Context context,glm::vec2 videoScale,ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures){
    
    this->buttonShader = buttonShader;

    //First text button
    this->textButton1 = Button(ELEMENT_STYLE_STYLIZED,glm::vec2(8,2),colorPalette,buttonShader,"Welcome To The LigidPainter",Texture(),0.f,false);
    this->textButton1.color = glm::vec4(0);
    this->textButton1.pos.x = 40;
    this->textButton1.pos.y = 40;
    this->textButton1.pos.z = 0.9f;
    this->textButton1.textScale = 1.0f;
    
    //Load project button
    this->loadProjectButton = Button(ELEMENT_STYLE_STYLIZED,glm::vec2(4,2),colorPalette,buttonShader,"Load",Texture(),0.f,false);
    this->loadProjectButton.pos.x = 45;
    this->loadProjectButton.pos.y = 58;
    this->loadProjectButton.pos.z = 0.9f;
    this->loadProjectButton.color = glm::vec4(1);
    this->loadProjectButton.outlineColor = glm::vec4(1);
    this->loadProjectButton.textColor = glm::vec4(1);
    
    //Create project button
    this->createProjectButton = Button(ELEMENT_STYLE_STYLIZED,glm::vec2(4,2),colorPalette,buttonShader,"Create",Texture(),0.f,false);
    this->createProjectButton.pos.x = 55;
    this->createProjectButton.pos.y = 58;
    this->createProjectButton.pos.z = 0.9f;
    this->createProjectButton.color = glm::vec4(1);
    this->createProjectButton.outlineColor = glm::vec4(1);
    this->createProjectButton.textColor = glm::vec4(1);
    
    //Decoration texture displayer button
    this->textureDisplayerButton = Button(ELEMENT_STYLE_STYLIZED,glm::vec2(30,30),colorPalette,buttonShader,"",appTextures.greetingDialogImage,0.f,false);
    this->textureDisplayerButton.pos.x = 50;
    this->textureDisplayerButton.pos.y = 50;
    this->textureDisplayerButton.pos.z = 0.8f;
    this->textureDisplayerButton.color = glm::vec4(0);
    this->textureDisplayerButton.color2 = glm::vec4(0);
    this->textureDisplayerButton.outlineColor = glm::vec4(0);
    this->textureDisplayerButton.outlineColor2 = glm::vec4(0);
    
    //Background panel
    this->bgPanel = Panel(buttonShader,colorPalette,{},glm::vec2(20),glm::vec3(50.f,50.f,0.8f),colorPalette.mainColor,colorPalette.thirdColor,false,true,true,true,true,1.f,1.f,{},0.25f,false);

    bgPanel.scale.x = textureDisplayerButton.scale.x;

    this->dialogControl.activate();
}

void GreetingDialog::render(LigidWindow originalWindow,ColorPalette colorPalette,Mouse& mouse,Timer timer,TextRenderer &textRenderer,glm::vec2 videoScale, NewProjectDialog &newProjectDialog,LoadProjectDialog &loadProjectDialog){

    dialogControl.updateStart(buttonShader);

    //Render elements 
    
    //Render the texture displayer button 
    textureDisplayerButton.render(videoScale,mouse,timer,textRenderer,false);

    //bgPanel.render(videoScale,mouse,timer,textRenderer,false);
    loadProjectButton.render(videoScale,mouse,timer,textRenderer,dialogControl.isComplete());
    createProjectButton.render(videoScale,mouse,timer,textRenderer,dialogControl.isComplete());
    
    //Render text elements
    // textButton1.render(videoScale,mouse,timer,textRenderer,false);
    
    //Show new project dialog if create project button is pressed
    if(createProjectButton.hover && mouse.LClick){
        newProjectDialog.dialogControl.activate();
        this->dialogControl.unActivate();
    }

    //Show load project dialog if load project button is pressed
    if(loadProjectButton.hover && mouse.LClick){
        loadProjectDialog.dialogControl.activate();
        this->dialogControl.unActivate();
    }

    //Render the decoration texture displayer button without depth testing
    glDepthFunc(GL_ALWAYS);
    
    //Set the depth func back to default (less or equal)
    glDepthFunc(GL_LEQUAL);

    dialogControl.updateEnd(timer,buttonShader,0.15f);
}