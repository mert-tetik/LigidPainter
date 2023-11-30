
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
    Texture alteredTexture;
    int textureIndex;
    Material material;
    Brush brush;
    
    std::string element;
    std::string name;

    LibraryAction(std::string title, unsigned int ID, Texture icon, int index, std::string element, std::string name){
        this->title = title;
        this->ID = ID;
        this->icon = icon;
        this->textureIndex = index;
        this->element = element;
        this->name = name;
    }
    
    LibraryAction(std::string title, unsigned int ID, Texture icon, Texture texture){
        this->title = title;
        this->ID = ID;
        this->icon = icon;
        this->texture = texture;
    }

    LibraryAction(std::string title, unsigned int ID, Texture icon, Texture texture, Texture alteredTexture){
        this->title = title;
        this->ID = ID;
        this->icon = icon;
        this->texture = texture;
        this->alteredTexture = alteredTexture;
    }
    
    LibraryAction(std::string title, unsigned int ID, Texture icon, Texture texture, int textureIndex){
        this->title = title;
        this->ID = ID;
        this->icon = icon;
        this->texture = texture;
        this->textureIndex = textureIndex;
    }
    
    LibraryAction(std::string title, unsigned int ID, Texture icon, Material material, int textureIndex){
        this->title = title;
        this->ID = ID;
        this->icon = icon;
        this->material = material;
        this->textureIndex = textureIndex;
    }

    LibraryAction(std::string title, unsigned int ID, Texture icon, Brush brush, int textureIndex){
        this->title = title;
        this->ID = ID;
        this->icon = icon;
        this->brush = brush;
        this->textureIndex = textureIndex;
    }
};

struct PaintingAction{
    std::string title;
    Texture icon;

    Texture albedo; 
    bool albedoPainted;
    Texture roughness; 
    bool roughnessPainted;
    Texture metallic; 
    bool metallicPainted;
    Texture normal; 
    bool normalPainted;
    Texture height; 
    bool heightPainted;
    Texture ao; 
    bool aoPainted;

    PaintingAction(
                        std::string title, Texture icon,
                        Texture albedo, bool albedoPainted, 
                        Texture roughness, bool roughnessPainted, 
                        Texture metallic, bool metallicPainted,
                        Texture normal, bool normalPainted, 
                        Texture height, bool heightPainted, 
                        Texture ao, bool aoPainted
                    )
{
        this->title = title;
        this->icon = icon;
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
    Material material;

    MaterialEditorAction(){}

    MaterialEditorAction(std::string title, Material material){
        this->title = title;
        this->material = material;
    }
};

extern std::vector<LibraryAction> actions_Library;
extern std::vector<PaintingAction> actions_Painting;
extern std::vector<VectorsAction> actions_Vectors;
extern std::vector<ObjectSelectionAction> actions_ObjectSelection;
extern std::vector<FaceSelectionAction> actions_FaceSelection;
extern std::vector<TextureFieldsAction> actions_TextureFields;
extern std::vector<MaterialEditorAction> actions_MaterialEditor;

void registerPaintingAction(
                                const std::string title, const Texture icon, 
                                Texture albedo, bool albedoPainted, 
                                Texture roughness, bool roughnessPainted, 
                                Texture metallic, bool metallicPainted,
                                Texture normal, bool normalPainted, 
                                Texture height, bool heightPainted, 
                                Texture ao, bool aoPainted
                            )
{
    if(albedoPainted)
        albedo.writeTMP("_history_" + std::to_string(actions_Painting.size()) + "_" + std::to_string(albedo.ID));
    if(roughnessPainted)
        roughness.writeTMP("_history_" + std::to_string(actions_Painting.size()) + "_" + std::to_string(roughness.ID));
    if(metallicPainted)
        metallic.writeTMP("_history_" + std::to_string(actions_Painting.size()) + "_" + std::to_string(metallic.ID));
    if(normalPainted)
        normal.writeTMP("_history_" + std::to_string(actions_Painting.size()) + "_" + std::to_string(normal.ID));
    if(heightPainted)
        height.writeTMP("_history_" + std::to_string(actions_Painting.size()) + "_" + std::to_string(height.ID));
    if(aoPainted)
        ao.writeTMP("_history_" + std::to_string(actions_Painting.size()) + "_" + std::to_string(ao.ID));
    
    actions_Painting.push_back(PaintingAction(title, icon, albedo, albedoPainted, roughness, roughnessPainted, metallic, metallicPainted, normal, normalPainted, height, heightPainted, ao, aoPainted));
}

//void registerMultiMatChannelPaintingAction(const std::string)

void registerTextureDeletionAction(const std::string title, const Texture icon, Texture texture, const int index){
    actions_Library.push_back(LibraryAction(title, TEXTURE_DELETION_ACTION, icon, texture, index));
}

void registerTextureAdditionAction(const std::string title, const Texture icon, Texture texture, const int index){
    actions_Library.push_back(LibraryAction(title, TEXTURE_ADDITION_ACTION, icon, texture, index));
}

void registerMaterialDeletionAction(const std::string title, const Texture icon, Material material, const int index){
    actions_Library.push_back(LibraryAction(title, MATERIAL_DELETION_ACTION, icon, material, index));
}

void registerMaterialAdditionAction(const std::string title, const Texture icon, Material material, const int index){
    actions_Library.push_back(LibraryAction(title, MATERIAL_ADDITION_ACTION, icon, material, index));
}

void registerBrushDeletionAction(const std::string title, const Texture icon, Brush brush, const int index){
    actions_Library.push_back(LibraryAction(title, BRUSH_DELETION_ACTION, icon, brush, index));
}

void registerBrushAdditionAction(const std::string title, const Texture icon, Brush brush, const int index){
    actions_Library.push_back(LibraryAction(title, BRUSH_ADDITION_ACTION, icon, brush, index));
}

void registerBrushChangedAction(const std::string title, const Texture icon, Brush brush, const int index){
    actions_Library.push_back(LibraryAction(title, BRUSH_CHANGED_ACTION, icon, brush, index));
}

void registerImageEditorAction(const std::string title, const Texture icon, Texture texture){
    actions_Library.push_back(LibraryAction(title, TEXTURE_IMAGE_EDITOR_ACTION, icon, texture, Texture(texture.duplicateTexture())));
}

void registerLibraryElementRenamingAction(const std::string title, const Texture icon, const int index, const std::string element, const std::string name){
    actions_Library.push_back(LibraryAction(title, LIBRARY_ELEMENT_RENAMING, icon, index, element, name));
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

void registerMaterialAction(const std::string title, Material material){
    actions_MaterialEditor.push_back(MaterialEditorAction(title, material.duplicateMaterial()));
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