
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

#include "3D/ThreeD.hpp" 

#include "GUI/GUI.hpp" 

#include "UTIL/Mouse/Mouse.hpp" 
#include "UTIL/Library/Library.hpp" 
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp" 

#include "./HistoryActions/HistoryActions.hpp" 

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

#ifndef LOGDIALOG_REGISTERING_HPP
#define LOGDIALOG_REGISTERING_HPP

void LogDialog::registerPaintingAction(
                                        const std::string title, const Texture icon, 
                                        Texture albedo, bool albedoPainted, 
                                        Texture roughness, bool roughnessPainted, 
                                        Texture metallic, bool metallicPainted,
                                        Texture normal, bool normalPainted, 
                                        Texture height, bool heightPainted, 
                                        Texture ao, bool aoPainted
                                    )
{

    if(albedoPainted){
        albedo.writeTMP("_history_" + std::to_string(this->history_action_records.actions_Painting[albedo.ID].size()) + "_" + std::to_string(albedo.ID));
        this->history_action_records.actions_Painting[albedo.ID].push_back(PaintingAction(title, icon, albedo));
    }

    if(roughnessPainted){
        roughness.writeTMP("_history_" + std::to_string(this->history_action_records.actions_Painting[roughness.ID].size()) + "_" + std::to_string(roughness.ID));
        this->history_action_records.actions_Painting[roughness.ID].push_back(PaintingAction(title, icon, roughness));
    }
    
    if(metallicPainted){
        metallic.writeTMP("_history_" + std::to_string(this->history_action_records.actions_Painting[metallic.ID].size()) + "_" + std::to_string(metallic.ID));
        this->history_action_records.actions_Painting[metallic.ID].push_back(PaintingAction(title, icon, metallic));
    }
    
    if(normalPainted){
        normal.writeTMP("_history_" + std::to_string(this->history_action_records.actions_Painting[normal.ID].size()) + "_" + std::to_string(normal.ID));
        this->history_action_records.actions_Painting[normal.ID].push_back(PaintingAction(title, icon, normal));
    }
    
    if(heightPainted){
        height.writeTMP("_history_" + std::to_string(this->history_action_records.actions_Painting[height.ID].size()) + "_" + std::to_string(height.ID));
        this->history_action_records.actions_Painting[height.ID].push_back(PaintingAction(title, icon, height));
    }
    
    if(aoPainted){
        ao.writeTMP("_history_" + std::to_string(this->history_action_records.actions_Painting[ao.ID].size()) + "_" + std::to_string(ao.ID));
        this->history_action_records.actions_Painting[ao.ID].push_back(PaintingAction(title, icon, ao));
    }
    
    if(roughnessPainted || metallicPainted || normalPainted || heightPainted || aoPainted){
        this->history_action_records.actions_MultiChannelPainting.push_back(PaintingAction(title, icon, albedo, albedoPainted, roughness, roughnessPainted, metallic, metallicPainted, normal, normalPainted, height, heightPainted, ao, aoPainted));
    }

}

//void LogDialog::registerMultiMatChannelPaintingAction(const std::string)

void LogDialog::registerTextureDeletionAction(const std::string title, const Texture icon, Texture texture, const int index){
    this->history_action_records.actions_Library.push_back(LibraryAction(title, TEXTURE_DELETION_ACTION, icon, texture, index));
}

void LogDialog::registerTextureAdditionAction(const std::string title, const Texture icon, Texture texture, const int index){
    this->history_action_records.actions_Library.push_back(LibraryAction(title, TEXTURE_ADDITION_ACTION, icon, texture, index));
}

void LogDialog::registerMaterialDeletionAction(const std::string title, const Texture icon, Material material, const int index){
    this->history_action_records.actions_Library.push_back(LibraryAction(title, MATERIAL_DELETION_ACTION, icon, material, index));
}

void LogDialog::registerMaterialAdditionAction(const std::string title, const Texture icon, Material material, const int index){
    this->history_action_records.actions_Library.push_back(LibraryAction(title, MATERIAL_ADDITION_ACTION, icon, material, index));
}

void LogDialog::registerBrushDeletionAction(const std::string title, const Texture icon, Brush brush, const int index){
    this->history_action_records.actions_Library.push_back(LibraryAction(title, BRUSH_DELETION_ACTION, icon, brush, index));
}

void LogDialog::registerBrushAdditionAction(const std::string title, const Texture icon, Brush brush, const int index){
    this->history_action_records.actions_Library.push_back(LibraryAction(title, BRUSH_ADDITION_ACTION, icon, brush, index));
}

void LogDialog::registerBrushChangedAction(const std::string title, const Texture icon, Brush brush, const int index){
    this->history_action_records.actions_Library.push_back(LibraryAction(title, BRUSH_CHANGED_ACTION, icon, brush, index));
}

void LogDialog::registerImageEditorAction(const std::string title, const Texture icon, Texture texture){
    this->history_action_records.actions_Library.push_back(LibraryAction(title, TEXTURE_IMAGE_EDITOR_ACTION, icon, texture, Texture(texture.duplicateTexture("registerImageEditorAction"))));
}

void LogDialog::registerLibraryElementRenamingAction(const std::string title, const Texture icon, const int index, const std::string element, const std::string name){
    this->history_action_records.actions_Library.push_back(LibraryAction(title, LIBRARY_ELEMENT_RENAMING, icon, index, element, name));
}

void LogDialog::registerVectorAction(const std::string title, VectorScene* vectorScene){
    if(vectorScene == nullptr)
        return;

    //this->history_action_records.actions_Vectors[vectorScene].push_back(VectorsAction(title, *vectorScene));
}

void LogDialog::registerFaceSelectionAction(const std::string title, std::vector<byte> primitivesArray, int meshI){
    this->history_action_records.actions_FaceSelection.push_back(FaceSelectionAction(title, FACE_SELECTION_PAINTER_ACTION, primitivesArray, meshI));
}

void LogDialog::registerFaceSelectionActionObjectTexturingDialog(const std::string title, std::vector<std::vector<byte>> primitivesArray){
    this->history_action_records.actions_FaceSelection.push_back(FaceSelectionAction(title, FACE_SELECTION_OBJECTTEXTURING_ACTION, primitivesArray));
}

void LogDialog::registerTextureFieldAction(const std::string title, std::vector<TextureField> fields){
    this->history_action_records.actions_TextureFields.push_back(TextureFieldsAction(title, fields));
}

void LogDialog::registerMaterialAction(const std::string title, Material material){
    this->history_action_records.actions_MaterialEditor.push_back(MaterialEditorAction(title, material.duplicateMaterial()));
}




void LogDialog::registerButtonAction(const std::string title, const Texture icon, Button* button, Button previousButton){
    Texture previousBtnTexture = previousButton.texture.duplicateTexture("registerButtonAction");
    previousBtnTexture.proceduralProps = previousButton.texture.proceduralProps;
    previousBtnTexture.title = previousButton.texture.title;

    //__actions.push_back(Action(title, BUTTON_ACTION, icon, BtnAction(button, previousButton, previousBtnTexture)));
}

void LogDialog::registerNodeAction(const std::string title, const Texture icon){
    //__actions.push_back(Action(title, NODE_ACTION, icon, *NodeScene::getNodeArrayPointer()));
}

#endif // LOGDIALOG_REGISTERING_HPP