
/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

All rights reserved.

Official GitHub Link : https:github.com/mert-tetik/LigidPainter
Official Web Page : https:ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "3D/ThreeD.hpp" 

#include "GUI/GUI.hpp" 

#include "UTIL/Mouse/Mouse.hpp" 
#include "UTIL/Library/Library.hpp" 
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp" 
#include "UTIL/Painting/Painter.hpp" 
#include "UTIL/Threads/Threads.hpp" 

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

extern std::atomic<bool> load_ligidpainter_done;
extern std::atomic<bool> project_updating_thread_working;

static void render_paws(Timer& timer, Button logBtn, Panel menu_bar);

static void render_selected_panel(Timer& timer, Panel* messagesPanel, float messagesPanel_mixVal, std::vector<std::string>& messages, 
                                    Panel* historyPanel, float historyPanel_mixVal, Panel* libraryHistoryPanel, float libraryHistoryPanel_mixVal, 
                                    Panel* multiThreadInfoPanel, float multiThreadInfoPanel_mixVal, Panel menu_bar, float menu_mode_mix_val, 
                                    HistoryActionRecords& history_action_records, int activeHistoryMode, LogDialog* log_dialog);

static void render_menu_bar(Timer& timer, Panel* menu_bar, bool* menu_mode, float* menu_mode_mix_val, Button logBtn, 
                            Panel messagesPanel, Panel historyPanel, Panel libraryHistoryPanel, Panel multiThreadInfoPanel);

static void render_talking_text(Timer& timer, Button logBtn, bool* windowShouldClose);

static size_t emotion_assertion_time = 0;
static int emotion_assertion_duration = 0;
void LogDialog::assert_emotion(Timer& timer, const unsigned int emotion, const int duration){
    if(!getContext()->window.shouldClose()){
      
        emotion_assertion_time = timer.seconds;
        emotion_assertion_duration = duration;
        
        if(emotion == CAT_EMOTION_DEFAULT){
            this->logBtn.texture = appTextures.mascotCat_default;
        }
        if(emotion == CAT_EMOTION_CRYING){
            this->logBtn.texture = appTextures.mascotCat_crying;
        }
        if(emotion == CAT_EMOTION_ROCK){
            this->logBtn.texture = appTextures.mascotCat_rock;
        }
        if(emotion == CAT_EMOTION_THINKING){
            this->logBtn.texture = appTextures.mascotCat_thinking;
        }
        if(emotion == CAT_EMOTION_DIZZY){
            this->logBtn.texture = appTextures.mascotCat_dizzy;
        }
        if(emotion == CAT_EMOTION_SLEEPY){
            this->logBtn.texture = appTextures.mascotCat_sleeping;
        }
        if(emotion == CAT_EMOTION_RANDOM){
            this->logBtn.texture = appTextures.mascotCat_bread;
        }
        if(emotion == CAT_EMOTION_RELAXED){
            this->logBtn.texture = appTextures.mascotCat_relaxed;
        }
        if(emotion == CAT_EMOTION_THINKING){
            this->logBtn.texture = appTextures.mascotCat_thinking;
        }
        if(emotion == CAT_EMOTION_SMILING){
            this->logBtn.texture = appTextures.mascotCat_smile;
        }

    }
}

