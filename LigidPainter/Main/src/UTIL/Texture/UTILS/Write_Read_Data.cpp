/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include "UTIL/Settings/Settings.hpp"
#include "UTIL/GL/GL.hpp"
#include "UTIL/Texture/Texture.hpp"
#include "UTIL/Library/Library.hpp"

#define LGDTEXTURE_WRITEBITS(var, type, loc) if(!wf.write(reinterpret_cast<char*>(   &var     ), sizeof(type))){ \
                                    LGDLOG::start<< "ERROR : Writing texture data. Failed to write at : " << loc << LGDLOG::end;\
                                    return false; \
                                }

bool Texture::writeTextureData(std::ofstream& wf){
    
    // -------- Procedural Data -------- 
    
    LGDTEXTURE_WRITEBITS(this->proceduralProps.proceduralID, int, "Property texture - procedural ID");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.proceduralScale, float, "Property texture - procedural Scale");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.proceduralnverted, int, "Property texture - procedural nverted");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.proceduralNormalMap, bool, "Property texture - procedural NormalMap");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.proceduralNormalGrayScale, bool, "Property texture - procedural NormalGrayScale");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.proceduralNormalStrength, float, "Property texture - procedural NormalStrength");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.proceduralUseTexCoords, bool, "Property texture - procedural UseTexCoords");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.proceduralGrayScale, bool, "Property texture - procedural GrayScale");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.proceduralBrightness, float, "Property texture - procedural Brightness");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.smartProperties.x, float, "Property texture - Smart Properties");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.smartProperties.y, float, "Property texture - Smart Properties");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.smartProperties.z, float, "Property texture - Smart Properties");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.smartProperties.w, float, "Property texture - Smart Properties");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.txtrPackScale, float, "Property texture - txtrPackScale");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.txtrPackCount, float, "Property texture - txtrPackCount");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.txtrPackRotation_Jitter, float, "Property texture - txtrPackRotation_Jitter");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.txtrPackSize_Jitter, float, "Property texture - txtrPackSize_Jitter");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.txtrPackOpacity_Jitter, float, "Property texture - txtrPackOpacity_Jitter");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.txtrPackScatter, float, "Property texture - txtrPackScatter");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.textureSelectionDialog_selectedTextureIndex, int, "Property texture - textureSelectionDialog_selectedTextureIndex");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.textureSelectionDialog_selectedMode, int, "Property texture - textureSelectionDialog_selectedMode");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.proceduralMirroredRepeat, bool, "Property texture - proceduralMirroredRepeat");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.proceduralStretch, float, "Property texture - proceduralStretch");
    LGDTEXTURE_WRITEBITS(this->proceduralProps.proceduralScaleModelPos, float, "Property texture - proceduralScaleModelPos");

    // -------- Procedural Texture Data --------
    std::string txtrTitle = "";
    for (size_t i = 0; i < Library::getTextureArraySize(); i++)
    {
        if(i < Library::getTextureArraySize()){
            if(Library::getTexture(i)->ID == this->proceduralProps.proceduralTextureID){
                if(i < Library::getTextureArraySize())
                    txtrTitle = Library::getTexture(i)->title;
            }
        }
    }
    
    int txtrTitleSize = txtrTitle.size();
    LGDTEXTURE_WRITEBITS(txtrTitleSize, int, "Property texture - Texture title size");
    for (size_t i = 0; i < txtrTitleSize; i++)
    {
        char c = txtrTitle[i];
        LGDTEXTURE_WRITEBITS(c, char, "Property texture - Texture title character");
    }
    
    return true;
}

#define LGDMATERIAL_READBITS(var, type, loc) if(!rf.read(reinterpret_cast<char*>(   &var     ), sizeof(type))){ \
                                LGDLOG::start<< "ERROR : Reading lgdmaterial file. Failed to read at : " << loc << LGDLOG::end;\
                                return false; \
                            }

