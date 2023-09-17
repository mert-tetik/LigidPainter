/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Texture selection dialog is responsible of modifying the procedural variables inside of a Texture class & generating displaying textures

*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"
#include "LibrarySystem/Library.hpp"
#include "MouseSystem/Mouse.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>

TexturePackEditorDialog::TexturePackEditorDialog(){
    this->bgPanel = Panel({}, scale, pos, ColorPalette::secondColor, ColorPalette::thirdColor, true, true, false, true, true, 1.f, 15.f, {}, 20.f, true);
    this->textureSelectingPanel = Panel(
                                                    
                                                    {
                                                        /*
                                                        Section(
                                                            Button(),
                                                            {
                                                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2, scale.y / 2.f),"", Texture(), 0.f, true),
                                                            }
                                                        )
                                                        */
                                                    },
                                                    glm::vec2(scale.x / 6.f, scale.y / 2.f),
                                                    glm::vec3(pos.x, pos.y + scale.y - scale.y / 2.f, pos.z),
                                                    ColorPalette::mainColor,
                                                    ColorPalette::thirdColor,
                                                    true,
                                                    true,
                                                    false,
                                                    true,
                                                    true,
                                                    1.f,
                                                    12.f,
                                                    {},
                                                    20.f,
                                                    true
                                                );
    
    this->selectedTextureDisplayingPanel = Panel(
                                                    
                                                    
                                                    {
                                                        Section(
                                                            Button(),
                                                            {
                                                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2, scale.y / 2.f),"", Texture(), 0.f, true),
                                                            }
                                                        )
                                                    },
                                                    glm::vec2(scale.x / 6.f, scale.y / 2.f),
                                                    glm::vec3(pos.x, pos.y - scale.y + scale.y / 2.f, pos.z),
                                                    ColorPalette::mainColor,
                                                    ColorPalette::thirdColor,
                                                    true,
                                                    true,
                                                    false,
                                                    true,
                                                    true,
                                                    1.f,
                                                    1.f,
                                                    {},
                                                    20.f,
                                                    true
                                                );
    this->subPanel = Panel(
                                
                                
                                {
                                    Section(
                                        Button(),
                                        {
                                            Button(ELEMENT_STYLE_STYLIZED,glm::vec2(2,2.f),"Saperate Sprites", Texture(), 2.f, false),
                                            Button(ELEMENT_STYLE_STYLIZED,glm::vec2(2,2.f),"Add Texture From The Computer", Texture(), 2.f, false),
                                            Button(ELEMENT_STYLE_STYLIZED,glm::vec2(2,2.f),"Add Texture From The Library", Texture(), 2.f, false),
                                            Button(ELEMENT_STYLE_STYLIZED,glm::vec2(2,2.f),"Add Folder From The Computer", Texture(), 2.f, false),
                                            Button(ELEMENT_STYLE_STYLIZED,glm::vec2(2,2.f),"Delete The Selected Texture", Texture(), 2.f, false),
                                            Button(ELEMENT_STYLE_STYLIZED,glm::vec2(2,2.f),"Generate Sprite Texture", Texture(), 2.f, false)
                                        }
                                    )
                                },
                                glm::vec2(scale.x / 6.f, scale.y),
                                glm::vec3(pos.x - scale.x + (scale.x / 6.f) , pos.y, pos.z),
                                ColorPalette::mainColor,
                                ColorPalette::thirdColor,
                                true,
                                true,
                                false,
                                true,
                                true,
                                1.f,
                                1.f,
                                {},
                                20.f,
                                true
                            );
    
    this->sapSpritesPanel = Panel(
                                {
                                    Section(
                                        Button(),
                                        {
                                            Button(ELEMENT_STYLE_SOLID, glm::vec2(2,2.f),"Sprite Texture", Texture(), 1.f, false),
                                            Button(ELEMENT_STYLE_SOLID, glm::vec2(2,2.f),"Alpha Map", Texture(), 0.f, false),
                                            Button(ELEMENT_STYLE_STYLIZED, glm::vec2(2,2.f),"Generate", Texture(), 2.f, false)
                                        }
                                    )
                                },
                                glm::vec2(9.f),
                                glm::vec3(pos.x - scale.x + (scale.x / 6.f) , pos.y, pos.z),
                                ColorPalette::mainColor,
                                ColorPalette::thirdColor,
                                true,
                                true,
                                false,
                                true,
                                true,
                                1.f,
                                1.f,
                                {},
                                20.f,
                                true
                            );

}