void LogDialog::render_elements(Timer& timer)
{
    // Move to the center
    if(getContext()->window.shouldClose()){
        glm::vec2 center = glm::vec2(50.);
        pos -= (pos - center) / 10.f;
    }

    if(logBtn.clickState1){ 
        pos.x += Mouse::mouseOffset()->x / Settings::videoScale()->x * 100.f;
        pos.y += Mouse::mouseOffset()->y / Settings::videoScale()->y * 100.f;
    }
    
    logBtn.pos.x = pos.x;
    logBtn.pos.y = pos.y;
    logBtn.pos.z = 0.7f;

    // Set the default emotion if requested emotion was expired
    if(timer.seconds - emotion_assertion_time > emotion_assertion_duration)
        logBtn.texture = appTextures.mascotCat_default;

    // Render the selected panel
    render_selected_panel(
                            timer,
                            
                            &this->messagesPanel,
                            menu_bar.sections[0].elements[0].button.clickedMixVal,
                            this->messages,

                            &this->historyPanel,
                            menu_bar.sections[0].elements[1].button.clickedMixVal,
                            
                            &this->libraryHistoryPanel,
                            menu_bar.sections[0].elements[2].button.clickedMixVal,
                            
                            &this->multiThreadInfoPanel,
                            menu_bar.sections[0].elements[3].button.clickedMixVal,
                            
                            this->menu_bar,
                            this->menu_mode_mix_val,

                            this->history_action_records,
                            this->get_active_history_mode(),
                            this
                        );

    // Render the menu bar
    render_menu_bar(
                        timer,
                        
                        &this->menu_bar,
                        &this->menu_mode,
                        &this->menu_mode_mix_val,

                        this->logBtn,
                        this->messagesPanel,
                        this->historyPanel,
                        this->libraryHistoryPanel,
                        this->multiThreadInfoPanel
                    );

    render_talking_text(
                            timer,
                            this->logBtn,
                            &this->windowShouldClose
                        );

    ShaderSystem::buttonShader().setFloat("rotation", std::sin(LigidGL::getTime() * 2.f) * 10.f * (float)!sleepingCat + (std::sin(logBtn.clickedMixVal) * 360.f));
    logBtn.render(timer, true);
    ShaderSystem::buttonShader().setFloat("rotation", 0.f);

    // Render loading icon if another thread is working
    if(this->any_thread_in_progress()){
        Button loading_icon = Button(ELEMENT_STYLE_SOLID, glm::vec2(1.f), "", Texture(), 0.f, false);
        loading_icon.pos = logBtn.pos;
        loading_icon.pos.y += logBtn.scale.y;
        loading_icon.pos.x += logBtn.scale.x / 2.f;
        loading_icon.color.a = 0.f;
        loading_icon.color2.a = 0.f;
        loading_icon.outline = false;
        loading_icon.texture = appTextures.loadingIcon;
    
        loading_icon.render(timer, false);
    }

    /* If no mouse interaction for a long time make the cat sleepy */
    {
        const int time_to_sleep = 5;
        if(Mouse::mouseOffset()->x || Mouse::mouseOffset()->y)
            this->sleepingCounter = timer.seconds;
        if(timer.seconds - this->sleepingCounter > time_to_sleep)
            this->assert_emotion(timer, CAT_EMOTION_SLEEPY, 1);
    }

    /* Flipping the cat (make the cat dizzy if flipped enough)*/
    {
        const int flips_to_get_dizzy = 4;

        if(this->logBtn.clicked){
            flipCount++;
        }
        if(flipCount >= flips_to_get_dizzy){
            this->assert_emotion(timer, CAT_EMOTION_DIZZY, 3);
            flipCount = 0;
        }
    }

    /* Update the petting points for the cat. (say something about it in the talking_interaction)*/
    {
        if(this->logBtn.hover)
            petPoints += std::abs(Mouse::mouseOffset()->x) + std::abs(Mouse::mouseOffset()->y);
        else
            petPoints = 0.f;
    }

    /* Set specific emotion if the menu mode is active and cat is not dizzy*/
    {
        if(this->menu_mode && logBtn.texture.ID != appTextures.mascotCat_dizzy.ID)
            this->assert_emotion(timer, CAT_EMOTION_RELAXED, 1);
    }

    /* Set specific emotion if cat is hovered and the cat's emote is default*/
    {
        if(logBtn.hover && logBtn.texture.ID == appTextures.mascotCat_default.ID){
            this->assert_emotion(timer, CAT_EMOTION_SMILING, 1);
        }
    }
    
    /* Set specific emotion if painting*/
    {
        if(painting_paint_condition()){
            this->assert_emotion(timer, CAT_EMOTION_SMILING, 1);
        }
    }

    // Render the paws
    render_paws(timer, logBtn, this->menu_bar);
}
























// ----------------------------------------- UTILITY FUNCTIONS ------------------------------------------------


