
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 Copyright (c) 2022-2023, Mert Tetik

 All rights reserved.

 Official GitHub Link : https:github.com/mert-tetik/LigidPainter
 Official Web Page : https:ligidtools.com/ligidpainter

 ---------------------------------------------------------------------------
 
    TODO USE ALL THE PROJECT SETTINGS WHILE CREATING A PROJECT 

 */

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "3D/ThreeD.hpp" 
#include "GUI/GUI.hpp" 
#include "MouseSystem/Mouse.hpp" 
#include "LibrarySystem/Library.hpp" 
#include "NodeSystem/Node/Node.hpp" 
#include "SettingsSystem/Settings.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp" 

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

#include "GUI/Dialogs/LogDialog/Registering.hpp"
#include "GUI/Dialogs/LogDialog/Rendering.hpp"
#include "GUI/Dialogs/LogDialog/Undo.hpp"

std::vector<Action> __actions;


namespace LGDLOG{
    LogMsg start;
    std::string end = "$#";
}

LogDialog::LogDialog(){

}

LogDialog::LogDialog(AppMaterialModifiers& appMaterialModifiers){
    this->messagesPanel = Panel(
        {
            {
                Section(
                    Element(Button()),
                    {

                    }
                )
            }
        },
        glm::vec2(10.f,16.f),
        glm::vec3(50.f,50.f,0.8f),
        ColorPalette::mainColor,
        ColorPalette::thirdColor,
        true,
        true,
        true,
        true,
        true,
        1.f,
        1.f,
        {},
        0.25f,
        false
    );
    this->messagesPanel.hasSlider = true;

    this->historyPanel = this->messagesPanel;
    
    this->messagesPanel.color = glm::vec4(1.f);
    this->historyPanel.color = glm::vec4(glm::vec3(ColorPalette::themeColor), 0.8f);

    this->logBtn = Button(ELEMENT_STYLE_SOLID, glm::vec2(2.f), "", Settings::appTextures().mascotCat_default, 0., false);
    this->logBtn.color = glm::vec4(0);
    this->logBtn.color2 = glm::vec4(0);
    this->logBtn.outlineColor = glm::vec4(0);
    this->logBtn.outlineColor2 = glm::vec4(0);
    this->logBtn.pos.z = 0.9f;
    
    this->logBtnL = Button(ELEMENT_STYLE_SOLID, glm::vec2(1.5f), "", Settings::appTextures().mascotCat_pawL, 0., false);
    this->logBtnL.color = glm::vec4(0);
    this->logBtnL.color2 = glm::vec4(0);
    this->logBtnL.outlineColor = glm::vec4(0);
    this->logBtnL.outlineColor2 = glm::vec4(0);
    this->logBtnL.pos.z = 0.9f;
    
    this->logBtnR = Button(ELEMENT_STYLE_SOLID, glm::vec2(1.5f), "", Settings::appTextures().mascotCat_pawR, 0., false);
    this->logBtnR.color = glm::vec4(0);
    this->logBtnR.color2 = glm::vec4(0);
    this->logBtnR.outlineColor = glm::vec4(0);
    this->logBtnR.outlineColor2 = glm::vec4(0);
    this->logBtnR.pos.z = 0.9f;
    
    this->messageInfoBtn = Button(ELEMENT_STYLE_SOLID, glm::vec2(1.5f), "", Texture(), 0., false);
    this->messageInfoBtn.pos.z = 0.9f;
    
    this->yesBtn = Button(ELEMENT_STYLE_STYLIZED, glm::vec2(1.5f), "Yes", Texture(), 0., false);
    this->noBtn = Button(ELEMENT_STYLE_STYLIZED, glm::vec2(1.5f), "No", Texture(), 0., false);

}

std::string pickText(Timer &timer ,std::vector<std::string> texts){
    for (size_t i = 0; i < texts.size(); i++)
    {
        if(timer.seconds % texts.size() == i)
            return texts[i];
    }
    return "";
}

static size_t lastMessagesSize = 0;

static std::string quitMSG = "";
static std::string catMSG = "";
static float petPoints = 0.f; 

