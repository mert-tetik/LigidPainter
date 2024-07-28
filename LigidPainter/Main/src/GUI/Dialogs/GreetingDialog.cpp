
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
static int currentVersion = 300;

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

int extractVersionNumber(const std::string& data, const std::string& tag) {
    // Find the position of the tag_name
    std::string searchStr = "\"" + tag + "\":\"v";
    size_t tagPos = data.find(searchStr);
    if (tagPos == std::string::npos) {
        // Tag not found
        return -1;
    }

    // Move position to the start of the version number
    tagPos += searchStr.length();

    // Find the end of the version number
    size_t endPos = data.find('"', tagPos);
    if (endPos == std::string::npos) {
        // Malformed data
        return -1;
    }

    // Extract the version string
    std::string versionStr = data.substr(tagPos, endPos - tagPos);

    // Convert version string to integer
    std::stringstream ss(versionStr);
    int major, minor, patch = 0;
    char dot1, dot2;

    if (!(ss >> major >> dot1 >> minor >> dot2 >> patch) || dot1 != '.' || (dot2 != '.' && dot2 != '\0')) {
        // Malformed version string
        return -1;
    }

    // Return version as integer (e.g., v2.4.0 => 20400)
    return major * 100 + minor * 10 + patch;
}

GreetingDialog::GreetingDialog(int){

    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if(curl) {
        struct curl_slist* headers = NULL;

        // Add User-Agent header
        headers = curl_slist_append(headers, "User-Agent: FloralPen");

        curl_easy_setopt(curl, CURLOPT_URL, "https://api.github.com/repos/mert-tetik/LigidPainter/releases/latest");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        
        res = curl_easy_perform(curl);

        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

        curl_slist_free_all(headers); // Free the header list
        curl_easy_cleanup(curl);
    }

    if(extractVersionNumber(readBuffer, "tag_name") > currentVersion){
        newVersionMode = true;
    }

    curl_global_cleanup();

    
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

void GreetingDialog::show(Timer& timer){

    dialogControl.updateStart(false);

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
        dialog_newProject.dialogControl.activate();
    }

    //Show load project dialog if load project button is pressed
    if(loadProjectButton.clicked){
        this->dialogControl.unActivate();
        dialog_loadProject.dialogControl.activate();
    }

    //Render the decoration texture displayer button without depth testing
    glDepthFunc(GL_ALWAYS);
    
    //Set the depth func back to default (less or equal)
    glDepthFunc(GL_LEQUAL);

    dialogControl.updateEnd(timer, 2.0f);
}