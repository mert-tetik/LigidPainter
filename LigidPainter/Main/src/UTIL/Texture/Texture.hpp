/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef TEXTURE_HPP
#define TEXTURE_HPP

//OpenGL & window 
#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

//OpenGL Math Library GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector> //std::vector
#include <string> //std::string
#include <map> //std::map
#include <atomic>

#include "UTIL/Shader/Shader.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"

/// @brief Forward declared Mesh class
class Mesh;
/// @brief Forward declared MaterialChannels struct
struct MaterialChannels;
/// @brief Forward declared MaterialIDColor struct
struct MaterialIDColor;
/// @brief Forward declared Camera struct
struct Camera;

struct ProceduralProperties{
    /// @brief Indicates which procedural texture shader function will be used (indexing the getProcedural function in the Shaders/Include/Procedural.frag)
    ///         -1 : Using the proceduralTextureID
    ///         1000 : Using the ID (Smart texture selected)
    int proceduralID = -1;
    float proceduralScale = 1.f;
    float proceduralStretch = 1.f;
    float proceduralScaleModelPos = 1.f;
    bool proceduralMirroredRepeat = false;
    int proceduralnverted = 0;
    bool proceduralNormalMap = false;
    bool proceduralNormalGrayScale = false;
    float proceduralNormalStrength = 10.f;
    unsigned int proceduralTextureID = 0;
    bool proceduralUseTexCoords = false;
    bool proceduralGrayScale = false;
    float proceduralBrightness = 1.f;
    glm::vec4 smartProperties;

    float txtrPackScale = 1.f;
    float txtrPackCount = 1.f;
    float txtrPackRotation_Jitter = 1.f;
    float txtrPackSize_Jitter = 1.f;
    float txtrPackOpacity_Jitter = 1.f;
    float txtrPackScatter = 1.f;

    int textureSelectionDialog_selectedTextureIndex = 0;
    int textureSelectionDialog_selectedMode = 0;

    bool operator==(const ProceduralProperties& props) const{
        return 
            this->proceduralID == props.proceduralID && 
            this->proceduralScale == props.proceduralScale && 
            this->proceduralStretch == props.proceduralStretch && 
            this->proceduralScaleModelPos == props.proceduralScaleModelPos && 
            this->proceduralMirroredRepeat == props.proceduralMirroredRepeat && 
            this->proceduralnverted == props.proceduralnverted && 
            this->proceduralNormalMap == props.proceduralNormalMap && 
            this->proceduralNormalGrayScale == props.proceduralNormalGrayScale && 
            this->proceduralNormalStrength == props.proceduralNormalStrength && 
            this->proceduralTextureID == props.proceduralTextureID && 
            this->proceduralUseTexCoords == props.proceduralUseTexCoords && 
            this->proceduralGrayScale == props.proceduralGrayScale && 
            this->proceduralBrightness == props.proceduralBrightness && 
            this->smartProperties == props.smartProperties && 
            this->txtrPackScale == props.txtrPackScale && 
            this->txtrPackCount == props.txtrPackCount && 
            this->txtrPackRotation_Jitter == props.txtrPackRotation_Jitter && 
            this->txtrPackSize_Jitter == props.txtrPackSize_Jitter && 
            this->txtrPackOpacity_Jitter == props.txtrPackOpacity_Jitter && 
            this->txtrPackScatter == props.txtrPackScatter &&
            this->textureSelectionDialog_selectedTextureIndex == props.textureSelectionDialog_selectedTextureIndex &&
            this->textureSelectionDialog_selectedMode == props.textureSelectionDialog_selectedMode;
    }
    
    bool operator!=(const ProceduralProperties& props) const{
        return !(*this == props); 
    }
};

class Texture
{
private:
    // --- util functions --- 

    std::string generateTMPTitle();
    std::string getTMPTitleWithGreatestIndex();

public:
    /// @brief OpenGL texture buffer object id
    unsigned int ID = 0;

    /// @brief OpenGL texture buffer object id for the copy context in order to use for multithreading
    ///        Generally used for the library elements
    unsigned int copyContextID = 0;

    ProceduralProperties proceduralProps;

    /// @brief Title of the texture (myTexture)
    std::string title = "";

    /// @brief unique id (unique to the other texture elements in the library.textures)
    ///        (used to generate tmp file names)
    int uniqueId;

    /// @brief default constructor (ID is generated when @ref load is called)
    Texture();
    
    /// @brief create a texture class with existing OpenGL texture buffer object id
    Texture(unsigned int ID);
    
    /// @brief Create a texture class using texture data & generate OpenGL texture buffer object id (channels is always rgba)
    /// @param pixels pixels of the texture
    /// @param w width
    /// @param h height
    Texture(char* pixels, int w, int h);
    Texture(char* pixels, int w, int h, GLenum filterParam);
    Texture(char* pixels, int w, int h, GLenum filterParam, int proceduralID);
    Texture(char* pixels, int w, int h, GLenum filterParam, GLenum format, GLenum internalFormat);
    
    Texture(unsigned char* pixels, int w, int h);
    Texture(unsigned char* pixels, int w, int h, GLenum filterParam);
    Texture(unsigned char* pixels, int w, int h, GLenum filterParam, int proceduralID);
    Texture(unsigned char* pixels, int w, int h, GLenum filterParam, GLenum format, GLenum internalFormat);
    
