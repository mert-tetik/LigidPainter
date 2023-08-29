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

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"

TexturePack::TexturePack(){

}

int TexturePack::load(std::string path){

    try
    {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            Texture txtr;
            txtr.load(entry.path().string().c_str());

            this->textures.push_back(txtr);
        }     
    }
    catch (const std::filesystem::filesystem_error& ex) {
        LGDLOG::start << "ERROR : Filesystem : Location ID 448993 " << ex.what() << LGDLOG::end;
    }
    
    this->title = UTIL::getLastWordBySeparatingWithChar(path, UTIL::folderDistinguisher());

    return 1;
}

static struct Region{
    glm::ivec2 startPos;
    glm::ivec2 endPos;
};

static int findMostRightPhase1(Texture txtr, char*& pixels, glm::ivec2 foundingPos, float maxY){
    glm::ivec2 curPos = foundingPos;
    
    bool fullAlphaZeroLineDetected = false;

    while (!fullAlphaZeroLineDetected)
    {
        bool opaquePixelDetected = false;
        for (size_t y = foundingPos.y; y < maxY; y++)
        {
            int pixelIndex = (y * txtr.getResolution().x + curPos.x) * 4; // Each pixel has 4 components (R, G, B, A)
        
            char pxAlpha = pixels[pixelIndex + 3];

            if(pxAlpha > 5)
                opaquePixelDetected = true; 
        }

        if(!opaquePixelDetected)
            fullAlphaZeroLineDetected = true;

        curPos.x++;
    }

    return curPos.x;
}

static int findMostBottomPhase1(Texture txtr, char*& pixels, glm::ivec2 foundingPos, float maxX){
    glm::ivec2 curPos = foundingPos;
    
    bool fullAlphaZeroLineDetected = false;

    while (!fullAlphaZeroLineDetected)
    {
        bool opaquePixelDetected = false;
        for (size_t x = foundingPos.x; x < maxX; x++)
        {
            int pixelIndex = (x * txtr.getResolution().y + curPos.y) * 4; // Each pixel has 4 components (R, G, B, A)
        
            char pxAlpha = pixels[pixelIndex + 3];

            if(pxAlpha > 5)
                opaquePixelDetected = true; 
        }

        if(!opaquePixelDetected)
            fullAlphaZeroLineDetected = true;

        curPos.y++;
    }

    return curPos.y;
}

static Region processRegion(Texture txtr, char*& pixels, glm::ivec2 foundingPos){
    
    Region region;
    
    glm::ivec2 phase1DestPos = glm::ivec2(0);

    phase1DestPos.x = findMostRightPhase1(txtr, pixels, foundingPos, txtr.getResolution().y);
    
    phase1DestPos.y = findMostBottomPhase1(txtr, pixels, foundingPos, phase1DestPos.x);
    
    glm::ivec2 phase2DestPos = glm::ivec2(0);

    phase2DestPos.x = findMostRightPhase1(txtr, pixels, foundingPos, phase1DestPos.y);
    
    phase2DestPos.y = findMostBottomPhase1(txtr, pixels, foundingPos, phase2DestPos.x);

    region.startPos = foundingPos;
    region.endPos = phase2DestPos;

    return region;
}

static bool isPixelIndexIsInAnyRegion(std::vector<Region> regions, glm::ivec2 pxPos){
    for (size_t i = 0; i < regions.size(); i++)
    {
        Region reg = regions[i];
        if(pxPos.x > reg.startPos.x && pxPos.x < reg.endPos.x){
            if(pxPos.y > reg.startPos.y && pxPos.y < reg.endPos.y){
                return true;
            }
        }
    }
    
    return false;
}

void TexturePack::saperateSprites(Texture txtr){
    
    char* pixels = new char[txtr.getResolution().x * txtr.getResolution().y * 4];
    
    txtr.getData(pixels);

    std::vector<Region> regions;

    for (size_t y = 0; y < txtr.getResolution().y; y++)
    {
        for (size_t x = 0; x < txtr.getResolution().x; x++)
        {
            int pixelIndex = (y * txtr.getResolution().x + x) * 4; // Each pixel has 4 components (R, G, B, A)
            
            char pxAlpha = pixels[pixelIndex + 3]; 

            bool pixelIndexIsInAnyRegion = isPixelIndexIsInAnyRegion(regions, glm::ivec2(x,y));

            // If opaque pixel found
            // TODO Check if already processed
            if(pxAlpha > 5 && !pixelIndexIsInAnyRegion){
                regions.push_back(processRegion(txtr, pixels, glm::ivec2(x,y)));
            }
        }
    }

    unsigned int FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, txtr.ID, 0);

    for (size_t i = 0; i < regions.size(); i++)
    {
        int regionW = regions[i].startPos.x - regions[i].endPos.x;
        int regionH = regions[i].startPos.y - regions[i].endPos.y;

        char* regionPxs = new char[regionW * regionH * 4];

        glReadPixels(
                            regions[i].startPos.x, 
                            regions[i].startPos.y, 
                            regions[i].startPos.x - regions[i].endPos.x, 
                            regions[i].startPos.y - regions[i].endPos.y, 
                            GL_RGBA, 
                            GL_BYTE,
                            regionPxs
                        );

        this->textures.push_back(Texture(regionPxs, regionW, regionH));    
    }

    //Finish
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &FBO);
    delete[] pixels;
}