bool Texture::readTextureData(std::ifstream& rf, bool threeDMode, unsigned int versionCode, bool generate_txtr){

    // --------- Read procedural data ---------

    int proceduralID;
    float proceduralScale;
    int proceduralnverted;
    bool proceduralNormalMap;
    bool proceduralNormalGrayScale;
    float proceduralNormalStrength;
    bool proceduralUseTexCoords;
    bool proceduralGrayScale;
    float proceduralBrightness;
    float smartProperties_x;
    float smartProperties_y;
    float smartProperties_z;
    float smartProperties_w;
    float txtrPackScale;
    float txtrPackCount;
    float txtrPackRotation_Jitter;
    float txtrPackSize_Jitter;
    float txtrPackOpacity_Jitter;
    float txtrPackScatter;
    int textureSelectionDialog_selectedTextureIndex;
    int textureSelectionDialog_selectedMode;
    bool proceduralMirroredRepeat;
    float proceduralStretch;
    float proceduralScaleModelPos;
    
    LGDMATERIAL_READBITS(proceduralID, int, "Property texture - procedural ID");
    LGDMATERIAL_READBITS(proceduralScale, float, "Property texture - procedural Scale");
    LGDMATERIAL_READBITS(proceduralnverted, int, "Property texture - procedural nverted");
    LGDMATERIAL_READBITS(proceduralNormalMap, bool, "Property texture - procedural NormalMap");
    LGDMATERIAL_READBITS(proceduralNormalGrayScale, bool, "Property texture - procedural NormalGrayScale");
    LGDMATERIAL_READBITS(proceduralNormalStrength, float, "Property texture - procedural NormalStrength");
    LGDMATERIAL_READBITS(proceduralUseTexCoords, bool, "Property texture - procedural UseTexCoords");
    LGDMATERIAL_READBITS(proceduralGrayScale, bool, "Property texture - procedural GrayScale");
    LGDMATERIAL_READBITS(proceduralBrightness, float, "Property texture - procedural Brightness");
    LGDMATERIAL_READBITS(smartProperties_x, float, "Property texture - Smart Properties");
    LGDMATERIAL_READBITS(smartProperties_y, float, "Property texture - Smart Properties");
    LGDMATERIAL_READBITS(smartProperties_z, float, "Property texture - Smart Properties");
    LGDMATERIAL_READBITS(smartProperties_w, float, "Property texture - Smart Properties");
    LGDMATERIAL_READBITS(txtrPackScale, float, "Property texture - txtrPackScale");
    LGDMATERIAL_READBITS(txtrPackCount, float, "Property texture - txtrPackCount");
    LGDMATERIAL_READBITS(txtrPackRotation_Jitter, float, "Property texture - txtrPackRotation_Jitter");
    LGDMATERIAL_READBITS(txtrPackSize_Jitter, float, "Property texture - txtrPackSize_Jitter");
    LGDMATERIAL_READBITS(txtrPackOpacity_Jitter, float, "Property texture - txtrPackOpacity_Jitter");
    LGDMATERIAL_READBITS(txtrPackScatter, float, "Property texture - txtrPackScatter");
    LGDMATERIAL_READBITS(textureSelectionDialog_selectedTextureIndex, int, "Property texture - textureSelectionDialog_selectedTextureIndex");
    LGDMATERIAL_READBITS(textureSelectionDialog_selectedMode, int, "Property texture - textureSelectionDialog_selectedMode");
    
    if(versionCode == 1){
        LGDMATERIAL_READBITS(proceduralMirroredRepeat, bool, "Property texture - proceduralMirroredRepeat");
        LGDMATERIAL_READBITS(proceduralStretch, float, "Property texture - proceduralStretch");
        proceduralScaleModelPos = 1.f;
    }
    else if(versionCode == 2){
        LGDMATERIAL_READBITS(proceduralMirroredRepeat, bool, "Property texture - proceduralMirroredRepeat");
        LGDMATERIAL_READBITS(proceduralStretch, float, "Property texture - proceduralStretch");
        LGDMATERIAL_READBITS(proceduralScaleModelPos, float, "Property texture - proceduralScaleModelPos");
    }
    else{
        proceduralMirroredRepeat = true;
        proceduralStretch = 1.f;
        proceduralScaleModelPos = 1.f;
    }

    // --------- Recalculate procedural ID ---------
    int MAX_PROCEDURAL_PATTERN_TEXTURE_SIZE = 66;
    int MAX_PROCEDURAL_NOISE_TEXTURE_SIZE = 50;
    int MAX_PROCEDURAL_SMART_TEXTURE_SIZE = 7;

    if(textureSelectionDialog_selectedMode == 1)
        proceduralID = textureSelectionDialog_selectedTextureIndex;
    else if(textureSelectionDialog_selectedMode == 2)
        proceduralID = textureSelectionDialog_selectedTextureIndex + MAX_PROCEDURAL_PATTERN_TEXTURE_SIZE;
    else if(textureSelectionDialog_selectedMode == 4)
        proceduralID = textureSelectionDialog_selectedTextureIndex + MAX_PROCEDURAL_PATTERN_TEXTURE_SIZE + MAX_PROCEDURAL_NOISE_TEXTURE_SIZE;

    // --------- Read texture data ---------
    std::string txtrTitle = "";
    int txtrTitleSize;
    LGDMATERIAL_READBITS(txtrTitleSize, int, "Property texture - Texture title size");
    for (size_t i = 0; i < txtrTitleSize; i++)
    {
        char c;
        LGDMATERIAL_READBITS(c, char, "Property texture - Texture title character");
        txtrTitle.push_back(c);
    }

    unsigned int proceduralTextureID = 0;
    for (size_t i = 0; i < Library::getTextureArraySize(); i++)
    {
        if(i < Library::getTextureArraySize()){
            if(Library::getTexture(i)->title == txtrTitle){
                if(i < Library::getTextureArraySize())
                    proceduralTextureID = Library::getTexture(i)->ID;
            }
        }
    }

    if(textureSelectionDialog_selectedMode == 5){
        proceduralID = -1;
        if(textureSelectionDialog_selectedTextureIndex < Library::getgetSrcLibTxtrsArraySize()){
            proceduralTextureID = Library::getSrcLibTxtr(textureSelectionDialog_selectedTextureIndex).getTexture().ID;
        }
    }

    // --------- Create the texture ---------

    this->proceduralProps.proceduralID = proceduralID;
    this->proceduralProps.proceduralScale = proceduralScale;
    this->proceduralProps.proceduralnverted = proceduralnverted;
    this->proceduralProps.proceduralNormalMap = proceduralNormalMap;
    this->proceduralProps.proceduralNormalGrayScale = proceduralNormalGrayScale;
    this->proceduralProps.proceduralNormalStrength = proceduralNormalStrength;
    this->proceduralProps.proceduralTextureID = proceduralTextureID;
    this->proceduralProps.proceduralUseTexCoords = proceduralUseTexCoords;
    this->proceduralProps.proceduralGrayScale = proceduralGrayScale;
    this->proceduralProps.proceduralBrightness = proceduralBrightness;
    this->proceduralProps.smartProperties.x = smartProperties_x;
    this->proceduralProps.smartProperties.y = smartProperties_y;
    this->proceduralProps.smartProperties.z = smartProperties_z;
    this->proceduralProps.smartProperties.w = smartProperties_w;
    this->proceduralProps.txtrPackScale = txtrPackScale;
    this->proceduralProps.txtrPackCount = txtrPackCount;
    this->proceduralProps.txtrPackRotation_Jitter = txtrPackRotation_Jitter;
    this->proceduralProps.txtrPackSize_Jitter = txtrPackSize_Jitter;
    this->proceduralProps.txtrPackOpacity_Jitter = txtrPackOpacity_Jitter;
    this->proceduralProps.txtrPackScatter = txtrPackScatter;
    this->proceduralProps.textureSelectionDialog_selectedTextureIndex = textureSelectionDialog_selectedTextureIndex;
    this->proceduralProps.textureSelectionDialog_selectedMode = textureSelectionDialog_selectedMode;
    this->proceduralProps.proceduralMirroredRepeat = proceduralMirroredRepeat;
    this->proceduralProps.proceduralStretch = proceduralStretch;
    this->proceduralProps.proceduralScaleModelPos = proceduralScaleModelPos;

    if(generate_txtr){
        glGenTextures(1, &this->ID);

        this->generateProceduralDisplayingTexture(512, threeDMode);
    }

    return true;
}