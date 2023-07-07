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

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <ctime>

//forward declerations of the util functions
void readmeshNodeSceneData(std::ifstream &rf, std::vector<Node> &meshNodeScene, Shaders shaders, ColorPalette colorPalette, AppTextures appTextures, glm::vec2 videoScale);

bool Project::readLigidFile(
                                std::string path,
                                time_t &creationDate,
                                time_t &lastOpenedDate, 
                                std::vector<Node> &meshNodeScene, 
                                int& textureRes,
                                Shaders shaders, 
                                ColorPalette colorPalette, 
                                AppTextures appTextures, 
                                glm::vec2 videoScale
                            ){ //Returns true if path is a ligid file
    
    if(path.size()){
        std::ifstream rf(path, std::ios::out | std::ios::binary);
		
        if(!rf) {
            std::cout << "ERROR WHILE READING LIGID FILE! Cannot open file : " << path << std::endl;
            return false;
        }

        //!HEADER

        //!Description
        uint64_t h1 = 0xBBBBBBBB; 
        uint64_t h2 = 0xCA4B6C78; 
        uint64_t h3 = 0x9A9A2C48; 
        
        uint64_t ch1; 
        uint64_t ch2; 
        uint64_t ch3; 

        rf.read(reinterpret_cast<char*>(   &ch1    ),sizeof(uint64_t));
        rf.read(reinterpret_cast<char*>(   &ch2    ),sizeof(uint64_t));
        rf.read(reinterpret_cast<char*>(   &ch3    ),sizeof(uint64_t));

        if(ch1 != h1 || ch2 != h2 || ch3 != h3){
            return false;
        }

        //! Read the creation date
        rf.read(reinterpret_cast<char*>(   &creationDate    ),sizeof(time_t));

        //! Read the last opened date
        rf.read(reinterpret_cast<char*>(   &lastOpenedDate    ),sizeof(time_t));

        //!meshNodeScene
        readmeshNodeSceneData(rf, meshNodeScene, shaders, colorPalette, appTextures, videoScale);

        //!Texture resolution
        rf.read(reinterpret_cast<char*>(   &textureRes    ),sizeof(int));
        
        return true;
    }
}


void readmeshNodeSceneData(std::ifstream &rf, std::vector<Node> &meshNodeScene, Shaders shaders, ColorPalette colorPalette, AppTextures appTextures, glm::vec2 videoScale){
    
    //Read the node size
    uint64_t nodeSize;
    rf.read(reinterpret_cast<char*>(   &nodeSize    )    , sizeof(uint64_t));

    if(nodeSize)
        meshNodeScene.clear();
    
    //For each node
    for (size_t i = 0; i < nodeSize; i++)
    {

        //Read the node index, (MATERIAL_NODE , MESH_NODE)
        int nodeIndex;
        rf.read(reinterpret_cast<char*>(   &nodeIndex    ), sizeof(int));

        Node node = Node(
                            nodeIndex, 
                            0,
                            shaders.buttonShader,
                            shaders.connectionCurve,
                            colorPalette,
                            appTextures,
                            videoScale
                        );

        //Read the material ID 
        rf.read(reinterpret_cast<char*>(   &node.materialID    ), sizeof(int));

        //Read the node pos
        rf.read(reinterpret_cast<char*>(   &node.nodePanel.pos    ), sizeof(glm::vec3));

        //Read the IO size
        uint64_t IOSize;
        rf.read(reinterpret_cast<char*>(   &IOSize    )    , sizeof(uint64_t));
        
        //For each IO
        for (size_t IOI = 0; IOI < IOSize; IOI++)
        {   
            //Read the connection size
            uint64_t connectionSize;
            rf.read(reinterpret_cast<char*>(   &connectionSize    )    , sizeof(uint64_t));
            
            //For each connection of the IO
            for (size_t conI = 0; conI < connectionSize; conI++)
            {
                int inputIndex;
                int nodeIndex;
                rf.read(reinterpret_cast<char*>(   &inputIndex    )    , sizeof(int));
                rf.read(reinterpret_cast<char*>(   &nodeIndex     )    , sizeof(int));
            
                // NodeConnection connection(nodeIndex,inputIndex);

                // node.IOs[IOI].connections.push_back(connection);
            }
        }

        meshNodeScene.push_back(node);
    }
}