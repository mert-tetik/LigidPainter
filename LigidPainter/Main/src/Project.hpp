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

#ifndef LGDPROJECT_HPP
#define LGDPROJECT_HPP

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Shader.hpp"
#include "Box.hpp"
#include "Renderer.h"
#include "Util.hpp"
#include "Timer.hpp"
#include "Texture.hpp"

#include "Mouse.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>
#include <filesystem>

class Project
{
private:
    /* data */
public:
    //Project folder

    std::string folderPath;
    std::string ligidFilePath;
    std::string projectName;


    #if defined(_WIN32) || defined(_WIN64)
		    char folderDistinguisher = '\\';
	#else
			char folderDistinguisher = '/'; 
	#endif


    Project(){

    }
    void createProject(std::string destinationPath,std::string name){
        if(destinationPath[destinationPath.size()-1] == '/' || destinationPath[destinationPath.size()-1] == '\\') //Make sure destination path doesn't have seperator at the end
            destinationPath.pop_back();

        std::string resultPath = destinationPath + folderDistinguisher + name;

        //Create the project folder
        std::filesystem::create_directory(resultPath);
    
        //Create the textures folder
        std::string textureFolderPath = resultPath + folderDistinguisher + "Textures";
        std::filesystem::create_directory(textureFolderPath);

        //Materials
        std::string materialsFolderPath = resultPath + folderDistinguisher + "Materials";
        std::filesystem::create_directory(materialsFolderPath);
        
        //Brushes
        std::string brushesFolderPath = resultPath + folderDistinguisher + "Brushes";
        std::filesystem::create_directory(brushesFolderPath);
        
        //Fonts
        std::string fontsFolderPath = resultPath + folderDistinguisher + "Fonts";
        std::filesystem::create_directory(fontsFolderPath);
        
        //Scripts
        std::string scriptsFolderPath = resultPath + folderDistinguisher + "Scripts";
        std::filesystem::create_directory(scriptsFolderPath);
        
        //Filters
        std::string filtersFolderPath = resultPath + folderDistinguisher + "Filters";
        std::filesystem::create_directory(filtersFolderPath);
        
        //Layers
        std::string layersFolderPath = resultPath + folderDistinguisher + "Layers";
        std::filesystem::create_directory(layersFolderPath);
        
        //3D Models
        std::string tdModelFolderPath = resultPath + folderDistinguisher + "3DModels";
        std::filesystem::create_directory(tdModelFolderPath);
    }
};

#endif