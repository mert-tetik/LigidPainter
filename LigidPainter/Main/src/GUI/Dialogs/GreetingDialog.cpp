
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 (c) 2024 Mert Tetik. All rights reserved.

 Official GitHub Link : https://github.com/mert-tetik/LigidPainter
 Official Web Page : https:ligidtools.com/ligidpainter

 ---------------------------------------------------------------------------
 */

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"

#include <curl/curl.h>

#include <string>
#include <iostream>
#include <vector>

static bool newVersionMode = false;
static int currentVersion = 241;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

bool isNumeric(const std::string& str) {
    for (char c : str) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}

GreetingDialog::GreetingDialog(int){

    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if(curl) {

        curl_easy_setopt(curl, CURLOPT_URL, "http://storage.googleapis.com/ligidtoolsbucket/LigidPainter_Latest_Version.txt");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if(readBuffer.size()){
            if(isNumeric(readBuffer)){
                try
                {
                    if(std::stoi(readBuffer) > currentVersion){
                        newVersionMode = true;
                    }
                }
                catch(const std::exception& e)
                {
                }
            }
        }
    }
    else{
        std::cout << "Failed to initialize libcurl!" << std::endl;
    }

    
    //First text button
    this->textButton1 = Button(ELEMENT_STYLE_STYLIZED,glm::vec2(8,2), "Welcome To The LigidPainter",Texture(),0.f,false);
    this->textButton1.color = glm::vec4(0);
    this->textButton1.pos.x = 40;
    this->textButton1.pos.y = 40;
    this->textButton1.pos.z = 0.9f;
    this->textButton1.textScale = 1.0f;
    
    //Load project button
    this->loadProjectButton = Button(ELEMENT_STYLE_STYLIZED,glm::vec2(4,2), "Load",Texture(),0.f,false);
    this->loadProjectButton.pos.x = 45;
    this->loadProjectButton.pos.y = 62;
    this->loadProjectButton.pos.z = 0.9f;
    this->loadProjectButton.color = glm::vec4(1);
    this->loadProjectButton.outlineColor = glm::vec4(1);
    this->loadProjectButton.textColor = glm::vec4(1);
    
    //Create project button
    this->createProjectButton = Button(ELEMENT_STYLE_STYLIZED,glm::vec2(4,2), "Create",Texture(),0.f,false);
    this->createProjectButton.pos.x = 55;
    this->createProjectButton.pos.y = 62;
    this->createProjectButton.pos.z = 0.9f;
    this->createProjectButton.color = glm::vec4(1);
    this->createProjectButton.outlineColor = glm::vec4(1);
    this->createProjectButton.textColor = glm::vec4(1);
    
    //Decoration texture displayer button
    this->textureDisplayerButton = Button(ELEMENT_STYLE_STYLIZED, glm::vec2(40,40), "", appTextures.greetingDialogImage, 0.f, false);
    this->textureDisplayerButton.radius = 0.35;
    this->textureDisplayerButton.pos.x = 50;
    this->textureDisplayerButton.pos.y = 50;
    this->textureDisplayerButton.pos.z = 0.8f;
    this->textureDisplayerButton.color = glm::vec4(0);
    this->textureDisplayerButton.color2 = glm::vec4(0);
    this->textureDisplayerButton.outlineColor = glm::vec4(0);
    this->textureDisplayerButton.outlineColor2 = glm::vec4(0);
    
    this->newVersionBannerButton = Button(ELEMENT_STYLE_STYLIZED, glm::vec2(40,40), "", appTextures.newVersionBanner, 0.f, false);
    this->newVersionBannerButton.radius = 0.35;
    this->newVersionBannerButton.pos.x = 50;
    this->newVersionBannerButton.pos.y = 80;
    this->newVersionBannerButton.pos.z = 0.8f;
    this->newVersionBannerButton.color = glm::vec4(0);
    this->newVersionBannerButton.color2 = glm::vec4(0);
    this->newVersionBannerButton.outlineColor = glm::vec4(0);
    this->newVersionBannerButton.outlineColor2 = glm::vec4(0);
    
    this->downloadNewVersionButton = Button(ELEMENT_STYLE_STYLIZED, glm::vec2(10.f,2), "Download", Texture(), 0.f, false);
    this->downloadNewVersionButton.pos.x = 40;
    this->downloadNewVersionButton.pos.y = 82;
    this->downloadNewVersionButton.pos.z = 0.9f;
    
    //Background panel
    this->bgPanel = Panel({},glm::vec2(40),glm::vec3(50.f,50.f,0.8f),ColorPalette::mainColor,ColorPalette::thirdColor,false,true,true,true,true,1.f,1.f,{},0.25f,false);

    bgPanel.scale.x = textureDisplayerButton.scale.x;
}

void GreetingDialog::show(Timer& timer, Project& project){

    this->dialogControl.activate();

    while(!getContext()->window.shouldClose())
    {
        
        /* code */
        dialogControl.updateStart();

        //Render elements 
        
        //Render the texture displayer button 
        textureDisplayerButton.render(timer,false);
        if(newVersionMode){
            newVersionBannerButton.render(timer, false);
            downloadNewVersionButton.render(timer, dialogControl.isComplete());
            
            if(downloadNewVersionButton.clicked){
                Website website = Website("https://ligidtools.com/release");
                website.open();
            }
        }

        //bgPanel.render(timer,false);
        loadProjectButton.render(timer,dialogControl.isComplete());
        createProjectButton.render(timer,dialogControl.isComplete());
        
        //Render text elements
        // textButton1.render(timer,false);
        
        //Show new project dialog if create project button is pressed
        if(createProjectButton.clicked){
            this->dialogControl.unActivate();
            this->dialogControl.mixVal = 0.f;
            dialog_newProject.show(timer, project);
            break;
        }

        //Show load project dialog if load project button is pressed
        if(loadProjectButton.clicked){
            this->dialogControl.unActivate();
            this->dialogControl.mixVal = 0.f;
            dialog_loadProject.show(timer, project);
            break;
        }

        //Render the decoration texture displayer button without depth testing
        glDepthFunc(GL_ALWAYS);
        
        //Set the depth func back to default (less or equal)
        glDepthFunc(GL_LEQUAL);

        dialogControl.updateEnd(timer, 2.0f);

        if(dialogControl.mixVal == 0.f)
            break;
    }   
}