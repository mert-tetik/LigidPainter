
/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

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


//-------------- MATERIAL MODIFIER ------------

#define TEXTURE_MATERIAL_MODIFIER 0

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

    //Constructors
    MaterialModifier();
    MaterialModifier(ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures,int modifierIndex);

    //Public member functions
    std::vector<Section> createTextureModifier(ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures);
};


/// @brief Used by the material editor dialog
/// Holds the material modifiers used by the app
/// When needed material editor uses these material modifiers to attach to the material's material modifiers
struct AppMaterialModifiers{
    MaterialModifier textureModifier;
};



//-------------- MATERIAL ------------

class Material 
{
private:
    /// @brief Util function to init a OpenGL texture buffer
    /// @param txtr the initialized texture
    /// @param textureRes resolution of the texture
    void initTexture(Texture &txtr, int textureRes);
public:
    /// @brief title of the material (myMaterial_0)
    std::string title;


    /*----- Channels of the material -----

        Only manipulated by the material 
          modifiers and no other stuff
       (results of the material modifiers)
      (don't try to equate to another txtr) 
    
    */

    Texture albedo;
    Texture roughness;
    Texture metallic; 
    Texture normalMap;
    Texture heightMap;
    Texture ambientOcclusion;

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
    void updateMaterial(float textureRes, Box box, Context context, Shader buttonShader, Shader tdModelShader, Model sphereModel);

    /// @brief Returns a new material with the same material modifiers and different OpenGL texture objects 
    Material duplicateMaterial(int textureRes);
};

#endif 