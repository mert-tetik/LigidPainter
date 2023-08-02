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

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <ctime>

//Forward declerations for the util functions
void writeDescriptionHeader(std::ofstream &wf);
void writeDateData(std::ofstream &wf);
void writemeshNodeSceneData(std::ofstream &wf, std::vector<Node> meshNodeScene);



void Project::writeLigidFile(const std::vector<Node> meshNodeScene, int textureRes){
    std::ofstream wf(this->ligidFilePath(), std::ios::out | std::ios::binary);
		
    if(!wf) {
        std::cout << "ERROR WHILE WRITING LIGID FILE! Cannot open file : " << this->ligidFilePath() << std::endl;
        return;
    }

    //!Description
    writeDescriptionHeader(wf);

    //!Date
    writeDateData(wf);

    //!meshNodeScene
    writemeshNodeSceneData(wf,meshNodeScene);

    //!Texture resolution
    wf.write(reinterpret_cast<char*>(   &textureRes    ),sizeof(int));

}



//------------ UTIL FUNCTIONS -------------


void writeDescriptionHeader(std::ofstream &wf){
    uint64_t h1 = 0xBBBBBBBB; 
    uint64_t h2 = 0xCA4B6C78; 
    uint64_t h3 = 0x9A9A2C48; 

    wf.write(reinterpret_cast<char*>(   &h1    ),sizeof(uint64_t));
    wf.write(reinterpret_cast<char*>(   &h2    ),sizeof(uint64_t));
    wf.write(reinterpret_cast<char*>(   &h3    ),sizeof(uint64_t));
}

void writeDateData(std::ofstream &wf){
    //! Write the creation date
    time_t currentDate = time(0);
    wf.write(reinterpret_cast<char*>(   &currentDate    ),sizeof(time_t));
    
    //! Write the last opened date
    time_t lastOpenedDate = time(0);
    wf.write(reinterpret_cast<char*>(   &lastOpenedDate    ),sizeof(time_t));
}

void writemeshNodeSceneData(std::ofstream &wf, std::vector<Node> meshNodeScene){
    //Write the node size
    uint64_t nodeSize = meshNodeScene.size();
    wf.write(reinterpret_cast<char*>(   &nodeSize    )    , sizeof(uint64_t));
    
    //For each node
    for (size_t i = 0; i < nodeSize; i++)
    {
        //Write the node index, (MATERIAL_NODE , MESH_NODE ..)
        wf.write(reinterpret_cast<char*>(   &meshNodeScene[i].nodeIndex    ), sizeof(int));
        
        //Write the material ID
        wf.write(reinterpret_cast<char*>(   &meshNodeScene[i].materialID    ), sizeof(int));

        //Write the node pos
        wf.write(reinterpret_cast<char*>(   &meshNodeScene[i].nodePanel.pos    ), sizeof(glm::vec3));

        //Write the IO size
        uint64_t IOSize = meshNodeScene[i].IOs.size();
        wf.write(reinterpret_cast<char*>(   &IOSize    )    , sizeof(uint64_t));
        
        //For each IO
        for (size_t IOI = 0; IOI < IOSize; IOI++)
        {   

            //Write the connection size
            uint64_t connectionSize = meshNodeScene[i].IOs[IOI].connections.size();
            wf.write(reinterpret_cast<char*>(   &connectionSize    )    , sizeof(uint64_t));
            
            //For each connection of the IO
            for (size_t conI = 0; conI < connectionSize; conI++)
            {
                wf.write(reinterpret_cast<char*>(   &meshNodeScene[i].IOs[IOI].connections[conI].inputIndex    )    , sizeof(int));
                wf.write(reinterpret_cast<char*>(   &meshNodeScene[i].IOs[IOI].connections[conI].nodeIndex     )    , sizeof(int));
            }
        }
    }
}