static void render_paws(Timer& timer, Button logBtn, Panel menu_bar){
    float paw_get_close = 1.75f;
    float paw_scale = 1.5f;

    Button paw_L = Button(ELEMENT_STYLE_SOLID, glm::vec2(paw_scale), "", Texture(), 0.f, false);
    paw_L.pos = logBtn.pos;
    paw_L.pos.x -= (logBtn.scale.x + paw_L.scale.x) / paw_get_close;
    paw_L.pos.y += std::sin(LigidGL::getTime() * 1.5f) / 2.f;
    paw_L.color.a = 0.f;
    paw_L.color2.a = 0.f;
    paw_L.outline = false;
    paw_L.texture = appTextures.mascotCat_pawL;

    Button paw_R = Button(ELEMENT_STYLE_SOLID, glm::vec2(paw_scale), "", Texture(), 0.f, false);
    paw_R.pos = logBtn.pos;
    paw_R.pos.x += (logBtn.scale.x + paw_R.scale.x) / paw_get_close + (menu_bar.scale.x) * 2.f;
    paw_R.pos.y += std::sin(LigidGL::getTime() * 1.5f) / 2.f;
    paw_R.color.a = 0.f;
    paw_R.color2.a = 0.f;
    paw_R.outline = false;
    paw_R.texture = appTextures.mascotCat_pawR;

    ShaderSystem::buttonShader().setFloat("rotation", (std::sin(LigidGL::getTime() * 1.5f)) * 30.f);
    paw_L.render(timer, false);
    ShaderSystem::buttonShader().setFloat("rotation", -(std::sin(LigidGL::getTime() * 1.5f)) * 30.f);
    paw_R.render(timer, false);
    ShaderSystem::buttonShader().setFloat("rotation", 0.f);
}

