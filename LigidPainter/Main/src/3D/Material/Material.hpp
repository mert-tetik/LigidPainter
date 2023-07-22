
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

/// @brief Front decleration for the @ref Section structure
struct Section;

/// @brief Front decleration for the @ref Mesh class
class Mesh;


//-------------- MATERIAL MODIFIER ------------

#define TEXTURE_MATERIAL_MODIFIER 0
#define DUST_MATERIAL_MODIFIER 1
#define ASPHALT_MATERIAL_MODIFIER 2
#define FABRIC_MATERIAL_MODIFIER 3
#define MARBLE_MATERIAL_MODIFIER 4
#define MOSS_MATERIAL_MODIFIER 5
#define RUST_MATERIAL_MODIFIER 6
#define SKIN_MATERIAL_MODIFIER 7
#define SOLID_MATERIAL_MODIFIER 8
#define WOODEN_MATERIAL_MODIFIER 9

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

    void (*updateMaterialChannels)(Material &material, Mesh &mesh, int textureResolution, int curModI, glm::mat4 perspective, glm::mat4 view);

    //Constructors
    MaterialModifier();
    MaterialModifier(ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures,int modifierIndex);

    //Public member functions
    std::vector<Section> createTextureModifier(ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures);
    std::vector<Section> createDustModifier(ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures);

};


/// @brief Used by the material editor dialog
/// Holds the material modifiers used by the app
/// When needed material editor uses these material modifiers to attach to the material's material modifiers
struct AppMaterialModifiers{
    MaterialModifier textureModifier;
    MaterialModifier dustModifier;
    MaterialModifier asphaltModifier;
    MaterialModifier fabricModifier;
    MaterialModifier marbleModifier;
    MaterialModifier mossModifier;
    MaterialModifier rustModifier;
    MaterialModifier skinModifier;
    MaterialModifier solidModifier;
    MaterialModifier woodenModifier;
};



//-------------- MATERIAL ------------

class Material 
{
public:
    /// @brief title of the material (myMaterial_0)
    std::string title;

    /// @brief texture used to render the material to display
    unsigned int displayingTexture;
    
    /// @brief Unique id to distinguish the materials & connect them to the nodes 
    int uniqueID;
    
    /// @brief Material modifiers of the material
    /// Assigned by the material editor dialog
    std::vector<MaterialModifier> materialModifiers;

    /// @brief Default constructor
    Material();
    
    /// @brief Init the channel textures of the material + displayingTexture + assign the unique ID value
    /// @param textureRes Project texture resolution value assigned by the user using GUI 
    /// @param title Title of the material
    /// @param ID Generated unique ID for the material (Unique to other materials in the libray.materials)
    Material(int textureRes,std::string title,int ID);

    /// @brief Write the material data to the given path
    /// @param path is a path to a folder 
    void writeFile(std::string path);

    /// @brief Read a lgdmaterial file and write the data to the class 
    void readFile(std::string path,ColorPalette colorPalette ,Shader buttonShader ,AppTextures appTextures, const std::vector<Material> materials);

    /// @brief Interpret the @ref materialModifiers and write the shader results to the material channels then update the displaying texture
    void updateMaterialDisplayingTexture(float textureRes, Box box, Context context, Shader buttonShader, Shader tdModelShader, Model sphereModel);

    /// @brief Returns a new material with the same material modifiers and different OpenGL texture objects 
    Material duplicateMaterial(int textureRes);
};

#endif 