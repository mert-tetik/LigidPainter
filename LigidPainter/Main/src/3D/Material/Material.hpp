
/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef LGD_MATERIAL_HPP
#define LGD_MATERIAL_HPP

#include "UTIL/Util.hpp"
#include "ShaderSystem/Shader.hpp"
#include <iostream>

/// @brief Forward decleration for the @ref Section structure
struct Section;

/// @brief Forward decleration for the @ref Element structure
struct Element;

/// @brief Forward decleration for the @ref Camera structure
struct Camera;

/// @brief Forward decleration for the @ref Mesh class
class Mesh;

/// @brief Forward decleration for the @ref Material class
class Material;

//-------------- MATERIAL MODIFIER ------------

#define TEXTURE_MATERIAL_MODIFIER 0
#define DUST_MATERIAL_MODIFIER 1
#define ASPHALT_MATERIAL_MODIFIER 2
#define LIQUID_MATERIAL_MODIFIER 3
#define MOSS_MATERIAL_MODIFIER 4
#define RUST_MATERIAL_MODIFIER 5
#define SKIN_MATERIAL_MODIFIER 6
#define SOLID_MATERIAL_MODIFIER 7
#define WOODEN_MATERIAL_MODIFIER 8

class MaterialModifier
{
public:
    //0 = albedo
    //1 = roughness
    //2 = metallic 
    //3 = normal map
    //4 = height map
    //5 = ambient Occlusion

    Shader shader;  
    int modifierIndex;
    std::vector<Section> sections;   
    std::string title;

    bool hide = false;

    Texture maskTexture;

    void updateMaterialChannels(Material &material, Mesh &mesh, int textureResolution, int curModI, Texture meshMask, Texture selectedObjectPrimitivesTxtr, bool noPrevTxtrMode);

    //Constructors
    MaterialModifier();
    MaterialModifier(int modifierIndex);

    //Public member functions
    std::vector<Section> createTextureModifier();
    std::vector<Section> createDustModifier();
    std::vector<Section> createAsphaltModifier();
    std::vector<Section> createLiquidModifier();
    std::vector<Section> createMossModifier();
    std::vector<Section> createRustModifier();
    std::vector<Section> createSkinModifier();
    std::vector<Section> createWoodenModifier();
    std::vector<Section> createSolidModifier();

};


//-------------- MATERIAL ------------

struct MaterialShortcut{
    std::string title;
    Element* element = nullptr;
    Texture* maskTxtr = nullptr;
    int modI = 0;
    int secI = 0;
    int elementI = 0;

    MaterialShortcut(){}
    MaterialShortcut(std::string title, Element* element, Texture* maskTxtr, int modI, int secI, int elementI){
        this->title = title;
        this->element = element;
        this->maskTxtr = maskTxtr;
        this->modI = modI;
        this->secI = secI;
        this->elementI = elementI;
    }

    void updateElement(Material &material, int newModI);
};

class Material 
{
public:
    /// @brief title of the material (myMaterial_0)
    std::string title;

    /// @brief texture used to render the material to display
    Texture displayingTexture;

    /// @brief Framebuffer & Renderbuffer objects used to render the material to display
    Framebuffer displayingFBO;
    
    /// @brief Unique id to distinguish the materials & connect them to the nodes 
    int uniqueID;
    
    /// @brief Material modifiers of the material
    /// Assigned by the material editor dialog
    std::vector<MaterialModifier> materialModifiers;

    /// @brief Default constructor
    Material();
    
    /// @brief Init the channel textures of the material + displayingTexture + assign the unique ID value
    /// @param title Title of the material
    /// @param ID Generated unique ID for the material (Unique to other materials in the libray.materials)
    Material(std::string title,int ID);

    /// @brief Interpret the @ref materialModifiers and write the shader results to the material channels then update the displaying texture
    void updateMaterialDisplayingTexture(float textureRes, bool updateMaterial, Camera matCam, int displayingMode, bool useCustomCam);
    void updateMaterialDisplayingTexture(float textureRes, bool updateMaterial, Camera matCam, int displayingMode, bool useCustomCam, Framebuffer customFBO, Model &displayModel, int specificUpdateI);

    /// @brief Returns a new material with the same material modifiers and different OpenGL texture objects 
    Material duplicateMaterial();

    /// @brief Deletes all the OpenGL buffer objects inside of the material
    void deleteBuffers();

    std::vector<MaterialShortcut> materialShortcuts;
};

#endif 