static void render_selected_panel(
                                    Timer& timer, 
                                    
                                    Panel* messagesPanel, 
                                    float messagesPanel_mixVal, 
                                    std::vector<std::string>& messages, 
                                    
                                    Panel* historyPanel, 
                                    float historyPanel_mixVal, 
                                    
                                    Panel* libraryHistoryPanel, 
                                    float libraryHistoryPanel_mixVal, 
                                    
                                    Panel* multiThreadInfoPanel, 
                                    float multiThreadInfoPanel_mixVal, 
                                    
                                    Panel menu_bar, 
                                    float menu_mode_mix_val,
                                    
                                    HistoryActionRecords& history_action_records,
                                    int activeHistoryMode,

                                    LogDialog* log_dialog
                                )
{
    const float panel_scale = 15.f;
    
    if(messagesPanel_mixVal * menu_mode_mix_val){
        messagesPanel->sections[0].elements.clear();
        for (size_t i = 0; i < messages.size(); i++)
        {
            Button btn = Button(ELEMENT_STYLE_SOLID, glm::vec2(1), messages[i], Texture(), 0., false);
            
            // Is an error message
            if(UTIL::doHaveWordAsFirstWord(messages[i], "ERROR"))
                btn.color = glm::vec4(74.f / 255.f, 15.f / 255.f, 15.f / 255.f, 1.f);

            else if(UTIL::doHaveWordAsFirstWord(messages[i], "WARNING")){
                btn.color = glm::vec4(74.f / 255.f, 74.f / 255.f, 15.f / 255.f, 1.f);
            }

            messagesPanel->sections[0].elements.push_back(btn);
        }
        
        ShaderSystem::buttonShader().setFloat("properties.groupOpacity", messagesPanel_mixVal * menu_mode_mix_val);
        messagesPanel->scale.x = menu_bar.scale.x + 0.1f;
        messagesPanel->scale.y = messagesPanel_mixVal * menu_mode_mix_val * panel_scale + 0.1f;
        messagesPanel->pos = menu_bar.pos;
        messagesPanel->pos.y += menu_bar.scale.y + messagesPanel->scale.y;
        messagesPanel->preRenderingMode = true;
        messagesPanel->render(timer, true);
        ShaderSystem::buttonShader().use();
        ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.f);
    }
    else{
        messagesPanel->slideVal = 10000.f;
    }
    
    if(historyPanel_mixVal * menu_mode_mix_val){
        
        historyPanel->sections[0].elements.clear();

        std::string current_action_mode = "Empty";

        if(activeHistoryMode == HISTORY_MATERIALEDITOR_MODE){
            current_action_mode = "Material Editor";
            if(dialog_materialEditor.currently_edited_materialPtr != nullptr){
                for (size_t i = 0; i < history_action_records.actions_MaterialEditor[dialog_materialEditor.currently_edited_materialPtr].size(); i++)
                {
                    historyPanel->sections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(1), history_action_records.actions_MaterialEditor[dialog_materialEditor.currently_edited_materialPtr][i].title, Texture(), 0., false));
                }
            }
        }
        if(activeHistoryMode == HISTORY_PAINTING_MODE){
            current_action_mode = "Painting";
            if(panel_library_selected_texture.ID && glIsTexture(panel_library_selected_texture.ID) == GL_TRUE){
                for (size_t i = 0; i < history_action_records.actions_Painting[panel_library_selected_texture.ID].size(); i++)
                {
                    historyPanel->sections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(1), history_action_records.actions_Painting[panel_library_selected_texture.ID][i].title, Texture(), 0., false));
                }
            }
        }
        if(activeHistoryMode == HISTORY_MULTI_CHANNEL_PAINTING_MODE){
            current_action_mode = "Multi-Channel Painting";
            if(history_action_records.get_actively_painted_material_channels().albedo.ID && glIsTexture(history_action_records.get_actively_painted_material_channels().albedo.ID) == GL_TRUE){
                for (size_t i = 0; i < history_action_records.actions_MultiChannelPainting[history_action_records.get_actively_painted_material_channels().albedo.ID].size(); i++)
                {
                    historyPanel->sections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(1), history_action_records.actions_MultiChannelPainting[history_action_records.get_actively_painted_material_channels().albedo.ID][i].title, Texture(), 0., false));
                }
            }
        }
        if(activeHistoryMode == HISTORY_VECTORS_MODE){
            VectorScene* current_vector_scene = history_action_records.get_active_vectorScene();
            current_action_mode = "Vector";
            
            if(current_vector_scene != nullptr){
                for (size_t i = 0; i < history_action_records.actions_Vectors[current_vector_scene].size(); i++)
                {
                    historyPanel->sections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(1), history_action_records.actions_Vectors[current_vector_scene][i].title, Texture(), 0., false));
                }
            }
        }
        if(activeHistoryMode == HISTORY_TEXTUREFIELDS_MODE){
            TextureFieldScene* current_texture_field_scene = history_action_records.get_active_textureFieldScene();
            current_action_mode = "Texture Field";
            
            if(current_texture_field_scene != nullptr){
                for (size_t i = 0; i < history_action_records.actions_TextureFields[current_texture_field_scene].size(); i++)
                {
                    historyPanel->sections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(1), history_action_records.actions_TextureFields[current_texture_field_scene][i].title, Texture(), 0., false));
                }
            }
        }
        if(activeHistoryMode == HISTORY_FACESELECTION_MODE){
            Mesh* active_face_selected_mesh = history_action_records.get_active_face_selected_mesh();
            
            current_action_mode = "Face Selection";
            if(active_face_selected_mesh != nullptr){
                for (size_t i = 0; i < history_action_records.actions_FaceSelection[active_face_selected_mesh].size(); i++)
                {
                    historyPanel->sections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(1), history_action_records.actions_FaceSelection[active_face_selected_mesh][i].title, Texture(), 0., false));
                }
            }
        }

        historyPanel->sections[0].elements.push_back(Button(ELEMENT_STYLE_STYLIZED, glm::vec2(1), "Undo / CTRL+Z", Texture(), 1.f, false));
        
        Button current_action_mode_display_btn = Button(ELEMENT_STYLE_BASIC, glm::vec2(menu_bar.scale.x, 2.f * historyPanel_mixVal * menu_mode_mix_val), current_action_mode + " History Actions", Texture(), 0.f, false);
        current_action_mode_display_btn.pos = menu_bar.pos;
        current_action_mode_display_btn.pos.y += menu_bar.scale.y + current_action_mode_display_btn.scale.y;
        current_action_mode_display_btn.radius /= 4.f;
        current_action_mode_display_btn.render(timer, false);


        ShaderSystem::buttonShader().setFloat("properties.groupOpacity", historyPanel_mixVal * menu_mode_mix_val);
        historyPanel->scale.x = menu_bar.scale.x;
        historyPanel->scale.y = historyPanel_mixVal * menu_mode_mix_val * panel_scale;
        historyPanel->pos = current_action_mode_display_btn.pos;
        historyPanel->pos.y += current_action_mode_display_btn.scale.y + historyPanel->scale.y;
        historyPanel->render(timer, true);
        ShaderSystem::buttonShader().use();
        ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.f);

        // If pressed to undo button
        if(historyPanel->sections[0].elements[historyPanel->sections[0].elements.size() - 1].button.hover && *Mouse::LClick()){
            log_dialog->undo_general_history();
        }
    }

    if(libraryHistoryPanel_mixVal * menu_mode_mix_val){

        libraryHistoryPanel->sections[0].elements.clear();
        for (size_t i = 0; i < history_action_records.actions_Library.size(); i++)
        {
            libraryHistoryPanel->sections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(1), history_action_records.actions_Library[i].title, Texture(), 0., false));
        }

        libraryHistoryPanel->sections[0].elements.push_back(Button(ELEMENT_STYLE_STYLIZED, glm::vec2(1), "Undo / CTRL+Z", Texture(), 1., false));
        
        ShaderSystem::buttonShader().setFloat("properties.groupOpacity", libraryHistoryPanel_mixVal * menu_mode_mix_val);
        libraryHistoryPanel->scale.x = menu_bar.scale.x;
        libraryHistoryPanel->scale.y = libraryHistoryPanel_mixVal * menu_mode_mix_val * panel_scale;
        libraryHistoryPanel->pos = menu_bar.pos;
        libraryHistoryPanel->pos.y += menu_bar.scale.y + libraryHistoryPanel->scale.y;
        libraryHistoryPanel->render(timer, true);
        ShaderSystem::buttonShader().use();
        ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.f);
        
        if(libraryHistoryPanel->sections[0].elements[libraryHistoryPanel->sections[0].elements.size() - 1].button.hover && *Mouse::LClick()){
            log_dialog->history_action_records.undo_library_actions();
        }
    }
    
    if(multiThreadInfoPanel_mixVal * menu_mode_mix_val){

        multiThreadInfoPanel->sections[0].elements.clear();

        if(material_thread.active)
            multiThreadInfoPanel->sections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(2.5f), "Ongoing material processing action...", Texture(), 0., false));
        
        if(project_updating_thread_working)
            multiThreadInfoPanel->sections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(2.5f), "Ongoing project saving action...", Texture(), 0., false));
        
        if(!load_ligidpainter_done)
            multiThreadInfoPanel->sections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(2.5f), "LigidPainter is being loaded...", Texture(), 0., false));
        
        ShaderSystem::buttonShader().setFloat("properties.groupOpacity", multiThreadInfoPanel_mixVal * menu_mode_mix_val);
        multiThreadInfoPanel->scale.x = menu_bar.scale.x;
        multiThreadInfoPanel->scale.y = multiThreadInfoPanel_mixVal * menu_mode_mix_val * panel_scale;
        multiThreadInfoPanel->pos = menu_bar.pos;
        multiThreadInfoPanel->pos.y += menu_bar.scale.y + multiThreadInfoPanel->scale.y;
        multiThreadInfoPanel->render(timer, true);
        ShaderSystem::buttonShader().use();
        ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.f);
    }
}

