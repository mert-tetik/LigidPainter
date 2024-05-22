
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

void LogDialog::registerPaintingAction(const std::string title, Texture painted_texture){
    PaintingAction painting_action = PaintingAction(title, painted_texture);

    painted_texture.writeTMP(std::to_string(painting_action.unique_ID));

    this->history_action_records.actions_Painting[painted_texture.ID].push_back(painting_action);
}

void LogDialog::registerMultiChannelPaintingAction(const std::string title, MaterialChannels material_channels)
{
    MultiChannelPaintingAction painting_action = MultiChannelPaintingAction(title, material_channels);

    material_channels.albedo.writeTMP(std::to_string(painting_action.unique_ID) + "_0");
    material_channels.roughness.writeTMP(std::to_string(painting_action.unique_ID) + "_1");
    material_channels.metallic.writeTMP(std::to_string(painting_action.unique_ID) + "_2");
    material_channels.normalMap.writeTMP(std::to_string(painting_action.unique_ID) + "_3");
    material_channels.heightMap.writeTMP(std::to_string(painting_action.unique_ID) + "_4");
    material_channels.ambientOcclusion.writeTMP(std::to_string(painting_action.unique_ID) + "_5");

    this->history_action_records.actions_MultiChannelPainting[material_channels.albedo.ID].push_back(painting_action);
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

    this->history_action_records.actions_Vectors[vectorScene].push_back(VectorsAction(title, *vectorScene));
}

void LogDialog::registerFaceSelectionAction(const std::string title, Mesh* mesh){
    if(mesh == nullptr)
        return;

    this->history_action_records.actions_FaceSelection[mesh].push_back(FaceSelectionAction(title, mesh->face_selection_data));
}

void LogDialog::registerTextureFieldAction(const std::string title, TextureFieldScene* texture_field_scene){
    if(texture_field_scene == nullptr)
        return;
    
    this->history_action_records.actions_TextureFields[texture_field_scene].push_back(TextureFieldsAction(title, *texture_field_scene));
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