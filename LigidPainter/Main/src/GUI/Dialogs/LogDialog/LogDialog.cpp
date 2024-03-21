
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 (c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
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

#include "UTIL/Mouse/Mouse.hpp" 
#include "UTIL/Library/Library.hpp" 
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp" 
#include "UTIL/Project/Project.hpp" 

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

#include "GUI/Dialogs/LogDialog/Registering.hpp"
#include "GUI/Dialogs/LogDialog/Rendering.hpp"
#include "GUI/Dialogs/LogDialog/Undo.hpp"

static std::vector<LibraryAction> actions_Library;
static std::map<unsigned int, std::vector<PaintingAction>> actions_Painting;
static std::vector<PaintingAction> actions_MultiChannelPainting;
static std::vector<VectorsAction> actions_Vectors;
static std::vector<ObjectSelectionAction> actions_ObjectSelection;
static std::vector<FaceSelectionAction> actions_FaceSelection;
static std::vector<TextureFieldsAction> actions_TextureFields;
static std::vector<MaterialEditorAction> actions_MaterialEditor;

#define MAX_MATERIAL_HISTORY 8
#define MAX_PAINTING_HISTORY 25

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

LogDialog::LogDialog(int){
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
    
    this->logBtn = Button(ELEMENT_STYLE_SOLID, glm::vec2(2.f), "", appTextures.mascotCat_default, 0., false);
    this->logBtn.color = glm::vec4(0);
    this->logBtn.color2 = glm::vec4(0);
    this->logBtn.outlineColor = glm::vec4(0);
    this->logBtn.outlineColor2 = glm::vec4(0);
    this->logBtn.pos.z = 0.9f;
    
    this->logBtnL = Button(ELEMENT_STYLE_SOLID, glm::vec2(1.5f), "", appTextures.mascotCat_pawL, 0., false);
    this->logBtnL.color = glm::vec4(0);
    this->logBtnL.color2 = glm::vec4(0);
    this->logBtnL.outlineColor = glm::vec4(0);
    this->logBtnL.outlineColor2 = glm::vec4(0);
    this->logBtnL.pos.z = 0.9f;
    
    this->logBtnR = Button(ELEMENT_STYLE_SOLID, glm::vec2(1.5f), "", appTextures.mascotCat_pawR, 0., false);
    this->logBtnR.color = glm::vec4(0);
    this->logBtnR.color2 = glm::vec4(0);
    this->logBtnR.outlineColor = glm::vec4(0);
    this->logBtnR.outlineColor2 = glm::vec4(0);
    this->logBtnR.pos.z = 0.9f;
    
    this->messageInfoBtn = Button(ELEMENT_STYLE_SOLID, glm::vec2(1.5f), "", Texture(), 0., false);
    this->messageInfoBtn.pos.z = 0.9f;
    
    this->yesBtn = Button(ELEMENT_STYLE_STYLIZED, glm::vec2(1.5f), "Yes", Texture(), 0., false);
    this->noBtn = Button(ELEMENT_STYLE_STYLIZED, glm::vec2(1.5f), "No", Texture(), 0., false);
    
    this->libraryHistoryBtn = Button(ELEMENT_STYLE_BASIC, glm::vec2(1.5f), "Library History", Texture(), 0., true);
    this->libraryHistoryBtn.textColor2 = ColorPalette::oppositeColor;
    this->otherHistoryBtn = Button(ELEMENT_STYLE_BASIC, glm::vec2(1.5f), "Unknown History", Texture(), 0., true);
    this->otherHistoryBtn.textColor2 = ColorPalette::oppositeColor;

    this->libraryHistoryBtn.clickState1 = true;
}

CatMSG pickText(Timer &timer ,std::vector<CatMSG> texts){
    for (size_t i = 0; i < texts.size(); i++)
    {
        if(timer.seconds % texts.size() == i)
            return texts[i];
    }
    return CatMSG("", Texture());
}

bool newLibraryAction = false;
bool newOtherAction = false;

static size_t lastMessagesSize = 0;

static std::string quitMSG = "";
static std::string catMSG = "";
static float petPoints = 0.f; 
static bool sleepingCat = false;
static size_t sleepingCounter = 0;
static Texture msgFace;
static int flipCount = 0;
static int dizzyCounter = 0;

static int prevpPainterDisplayIndex = 0;
static unsigned int prevpPainterSelectedTxtr = 0;
static std::string lastProjectPath;

void LogDialog::render(
                            Timer timer, 
                            Painter& painter
                        )
{
    this->unded = false;

    glClear(GL_DEPTH_BUFFER_BIT);

    ShaderSystem::buttonShader().use();

    if(Mouse::mouseOffset()->x || Mouse::mouseOffset()->y)
        sleepingCounter = timer.seconds;

    sleepingCat = timer.seconds - sleepingCounter > 5; 

    rendering(
                this->messagesPanel, this->historyPanel, this->logBtn, this->logBtnR, this->logBtnL, this->messageInfoBtn, this->yesBtn, this->noBtn,
                this->messageInfoBtnMixVal, this->messageInfoActive, this->pos, this->messagesPanelXAxisMixVal, this->messagesPanelYAxisMixVal, 
                this->historyPanelXAxisMixVal, this->historyPanelYAxisMixVal, this->messagesActive, this->actionHistoryActive, this->dialogControl, 
                timer, painter, sleepingCat, msgFace, dizzyCounter, this->cryCounter, this->libraryHistoryBtn, this->otherHistoryBtn
            );


    if(libraryHistoryBtn.clickState1 && !libraryHistoryMode){
        libraryHistoryMode = true;
        otherHistoryBtn.clickState1 = false;
    }
    else if(otherHistoryBtn.clickState1 && libraryHistoryMode){
        libraryHistoryMode = false;
        libraryHistoryBtn.clickState1 = false;
    }
    
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
                                            CatMSG("Are you sure you want to exit the LigidPainter?", appTextures.mascotCat_rock),
                                            CatMSG("Do you really want to exit the LigidPainter?", appTextures.mascotCat_rock),
                                            CatMSG("Are you DETERMINED to close the LigidPainter???", appTextures.mascotCat_rock),
                                            CatMSG("Are you done with the LigidPainter?  ", appTextures.mascotCat_rock),
                                            CatMSG("Do you REALLY want to leave me???  ", appTextures.mascotCat_crying),
                                            CatMSG("Closing the LigidPainter already??  ", appTextures.mascotCat_thinking),
                                            CatMSG("Are you absolutely, positively sure you want to close the app", appTextures.mascotCat_rock),
                                            CatMSG("One last check: ready to exit and let the LigidPainter nap?", appTextures.mascotCat_rock),
                                            CatMSG("Leaving so soon?   ", appTextures.mascotCat_crying),
                                        });

                quitMSG = msg.text;
                msgFace = msg.face;
            }

            messageInfoBtn.text = quitMSG;

            if(noBtn.clicked || getContext()->window.isKeyClicked(LIGIDGL_KEY_ESCAPE)){
                messageInfoActive = false;
                quitMSG = "";
                getContext()->window.setShouldClose(false);
            }
            if(yesBtn.clicked || getContext()->window.isKeyClicked(LIGIDGL_KEY_ENTER)){
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
                                            CatMSG("Mrrrr :3   ", appTextures.mascotCat_bread),
                                            CatMSG("Purrr :3   ", appTextures.mascotCat_relaxed),
                                            CatMSG("Purrr <3   ", appTextures.mascotCat_bread),
                                            CatMSG("Meow :3   ", appTextures.mascotCat_relaxed),
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
                                            CatMSG("Starting off with style! Keep those colors flowing!", appTextures.mascotCat_smile),
                                            CatMSG("Looking good! Don't stop now!", appTextures.mascotCat_smile),
                                            CatMSG("Purr-fect progress! You're on a roll!", appTextures.mascotCat_smile),
                                            CatMSG("Wow, that's coming together nicely! Keep it up!", appTextures.mascotCat_smile),
                                            CatMSG("Impressive strokes! This is shaping up beautifully!", appTextures.mascotCat_smile),
                                            CatMSG("Meow-velous! You're almost there!", appTextures.mascotCat_smile),
                                            CatMSG("Simply purr-fect! Your art is amazing!", appTextures.mascotCat_smile),
                                            CatMSG("Absolutely clawsome! Can't wait to see the final touches!", appTextures.mascotCat_smile),
                                            CatMSG("You're a true artist! Keep painting!", appTextures.mascotCat_smile),
                                            CatMSG("Looking meow-tastic! Keep those creative juices flowing!", appTextures.mascotCat_smile),
                                            CatMSG("Purr-fact strokes!", appTextures.mascotCat_smile),
                                        });

                catMSG = msg.text;
                msgFace = msg.face;
            }
            messageInfoBtn.text = catMSG;
        }
        else if(dialog_settings.dialogControl.globalFirstFrameActivated){
            messageInfoActive = true;
            messageInfoBtnStartTime = timer.seconds;
            if(catMSG == ""){
                CatMSG msg = pickText(timer, {
                                            CatMSG("Yikes! So technical", appTextures.mascotCat_dizzy),
                                            CatMSG("Gears!! shiver me timbers", appTextures.mascotCat_dizzy),
                                            CatMSG("Here we come gears!", appTextures.mascotCat_smile),
                                            CatMSG("Don't be scared! It's just a few buttons and switches.", appTextures.mascotCat_thinking),
                                            CatMSG("Let's see what we can get into with these settings!!", appTextures.mascotCat_smile),
                                            CatMSG("Time to tinker with the cat-figurations!", appTextures.mascotCat_smile),
                                            CatMSG("Paws and reflect on these settings!", appTextures.mascotCat_smile),
                                            CatMSG("Meow-stering the art of customization!", appTextures.mascotCat_smile),
                                            CatMSG("Adjusting settings? Piece of catnip cake!", appTextures.mascotCat_smile),
                                            CatMSG("Ready to purr-sonalize your experience?", appTextures.mascotCat_smile),
                                            CatMSG("DON'T YOU DARE TURN ME OFF FROM THERE!!! >:3", appTextures.mascotCat_rock),
                                            CatMSG("You're not gonna turn me off are u??", appTextures.mascotCat_thinking),
                                            CatMSG("Pweasee don't turn me off. It's so dark here :3", appTextures.mascotCat_crying),
                                        });

                catMSG = msg.text;
                msgFace = msg.face;
            }
            messageInfoBtn.text = catMSG;
        }
        else if(dialog_baking.dialogControl.globalFirstFrameActivated){
            messageInfoActive = true;
            messageInfoBtnStartTime = timer.seconds;
            if(catMSG == ""){
                CatMSG msg = pickText(timer, {
                                            CatMSG("Bakey bakey!", appTextures.mascotCat_bread),
                                            CatMSG("Let's bake those textures!", appTextures.mascotCat_bread),
                                            CatMSG("Mixing pixels together for a mouthwatering result!", appTextures.mascotCat_bread),
                                            CatMSG("Turning up the heat to cook those textures to perfection!", appTextures.mascotCat_bread),
                                            CatMSG("Baking textures - the secret ingredient for a purr-fect render!", appTextures.mascotCat_bread),
                                            CatMSG("Just like baking cookies, but for your 3D masterpiece!", appTextures.mascotCat_bread),
                                            CatMSG("Baking those pixels like a baker layers cake!", appTextures.mascotCat_bread),
                                            CatMSG("Adding some flavor to your project with freshly baked textures!", appTextures.mascotCat_bread),
                                            CatMSG("Creating a baking recipe that'll make your project scrumptious!", appTextures.mascotCat_bread),
                                            CatMSG("Baking time!", appTextures.mascotCat_bread),
                                            CatMSG("The oven's preheated!", appTextures.mascotCat_bread),
                                        });

                catMSG = msg.text;
                msgFace = msg.face;
            }
            messageInfoBtn.text = catMSG;
        }
        else if(dialog_displayer.dialogControl.globalFirstFrameActivated){
            messageInfoActive = true;
            messageInfoBtnStartTime = timer.seconds;
            if(catMSG == ""){
                CatMSG msg = pickText(timer, {
                                            CatMSG("Let's tailor that purr-fect skybox.", appTextures.mascotCat_smile),
                                            CatMSG("How about a sunrise, or perhaps a cozy sunset?", appTextures.mascotCat_relaxed),
                                            CatMSG("Adjust the sky to match my mood :3", appTextures.mascotCat_relaxed),
                                            CatMSG("Pawsitively purr-fect skies!!", appTextures.mascotCat_relaxed),
                                            CatMSG("Sunrise or sunset? Let's make your skybox a masterpiece of the heavens!", appTextures.mascotCat_smile),
                                            CatMSG("Ahh the soft, pure sky <3", appTextures.mascotCat_relaxed),
                                            CatMSG("OoOW!! Did you get bored of ur background??", appTextures.mascotCat_smile),
                                        });

                catMSG = msg.text;
                msgFace = msg.face;
            }
            messageInfoBtn.text = catMSG;
        }
        else if(dialog_export.dialogControl.globalFirstFrameActivated){
            messageInfoActive = true;
            messageInfoBtnStartTime = timer.seconds;
            if(catMSG == ""){
                CatMSG msg = pickText(timer, {
                                            CatMSG("You can access your textures via file explorer! No need to export!!", appTextures.mascotCat_thinking),
                                            CatMSG("Time to package these textures!", appTextures.mascotCat_smile),
                                            CatMSG("Exporting textures with a sprinkle of whisker-dust!", appTextures.mascotCat_smile),
                                            CatMSG("Preparing your textures for a journey to your computer's catnip stash!", appTextures.mascotCat_smile),
                                            CatMSG("Sending your textures home!", appTextures.mascotCat_smile),
                                        });

                catMSG = msg.text;
                msgFace = msg.face;
            }
            messageInfoBtn.text = catMSG;
        }
        else if(dialog_newTexture.dialogControl.globalFirstFrameActivated){
            messageInfoActive = true;
            messageInfoBtnStartTime = timer.seconds;
            if(catMSG == ""){
                CatMSG msg = pickText(timer, {
                                            CatMSG("Brand-new textures!!", appTextures.mascotCat_smile),
                                            CatMSG("New textures on the horizon! Let's unleash the artist within!", appTextures.mascotCat_smile),
                                            CatMSG("Paws to new textures!!", appTextures.mascotCat_smile),
                                            CatMSG("Don't forget to name your texture!!", appTextures.mascotCat_smile),
                                            CatMSG("Pick the best color possible :3", appTextures.mascotCat_relaxed),
                                        });

                catMSG = msg.text;
                msgFace = msg.face;
            }
            messageInfoBtn.text = catMSG;
        }
        else if(dialog_newProject.dialogControl.globalFirstFrameActivated){
            messageInfoActive = true;
            messageInfoBtnStartTime = timer.seconds;
            if(catMSG == ""){
                CatMSG msg = pickText(timer, {
                                            CatMSG("New project on the horizon! Let's unleash the artist within!", appTextures.mascotCat_smile),
                                            CatMSG("Don't forget to name your project!!", appTextures.mascotCat_smile),
                                            CatMSG("New project new adventure!!", appTextures.mascotCat_smile),
                                            CatMSG("A project is a folder that held inside of a folder. Meownd-blowing!", appTextures.mascotCat_smile),
                                            CatMSG("Time to embark on a new project, whiskers ready!", appTextures.mascotCat_smile),
                                            CatMSG("Starting a new project, let's paw-tner up for some creative fun!", appTextures.mascotCat_smile),
                                            CatMSG("Crafting a new project with the finesse and curiosity of a cat!", appTextures.mascotCat_smile),
                                            CatMSG("I had a friend named 'project' once. Hope he is doing well now :3", appTextures.mascotCat_thinking),
                                        });

                catMSG = msg.text;
                msgFace = msg.face;
            }
            messageInfoBtn.text = catMSG;
        }
        else if(dialog_textureEditor.dialogControl.globalFirstFrameActivated){
            messageInfoActive = true;
            messageInfoBtnStartTime = timer.seconds;
            if(catMSG == ""){
                CatMSG msg = pickText(timer, {
                                            CatMSG("Time to give those pixels a cat-tastic makeover!", appTextures.mascotCat_smile),
                                            CatMSG("Meow-difying images!", appTextures.mascotCat_smile),
                                            CatMSG("Pawsitively purr-fect editing!", appTextures.mascotCat_smile),
                                            CatMSG("Whisking images into shape!", appTextures.mascotCat_smile),
                                            CatMSG("Catnip for your images! Let's create something paw-some :3", appTextures.mascotCat_smile),
                                            CatMSG("Tweaking pixels with whisker-precision!", appTextures.mascotCat_smile),
                                            CatMSG("Meow-gical image transformations!", appTextures.mascotCat_smile),
                                            CatMSG("Crafting pixel into purr-fection!", appTextures.mascotCat_smile)
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
            msgFace = appTextures.mascotCat_rock;
        }
    }
    
    if(messagesPanelYAxisMixVal != 1.f)
        messagesPanel.slideVal = 1000.f;
    
    // --------- INFO CLOSE -----------
    if(!getContext()->window.shouldClose()){
        if(timer.seconds - messageInfoBtnStartTime >= 3){
            catMSG = "";
            messageInfoActive = false;
        }
    }
    else{
        //if(timer.seconds - messageInfoBtnStartTime >= 20){
        if(false){
            messageInfoActive = false;
            quitMSG = "";
            getContext()->window.setShouldClose(false);
        }
    }

    timer.transition(messageInfoActive, messageInfoBtnMixVal, 0.2f);

    lastMessagesSize = messages.size();

    if(painter.selectedDisplayingModeIndex != 0){
        this->activeHistoryMode = HISTORY_PAINTING_MODE;
        otherHistoryBtn.text = "Painting History";
    }
    if(painter.selectedPaintingModeIndex == 5){
        this->activeHistoryMode = HISTORY_VECTORS_MODE;
        otherHistoryBtn.text = "Vectors History";
    }
    if(painter.selectedDisplayingModeIndex == 0){
        this->activeHistoryMode = HISTORY_OBJECTSELECTION_MODE;
        otherHistoryBtn.text = "Object Selection History";
    }
    if(painter.faceSelection.editMode || dialog_objectTexturing.faceSelectionMode){
        this->activeHistoryMode = HISTORY_FACESELECTION_MODE;
        otherHistoryBtn.text = "Face Selection History";
    }
    if(painter.paintingoverTextureEditorMode){
        this->activeHistoryMode = HISTORY_TEXTUREFIELDS_MODE;
        otherHistoryBtn.text = "Texture Fields History";
    }
    if(dialog_materialEditor.dialogControl.isActive()){
        this->activeHistoryMode = HISTORY_MATERIALEDITOR_MODE; 
        otherHistoryBtn.text = "Material History";
    }
    if(this->libraryHistoryMode)
        this->activeHistoryMode = HISTORY_LIBRARY_MODE;
        

    if(newLibraryAction){
        this->libraryHistoryMode = true;
        this->libraryHistoryBtn.clickState1 = true;
        this->otherHistoryBtn.clickState1 = false;
    }
    if(newOtherAction){
        this->libraryHistoryMode = false;
        this->libraryHistoryBtn.clickState1 = false;
        this->otherHistoryBtn.clickState1 = true;
    }

    // Restrict history counts 
    if(actions_MaterialEditor.size() > MAX_MATERIAL_HISTORY){
        actions_MaterialEditor[0].material.deleteBuffers();
        actions_MaterialEditor.erase(actions_MaterialEditor.begin());
    }

    if(painter.selectedDisplayingModeIndex != 1)
        actions_MultiChannelPainting.clear();

    std::vector<PaintingAction>* paintingActions;
    if(painter.materialPainting)
        paintingActions = &actions_MultiChannelPainting;
    else
        paintingActions = &actions_Painting[painter.selectedTexture.ID];

    if(actions_MultiChannelPainting.size() > MAX_PAINTING_HISTORY){
        actions_MultiChannelPainting.erase(actions_MultiChannelPainting.begin());
    }

    try
    {
        int overflowID = -1;

        for (const auto& entry : std::filesystem::directory_iterator(UTIL::environmentSpecificAppDataFolderPath() + "LigidPainter/tmp")) {
            if (entry.is_regular_file()) {
                std::string fileName = entry.path().filename().string();
                // Check if the file starts with "_history_"
                if (fileName.find("_history_") == 0) {
                    std::istringstream iss(fileName);
                    std::string part;
                    std::getline(iss, part, '_'); // Skip the first part "_history_"
                    std::getline(iss, part, '_'); // Read the first integer value
                    std::getline(iss, part, '_'); // Read the first integer value
                    int indexVal = std::stoi(part);

                    std::getline(iss, part, '_'); // Read the second integer value
                    int IDVal = std::stoi(part);

                    if(indexVal == MAX_PAINTING_HISTORY){
                        overflowID = IDVal;
                    }
                }
            }
        }

        if(overflowID != -1){
            
            actions_Painting[overflowID].erase(actions_Painting[overflowID].begin());
            
            const std::string deletingPath = UTIL::environmentSpecificAppDataFolderPath() + "LigidPainter/tmp/_history_0_" + std::to_string(overflowID);  
            if(!std::filesystem::remove(deletingPath))
                LGDLOG::start << "ERROR : Can't delete the unbinded history tmp file! : " << deletingPath << LGDLOG::end;  

            for (const auto& entry : std::filesystem::directory_iterator(UTIL::environmentSpecificAppDataFolderPath() + "LigidPainter/tmp")) {
                if (entry.is_regular_file()) {
                    std::string fileName = entry.path().filename().string();

                    // Check if the file starts with "_history_"
                    if (fileName.find("_history_") == 0) {
                        // Use string stream to split the filename into parts
                        std::istringstream iss(fileName);
                        std::string part;
                        std::getline(iss, part, '_'); // Skip the first part "_history_"
                        std::getline(iss, part, '_'); // Read the first integer value
                        std::getline(iss, part, '_'); // Read the first integer value
                        int indexVal = std::stoi(part);

                        std::getline(iss, part, '_'); // Read the second integer value
                        int IDVal = std::stoi(part);
                        
                        /*
                            TODO
                            Library::getTexture(i)->copyDataToTheCopyContext();
                            projectUpdatingThreadElements.updateTextures = true;
                        */

                        std::filesystem::rename(
                                                    UTIL::environmentSpecificAppDataFolderPath() + "LigidPainter/tmp/" + "_history_" + std::to_string(indexVal) + "_" + std::to_string(IDVal) + ".tmp",
                                                    UTIL::environmentSpecificAppDataFolderPath() + "LigidPainter/tmp/" + "_history_" + std::to_string(indexVal - 1) + "_" + std::to_string(IDVal) + ".tmp"
                                                );
                    }
                }
            }
        }        

    }
    catch (const std::filesystem::filesystem_error& ex) {
        LGDLOG::start << "ERROR : While deleting the outdated history tmp files!" << ex.what() << LGDLOG::end;
    }

    // Delete the unrelated history data
    if(!dialog_materialEditor.dialogControl.isActive() || project_path() != lastProjectPath){
        for (size_t i = 0; i < actions_MaterialEditor.size(); i++)
        {
            actions_MaterialEditor[i].material.deleteBuffers();
        }
        actions_MaterialEditor.clear();
    }

    prevpPainterDisplayIndex = painter.selectedDisplayingModeIndex;
    prevpPainterSelectedTxtr = painter.selectedTexture.ID;

    if(!painter.paintingoverTextureEditorMode || project_path() != lastProjectPath){
        for (size_t i = 0; i < actions_TextureFields.size(); i++)
        {
            for (size_t fieldI = 0; fieldI < actions_TextureFields[i].fields.size(); fieldI++)
            {
                bool match = false;

                for (size_t cI = 0; cI < paintingOverTextureFields.size(); cI++)
                {
                    if(paintingOverTextureFields[cI].texture.ID == actions_TextureFields[i].fields[fieldI].texture.ID)
                        match = true;
                }
                
                if(!match)
                    glDeleteTextures(1, &actions_TextureFields[i].fields[fieldI].texture.ID);
            }
        }

        actions_TextureFields.clear();
    }
    
    if(!(painter.faceSelection.editMode || dialog_objectTexturing.faceSelectionMode) || project_path() != lastProjectPath){
        actions_FaceSelection.clear();
    }

    if(project_path() != lastProjectPath){
        /*
        for (size_t i = 0; i < actions_Library.size(); i++)
        {
            glDeleteTextures(1, &actions_Library[i].texture.ID);
            glDeleteTextures(1, &actions_Library[i].alteredTexture.ID);
        }
        */

        actions_Library.clear();
    }

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
            for (size_t i = 0; i < paintingActions->size(); i++)
            {
                logSections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(1), (*paintingActions)[i].title, Texture(), 0., false));
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
            shortcuts_CTRL_Z()
        )
    {
        undo(painter);
    }

    newLibraryAction = false;
    newOtherAction = false;
    lastProjectPath = project_path();
}

bool LogDialog::isHovered(){
    return this->messagesPanel.hover || 
    this->historyPanel.hover || 
    this->logBtn.hover || 
    this->logBtnL.hover || 
    this->logBtnR.hover || 
    this->libraryHistoryBtn.hover || 
    this->otherHistoryBtn.hover ||
    this->noBtn.hover ||
    this->yesBtn.hover;
}