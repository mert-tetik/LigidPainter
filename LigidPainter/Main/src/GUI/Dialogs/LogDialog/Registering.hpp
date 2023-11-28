
/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https:github.com/mert-tetik/LigidPainter
Official Web Page : https:ligidtools.com/ligidpainter

---------------------------------------------------------------------------
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

#ifndef LOGDIALOG_REGISTERING_HPP
#define LOGDIALOG_REGISTERING_HPP

struct BtnAction{
    Button* button;
    Button previousBtn;
    Texture previousBtnTexture;

    BtnAction(){}
    BtnAction(Button* button, Button previousBtn, Texture previousBtnTexture){
        this->button = button; 
        this->previousBtn = previousBtn; 
        this->previousBtnTexture = previousBtnTexture; 
    }
};

struct LibraryAction{
    std::string title;
    unsigned int ID;
    Texture icon;
    Texture texture;
    int textureIndex;

    LibraryAction(std::string title, unsigned int ID, Texture icon, Texture texture){
        this->title = title;
        this->ID = ID;
        this->icon = icon;
        this->texture = texture;
    }
    
    LibraryAction(std::string title, unsigned int ID, Texture icon, Texture texture, int textureIndex){
        this->title = title;
        this->ID = ID;
        this->icon = icon;
        this->texture = texture;
        this->textureIndex = textureIndex;
    }
};

struct PaintingAction{
    std::string title;
    unsigned int ID;
    Texture icon;
    Texture texture;

    PaintingAction(std::string title, unsigned int ID, Texture icon, Texture texture){
        this->title = title;
        this->ID = ID;
        this->icon = icon;
        this->texture = texture;
    }
};

struct VectorsAction{
    std::string title;
    std::vector<VectorStroke> vectorStrokes;
    unsigned int ID;

    VectorsAction(){}

    VectorsAction(std::string title, unsigned int ID, std::vector<VectorStroke> vectorStrokes){
        this->title = title;
        this->vectorStrokes = vectorStrokes;
        this->ID = ID;
    }
};

struct ObjectSelectionAction{
    std::string title;
    std::vector<int> selectedObjectIndices;
    int meshI;

    ObjectSelectionAction(){}

    ObjectSelectionAction(std::string title, int meshI, std::vector<int> selectedObjectIndices){
        this->title = title;
        this->meshI = meshI;
        this->selectedObjectIndices = selectedObjectIndices;
    }
};

struct FaceSelectionAction{
    std::string title;
    std::vector<byte> primitivesArray;
    std::vector<byte> prevPrimArray;
    std::vector<std::vector<byte>> primitivesArray_M;
    std::vector<std::vector<byte>> prevPrimArray_M;
    int meshI;
    unsigned int ID;

    FaceSelectionAction(){}

    FaceSelectionAction(std::string title, unsigned int ID, std::vector<byte> primitivesArray, std::vector<byte> prevPrimArray, int meshI){
        this->title = title;
        this->ID = ID;
        this->primitivesArray = primitivesArray;
        this->prevPrimArray = prevPrimArray;
        this->meshI = meshI;
    }
    
    FaceSelectionAction(std::string title, unsigned int ID, std::vector<std::vector<byte>> primitivesArray_M, std::vector<std::vector<byte>> prevPrimArray_M){
        this->title = title;
        this->ID = ID;
        this->primitivesArray_M = primitivesArray_M;
        this->prevPrimArray_M = prevPrimArray_M;
    }
};

struct TextureFieldsAction{
    std::string title;
    std::vector<TextureField> fields;

    TextureFieldsAction(){}

    TextureFieldsAction(std::string title, std::vector<TextureField> fields){
        this->title = title;
        this->fields = fields;
    }
};

struct MaterialEditorAction{
    std::string title;
};

extern std::vector<LibraryAction> actions_Library;
extern std::vector<PaintingAction> actions_Painting;
extern std::vector<VectorsAction> actions_Vectors;
extern std::vector<ObjectSelectionAction> actions_ObjectSelection;
extern std::vector<FaceSelectionAction> actions_FaceSelection;
extern std::vector<TextureFieldsAction> actions_TextureFields;
extern std::vector<MaterialEditorAction> actions_MaterialEditor;

void registerTextureAction(const std::string title, const Texture icon, Texture texture){
    texture.writeTMP("_history_" + std::to_string(actions_Painting.size()) + "_" + std::to_string(texture.uniqueId));
    
    actions_Painting.push_back(PaintingAction(title, TEXTURE_UPDATING_ACTION, icon, texture));
}

void registerTextureDeletionAction(const std::string title, const Texture icon, Texture texture, const int index){
    texture.writeTMP("_history_" + std::to_string(actions_Library.size()) + "_" + std::to_string(texture.uniqueId));

    actions_Library.push_back(LibraryAction(title, TEXTURE_DELETION_ACTION, icon, texture, index));
}

void registerTextureAdditionAction(const std::string title, const Texture icon, Texture texture, const int index){
    actions_Library.push_back(LibraryAction(title, TEXTURE_ADDITION_ACTION, icon, texture, index));
}

void registerVectorAction(const std::string title, std::vector<VectorStroke> vectorStrokes){
    actions_Vectors.push_back(VectorsAction(title, VECTOR_ACTION, vectorStrokes));
}

void registerObjectSelectionAction(const std::string title, int meshI, std::vector<int> selectedObjectIndices){
    actions_ObjectSelection.push_back(ObjectSelectionAction(title, meshI, selectedObjectIndices));
}

void registerFaceSelectionAction(const std::string title, std::vector<byte> primitivesArray, std::vector<byte> prevPrimArray, int meshI){
    actions_FaceSelection.push_back(FaceSelectionAction(title, FACE_SELECTION_PAINTER_ACTION, primitivesArray, prevPrimArray, meshI));
}

void registerFaceSelectionActionObjectTexturingDialog(const std::string title, std::vector<std::vector<byte>> primitivesArray, std::vector<std::vector<byte>> prevPrimArray){
    actions_FaceSelection.push_back(FaceSelectionAction(title, FACE_SELECTION_OBJECTTEXTURING_ACTION, primitivesArray, prevPrimArray));
}

void registerTextureFieldAction(const std::string title, std::vector<TextureField> fields){
    actions_TextureFields.push_back(TextureFieldsAction(title, fields));
}





void registerButtonAction(const std::string title, const Texture icon, Button* button, Button previousButton){
    Texture previousBtnTexture = previousButton.texture.duplicateTexture();
    previousBtnTexture.proceduralProps = previousButton.texture.proceduralProps;
    previousBtnTexture.title = previousButton.texture.title;

    //__actions.push_back(Action(title, BUTTON_ACTION, icon, BtnAction(button, previousButton, previousBtnTexture)));
}

void registerNodeAction(const std::string title, const Texture icon){
    //__actions.push_back(Action(title, NODE_ACTION, icon, *NodeScene::getNodeArrayPointer()));
}

#endif // LOGDIALOG_REGISTERING_HPP