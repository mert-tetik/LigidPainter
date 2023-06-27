/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    TODO UPDATE LIGID FILE DATE

*/

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "UTIL/Util.hpp"
#include "GUI/Elements.hpp"
#include "3D/ThreeD.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <ctime>


void Project::updateProject(Library &library, std::vector<Node> &nodeScene, int& textureRes){
    if(!std::filesystem::exists(folderPath)){
        std::cout << "ERROR CAN'T UPDATE THE PROJECT FOLDER : " << ligidFilePath << std::endl;
        return;
    }
    
    //!Textures
    std::string textureFolderPath = this->folderPath + UTIL::folderDistinguisher() + "Textures";

    //Clear the textures folder
    UTIL::deleteFilesInFolder(textureFolderPath);

    //Write the textures
    for (size_t i = 0; i < library.textures.size(); i++)
    {
        //Export texture
        library.textures[i].exportTexture(textureFolderPath,"PNG");
    }
    
    
    //!Materials
    std::string materialFolderPath = this->folderPath + UTIL::folderDistinguisher() + "Materials";

    //Clear the materials folder
    UTIL::deleteFilesInFolder(materialFolderPath);

    //Write the materials
    for (size_t i = 0; i < library.materials.size(); i++)
    {
        //Export material
        library.materials[i].writeFile(materialFolderPath);
    }


    //!Brushes
    std::string brushFolderPath = this->folderPath + UTIL::folderDistinguisher() + "Brushes";

    //Clear the brushes folder
    UTIL::deleteFilesInFolder(brushFolderPath);

    //Write the brushes
    for (size_t i = 0; i < library.brushes.size(); i++)
    {
        //Export brush
        library.brushes[i].saveFile(brushFolderPath);
    }
    
    
    //!3D Models
    std::string tdModelFolderPath = this->folderPath + UTIL::folderDistinguisher() + "3DModels";

    //Clear the brushes folder
    UTIL::deleteFilesInFolder(tdModelFolderPath);

    //Write the brushes
    for (size_t i = 0; i < library.TDModels.size(); i++)
    {
        //Export brush
        library.TDModels[i].exportModel(tdModelFolderPath);
    }

    writeLigidFile(nodeScene, textureRes);
}