static void render_menu_bar(Timer& timer, Panel* menu_bar, bool* menu_mode, float* menu_mode_mix_val, Button logBtn, Panel messagesPanel, Panel historyPanel, Panel libraryHistoryPanel, Panel multiThreadInfoPanel){
    const float menu_mode_panel_scale = 10.f;

    timer.transition(*menu_mode, *menu_mode_mix_val, 0.5f);
    menu_bar->scale.x = *menu_mode_mix_val * menu_mode_panel_scale;
    menu_bar->pos = logBtn.pos;
    menu_bar->pos.x += logBtn.scale.x + menu_bar->scale.x - 1.5f;
    
    for (size_t i = 0; i < menu_bar->sections[0].elements.size(); i++)
    {
        menu_bar->sections[0].elements[i].scale.x = menu_bar->scale.x / menu_bar->sections[0].elements.size();
    }
    
    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", *menu_mode_mix_val);
    if(*menu_mode_mix_val)
        menu_bar->render(timer, true);
    ShaderSystem::buttonShader().use();
    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.f);

    if(
            *Mouse::LDoubleClick() && 
            !menu_bar->hover && 
            (!messagesPanel.hover || !menu_bar->sections[0].elements[0].button.clickState1) && 
            (!historyPanel.hover || !menu_bar->sections[0].elements[1].button.clickState1) && 
            (!libraryHistoryPanel.hover || !menu_bar->sections[0].elements[2].button.clickState1) &&
            (!multiThreadInfoPanel.hover || !menu_bar->sections[0].elements[3].button.clickState1) 
        )
    {
        *menu_mode = false;
    }
    
    if(logBtn.hover && *Mouse::LClick())
    {
        *menu_mode = !*menu_mode;
    }
    
    if((Mouse::mouseOffset()->x || Mouse::mouseOffset()->y) && *Mouse::LPressed() && *menu_mode_mix_val != 1.f)
    {
        *menu_mode = false;
    }
}

