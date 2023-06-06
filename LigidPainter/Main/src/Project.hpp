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
#include <ctime>

class Project
{
private:
    void deleteFilesInFolder(const std::string folderPath) {
        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            if (entry.is_regular_file()) {
                std::filesystem::remove(entry.path());
            }
        }
    }
    
    void updateLigidFile(){
        std::ofstream wf(this->ligidFilePath, std::ios::out | std::ios::binary);
		
        if(!wf) {
            std::cout << "ERROR WHILE WRITING LIGID FILE! Cannot open file : " << this->ligidFilePath << std::endl;
            return;
        }

        //!HEADER
            
        //!Description
        uint64_t h1 = 0xBBBBBBBB; 
        uint64_t h2 = 0xCA4B6C78; 
        uint64_t h3 = 0x9A9A2C48; 

        wf.write(reinterpret_cast<char*>(   &h1    ),sizeof(uint64_t));
        wf.write(reinterpret_cast<char*>(   &h2    ),sizeof(uint64_t));
        wf.write(reinterpret_cast<char*>(   &h3    ),sizeof(uint64_t));

        //! Write the creation date
        time_t currentDate = time(0);
        wf.write(reinterpret_cast<char*>(   &currentDate    ),sizeof(time_t));
        
        //! Write the last opened date
        time_t lastOpenedDate = time(0);
        wf.write(reinterpret_cast<char*>(   &lastOpenedDate    ),sizeof(time_t));
    
    }
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
    bool createProject(std::string destinationPath,std::string name,std::string TDModelPath){
        if(destinationPath[destinationPath.size()-1] == '/' || destinationPath[destinationPath.size()-1] == '\\') //Make sure destination path doesn't have seperator at the end
            destinationPath.pop_back();

        if(!std::filesystem::exists(destinationPath)){
            const char* title = "Warning";
            const char* message = "Project path is not valid! Please try another directory.";
            const char* icon = "warning";
            const char* button = "Ok";
            
            tinyfd_messageBox(title, message, button, icon, 1);
            return false;
        }
        if(!std::filesystem::exists(TDModelPath)){
            const char* title = "Warning";
            const char* message = "3D model path is not valid. Please try another 3D model file.";
            const char* icon = "warning";
            const char* button = "Ok";
            
            tinyfd_messageBox(title, message, button, icon, 1);
            return false;
        }
        if(std::filesystem::exists(destinationPath + folderDistinguisher + name)){
            const char* title = "Warning";
            const char* message = "This folder is already exists. Do you want to overwrite?";
            const char* icon = "warning";
            const char* button = "yesno";
            
            int res = tinyfd_messageBox(title, message, button, icon, 0);

            if(res == 0)
                return false;
            else if(res == 1)
                std::filesystem::remove_all(destinationPath + folderDistinguisher + name);
        }

        this->folderPath = destinationPath + folderDistinguisher + name;
        this->ligidFilePath = folderPath + folderDistinguisher + name + ".ligid";
        this->projectName = name;
        

        //Create the project folder
        std::filesystem::create_directory(this->folderPath);
    
        //Create the textures folder
        std::string textureFolderPath = this->folderPath + folderDistinguisher + "Textures";
        std::filesystem::create_directory(textureFolderPath);

        //Materials
        std::string materialsFolderPath = this->folderPath + folderDistinguisher + "Materials";
        std::filesystem::create_directory(materialsFolderPath);
        
        //Brushes
        std::string brushesFolderPath = this->folderPath + folderDistinguisher + "Brushes";
        std::filesystem::create_directory(brushesFolderPath);
        
        //Fonts
        std::string fontsFolderPath = this->folderPath + folderDistinguisher + "Fonts";
        std::filesystem::create_directory(fontsFolderPath);
        
        //Scripts
        std::string scriptsFolderPath = this->folderPath + folderDistinguisher + "Scripts";
        std::filesystem::create_directory(scriptsFolderPath);
        
        //Filters
        std::string filtersFolderPath = this->folderPath + folderDistinguisher + "Filters";
        std::filesystem::create_directory(filtersFolderPath);
        
        //Layers
        std::string layersFolderPath = this->folderPath + folderDistinguisher + "Layers";
        std::filesystem::create_directory(layersFolderPath);
        
        //3D Models
        std::string tdModelFolderPath = this->folderPath + folderDistinguisher + "3DModels";
        std::filesystem::create_directory(tdModelFolderPath);
        std::filesystem::copy(TDModelPath,tdModelFolderPath);

        //Create the .ligid file
        updateLigidFile();


        return true;

    }


    void updateProject(Library &library){
        if(!std::filesystem::exists(folderPath)){
            std::cout << "ERROR CAN'T UPDATE THE PROJECT FOLDER : " << ligidFilePath << std::endl;
            return;
        }
        
        //!Textures
        std::string textureFolderPath = this->folderPath + folderDistinguisher + "Textures";
    
        //Clear the textures folder
        deleteFilesInFolder(textureFolderPath);

        //Write the textures
        for (size_t i = 0; i < library.textures.size(); i++)
        {
            //Export texture
            library.textures[i].exportTexture(textureFolderPath);
        }
        
        //!Brushes
        std::string brushFolderPath = this->folderPath + folderDistinguisher + "Brushes";
    
        //Clear the brushes folder
        deleteFilesInFolder(brushFolderPath);

        //Write the brushes
        for (size_t i = 0; i < library.brushes.size(); i++)
        {
            //Export brush
            library.brushes[i].saveFile(brushFolderPath);
        }
        
    }


    void loadProject(std::string ligidFilePath,Library &library,Shaders shaders){
        Util util;

        //Return if the ligidFilePath doesn't exists
        if(!std::filesystem::exists(ligidFilePath)){
            std::cout << "ERROR CAN'T LOCATE THE LIGID FILE : " << ligidFilePath << std::endl;
            return;
        }

        std::ifstream rf(ligidFilePath, std::ios::out | std::ios::binary);
		
        //Return if can't open the ligidFilePath
        if(!rf) {
            std::cout << "ERROR WHILE READING THE LIGID FILE! Cannot open file : " << ligidFilePath << std::endl;
            return;
        }

        uint64_t h1 = 0xBBBBBBBB; 
        uint64_t h2 = 0xCA4B6C78; 
        uint64_t h3 = 0x9A9A2C48; 
        
        uint64_t ch1; 
        uint64_t ch2; 
        uint64_t ch3; 
        
        //Read the description of the ligid file
        rf.read(reinterpret_cast<char*>(   &ch1    ),sizeof(uint64_t));
        rf.read(reinterpret_cast<char*>(   &ch2    ),sizeof(uint64_t));
        rf.read(reinterpret_cast<char*>(   &ch3    ),sizeof(uint64_t));
        
        //If the description doesn't matches
        if(ch1 != h1 || ch2 != h2 || ch3 != h3){
            std::cout << "ERROR THIS IS NOT A LIGID FILE! Description header doesn't match : " << ligidFilePath << std::endl;
            return;
        }

        this->ligidFilePath = ligidFilePath;
        this->folderPath = util.removeExtension(ligidFilePath);
        if(this->folderPath[this->folderPath.size()-1] == '/' || this->folderPath[this->folderPath.size()-1] == '\\') //Make sure folder path doesn't have seperator at the end
            this->folderPath.pop_back();
        this->projectName = util.getLastWordBySeparatingWithChar(folderPath,folderDistinguisher);

        
        updateLigidFile();

        
        //Remove the / or \ from the projectName if there are any     
        if(projectName[projectName.size()-1] == '/' || projectName[projectName.size()-1] == '\\')
            projectName.pop_back();
        if(projectName[0] == '/' || projectName[0] == '\\')
            projectName.erase(projectName.begin());
    
        
        //Load the textures
        for (const auto & entry : std::filesystem::directory_iterator(this->folderPath + folderDistinguisher + "Textures")){
            std::string texturePath = entry.path().string();

            Texture texture;
            texture.load(texturePath.c_str());

            library.textures.push_back(texture);
        }

        //Load the brushes
        for (const auto & entry : std::filesystem::directory_iterator(this->folderPath + folderDistinguisher + "Brushes")){
            std::string brushPath = entry.path().string();

            Brush brush;
            brush.readFile(brushPath);
            brush.updateDisplayTexture(shaders.twoDPainting,shaders.buttonShader);


            library.brushes.push_back(brush);
        }
    }
};

#endif