void LogDialog::render(
                            Timer timer, 
                            Painter& painter, 
                            GreetingDialog& greetingDialog, 
                            NewProjectDialog& newProjectDialog, 
                            ExportDialog& exportDialog,
                            MaterialDisplayerDialog& materialDisplayerDialog,
                            FilterDisplayerDialog& filterDisplayerDialog,
                            LoadProjectDialog& loadProjectDialog,
                            MaterialEditorDialog& materialEditorDialog,
                            TextureSelectionDialog& textureSelectionDialog,
                            BakingDialog& bakingDialog,
                            FilterSelectionDialog& filterSelectionDialog,
                            NewTextureDialog& newTextureDialog,
                            PaintingChannelsAutoCreateTexturesDialog& paintingChannelsAutoCreateTexturesDialog,
                            SettingsDialog& settingsDialog,
                            DisplayerDialog& displayerDialog,
                            TextureEditorDialog& textureEditorDialog,
                            TexturePackEditorDialog& texturePackEditorDialog,
                            ProjectRecoverDialog& projectRecoverDialog,
                            ObjectTexturingDialog& objectTexturingDialog
                        )
{

    glClear(GL_DEPTH_BUFFER_BIT);

    ShaderSystem::buttonShader().use();

    rendering(
                this->messagesPanel, this->historyPanel, this->logBtn, this->logBtnR, this->logBtnL, this->messageInfoBtn, this->yesBtn, this->noBtn,
                this->messageInfoBtnMixVal, this->messageInfoActive, this->pos, this->messagesPanelXAxisMixVal, this->messagesPanelYAxisMixVal, 
                this->historyPanelXAxisMixVal, this->historyPanelYAxisMixVal, this->messagesActive, this->actionHistoryActive, this->dialogControl, 
                timer, painter
            );

    if(this->logBtn.hover)
        petPoints += std::abs(Mouse::mouseOffset()->x) + std::abs(Mouse::mouseOffset()->y);
    else
        petPoints = 0.f;

    //if(!std::abs(Mouse::mouseOffset()->x) && !std::abs(Mouse::mouseOffset()->y))
    //    petPoints = 0.f;

    std::vector<std::string> messages;

    std::string a;
    LGDLOG::start >> a;

    for (size_t i = 0; i < a.size(); i++)
    {
        if(i < a.size()-1){
            if(a[i] == '$' && a[i+1] == '#'){
                std::string res(a.begin(), a.begin() + i );

                a.erase(a.begin(), a.begin() + i + 2);
                i = 0;
                messages.push_back(res);
            }
        }
    }

    // --------- INFO MESSAGE -----------
    if(getContext()->window.shouldClose()){
        messageInfoActive = true;
        if(quitMSG == ""){
            messageInfoBtnStartTime = timer.seconds;
            quitMSG = pickText(timer, {
                                        "Are you sure you want to exit the LigidPainter?",
                                        "Do you really want to exit the LigidPainter?",
                                        "Are you DETERMINED to close the LigidPainter???",
                                        "Are you done with the LigidPainter?",
                                        "Do you really want to leave me???",
                                        "Closing the LigidPainter already??",
                                        "Are you absolutely, positively sure you want to close the app",
                                        "One last check: ready to exit and let the LigidPainter nap?"
                                    });
        }

        messageInfoBtn.text = quitMSG;

        if(noBtn.clicked){
            messageInfoActive = false;
            quitMSG = "";
            getContext()->window.setShouldClose(false);
        }
        if(yesBtn.clicked){
            this->windowShouldClose = true;
        }
    }
    else if(petPoints > 100.f){
        petPoints = 0.f;
        messageInfoActive = true;
        messageInfoBtnStartTime = timer.seconds;
        if(catMSG == ""){
            catMSG = pickText(timer, {
                                        "Mrrrr :3   ",
                                        "Purrr :3   ",
                                        "Purrr <3   ",
                                        "Meow :3   "
                                    });
        }
        
        messageInfoBtn.text = catMSG;
    }
    else if(painter.refreshable){
        messageInfoActive = true;
        messageInfoBtnStartTime = timer.seconds;
        if(catMSG == ""){
            catMSG = pickText(timer, {
                                        "Starting off with style! Keep those colors flowing!",
                                        "Looking good! Don't stop now!",
                                        "Purr-fect progress! You're on a roll!",
                                        "Wow, that's coming together nicely! Keep it up!",
                                        "Impressive strokes! This is shaping up beautifully!",
                                        "Meow-velous! You're almost there!",
                                        "Simply purr-fect! Your art is amazing!",
                                        "Absolutely clawsome! Can't wait to see the final touches!",
                                        "You're a true artist! Keep painting!",
                                        "Looking meow-tastic! Keep those creative juices flowing!",
                                        "Purr-fact strokes!"
                                    });
        }
        messageInfoBtn.text = catMSG;
    }
    else if(settingsDialog.dialogControl.globalFirstFrameActivated){
        messageInfoActive = true;
        messageInfoBtnStartTime = timer.seconds;
        if(catMSG == ""){
            catMSG = pickText(timer, {
                                        "Yikes! So technical",
                                        "Gears!! shiver me timbers",
                                        "Here we come gears!",
                                        "Don't be scared! It's just a few buttons and switches.",
                                        "Let's see what we can get into with these settings!!",
                                        "Time to tinker with the cat-figurations!",
                                        "Paws and reflect on these settings!",
                                        "Meow-stering the art of customization!",
                                        "Adjusting settings? Piece of catnip cake!",
                                        "Ready to purr-sonalize your experience?",
                                        "DON'T YOU DARE TURN ME OFF FROM THERE!!! >:3",
                                        "You're not gonna turn me off are u??",
                                        "Pweasee don't turn me off. It's so dark here :3"
                                    });
        }
        messageInfoBtn.text = catMSG;
    }
    else if(bakingDialog.dialogControl.globalFirstFrameActivated){
        messageInfoActive = true;
        messageInfoBtnStartTime = timer.seconds;
        if(catMSG == ""){
            catMSG = pickText(timer, {
                                        "Bakey bakey!",
                                        "Let's bake those textures!",
                                        "Mixing pixels together for a mouthwatering result!",
                                        "Turning up the heat to cook those textures to perfection!",
                                        "Baking textures - the secret ingredient for a purr-fect render!",
                                        "Just like baking cookies, but for your 3D masterpiece!",
                                        "Baking those pixels like a baker layers cake!",
                                        "Adding some flavor to your project with freshly baked textures!",
                                        "Creating a baking recipe that'll make your project scrumptious!",
                                        "Baking time!",
                                        "The oven's preheated!"
                                    });
        }
        messageInfoBtn.text = catMSG;
    }
    else if(displayerDialog.dialogControl.globalFirstFrameActivated){
        messageInfoActive = true;
        messageInfoBtnStartTime = timer.seconds;
        if(catMSG == ""){
            catMSG = pickText(timer, {
                                        "Let's tailor that purr-fect skybox.",
                                        "How about a sunrise, or perhaps a cozy sunset?",
                                        "Adjust the sky to match my mood :3",
                                        "Pawsitively purr-fect skies!!",
                                        "Sunrise or sunset? Let's make your skybox a masterpiece of the heavens!",
                                        "Ahh the soft, pure sky <3",
                                        "OoOW!! Did you get bored of ur background??"
                                    });
        }
        messageInfoBtn.text = catMSG;
    }
    else if(exportDialog.dialogControl.globalFirstFrameActivated){
        messageInfoActive = true;
        messageInfoBtnStartTime = timer.seconds;
        if(catMSG == ""){
            catMSG = pickText(timer, {
                                        "You can access your textures via file explorer! No need to export!!",
                                        "Time to package these textures!",
                                        "Exporting textures with a sprinkle of whisker-dust!",
                                        "Preparing your textures for a journey to your computer's catnip stash!",
                                        "Sending your textures home!",
                                    });
        }
        messageInfoBtn.text = catMSG;
    }
    else if(newTextureDialog.dialogControl.globalFirstFrameActivated){
        messageInfoActive = true;
        messageInfoBtnStartTime = timer.seconds;
        if(catMSG == ""){
            catMSG = pickText(timer, {
                                        "Brand-new textures!!",
                                        "New textures on the horizon! Let's unleash the artist within!",
                                        "Paws to new textures!!",
                                        "Don't forget to name your texture!!",
                                        "Pick the best color possible :3"
                                    });
        }
        messageInfoBtn.text = catMSG;
    }
    else if(newProjectDialog.dialogControl.globalFirstFrameActivated){
        messageInfoActive = true;
        messageInfoBtnStartTime = timer.seconds;
        if(catMSG == ""){
            catMSG = pickText(timer, {
                                        "New project on the horizon! Let's unleash the artist within!",
                                        "Don't forget to name your project!!",
                                        "New project new adventure!!",
                                        "A project is a folder that held inside of a folder. Meownd-blowing!",
                                        "Time to embark on a new project, whiskers ready!",
                                        "Starting a new project, let's paw-tner up for some creative fun!",
                                        "Crafting a new project with the finesse and curiosity of a cat!",
                                        "I had a friend named 'project' once. Hope he is doing well now :3"
                                    });
        }
        messageInfoBtn.text = catMSG;
    }
    else if(textureEditorDialog.dialogControl.globalFirstFrameActivated){
        messageInfoActive = true;
        messageInfoBtnStartTime = timer.seconds;
        if(catMSG == ""){
            catMSG = pickText(timer, {
                                        "Time to give those pixels a cat-tastic makeover!",
                                        "Meow-difying images!",
                                        "Pawsitively purr-fect editing!",
                                        "Whisking images into shape!",
                                        "Catnip for your images! Let's create something paw-some :3",
                                        "Tweaking pixels with whisker-precision!",
                                        "Meow-gical image transformations!",
                                        "Crafting pixel into purr-fection!"
                                    });
        }
        messageInfoBtn.text = catMSG;
    }

    else if(lastMessagesSize != messages.size()){
        messageInfoActive = true;
        messageInfoBtnStartTime = timer.seconds;
        if(messages.size())
            messageInfoBtn.text = messages[messages.size()-1];
    }

    // --------- INFO CLOSE -----------
    if(!getContext()->window.shouldClose()){
        if(timer.seconds - messageInfoBtnStartTime >= 3){
            catMSG = "";
            messageInfoActive = false;
        }
    }
    else{
        if(timer.seconds - messageInfoBtnStartTime >= 10){
            messageInfoActive = false;
            quitMSG = "";
            getContext()->window.setShouldClose(false);
        }
    }

    timer.transition(messageInfoActive, messageInfoBtnMixVal, 0.2f);

    lastMessagesSize = messages.size();


    if(messagesActive){

        std::vector<Section> logSections;
        logSections.push_back(Section(
                                        Element(),
                                        {
                                            
                                        }
                                    )
                            );
        for (size_t i = 0; i < messages.size(); i++)
        {
            logSections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(1), messages[i], Texture(), 0., false));
        }
        
        this->messagesPanel.sections = logSections;
    }
    else if(actionHistoryActive){
        std::vector<Section> logSections;
        logSections.push_back(Section(
                                        Element(),
                                        {
                                            
                                        }
                                    )
                            );
        
        for (size_t i = 0; i < __actions.size(); i++)
        {
            logSections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(1), __actions[i].title, Texture(), 0., false));
        }

        if(this->historyPanel.sections[0].elements.size()){
            if(this->historyPanel.sections[0].elements[this->historyPanel.sections[0].elements.size()-1].button.text == "Undo / CTRL+Z"){
                logSections[0].elements.push_back(this->historyPanel.sections[0].elements[this->historyPanel.sections[0].elements.size()-1].button);
            }
            else
                logSections[0].elements.push_back(Button(ELEMENT_STYLE_STYLIZED, glm::vec2(1), "Undo / CTRL+Z", Texture(), 0., false));
        }
        else
            logSections[0].elements.push_back(Button(ELEMENT_STYLE_STYLIZED, glm::vec2(1), "Undo / CTRL+Z", Texture(), 0., false));
        
        this->historyPanel.sections = logSections;
    }

    if(
            actionHistoryActive && this->historyPanel.sections.size() && this->historyPanel.sections[0].elements.size() && this->historyPanel.sections[0].elements[this->historyPanel.sections[0].elements.size() - 1].button.clicked  ||
            Shortcuts::CTRL_Z()
        )
    {
        undo();
    }

}

bool LogDialog::isHovered(){
    return this->messagesPanel.hover || this->historyPanel.hover || this->logBtn.hover || this->logBtnL.hover || this->logBtnR.hover;
}