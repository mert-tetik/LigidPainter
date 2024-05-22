
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

extern std::vector<Texture> __textures;
extern std::vector<Material> __materials;
extern std::vector<Brush> __brushes;

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

extern size_t history_ID_val;
static size_t generate_unique_ID(){
    history_ID_val++;
    return history_ID_val;
}

void LogDialog::registerTextureAction(const std::string title){
    this->history_action_records.actions_Library.push_back(LibraryAction(title, __textures));
}
void LogDialog::registerMaterialAction(const std::string title){
    this->history_action_records.actions_Library.push_back(LibraryAction(title, __materials));
}
void LogDialog::registerBrushAction(const std::string title){
    this->history_action_records.actions_Library.push_back(LibraryAction(title, __brushes));
}
void LogDialog::registerTextureModificationAction(const std::string title, Texture texture){
    LibraryAction library_action = LibraryAction(title, texture);
    library_action.unique_id = generate_unique_ID();

    texture.writeTMP(std::to_string(library_action.unique_id));

    this->history_action_records.actions_Library.push_back(library_action);
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