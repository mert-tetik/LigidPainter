
/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

All rights reserved.

Official GitHub Link : https:github.com/mert-tetik/LigidPainter
Official Web Page : https:ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    TODO : Check multithreading effects

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

#include "./HistoryActions/HistoryActions.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

/*
void LogDialog::undo(){
    if(this->activeHistoryMode == HISTORY_VECTORS_MODE && actions_Vectors.size()){
        if(actions_Vectors[actions_Vectors.size() - 1].ID == VECTOR_ACTION){
            //painter.vectorStrokes = actions_Vectors[actions_Vectors.size() - 1].vectorStrokes;
            actions_Vectors.pop_back();
        }
        else if(actions_Vectors[actions_Vectors.size() - 1].ID == VECTOR_ACTION_3D){
            //painter.vectorStrokes3D = actions_Vectors[actions_Vectors.size() - 1].vectorStrokes3D;
            actions_Vectors.pop_back();
        }
        
    }
    if(this->activeHistoryMode == HISTORY_FACESELECTION_MODE && actions_FaceSelection.size()){
        FaceSelectionAction action = actions_FaceSelection[actions_FaceSelection.size() - 1];

        std::vector<int> changedIndices = {};
        
        if(action.ID == FACE_SELECTION_PAINTER_ACTION){
            if(action.meshI < getScene()->model->meshes.size()){
                if(getScene()->get_selected_mesh()->face_selection_data.selectedPrimitiveIDs.size() == action.primitivesArray.size()){
                    getScene()->get_selected_mesh()->face_selection_data.selectedPrimitiveIDs = action.primitivesArray;
                    update_face_selection_buffers(&getScene()->model->meshes[action.meshI], false);
                }
                else{
                    LGDLOG::start << "ERROR : Undo face selection failed - Mesh data doesn't match" << LGDLOG::end;
                }
                
            }
            else    
                LGDLOG::start << "ERROR : Undo face selection failed - Invalid mesh index" << LGDLOG::end;
        }

        actions_FaceSelection.pop_back();
    }
    if(this->activeHistoryMode == HISTORY_TEXTUREFIELDS_MODE && actions_TextureFields.size()){
        TextureFieldsAction action = actions_TextureFields[actions_TextureFields.size() - 1];
        
        if(action.fields.size() == getTextureFieldScene()->texture_fields.size()){
            for (size_t i = 0; i < action.fields.size(); i++)
            {
                if(action.fields[i].flippedH != getTextureFieldScene()->texture_fields[i].flippedH){
                    action.fields[i].texture.flipTexture(true, false);
                    Settings::defaultFramebuffer()->FBO.bind();
                }
                if(action.fields[i].flippedV != getTextureFieldScene()->texture_fields[i].flippedV){
                    action.fields[i].texture.flipTexture(false, true);
                    Settings::defaultFramebuffer()->FBO.bind();
                }
            }
        }
        
        
        getTextureFieldScene()->texture_fields = action.fields;

        for (size_t i = 0; i < getTextureFieldScene()->texture_fields.size(); i++)
        {
            getTextureFieldScene()->texture_fields[i].updateWrapBox();
        }
        

        actions_TextureFields.pop_back();
    }
    if(this->activeHistoryMode == HISTORY_MATERIALEDITOR_MODE && actions_MaterialEditor.size()){
        
        MaterialEditorAction action = actions_MaterialEditor[actions_MaterialEditor.size() - 1];
        if(dialog_materialEditor.material)
            dialog_materialEditor.material->deleteBuffers();

        *dialog_materialEditor.material = action.material;
        dialog_materialEditor.updateTheMaterial = true;
        if(dialog_materialEditor.selectedMaterialModifierIndex < action.material.materialModifiers.size())
            dialog_materialEditor.modifiersPanel.sections = action.material.materialModifiers[dialog_materialEditor.selectedMaterialModifierIndex].sections;
        dialog_materialEditor.material->updateMaterialDisplayingTexture(512, false, dialog_materialEditor.displayerCamera, dialog_materialEditor.displayModeComboBox.selectedIndex, true);

        for (size_t i = 0; i < dialog_materialEditor.material->materialShortcuts.size(); i++)
        {
            dialog_materialEditor.material->materialShortcuts[i].updateElement(*dialog_materialEditor.material, dialog_materialEditor.material->materialShortcuts[i].modI);
        }

        Library::setChanged(true);

        actions_MaterialEditor.pop_back();
    }
    
    if(this->activeHistoryMode == HISTORY_PAINTING_MODE){
        if(panel_displaying_modes.selectedElement == 1){
            if(actions_MultiChannelPainting.size()){
                
                if(actions_MultiChannelPainting[actions_MultiChannelPainting.size() - 1].albedoPainted){
                    Texture* albedo = &actions_MultiChannelPainting[actions_MultiChannelPainting.size() - 1].albedo; 
                    if(actions_Painting[albedo->ID].size()){
                        albedo->readTMP("_history_" + std::to_string(actions_Painting[albedo->ID].size() - 1) + "_" + std::to_string(albedo->ID));
                        actions_Painting[albedo->ID].pop_back();
                    }
                }

                if(actions_MultiChannelPainting[actions_MultiChannelPainting.size() - 1].roughnessPainted){
                    Texture* roughness = &actions_MultiChannelPainting[actions_MultiChannelPainting.size() - 1].roughness; 
                    if(actions_Painting[roughness->ID].size()){
                        roughness->readTMP("_history_" + std::to_string(actions_Painting[roughness->ID].size() - 1) + "_" + std::to_string(roughness->ID));
                        actions_Painting[roughness->ID].pop_back();
                    }
                }

                if(actions_MultiChannelPainting[actions_MultiChannelPainting.size() - 1].metallicPainted){
                    Texture* metallic = &actions_MultiChannelPainting[actions_MultiChannelPainting.size() - 1].metallic; 
                    if(actions_Painting[metallic->ID].size()){
                        metallic->readTMP("_history_" + std::to_string(actions_Painting[metallic->ID].size() - 1) + "_" + std::to_string(metallic->ID));
                        actions_Painting[metallic->ID].pop_back();
                    }
                }

                if(actions_MultiChannelPainting[actions_MultiChannelPainting.size() - 1].normalPainted){
                    Texture* normalMap = &actions_MultiChannelPainting[actions_MultiChannelPainting.size() - 1].normal; 
                    if(actions_Painting[normalMap->ID].size()){
                        normalMap->readTMP("_history_" + std::to_string(actions_Painting[normalMap->ID].size() - 1) + "_" + std::to_string(normalMap->ID));
                        actions_Painting[normalMap->ID].pop_back();
                    }
                }

                if(actions_MultiChannelPainting[actions_MultiChannelPainting.size() - 1].heightPainted){
                    Texture* heightMap = &actions_MultiChannelPainting[actions_MultiChannelPainting.size() - 1].height; 
                    if(actions_Painting[heightMap->ID].size()){
                        heightMap->readTMP("_history_" + std::to_string(actions_Painting[heightMap->ID].size() - 1) + "_" + std::to_string(heightMap->ID));
                        actions_Painting[heightMap->ID].pop_back();
                    }
                }

                if(actions_MultiChannelPainting[actions_MultiChannelPainting.size() - 1].aoPainted){
                    Texture* ambientOcclusion = &actions_MultiChannelPainting[actions_MultiChannelPainting.size() - 1].ao; 
                    if(actions_Painting[ambientOcclusion->ID].size()){
                        ambientOcclusion->readTMP("_history_" + std::to_string(actions_Painting[ambientOcclusion->ID].size() - 1) + "_" + std::to_string(ambientOcclusion->ID));
                        actions_Painting[ambientOcclusion->ID].pop_back();
                    }
                }

                actions_MultiChannelPainting.pop_back();
            }
        }
        else{
            if(actions_Painting[panel_library_selected_texture.ID].size()){
                panel_library_selected_texture.readTMP("_history_" + std::to_string(actions_Painting[panel_library_selected_texture.ID].size() - 1) + "_" + std::to_string(panel_library_selected_texture.ID));
                actions_Painting[panel_library_selected_texture.ID].pop_back();
            }
        }
    }

    if(this->activeHistoryMode == HISTORY_LIBRARY_MODE && actions_Library.size()){
        if(actions_Library[actions_Library.size()-1].ID == TEXTURE_ADDITION_ACTION){
            glDeleteTextures(1, &actions_Library[actions_Library.size()-1].texture.ID);
            Library::getTextureVectorPointer()->erase(Library::getTextureVectorPointer()->begin() + actions_Library[actions_Library.size()-1].textureIndex);
        }
        else if(actions_Library[actions_Library.size()-1].ID == TEXTURE_DELETION_ACTION){
            Library::getTextureVectorPointer()->insert(Library::getTextureVectorPointer()->begin() + actions_Library[actions_Library.size()-1].textureIndex, actions_Library[actions_Library.size()-1].texture);
        }
        else if(actions_Library[actions_Library.size()-1].ID == TEXTURE_IMAGE_EDITOR_ACTION){
            glm::ivec2 res;
            res = actions_Library[actions_Library.size()-1].alteredTexture.getResolution();
            char* pxs = new char[res.x * res.y * 4];
            actions_Library[actions_Library.size()-1].alteredTexture.getData(pxs);
            
            glDeleteTextures(1, &actions_Library[actions_Library.size()-1].alteredTexture.ID);

            actions_Library[actions_Library.size()-1].texture.update(pxs, res.x, res.y);

            delete[] pxs;
        }
        else if(actions_Library[actions_Library.size()-1].ID == LIBRARY_ELEMENT_RENAMING){
            if(actions_Library[actions_Library.size()-1].element == "TEXTURE" && actions_Library[actions_Library.size()-1].textureIndex < Library::getTextureArraySize())
                Library::getTexture(actions_Library[actions_Library.size()-1].textureIndex)->title = actions_Library[actions_Library.size()-1].name;
            
            if(actions_Library[actions_Library.size()-1].element == "MATERIAL" && actions_Library[actions_Library.size()-1].textureIndex < Library::getMaterialArraySize())
                Library::getMaterial(actions_Library[actions_Library.size()-1].textureIndex)->title = actions_Library[actions_Library.size()-1].name;
            
            if(actions_Library[actions_Library.size()-1].element == "BRUSH" && actions_Library[actions_Library.size()-1].textureIndex < Library::getBrushArraySize())
                Library::getBrush(actions_Library[actions_Library.size()-1].textureIndex)->title = actions_Library[actions_Library.size()-1].name;
        }
        else if(actions_Library[actions_Library.size()-1].ID == MATERIAL_ADDITION_ACTION){
            actions_Library[actions_Library.size()-1].material.deleteBuffers();
            Library::getMaterialVectorPointer()->erase(Library::getMaterialVectorPointer()->begin() + actions_Library[actions_Library.size()-1].textureIndex);
        }
        else if(actions_Library[actions_Library.size()-1].ID == MATERIAL_DELETION_ACTION){
            Library::getMaterialVectorPointer()->insert(Library::getMaterialVectorPointer()->begin() + actions_Library[actions_Library.size()-1].textureIndex, actions_Library[actions_Library.size()-1].material);
        }
        else if(actions_Library[actions_Library.size()-1].ID == BRUSH_ADDITION_ACTION){
            glDeleteTextures(1, &actions_Library[actions_Library.size()-1].brush.displayingTexture.ID);
            glDeleteTextures(1, &actions_Library[actions_Library.size()-1].brush.properties.brushTexture.ID);
            Library::getBrushVectorPointer()->erase(Library::getBrushVectorPointer()->begin() + actions_Library[actions_Library.size()-1].textureIndex);
        }
        else if(actions_Library[actions_Library.size()-1].ID == BRUSH_DELETION_ACTION){
            Library::getBrushVectorPointer()->insert(Library::getBrushVectorPointer()->begin() + actions_Library[actions_Library.size()-1].textureIndex, actions_Library[actions_Library.size()-1].brush);
        }
        else if(actions_Library[actions_Library.size()-1].ID == BRUSH_CHANGED_ACTION){
            *Library::getBrush(actions_Library[actions_Library.size()-1].textureIndex) = actions_Library[actions_Library.size()-1].brush;
            Library::getBrush(actions_Library[actions_Library.size()-1].textureIndex)->properties.brushTexture.generateProceduralDisplayingTexture(512, false);
            Library::getBrush(actions_Library[actions_Library.size()-1].textureIndex)->updateDisplayTexture(0.1);
        }

        Library::nameControl();
        Library::setChanged(true);
        actions_Library.pop_back();
    }
}
*/