    Texture(char* pixels, unsigned char* uPixels, bool use_unsigned, int w, int h, GLenum filterParam, GLenum format, GLenum internalFormat, GLenum textureWrap, GLenum target);

    void update(char* pixels, int w, int h);
    void update(char* pixels, int w, int h, GLenum filterParam);
    void update(char* pixels, int w, int h, GLenum filterParam, GLenum format);
    void update(char* pixels, int w, int h, GLenum filterParam, GLenum format, unsigned internalFormat);
    void update(char* pixels, int w, int h, GLenum filterParam, GLenum format, unsigned internalFormat, GLenum wrap);
    
    void update(unsigned char* pixels, int w, int h);
    void update(unsigned char* pixels, int w, int h, GLenum filterParam);
    void update(unsigned char* pixels, int w, int h, GLenum filterParam, GLenum format);
    void update(unsigned char* pixels, int w, int h, GLenum filterParam, GLenum format, unsigned internalFormat);
    void update(unsigned char* pixels, int w, int h, GLenum filterParam, GLenum format, unsigned internalFormat, GLenum wrap);
    
    void update(char* pixels, unsigned char* uPixels, bool use_unsigned, int w, int h, GLenum filterParam, GLenum format, GLenum internalFormat, GLenum wrap);


    /// @brief Load a texture by importing the texture in the given path via STBI
    bool load(const char* path);

    /// @brief Returns texture data in the given path & doesn't write anything to the member variables
    unsigned char* getTextureDataViaPath(const char* aPath,int &aWidth,int &aHeight,int &aChannels,int desiredChannels,bool flip);
    
    /// @brief Exports the texture to the given directory path
    /// @param path folder path (will be completed as : @param path + / + @ref title + . + format (in lowercase))
    /// @param format is PNG JPEG BMP TGA
    bool exportTexture(std::string path,const std::string format);    
    
    void copyDataToTheCopyContext();
    
    /*----------------- PROVIDE DATA --------------------*/
    void getData(char* pixels);
    void getData(unsigned char* pixels);
    glm::ivec2 getResolution();


    /*----------------- WRITE DATA --------------------*/
    /// @brief Write the tmp file of the texture from the tmp folder
    void writeTMP(std::string tmpTitle);
    /// @brief Read the tmp file of the texture from the tmp folder and delete the readen file
    void readTMP(std::string tmpTitle);
    bool writeTextureData(std::ofstream& wf);
    bool readTextureData(std::ifstream& rf, bool threeDMode, unsigned int versionCode, bool generate_txtr);

    /*------------------  PROCEDURAL -------------------*/
    /// @brief Generates 2D displaying texture using the proceduralProps & writes the texture into the this->ID
    /// @param displayMode 0 : solid 2D, 1 : Material ball PBR, 2 : Custom mesh PBR
    void generateProceduralDisplayingTexture(int displayingTextureRes, int displayMode, float radius, float rotation);
    void generateProceduralDisplayingTexture(int displayingTextureRes, int displayMode);
    /// @brief Generates procedural texture result using the mesh param
    Texture generateProceduralTexture(Mesh &mesh, int textureRes);
    void generateProceduralTexture(Mesh &mesh, Texture& destTxtr, int textureRes);
    
    /*------------------ REMOVE FROM THE TEXTURE -------------------*/
    /// @brief Expands the texture's boundaries according to the mesh uv mask. And the black part is being removed
    void removeSeams(Mesh& mesh);
    /// @brief Removes the unselected faces of the mesh
    void removeUnselectedFaces(Mesh& mesh);
    
    /*------------------ GENERATE NORMAL MAP -------------------*/
    /// @brief Writes the normal map version of the texture into itself
    void applyNormalMap(float proceduralNormalStrength, bool proceduralNormalGrayScale, bool alphaMode);
    /// @brief Writes the normal map version of the texture into the normalMap param (normalMap has to be initialized) 
    void generateNormalMap(Texture& normalMap, float proceduralNormalStrength, bool proceduralNormalGrayScale, bool alphaMode);

    /*------------------ COLOR PALETTE -------------------*/
    std::vector<MaterialIDColor> getMaterialIDPalette();

    /*------------------ FLIP -------------------*/
    void flipTexture(bool horizontal, bool vertical);

    /*----------------- RESIZE ---------------------*/
    void resize(const glm::ivec2 newResolution);

    /*----------------- MIX ---------------- */
    /*! @brief mix_txtr takes the white of the the and the original takes the black*/
    void mix(Texture mix_txtr, Texture mask, bool mask_alpha);
    /*! @brief mix_txtr takes the white of the the and the original takes the black*/
    void mix(Texture mix_txtr, Texture mask, bool maskAlpha, bool normalMapMode, bool invert);

    /*----------------- RENDERING MODEL TO TEXTURE ---------------*/
    void render_mesh(Mesh& mesh, MaterialChannels channels, Camera cam);

    /*----------------- DUPLICATION ----------------*/
    /*! @brief Get context specific another texture with the same pixel data as the original*/
    Texture get_temp_copy_txtr(std::string location);
    Texture duplicateTexture(std::string location);
    void duplicateTexture(Texture &txtr, std::string location);
    void duplicateTextureSub(Texture &txtr, std::string location);
};

#endif