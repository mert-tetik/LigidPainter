
/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef LGD_MATERIAL_HPP
#define LGD_MATERIAL_HPP

#include "UTIL/Util.hpp"
#include "UTIL/Framebuffers/Framebuffer_Pool.hpp"
#include "UTIL/Texture/Texture.hpp"
#include "UTIL/Shader/Shader.hpp"
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

/*! @brief Channel textures of a material */
struct MaterialChannels{
    Texture albedo;
    Texture roughness;
    Texture metallic;
    Texture normalMap;
    Texture heightMap;
    Texture ambientOcclusion;

    MaterialChannels(){}
    MaterialChannels(Texture albedo, Texture roughness, Texture metallic, Texture normalMap, Texture heightMap, Texture ambientOcclusion){
        this->albedo = albedo;
        this->roughness = roughness;
        this->metallic = metallic;
        this->normalMap = normalMap;
        this->heightMap = heightMap;
        this->ambientOcclusion = ambientOcclusion;
    }
};

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
#define MATH_MATERIAL_MODIFIER 9

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

    void updateMaterialChannels(Material &material, int curModI, Model& model, Mesh &mesh, int textureResolution, bool noPrevTxtrMode);

    //Constructors
    MaterialModifier();
    MaterialModifier(int modifierIndex);


private:
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
    std::vector<Section> createMathModifier();

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
private:
public:
    /// @brief Material modifiers of the material
    /// Assigned by the material editor dialog
    std::vector<MaterialModifier> materialModifiers;
    std::vector<MaterialShortcut> materialShortcuts;

    bool material_selection_dialog_initialized = false;
    std::string material_selection_dialog_path = "";
    
    /// @brief title of the material (myMaterial_0)
    std::string title;

    /// @brief texture used to render the material to display
    Texture displayingTexture;

    /// @brief Default constructor
    Material();
    
    /// @brief Init the channel textures of the material + displayingTexture + assign the unique ID value
    /// @param title Title of the material
    /// @param ID Generated unique ID for the material (Unique to other materials in the libray.materials)
    Material(std::string title, std::vector<MaterialModifier> materialModifiers);

    /// @brief Interpret the @ref materialModifiers and write the shader results to the material channels then update the displaying texture
    void updateMaterialDisplayingTexture(float textureRes, bool updateMaterial, Camera matCam, int displayingMode, bool useCustomCam);
    void updateMaterialDisplayingTexture(float textureRes, bool updateMaterial, Camera matCam, int displayingMode, bool useCustomCam, Texture custom_display_texture, Model &displayModel, int specificUpdateI);

    void apply_material(Model& model, Mesh &mesh, int textureResolution, bool noPrevTxtrMode);

    /// @brief Returns a new material with the same material modifiers and different OpenGL texture objects 
    Material duplicateMaterial();

    bool operator==(const Material& other) const;
    bool operator!=(const Material& other) const;

    /// @brief Deletes all the OpenGL buffer objects inside of the material
    void deleteBuffers();

};

#endif 