static Button yesBtn = Button(ELEMENT_STYLE_STYLIZED, glm::vec2(1.5f), "Yes", Texture(), 0., false);
static Button noBtn = Button(ELEMENT_STYLE_STYLIZED, glm::vec2(1.5f), "No", Texture(), 0., false);

static size_t talk_start_time = 0;
static std::string talk_msg = "";
static int talk_duration = 0; 

void LogDialog::make_cat_talk(Timer& timer, std::string text, const int duration){
    talk_start_time = timer.seconds;
    talk_msg = text;
    talk_duration = duration;
}

bool LogDialog::is_cat_talking(){
    return talk_msg != "";
}

static float speech_bubble_mixVal = 0.f;
static void render_talking_text(Timer& timer, Button logBtn, bool* windowShouldClose)
{
    if(timer.seconds - talk_start_time > talk_duration)
        talk_msg = "";

    timer.transition(talk_msg != "", speech_bubble_mixVal, 0.2f);

    if(speech_bubble_mixVal){
        glm::vec2 scale = glm::vec2(speech_bubble_mixVal * (talk_msg.size() / 4.5f), 2.f);
        glm::vec3 pos = glm::vec3(
                                    logBtn.pos.x - scale.x, 
                                    logBtn.pos.y + logBtn.scale.y + scale.y, 
                                    logBtn.pos.z
                                );

        render_sprite_image(timer, false, pos, scale, appTextures.mascotCat_speechBubble);

        Button text_btn = Button(ELEMENT_STYLE_SOLID, scale, talk_msg, Texture(), 0.f, false);
        text_btn.pos = pos;
        text_btn.pos.y += 0.25f;
        text_btn.pos.z += 0.01f;
        text_btn.color = glm::vec4(0.f);
        text_btn.color2 = glm::vec4(0.f);
        text_btn.outline = false;
        text_btn.textColor = glm::vec4(0.f, 0.f, 0.f, 1.f);

        text_btn.render(timer, false);

        if(getContext()->window.shouldClose())
        {
            yesBtn.radius = 0.2f;
            noBtn.radius = 0.2f;

            yesBtn.pos = pos;
            yesBtn.pos.x -= yesBtn.scale.x * 1.5f;
            yesBtn.pos.y += yesBtn.scale.y + scale.y + 1.f;
            
            noBtn.pos = pos;
            noBtn.pos.x += noBtn.scale.x * 1.5f;
            noBtn.pos.y += noBtn.scale.y + scale.y + 1.f;
        
            yesBtn.render(timer, true);
            noBtn.render(timer, true);

            if(noBtn.clicked || getContext()->window.isKeyClicked(LIGIDGL_KEY_ESCAPE)){
                talk_msg = "";
                getContext()->window.setShouldClose(false);
            }
            
            if(yesBtn.clicked || getContext()->window.isKeyClicked(LIGIDGL_KEY_ENTER)){
                *windowShouldClose = true;
            }
        }
    }        
}