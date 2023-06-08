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

#ifndef LGD3DMATERIAL_HPP
#define LGD3DMATERIAL_HPP

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.hpp"
#include "Box.hpp"
#include "Renderer.h"
#include "Util.hpp"
#include "Timer.hpp"
#include "Texture.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

class MaterialModifier;

class Material //TODO DON'T USE UNIQUE IDS. CALCULATE ID VALUES OF THE TEXTURE WHEN READING THE PROJECT FOLDER
{
private:
    #if defined(_WIN32) || defined(_WIN64)
		    char folderDistinguisher = '\\';
	#else
			char folderDistinguisher = '/'; 
	#endif

    void initTexture(Texture &txtr,int textureRes){
        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1,&txtr.ID);
        glBindTexture(GL_TEXTURE_2D,txtr.ID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureRes, textureRes, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
public:
    std::string title;

    Texture albedo;
    Texture roughness;
    Texture metallic; 
    Texture normalMap;
    Texture heightMap;
    Texture ambientOcclusion;

    unsigned int displayingTexture;

    int ID; //Unique ID

    std::vector<MaterialModifier> materialModifiers;

    Material(){}
    Material(int textureRes,std::string title,int ID){
        this->title = title;
        this->ID = ID;
        initTexture(albedo,textureRes);
        initTexture(roughness,textureRes);
        initTexture(metallic,textureRes);
        initTexture(normalMap,textureRes);
        initTexture(heightMap,textureRes);
        initTexture(ambientOcclusion,textureRes);


        //Init displaying texture
        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1,&displayingTexture);
        glBindTexture(GL_TEXTURE_2D,displayingTexture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 2048, 2048, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glGenerateMipmap(GL_TEXTURE_2D);
    }


    void writeFile(std::string path){
        //Saves the brush file
        if(path == "")
            path = tinyfd_selectFolderDialog("Select a folder to export the material file", "");
        
        if(path.size()){

            std::ofstream wf(path + folderDistinguisher + title + ".lgdmaterial", std::ios::out | std::ios::binary);

		    if(!wf) {
                std::cout << "ERROR WHILE WRITING MATERIAL FILE! Cannot open file : " << path << std::endl;
                return;
            }
            
            //!HEADER
            
            //!Description
            uint64_t h1 = 0xBBF2367A; 
            uint64_t h2 = 0xBB58BC66; 
            uint64_t h3 = 0xBBACB786; 

            wf.write(reinterpret_cast<char*>(   &h1    ),sizeof(uint64_t));
            wf.write(reinterpret_cast<char*>(   &h2    ),sizeof(uint64_t));
            wf.write(reinterpret_cast<char*>(   &h3    ),sizeof(uint64_t));
            
            //!Modifiers
            uint64_t materialModifierSize = materialModifiers.size();
            wf.write(reinterpret_cast<char*>(   &materialModifierSize     ),sizeof(uint64_t));
            
            for (size_t i = 0; i < materialModifierSize; i++)
            {

                size_t materialModifierModifierIndex = materialModifiers[i].modifierIndex;
                wf.write(reinterpret_cast<char*>(   &materialModifierModifierIndex     ),sizeof(uint64_t));

                if(materialModifierModifierIndex == 0){ //Is a texture modifier
                    for (size_t channelI = 0; channelI < 6; channelI++) //For each material channel (albedo, roughness, metallic, normal map, etc.)
                    {
                        //Get the resolution of the texture from the panel of the modifier
                        uint64_t txtrWidth = this->materialModifiers[i].sections[0].elements[channelI].button.texture.getResolution().x;
                        uint64_t txtrHeight = this->materialModifiers[i].sections[0].elements[channelI].button.texture.getResolution().y;
                        
                        //Write the texture resolution data
                        wf.write(reinterpret_cast<char*>(   &txtrWidth     ),sizeof(uint64_t));
                        wf.write(reinterpret_cast<char*>(   &txtrHeight     ),sizeof(uint64_t));
        
                        //Get pixels of the texture
                        char* pixels = new char[txtrWidth * txtrHeight * 4];
                        this->materialModifiers[i].sections[0].elements[channelI].button.texture.getData(pixels);

                        //Write the texture data
                        wf.write(pixels , txtrWidth * txtrHeight * 4 * sizeof(char));

                        delete[] pixels;
                    }
                }
            }
        }
    }

    void readFile(std::string path,ColorPalette colorPalette ,Shader buttonShader ,AppTextures appTextures){
        if(path.size()){
            std::ifstream rf(path, std::ios::out | std::ios::binary);
            
            Util util;
            title = util.getLastWordBySeparatingWithChar(path,folderDistinguisher);
            title = util.removeExtension(title);
		    
            if(!rf) {
                std::cout << "ERROR WHILE READING MATERIAL FILE! Cannot open file : " << path << std::endl;
                return;
            }
            
            //!HEADER
            
            //!Description
            uint64_t h1 = 0xBBF2367A; 
            uint64_t h2 = 0xBB58BC66; 
            uint64_t h3 = 0xBBACB786; 
            
            uint64_t ch1; 
            uint64_t ch2; 
            uint64_t ch3; 

            rf.read(reinterpret_cast<char*>(   &ch1    ),sizeof(uint64_t));
            rf.read(reinterpret_cast<char*>(   &ch2    ),sizeof(uint64_t));
            rf.read(reinterpret_cast<char*>(   &ch3    ),sizeof(uint64_t));
            
            if(ch1 != h1 || ch2 != h2 || ch3 != h3){
                std::cout << "ERROR This is not a material file : " << path << std::endl;
                return;
            }

            //!Modifiers
            uint64_t materialModifierSize;
            rf.read(reinterpret_cast<char*>(   &materialModifierSize     ),sizeof(uint64_t));
            
            for (size_t i = 0; i < materialModifierSize; i++)
            {

                size_t materialModifierModifierIndex;
                rf.read(reinterpret_cast<char*>(   &materialModifierModifierIndex     ),sizeof(uint64_t));
                
                MaterialModifier modifier(colorPalette , buttonShader , appTextures , materialModifierModifierIndex);

                if(materialModifierModifierIndex == 0){ //Is a texture modifier
                    for (size_t channelI = 0; channelI < 6; channelI++) //For each material channel (albedo, roughness, metallic, normal map, etc.)
                    {
                        //Get the resolution of the texture from the panel of the modifier
                        uint64_t txtrWidth;
                        uint64_t txtrHeight;
                        
                        //Read the texture resolution data
                        rf.read(reinterpret_cast<char*> (       &txtrWidth          )       ,sizeof(uint64_t));
                        rf.read(reinterpret_cast<char*> (       &txtrHeight         )       ,sizeof(uint64_t));
        
                        //Get pixels of the texture
                        char* pixels = new char[txtrWidth * txtrHeight * 4];

                        //Read the texture data
                        rf.read(pixels , txtrWidth * txtrHeight * 4 * sizeof(char));

                        //Give the texture to the panel of the modifier
                        modifier.sections[0].elements[channelI].button.texture = Texture(pixels,txtrWidth,txtrHeight);

                        delete[] pixels;
                    }
                }
            }
        }
    }
};

#endif