//Forward declarations for the utility functions
static void drawBG(unsigned int bgTexture);
static void updateTextureSelectingPanelElements(Panel& textureSelectingPanel, TexturePack &receivedTexturePack, int selectedTextureMode);

void TexturePackEditorDialog::show(Timer &timer, glm::mat4 guiProjection, TexturePack& receivedTexturePack){
    
    this->dialogControl.activate();

    this->selectedTextureMode = 0;
    this->selectedTextureIndex = 0;

    while (!getContext()->window.shouldClose())
    {
        getContext()->window.pollEvents();
        
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawBG(Settings::defaultFramebuffer()->bgTxtr);

        dialogControl.updateStart();

        updateTextureSelectingPanelElements(this->textureSelectingPanel, receivedTexturePack, this->selectedTextureMode);

        //Render the panel
        this->bgPanel.render(timer, true);

        this->subPanel.render(timer, true);

        selectedTextureDisplayingPanel.scale.x = this->scale.x - subPanel.scale.x;
        selectedTextureDisplayingPanel.pos.x = this->pos.x + subPanel.scale.x;

        textureSelectingPanel.pos.x = selectedTextureDisplayingPanel.pos.x;
        textureSelectingPanel.scale.x = selectedTextureDisplayingPanel.scale.x;

        if(this->selectedTextureIndex < receivedTexturePack.textures.size())
            selectedTextureDisplayingPanel.sections[0].elements[0].button.texture = receivedTexturePack.textures[this->selectedTextureIndex];

        this->textureSelectingPanel.render(timer, true);

        this->selectedTextureDisplayingPanel.render(timer, false);

        if(this->sapSpritesPanelActive){
            this->sapSpritesPanel.render(timer, true);

            if(this->sapSpritesPanel.sections[0].elements[0].button.clicked){
                std::string test = showFileSystemObjectSelectionDialog("Select a sprite texture.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_TEXTURE, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);

                if(test.size()){
                    this->sapSpritesPanel.sections[0].elements[0].button.texture.load(test.c_str());
                }
            }
            else if(this->sapSpritesPanel.sections[0].elements[1].button.clicked){
                std::string test = showFileSystemObjectSelectionDialog("Select an alpha texture.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_TEXTURE, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);

                if(test.size()){
                    this->sapSpritesPanel.sections[0].elements[1].button.texture.load(test.c_str());
                }
            }
            else if(this->sapSpritesPanel.sections[0].elements[2].button.clicked){
                receivedTexturePack.saperateSprites(this->sapSpritesPanel.sections[0].elements[0].button.texture, this->sapSpritesPanel.sections[0].elements[1].button.texture);
                this->sapSpritesPanelActive = false;
                glDeleteTextures(1, &this->sapSpritesPanel.sections[0].elements[0].button.texture.ID);
                this->sapSpritesPanel.sections[0].elements[0].button.texture.ID = 0;
                this->sapSpritesPanel.sections[0].elements[0].button.texture.title = "";
                glDeleteTextures(1, &this->sapSpritesPanel.sections[0].elements[1].button.texture.ID);
                this->sapSpritesPanel.sections[0].elements[0].button.texture.ID = 0;
                this->sapSpritesPanel.sections[0].elements[0].button.texture.title = "";
            }
            else{
                if(!this->sapSpritesPanel.hover && *Mouse::LClick()){
                    this->sapSpritesPanelActive = false;
                    glDeleteTextures(1, &this->sapSpritesPanel.sections[0].elements[0].button.texture.ID);
                    this->sapSpritesPanel.sections[0].elements[0].button.texture.ID = 0;
                    this->sapSpritesPanel.sections[0].elements[0].button.texture.title = "";
                    glDeleteTextures(1, &this->sapSpritesPanel.sections[0].elements[1].button.texture.ID);
                    this->sapSpritesPanel.sections[0].elements[0].button.texture.ID = 0;
                    this->sapSpritesPanel.sections[0].elements[0].button.texture.title = "";
                }
            }
        }

        dialogControl.updateEnd(timer,0.15f);

        ShaderSystem::buttonShader().use();

        //If pressed any of the texture select the texture
        for (size_t i = 0; i < this->textureSelectingPanel.sections[0].elements.size(); i++)
        {
            if(this->textureSelectingPanel.sections[0].elements[i].button.hover && *Mouse::LClick()){
                selectedTextureIndex = i;
            }
        }

        // Pressed to the select button
        if(this->subPanel.sections[0].elements[0].button.clicked){
            this->sapSpritesPanelActive = true;
            sapSpritesPanel.pos = this->subPanel.pos;
            sapSpritesPanel.pos.x += this->subPanel.scale.x + sapSpritesPanel.scale.x;
            sapSpritesPanel.pos.y -= this->subPanel.scale.y - sapSpritesPanel.scale.y;
            sapSpritesPanel.pos.y += this->subPanel.sections[0].elements[0].button.panelOffset;
        }
        
        // Pressed to the add texture from the computer button
        if(this->subPanel.sections[0].elements[1].button.clicked){
            std::string test = showFileSystemObjectSelectionDialog("Select a texture.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_TEXTURE, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);
            
            if(test.size()){
                Texture txtr;
                txtr.load(test.c_str());
                receivedTexturePack.textures.push_back(txtr);
            }
        }

        // Pressed to the add texture from the library button
        if(this->subPanel.sections[0].elements[2].button.clicked){
        
            //Create the texture class
            Texture txtr;
            txtr.title = "TextureSelectionDialogRes";

            showTextureSelectionDialog(txtr, 512);

            //Send the created texture to the library
            if(txtr.ID){
                receivedTexturePack.textures.push_back(txtr);
            }
        }

        // Pressed to the add folder from the computer button
        if(this->subPanel.sections[0].elements[3].button.clicked){
            std::string test = showFileSystemObjectSelectionDialog("Select a folder.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_TEXTURE, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FOLDER);
            
            if(test.size()){
                receivedTexturePack.load(test);
            }
        }
        
        if(this->subPanel.sections[0].elements[4].button.clicked){
            if(this->selectedTextureIndex < receivedTexturePack.textures.size()){
                receivedTexturePack.textures.erase(receivedTexturePack.textures.begin() + this->selectedTextureIndex);
                if(this->selectedTextureIndex != 0)
                    this->selectedTextureIndex--;
            }
        }

        if(this->subPanel.sections[0].elements[5].button.clicked){
            Library::addTexture(receivedTexturePack.generateSpriteTexture());
            ShaderSystem::buttonShader().use();
        }

        //End the dialog
        if((getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE)) || (!this->bgPanel.hover && *Mouse::LClick())){
            if(!this->subPanel.sections[0].elements[2].button.clicked){
                dialogControl.unActivate();
                selectedTextureIndex = 0;
            }
        }

        if(!this->dialogControl.isActive())
            break;
            
        dialogControl.updateEnd(timer,0.15f);

        getContext()->window.swapBuffers();

        //Set mouse states to default
        *Mouse::LClick() = false;
        *Mouse::RClick() = false;
        *Mouse::MClick() = false;
        *Mouse::LDoubleClick() = false;
        *Mouse::mouseOffset() = glm::vec2(0);
        *Mouse::mods() = 0;
        *Mouse::mouseScroll() = 0;
        *Mouse::action() = 0;

        //Set keyboard states to default
        textRenderer.keyInput = false;
        textRenderer.mods = 0;

        Settings::defaultFramebuffer()->render();    
        Settings::defaultFramebuffer()->setViewport();    
    }
}




static void drawBG(
                    unsigned int bgTexture 
                )
{
    ShaderSystem::defaultFramebufferShader().use();
    ShaderSystem::defaultFramebufferShader().setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f));
    ShaderSystem::defaultFramebufferShader().setVec3("pos", glm::vec3(0.5f, 0.5f, 0.9f));
    ShaderSystem::defaultFramebufferShader().setVec2("scale", glm::vec2(0.5f));
    
    ShaderSystem::defaultFramebufferShader().setInt("txtr", 0);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, bgTexture);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    ShaderSystem::buttonShader().use();
}

static void updateTextureSelectingPanelElements(Panel& textureSelectingPanel, TexturePack &receivedTexturePack, int selectedTextureMode){
    textureSelectingPanel.sections.clear();
    std::vector<Element> sectionElements;
    
    for (size_t i = 0; i < receivedTexturePack.textures.size(); i++)
    {
        sectionElements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2, 3.f),"", receivedTexturePack.textures[i], 0.f,false)));
    }

    textureSelectingPanel.sections.push_back
                                            (
                                                Section
                                                        (
                                                            Element(Button()),
                                                            sectionElements
                                                        )
                                            );  
}