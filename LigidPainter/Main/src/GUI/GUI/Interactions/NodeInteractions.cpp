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

#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "NodeSystem/Node/Node.hpp"
#include "SettingsSystem/Settings.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <filesystem>

void nodeInteraction(
                        
                        ColorPalette colorPalette,
                        glm::vec2 videoScale
                    )
{
    
    //Update the mesh node if a new model is added
    if(getModel()->newModelAdded){
        
        Node previousNode = *NodeScene::getNode(0); //(Current node (unchanged))

        NodeScene::getNode(0)->uploadNewIOs(colorPalette);

        for (size_t i = 0; i < previousNode.IOs.size(); i++)
        {
            if(i >= NodeScene::getNode(0)->IOs.size())
                break;

            NodeScene::getNode(0)->IOs[i].connections = previousNode.IOs[i].connections;
        }
        
        //Update all the connections
        NodeScene::updateAllTheNodeConnections();
    
        NodeScene::updateNodeResults(Settings::properties()->textureRes, -1);
    }
}