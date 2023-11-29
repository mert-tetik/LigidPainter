
/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

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

// Defined in Painter/faceSelection.cpp
void updatePrimitivesArrayTexture(Texture& primitivesArrayTexture, std::vector<byte> primitivesArray, std::vector<byte>& prevPrimArray, Mesh& selectedMesh, std::vector<int>& changedIndices, bool updateAll);

void LogDialog::undo(Painter& painter, ObjectTexturingDialog& objectTexturingDialog, std::vector<TextureField>& paintingOverTextureFields, MaterialEditorDialog& materialEditorDialog){
    if(this->activeHistoryMode == HISTORY_VECTORS_MODE && actions_Vectors.size()){
        painter.vectorStrokes = actions_Vectors[actions_Vectors.size() - 1].vectorStrokes;
        actions_Vectors.pop_back();
        unded = true;
    }
    if(this->activeHistoryMode == HISTORY_OBJECTSELECTION_MODE && actions_ObjectSelection.size()){
        ObjectSelectionAction action = actions_ObjectSelection[actions_ObjectSelection.size() - 1];

        if(action.meshI < getModel()->meshes.size())
            getModel()->meshes[action.meshI].selectedObjectIndices = action.selectedObjectIndices;
        else    
            LGDLOG::start << "ERROR : Undo object selection failed - Invalid mesh index" << LGDLOG::end;

        actions_ObjectSelection.pop_back();
        unded = true;
    }
    if(this->activeHistoryMode == HISTORY_FACESELECTION_MODE && actions_FaceSelection.size()){
        FaceSelectionAction action = actions_FaceSelection[actions_FaceSelection.size() - 1];

        std::vector<int> changedIndices = {};
        
        if(action.ID == FACE_SELECTION_PAINTER_ACTION){
            if(action.meshI < getModel()->meshes.size()){
                if(painter.faceSelection.selectedPrimitiveIDs.size() == action.primitivesArray.size()){
                    painter.faceSelection.prevPrimArray = action.prevPrimArray;
                    painter.faceSelection.selectedPrimitiveIDs = action.primitivesArray;
                    updatePrimitivesArrayTexture(painter.faceSelection.selectedFaces, action.primitivesArray, action.primitivesArray, getModel()->meshes[action.meshI], changedIndices, true);
                }
                else{
                    LGDLOG::start << "ERROR : Undo face selection failed - Mesh data doesn't match" << LGDLOG::end;
                }
                
            }
            else    
                LGDLOG::start << "ERROR : Undo face selection failed - Invalid mesh index" << LGDLOG::end;
        }
        else{
            if(action.primitivesArray_M.size() == objectTexturingDialog.faceSelection.size() && action.primitivesArray_M.size() == getModel()->meshes.size()){
                for (size_t i = 0; i < action.primitivesArray_M.size(); i++)
                {
                    objectTexturingDialog.faceSelection[i].prevPrimArray = action.primitivesArray_M[i];
                    objectTexturingDialog.faceSelection[i].selectedPrimitiveIDs = action.prevPrimArray_M[i];
                    updatePrimitivesArrayTexture(objectTexturingDialog.faceSelection[i].selectedFaces, action.primitivesArray_M[i], action.primitivesArray_M[i], getModel()->meshes[i], changedIndices, true);
                }
            }
            else{
                LGDLOG::start << "ERROR : Undo face selection failed - Mesh data doesn't match" << LGDLOG::end;
            }
        }


        actions_FaceSelection.pop_back();
        unded = true;
    }
    if(this->activeHistoryMode == HISTORY_TEXTUREFIELDS_MODE && actions_TextureFields.size()){
        TextureFieldsAction action = actions_TextureFields[actions_TextureFields.size() - 1];
        
        if(action.fields.size() == paintingOverTextureFields.size()){
            for (size_t i = 0; i < action.fields.size(); i++)
            {
                if(action.fields[i].flippedH != paintingOverTextureFields[i].flippedH){
                    action.fields[i].texture.flipTexture(true, false);
                    Settings::defaultFramebuffer()->FBO.bind();
                }
                if(action.fields[i].flippedV != paintingOverTextureFields[i].flippedV){
                    action.fields[i].texture.flipTexture(false, true);
                    Settings::defaultFramebuffer()->FBO.bind();
                }
            }
        }
        
        
        paintingOverTextureFields = action.fields;

        actions_TextureFields.pop_back();
        unded = true;
    }
    if(this->activeHistoryMode == HISTORY_MATERIALEDITOR_MODE && actions_MaterialEditor.size()){
        MaterialEditorAction action = actions_MaterialEditor[actions_MaterialEditor.size() - 1];
        if(materialEditorDialog.material)
            materialEditorDialog.material->deleteBuffers();

        *materialEditorDialog.material = action.material;
        materialEditorDialog.updateLayerPanel(*materialEditorDialog.material);
        if(materialEditorDialog.selectedMaterialModifierIndex < action.material.materialModifiers.size())
            materialEditorDialog.modifiersPanel.sections = action.material.materialModifiers[materialEditorDialog.selectedMaterialModifierIndex].sections;
        materialEditorDialog.material->updateMaterialDisplayingTexture(512, false, materialEditorDialog.displayerCamera, 0);

        Library::setChanged(true);

        actions_MaterialEditor.pop_back();
        unded = true;
    }
    
    if(this->activeHistoryMode == HISTORY_PAINTING_MODE && actions_Painting.size()){
        try
        {
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

                        if(indexVal == actions_Painting.size() - 1){
                            for (size_t i = 0; i < Library::getTextureArraySize(); i++)
                            {   
                                if(Library::getTexture(i)->ID == IDVal){
                                    Library::getTexture(i)->readTMP("_history_" + std::to_string(indexVal) + "_" + std::to_string(IDVal));
                                }
                            }
                        }
                    }
                }
            }
        }
        catch (const std::filesystem::filesystem_error& ex) {
            LGDLOG::start << "ERROR : Filesystem : Location ID 337523 " << ex.what() << LGDLOG::end;
        }

        actions_Painting.pop_back();
    }

    else if(actions_Library[actions_Library.size()-1].ID == TEXTURE_ADDITION_ACTION){
        Library::getTextureVectorPointer()->erase(Library::getTextureVectorPointer()->begin() + actions_Library[actions_Library.size()-1].textureIndex);
        Library::setChanged(true);
    }
}