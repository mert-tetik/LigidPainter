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

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "SettingsSystem/Settings.hpp"
#include "NodeSystem/Node/Node.hpp"
#include "LibrarySystem/Library.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <ctime>

#define WRITE_BITS(var, type, loc)   if(!wf.write(reinterpret_cast<char*>(   &var     ), sizeof(type))){ \
                                                    LGDLOG::start<< "ERROR : Writing ligid file. Failed to write at : " << loc << LGDLOG::end;\
                                                    return false; \
                                                }

#define WRITE_STR(str)  int strSize = str.size(); \
                        WRITE_BITS(strSize, int, "String size"); \
                        for (size_t i = 0; i < strSize; i++) \
                        {\
                            char c = str[i]; \
                            WRITE_BITS(c, char, "String character");\
                        }\


//Forward declerations for the util functions
bool writemeshNodeSceneData(std::ofstream &wf);

bool Project::wrtLigidFile(std::string path){
    std::ofstream wf;
    
    wf = std::ofstream(path, std::ios::out | std::ios::binary);

    if(!wf) {
        LGDLOG::start<< "ERROR! : Writing ligid file." << LGDLOG::end;
        return false;
    }

    // ------------- Description ------------
    uint64_t h1 = 0xBBBBBBBB; 
    uint64_t h2 = 0xCA4B6C78; 
    uint64_t h3 = 0x9A9A2C48; 
    WRITE_BITS(h1, uint64_t, "");
    WRITE_BITS(h2, uint64_t, "");
    WRITE_BITS(h3, uint64_t, "");
    
    // ------------- Version number ------------
    uint32_t versionNumber2000 = 2000;  
    uint32_t versionNumber2100 = 2100;  
    uint32_t versionNumber2200 = 2200;

    WRITE_BITS(versionNumber2200, uint32_t, "");

    // ------------- Date ------------
    time_t currentDate = time(0);
    WRITE_BITS(currentDate, time_t, "");

    time_t lastOpenedDate = time(0);
    WRITE_BITS(lastOpenedDate, time_t, "");

    // ------------- Current model data ------------
    uint32_t modelTitleSize = getModel()->title.size();
    wf.write(reinterpret_cast<char*>(   &modelTitleSize    )    , sizeof(uint32_t));
    for (size_t i = 0; i < modelTitleSize; i++)
    {
        char c = getModel()->title[i];
        wf.write(reinterpret_cast<char*>(   &c     )    , sizeof(char));
    }
    
    // ------------- Node scene ------------
    if(!writemeshNodeSceneData(wf))
        return false;

    // ------------- Settings ------------
    int textureRes = Settings::properties()->textureRes;
    WRITE_BITS(textureRes, int, "");

    return true;
}

bool Project::writeLigidFile(std::string path){
    bool res = wrtLigidFile(path);
    if(!res){
        try
        {
            if(std::filesystem::exists(path))
                std::filesystem::remove(path);
            
            std::ofstream outputFile(path);
            
            outputFile.close();
        }
        catch (const std::filesystem::filesystem_error& ex) {
            LGDLOG::start << "ERROR : Filesystem : Recreating ligid file : " << ex.what() << LGDLOG::end;
        }
    }

    return res;
}



//------------ UTIL FUNCTIONS -------------


bool writemeshNodeSceneData(std::ofstream &wf){
    
    //Write the node size
    uint64_t nodeSize = NodeScene::getArraySize();
    WRITE_BITS(nodeSize, uint64_t, "");
    
    //For each node
    for (size_t i = 0; i < nodeSize; i++)
    {
        //Write the node index, (MATERIAL_NODE , MESH_NODE ..)
        wf.write(reinterpret_cast<char*>(   &NodeScene::getNode(i)->nodeIndex    ), sizeof(int));

        if(NodeScene::getNode(i)->nodeIndex == MATERIAL_MASK_NODE || NodeScene::getNode(i)->nodeIndex == MATERIAL_ID_NODE)
            NodeScene::getNode(i)->nodePanel.sections[0].elements[0].button.texture.writeTextureData(wf);

        if(NodeScene::getNode(i)->nodeIndex == MATERIAL_MASK_NODE)
            wf.write(reinterpret_cast<char*>(   &NodeScene::getNode(i)->nodePanel.sections[0].elements[1].rangeBar.value    ), sizeof(int));
        
        //Write the material title
        std::string destMaterialTitle = "Disappeared Material";
        for (size_t matI = 0; matI < Library::getMaterialArraySize(); matI++)
        {
            if(Library::getMaterialObj(matI).uniqueID == NodeScene::getNode(i)->materialID){
                if(destMaterialTitle != "Disappeared Material")
                    LGDLOG::start << "WARNING! : Multiple materials with the same ID detected." << LGDLOG::end;
                
                destMaterialTitle = Library::getMaterialObj(matI).title;
            }
        }
        
        WRITE_STR(destMaterialTitle)
        
        //Write the node pos
        wf.write(reinterpret_cast<char*>(   &NodeScene::getNode(i)->nodePanel.pos    ), sizeof(glm::vec3));

        //Write the IO size
        uint64_t IOSize = NodeScene::getNode(i)->IOs.size();
        wf.write(reinterpret_cast<char*>(   &IOSize    )    , sizeof(uint64_t));
        
        //For each IO
        for (size_t IOI = 0; IOI < IOSize; IOI++)
        {   

            //Write the connection size
            uint64_t connectionSize = NodeScene::getNode(i)->IOs[IOI].connections.size();
            wf.write(reinterpret_cast<char*>(   &connectionSize    )    , sizeof(uint64_t));
            
            //For each connection of the IO
            for (size_t conI = 0; conI < connectionSize; conI++)
            {
                wf.write(reinterpret_cast<char*>(   &NodeScene::getNode(i)->IOs[IOI].connections[conI].inputIndex    )    , sizeof(int));
                wf.write(reinterpret_cast<char*>(   &NodeScene::getNode(i)->IOs[IOI].connections[conI].nodeIndex     )    , sizeof(int));
            }
        }
    }

    return true;
}