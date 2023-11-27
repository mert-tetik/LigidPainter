
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

static std::vector<LibraryAction> actions_Library;
static std::vector<PaintingAction> actions_Painting;
static std::vector<VectorsAction> actions_Vectors;
static std::vector<ObjectSelectionAction> actions_ObjectSelection;
static std::vector<FaceSelectionAction> actions_FaceSelection;
static std::vector<TextureFieldsAction> actions_TextureFields;
static std::vector<MaterialEditorAction> actions_MaterialEditor;

namespace LGDLOG{
    LogMsg start;
    std::string end = "$#";
}

struct CatMSG{
    std::string text;
    Texture face;

    CatMSG(){}
    CatMSG(std::string text, Texture face){
        this->text = text;
        this->face = face;
    }
};

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

CatMSG pickText(Timer &timer ,std::vector<CatMSG> texts){
    for (size_t i = 0; i < texts.size(); i++)
    {
        if(timer.seconds % texts.size() == i)
            return texts[i];
    }
    return CatMSG("", Texture());
}

static size_t lastMessagesSize = 0;

static std::string quitMSG = "";
static std::string catMSG = "";
static float petPoints = 0.f; 
static bool sleepingCat = false;
static size_t sleepingCounter = 0;
static Texture msgFace;
static int flipCount = 0;
static int dizzyCounter = 0;

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

    if(Mouse::mouseOffset()->x || Mouse::mouseOffset()->y)
        sleepingCounter = timer.seconds;

    sleepingCat = timer.seconds - sleepingCounter > 5; 

    rendering(
                this->messagesPanel, this->historyPanel, this->logBtn, this->logBtnR, this->logBtnL, this->messageInfoBtn, this->yesBtn, this->noBtn,
                this->messageInfoBtnMixVal, this->messageInfoActive, this->pos, this->messagesPanelXAxisMixVal, this->messagesPanelYAxisMixVal, 
                this->historyPanelXAxisMixVal, this->historyPanelYAxisMixVal, this->messagesActive, this->actionHistoryActive, this->dialogControl, 
                timer, painter, sleepingCat, msgFace, dizzyCounter, this->cryCounter
            );

    if(this->logBtn.clicked){
        flipCount++;
    }

    if(flipCount >= 4){
        dizzyCounter = 3;
    }

    if(timer.tick){
        if(dizzyCounter)
            dizzyCounter--;
        if(cryCounter)
            cryCounter--;
        flipCount = 0;
    }

    if(this->logBtn.hover)
        petPoints += std::abs(Mouse::mouseOffset()->x) + std::abs(Mouse::mouseOffset()->y);
    else
        petPoints = 0.f;

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
        if(Settings::properties()->cat_verifyTheExit){
            messageInfoActive = true;
            if(quitMSG == ""){
                messageInfoBtnStartTime = timer.seconds;
                CatMSG msg = pickText(timer, {
                                            CatMSG("Are you sure you want to exit the LigidPainter?", Settings::appTextures().mascotCat_rock),
                                            CatMSG("Do you really want to exit the LigidPainter?", Settings::appTextures().mascotCat_rock),
                                            CatMSG("Are you DETERMINED to close the LigidPainter???", Settings::appTextures().mascotCat_rock),
                                            CatMSG("Are you done with the LigidPainter?  ", Settings::appTextures().mascotCat_rock),
                                            CatMSG("Do you really want to leave me???  ", Settings::appTextures().mascotCat_crying),
                                            CatMSG("Closing the LigidPainter already??  ", Settings::appTextures().mascotCat_thinking),
                                            CatMSG("Are you absolutely, positively sure you want to close the app", Settings::appTextures().mascotCat_rock),
                                            CatMSG("One last check: ready to exit and let the LigidPainter nap?", Settings::appTextures().mascotCat_rock),
                                            CatMSG("Leaving so soon?   ", Settings::appTextures().mascotCat_crying),
                                        });

                quitMSG = msg.text;
                msgFace = msg.face;
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
        else{
            this->windowShouldClose = true;
        }
    }
    else if(Settings::properties()->cat_allowComments){
        if(petPoints > 300.f){
            petPoints = 0.f;
            messageInfoActive = true;
            messageInfoBtnStartTime = timer.seconds;
            if(catMSG == ""){
                CatMSG msg = pickText(timer, {
                                            CatMSG("Mrrrr :3   ", Settings::appTextures().mascotCat_bread),
                                            CatMSG("Purrr :3   ", Settings::appTextures().mascotCat_relaxed),
                                            CatMSG("Purrr <3   ", Settings::appTextures().mascotCat_bread),
                                            CatMSG("Meow :3   ", Settings::appTextures().mascotCat_relaxed),
                                        });

                catMSG = msg.text;
                msgFace = msg.face;
            }
            
            messageInfoBtn.text = catMSG;
        }
        else if(painter.refreshable){
            messageInfoActive = true;
            messageInfoBtnStartTime = timer.seconds;
            if(catMSG == ""){
                CatMSG msg = pickText(timer, {
                                            CatMSG("Starting off with style! Keep those colors flowing!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("Looking good! Don't stop now!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("Purr-fect progress! You're on a roll!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("Wow, that's coming together nicely! Keep it up!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("Impressive strokes! This is shaping up beautifully!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("Meow-velous! You're almost there!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("Simply purr-fect! Your art is amazing!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("Absolutely clawsome! Can't wait to see the final touches!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("You're a true artist! Keep painting!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("Looking meow-tastic! Keep those creative juices flowing!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("Purr-fact strokes!", Settings::appTextures().mascotCat_smile),
                                        });

                catMSG = msg.text;
                msgFace = msg.face;
            }
            messageInfoBtn.text = catMSG;
        }
        else if(settingsDialog.dialogControl.globalFirstFrameActivated){
            messageInfoActive = true;
            messageInfoBtnStartTime = timer.seconds;
            if(catMSG == ""){
                CatMSG msg = pickText(timer, {
                                            CatMSG("Yikes! So technical", Settings::appTextures().mascotCat_dizzy),
                                            CatMSG("Gears!! shiver me timbers", Settings::appTextures().mascotCat_dizzy),
                                            CatMSG("Here we come gears!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("Don't be scared! It's just a few buttons and switches.", Settings::appTextures().mascotCat_thinking),
                                            CatMSG("Let's see what we can get into with these settings!!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("Time to tinker with the cat-figurations!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("Paws and reflect on these settings!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("Meow-stering the art of customization!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("Adjusting settings? Piece of catnip cake!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("Ready to purr-sonalize your experience?", Settings::appTextures().mascotCat_smile),
                                            CatMSG("DON'T YOU DARE TURN ME OFF FROM THERE!!! >:3", Settings::appTextures().mascotCat_rock),
                                            CatMSG("You're not gonna turn me off are u??", Settings::appTextures().mascotCat_thinking),
                                            CatMSG("Pweasee don't turn me off. It's so dark here :3", Settings::appTextures().mascotCat_crying),
                                        });

                catMSG = msg.text;
                msgFace = msg.face;
            }
            messageInfoBtn.text = catMSG;
        }
        else if(bakingDialog.dialogControl.globalFirstFrameActivated){
            messageInfoActive = true;
            messageInfoBtnStartTime = timer.seconds;
            if(catMSG == ""){
                CatMSG msg = pickText(timer, {
                                            CatMSG("Bakey bakey!", Settings::appTextures().mascotCat_bread),
                                            CatMSG("Let's bake those textures!", Settings::appTextures().mascotCat_bread),
                                            CatMSG("Mixing pixels together for a mouthwatering result!", Settings::appTextures().mascotCat_bread),
                                            CatMSG("Turning up the heat to cook those textures to perfection!", Settings::appTextures().mascotCat_bread),
                                            CatMSG("Baking textures - the secret ingredient for a purr-fect render!", Settings::appTextures().mascotCat_bread),
                                            CatMSG("Just like baking cookies, but for your 3D masterpiece!", Settings::appTextures().mascotCat_bread),
                                            CatMSG("Baking those pixels like a baker layers cake!", Settings::appTextures().mascotCat_bread),
                                            CatMSG("Adding some flavor to your project with freshly baked textures!", Settings::appTextures().mascotCat_bread),
                                            CatMSG("Creating a baking recipe that'll make your project scrumptious!", Settings::appTextures().mascotCat_bread),
                                            CatMSG("Baking time!", Settings::appTextures().mascotCat_bread),
                                            CatMSG("The oven's preheated!", Settings::appTextures().mascotCat_bread),
                                        });

                catMSG = msg.text;
                msgFace = msg.face;
            }
            messageInfoBtn.text = catMSG;
        }
        else if(displayerDialog.dialogControl.globalFirstFrameActivated){
            messageInfoActive = true;
            messageInfoBtnStartTime = timer.seconds;
            if(catMSG == ""){
                CatMSG msg = pickText(timer, {
                                            CatMSG("Let's tailor that purr-fect skybox.", Settings::appTextures().mascotCat_smile),
                                            CatMSG("How about a sunrise, or perhaps a cozy sunset?", Settings::appTextures().mascotCat_relaxed),
                                            CatMSG("Adjust the sky to match my mood :3", Settings::appTextures().mascotCat_relaxed),
                                            CatMSG("Pawsitively purr-fect skies!!", Settings::appTextures().mascotCat_relaxed),
                                            CatMSG("Sunrise or sunset? Let's make your skybox a masterpiece of the heavens!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("Ahh the soft, pure sky <3", Settings::appTextures().mascotCat_relaxed),
                                            CatMSG("OoOW!! Did you get bored of ur background??", Settings::appTextures().mascotCat_smile),
                                        });

                catMSG = msg.text;
                msgFace = msg.face;
            }
            messageInfoBtn.text = catMSG;
        }
        else if(exportDialog.dialogControl.globalFirstFrameActivated){
            messageInfoActive = true;
            messageInfoBtnStartTime = timer.seconds;
            if(catMSG == ""){
                CatMSG msg = pickText(timer, {
                                            CatMSG("You can access your textures via file explorer! No need to export!!", Settings::appTextures().mascotCat_thinking),
                                            CatMSG("Time to package these textures!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("Exporting textures with a sprinkle of whisker-dust!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("Preparing your textures for a journey to your computer's catnip stash!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("Sending your textures home!", Settings::appTextures().mascotCat_smile),
                                        });

                catMSG = msg.text;
                msgFace = msg.face;
            }
            messageInfoBtn.text = catMSG;
        }
        else if(newTextureDialog.dialogControl.globalFirstFrameActivated){
            messageInfoActive = true;
            messageInfoBtnStartTime = timer.seconds;
            if(catMSG == ""){
                CatMSG msg = pickText(timer, {
                                            CatMSG("Brand-new textures!!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("New textures on the horizon! Let's unleash the artist within!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("Paws to new textures!!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("Don't forget to name your texture!!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("Pick the best color possible :3", Settings::appTextures().mascotCat_relaxed),
                                        });

                catMSG = msg.text;
                msgFace = msg.face;
            }
            messageInfoBtn.text = catMSG;
        }
        else if(newProjectDialog.dialogControl.globalFirstFrameActivated){
            messageInfoActive = true;
            messageInfoBtnStartTime = timer.seconds;
            if(catMSG == ""){
                CatMSG msg = pickText(timer, {
                                            CatMSG("New project on the horizon! Let's unleash the artist within!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("Don't forget to name your project!!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("New project new adventure!!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("A project is a folder that held inside of a folder. Meownd-blowing!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("Time to embark on a new project, whiskers ready!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("Starting a new project, let's paw-tner up for some creative fun!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("Crafting a new project with the finesse and curiosity of a cat!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("I had a friend named 'project' once. Hope he is doing well now :3", Settings::appTextures().mascotCat_thinking),
                                        });

                catMSG = msg.text;
                msgFace = msg.face;
            }
            messageInfoBtn.text = catMSG;
        }
        else if(textureEditorDialog.dialogControl.globalFirstFrameActivated){
            messageInfoActive = true;
            messageInfoBtnStartTime = timer.seconds;
            if(catMSG == ""){
                CatMSG msg = pickText(timer, {
                                            CatMSG("Time to give those pixels a cat-tastic makeover!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("Meow-difying images!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("Pawsitively purr-fect editing!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("Whisking images into shape!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("Catnip for your images! Let's create something paw-some :3", Settings::appTextures().mascotCat_smile),
                                            CatMSG("Tweaking pixels with whisker-precision!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("Meow-gical image transformations!", Settings::appTextures().mascotCat_smile),
                                            CatMSG("Crafting pixel into purr-fection!", Settings::appTextures().mascotCat_smile)
                                        });

                catMSG = msg.text;
                msgFace = msg.face;
            }
            messageInfoBtn.text = catMSG;
        }
    }

    if(lastMessagesSize != messages.size() && !getContext()->window.shouldClose()){
        messageInfoActive = true;
        messageInfoBtnStartTime = timer.seconds;
        if(messages.size()){
            messageInfoBtn.text = messages[messages.size()-1];
            msgFace = Settings::appTextures().mascotCat_rock;
        }
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

    //if()
    //    this->activeHistoryMode = HISTORY_LIBRARY_MODE;
    if(painter.selectedDisplayingModeIndex != 0)
        this->activeHistoryMode = HISTORY_PAINTING_MODE;
    if(painter.selectedPaintingModeIndex == 5)
        this->activeHistoryMode = HISTORY_VECTORS_MODE;
    if(painter.selectedDisplayingModeIndex == 0)
        this->activeHistoryMode = HISTORY_OBJECTSELECTION_MODE;
    if(painter.faceSelection.editMode)
        this->activeHistoryMode = HISTORY_FACESELECTION_MODE;
    if(painter.paintingoverTextureEditorMode)
        this->activeHistoryMode = HISTORY_TEXTUREFIELDS_MODE;

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
        

        if(this->activeHistoryMode == HISTORY_LIBRARY_MODE){
            for (size_t i = 0; i < actions_Library.size(); i++)
            {
                logSections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(1), actions_Library[i].title, Texture(), 0., false));
            }
        }
        if(this->activeHistoryMode == HISTORY_PAINTING_MODE){
            for (size_t i = 0; i < actions_Painting.size(); i++)
            {
                logSections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(1), actions_Painting[i].title, Texture(), 0., false));
            }
        }
        if(this->activeHistoryMode == HISTORY_VECTORS_MODE){
            for (size_t i = 0; i < actions_Vectors.size(); i++)
            {
                logSections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(1), actions_Vectors[i].title, Texture(), 0., false));
            }
        }
        if(this->activeHistoryMode == HISTORY_OBJECTSELECTION_MODE){
            for (size_t i = 0; i < actions_ObjectSelection.size(); i++)
            {
                logSections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(1), actions_ObjectSelection[i].title, Texture(), 0., false));
            }
        }
        if(this->activeHistoryMode == HISTORY_FACESELECTION_MODE){
            for (size_t i = 0; i < actions_FaceSelection.size(); i++)
            {
                logSections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(1), actions_FaceSelection[i].title, Texture(), 0., false));
            }
        }
        if(this->activeHistoryMode == HISTORY_TEXTUREFIELDS_MODE){
            for (size_t i = 0; i < actions_TextureFields.size(); i++)
            {
                logSections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(1), actions_TextureFields[i].title, Texture(), 0., false));
            }
        }
        if(this->activeHistoryMode == HISTORY_MATERIALEDITOR_MODE){
            for (size_t i = 0; i < actions_MaterialEditor.size(); i++)
            {
                logSections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(1), actions_MaterialEditor[i].title, Texture(), 0., false));
            }
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
        undo(painter);
    }

}

bool LogDialog::isHovered(){
    return this->messagesPanel.hover || this->historyPanel.hover || this->logBtn.hover || this->logBtnL.hover || this->logBtnR.hover;
}