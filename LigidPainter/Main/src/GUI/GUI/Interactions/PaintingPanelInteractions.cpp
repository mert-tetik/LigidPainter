/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/


#include<glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "MouseSystem/Mouse.hpp"
#include "SettingsSystem/Settings.hpp"
#include "LibrarySystem/Library.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <filesystem>


void UI::paintingPanelInteraction(
                                Painter &painter 
                            )
{


    painter.oSide.active = true;
    
    painter.oXSide.active = mirrorSection.elements[0].checkBox.clickState1; 
    painter.mirrorXOffset = mirrorSection.elements[1].rangeBar.value;
    painter.oYSide.active = mirrorSection.elements[2].checkBox.clickState1; 
    painter.mirrorYOffset = mirrorSection.elements[3].rangeBar.value;
    painter.oZSide.active = mirrorSection.elements[4].checkBox.clickState1; 
    painter.mirrorZOffset = mirrorSection.elements[5].rangeBar.value;


    painter.oXYSide.active = painter.oXSide.active && painter.oYSide.active;     
    painter.oXZSide.active = painter.oXSide.active && painter.oZSide.active; 
    painter.oYZSide.active = painter.oYSide.active && painter.oZSide.active; 
    painter.oXYZSide.active = painter.oXSide.active && painter.oYSide.active && painter.oZSide.active; 

    // Updating the depth texture if interacted with the gui elements related to mirroring
    if( 
        mirrorSection.elements[0].isInteracted() || 
        mirrorSection.elements[1].isInteracted() || 
        mirrorSection.elements[2].isInteracted() || 
        mirrorSection.elements[3].isInteracted() || 
        mirrorSection.elements[4].isInteracted() || 
        mirrorSection.elements[5].isInteracted()
    ) {
        painter.updateDepthTexture();
    }

    if(!painter.displayingBrush.displayingTexture.ID){
        // Init brush settings displaying brush
        painter.displayingBrush = Brush(
                                            painter.brushProperties.radius * 2.f,
                                            painter.brushProperties.spacing,
                                            painter.brushProperties.hardness,
                                            painter.brushProperties.sizeJitter,
                                            painter.brushProperties.scatter,
                                            painter.brushProperties.fade,
                                            painter.brushProperties.rotation,
                                            painter.brushProperties.rotationJitter,
                                            painter.brushProperties.alphaJitter,
                                            painter.brushProperties.individualTexture,
                                            painter.brushProperties.sinWavePattern,
                                            "BrushSettingsDisplayingBrush",
                                            painter.brushProperties.brushTexture
                                        );
    }

    if(getModel()->newModelAdded){
        paintingChannelsSection.clear(); 

        paintingChannelsSection.push_back(
                                            Section(
                                                Element(),
                                                {
                                                    Element(Button(ELEMENT_STYLE_STYLIZED, glm::vec2(2,2), "Auto Create Textures"  , Texture(), 7.f, false)),//4
                                                }
                                            )
                                        );
        
        for (size_t i = 0; i < getModel()->meshes.size(); i++)
        {
            paintingChannelsSection.push_back(
                                                Section(
                                                    SectionHolder(ColorPalette::secondColor, 2.f, getModel()->meshes[i].materialName),
                                                    {
                                                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2, 2),"Albedo Texture"  , getModel()->meshes[i].albedo, 1.f, true)),//4
                                                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2, 2),"Roughness Texture"  , getModel()->meshes[i].roughness, 1.f, true)),//4
                                                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2, 2),"Metallic Texture"  , getModel()->meshes[i].metallic, 1.f, true)),//4
                                                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2, 2),"Normal Map Texture"  , getModel()->meshes[i].normalMap, 1.f, true)),//4
                                                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2, 2),"Height Map Texture"  , getModel()->meshes[i].heightMap, 1.f, true)),//4
                                                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2, 2),"Ambient Occlusion Texture"  , getModel()->meshes[i].ambientOcclusion, 1.f, true)),//4
                                                        Element(Button(ELEMENT_STYLE_STYLIZED, glm::vec2(2, 2), "Material ID Texture"  , getModel()->meshes[i].materialIDTxtr, 2.f, false)),//4
                                                        Element(Button(ELEMENT_STYLE_BASIC, glm::vec2(2, 2), "Remove Material ID Texture"  , appTextures.X, 1.f, false))//4
                                                    }
                                                )
                                            );
        }
    }

    if(paintingChannelsSection.size()){
        if(paintingChannelsSection[0].elements.size()){
            if(paintingChannelsSection[0].elements[0].button.clicked)
                paintingChannelsAutoCreateTexturesDialog.dialogControl.activate();
        }
    }

    for (size_t secI = 1; secI < paintingChannelsSection.size(); secI++)
    {
        for (size_t elI = 0; elI < 6; elI++){
            if(paintingChannelsSection[secI].elements[elI].button.clickState1){
                this->paintingChannelsTextureSelectionPanelActive = true;
                this->paintingChannelsTextureSelectionPanel.pos = glm::vec3(50.f, 50.f, 0.7f);
            }
        }

        if(paintingChannelsSection[secI].elements[6].button.clicked){
            std::string test = showFileSystemObjectSelectionDialog("Select a material id texture.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_TEXTURE, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);
    
            if(test.size()){
                paintingChannelsSection[secI].elements[6].button.texture.load(test.c_str());
                getModel()->meshes[secI-1].materialIDColors = paintingChannelsSection[secI].elements[6].button.texture.getMaterialIDPalette();
                getModel()->meshes[secI-1].materialIDTxtrPath = test;
            }
        }
        if(paintingChannelsSection[secI].elements[7].button.clicked){
            for (size_t i = 0; i < getModel()->meshes[secI-1].materialIDColors.size(); i++)
            {
                glDeleteTextures(1, &getModel()->meshes[secI-1].materialIDColors[i].grayScaleTxtr.ID);
            }
            
            getModel()->meshes[secI-1].materialIDColors.clear();
            paintingChannelsSection[secI].elements.erase(paintingChannelsSection[secI].elements.begin() + 8, paintingChannelsSection[secI].elements.end());
            glDeleteTextures(1, &paintingChannelsSection[secI].elements[6].button.texture.ID);
            paintingChannelsSection[secI].elements[6].button.texture.ID = 0;
        }

        paintingChannelsSection[secI].elements.erase(paintingChannelsSection[secI].elements.begin() + 8, paintingChannelsSection[secI].elements.end());

        for (size_t i = 0; i < getModel()->meshes[secI-1].materialIDColors.size(); i++)
        {
            float btnOffset = 0.f;
            if(i == 0)
                btnOffset = 1.f;

            Button btn = Button(ELEMENT_STYLE_SOLID, glm::vec2(0.3f),"", Texture(), btnOffset, false);
            btn.color = glm::vec4(getModel()->meshes[secI-1].materialIDColors[i].color, 1.f);
            btn.color2 = btn.color;
            btn.outline = false;
            paintingChannelsSection[secI].elements.push_back(btn); 
        }
    }

    if(!objectTexturingDialog.dialogControl.isActive()){
        for (size_t i = 0; i < getModel()->meshes.size(); i++)
        {
            if(i + 1 >= paintingChannelsSection.size())
                break;
            
            getModel()->meshes[i].albedo = paintingChannelsSection[i + 1].elements[0].button.texture;
            getModel()->meshes[i].roughness = paintingChannelsSection[i + 1].elements[1].button.texture;
            getModel()->meshes[i].metallic = paintingChannelsSection[i + 1].elements[2].button.texture;
            getModel()->meshes[i].normalMap = paintingChannelsSection[i + 1].elements[3].button.texture;
            getModel()->meshes[i].heightMap = paintingChannelsSection[i + 1].elements[4].button.texture;
            getModel()->meshes[i].ambientOcclusion = paintingChannelsSection[i + 1].elements[5].button.texture;
            getModel()->meshes[i].materialIDTxtr = paintingChannelsSection[i + 1].elements[6].button.texture;
        }
    }
}