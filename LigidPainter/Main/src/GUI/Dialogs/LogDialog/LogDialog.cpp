
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
#include "UTIL/Threads/Threads.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

#include "GUI/Dialogs/LogDialog/Registering.hpp"
#include "GUI/Dialogs/LogDialog/Rendering.hpp"
#include "GUI/Dialogs/LogDialog/Undo.hpp"
#include "GUI/Dialogs/LogDialog/Talking.hpp"

static std::vector<LibraryAction> actions_Library;
static std::map<unsigned int, std::vector<PaintingAction>> actions_Painting;
static std::vector<PaintingAction> actions_MultiChannelPainting;
static std::vector<VectorsAction> actions_Vectors;
static std::vector<FaceSelectionAction> actions_FaceSelection;
static std::vector<TextureFieldsAction> actions_TextureFields;
static std::vector<MaterialEditorAction> actions_MaterialEditor;

#define MAX_MATERIAL_HISTORY 8
#define MAX_PAINTING_HISTORY 25

namespace LGDLOG{
    LogMsg start;
    std::string end = "$#";
}

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

int prevpPainterDisplayIndex = 0;
unsigned int prevpPainterSelectedTxtr = 0;
std::string lastProjectPath;

bool newLibraryAction = false;
bool newOtherAction = false;

void LogDialog::render(Timer& timer)
{
    this->unded = false;

    glClear(GL_DEPTH_BUFFER_BIT);

    ShaderSystem::buttonShader().use();

    if(Mouse::mouseOffset()->x || Mouse::mouseOffset()->y)
        sleepingCounter = timer.seconds;

    sleepingCat = timer.seconds - sleepingCounter > 5; 

    this->render_elements(timer);

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
                this->msgFace = msg.face;
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
        talking(timer);    
    }
    
    if(lastMessagesSize != messages.size() && !getContext()->window.shouldClose()){
        messageInfoActive = true;
        messageInfoBtnStartTime = timer.seconds;
        if(messages.size()){
            messageInfoBtn.text = messages[messages.size()-1];
            this->msgFace = appTextures.mascotCat_rock;
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

    if(panel_displaying_modes.selectedElement != 0){
        this->activeHistoryMode = HISTORY_PAINTING_MODE;
        otherHistoryBtn.text = "Painting History";
    }
    if(panel_painting_modes.selectedElement == 5){
        this->activeHistoryMode = HISTORY_VECTORS_MODE;
        otherHistoryBtn.text = "Vectors History";
    }
    if(getScene()->get_selected_mesh()->face_selection_data.editMode){
        this->activeHistoryMode = HISTORY_FACESELECTION_MODE;
        otherHistoryBtn.text = "Face Selection History";
    }
    if(checkComboList_painting_over.panel.sections[0].elements[1].checkBox.clickState1){
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

    if(panel_displaying_modes.selectedElement != 1)
        actions_MultiChannelPainting.clear();

    std::vector<PaintingAction>* paintingActions;
    if(panel_displaying_modes.selectedElement == 1)
        paintingActions = &actions_MultiChannelPainting;
    else
        paintingActions = &actions_Painting[panel_library_selected_texture.ID];

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

    prevpPainterDisplayIndex = panel_displaying_modes.selectedElement;
    prevpPainterSelectedTxtr = panel_library_selected_texture.ID;

    if(!checkComboList_painting_over.panel.sections[0].elements[1].checkBox.clickState1 || project_path() != lastProjectPath){
        for (size_t i = 0; i < actions_TextureFields.size(); i++)
        {
            for (size_t fieldI = 0; fieldI < actions_TextureFields[i].fields.size(); fieldI++)
            {
                bool match = false;

                for (size_t cI = 0; cI < getTextureFieldScene()->texture_fields.size(); cI++)
                {
                    if(getTextureFieldScene()->texture_fields[cI].texture.ID == actions_TextureFields[i].fields[fieldI].texture.ID)
                        match = true;
                }
                
                if(!match)
                    glDeleteTextures(1, &actions_TextureFields[i].fields[fieldI].texture.ID);
            }
        }

        actions_TextureFields.clear();
    }
    
    if(!(getScene()->get_selected_mesh()->face_selection_data.editMode) || project_path() != lastProjectPath){
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
        undo();
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