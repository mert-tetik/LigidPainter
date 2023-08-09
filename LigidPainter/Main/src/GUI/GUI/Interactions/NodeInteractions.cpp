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

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <filesystem>

void nodeInteraction(
                        Model &model, 
                        std::vector<Node> &meshNodeScene,
                        Shaders shaders,
                        ColorPalette colorPalette,
                        glm::vec2 videoScale,
                        Library library, 
                        Scene scene, 
                        int textureRes
                    )
{
    
    //Update the mesh node if a new model is added
    if(model.newModelAdded){
        
        Node previousNode = meshNodeScene[0]; //(Current node (unchanged))

        meshNodeScene[0].uploadNewIOs(model, colorPalette);

        for (size_t i = 0; i < previousNode.IOs.size(); i++)
        {
            if(i >= meshNodeScene[0].IOs.size())
                break;

            meshNodeScene[0].IOs[i].connections = previousNode.IOs[i].connections;
        }
        
        //Update all the connections
        UTIL::updateAllTheNodeConnections(meshNodeScene);
    
        UTIL::updateNodeResults(meshNodeScene, model, library, shaders.heightToNormalMap, shaders.boundaryExpandingShader, scene, textureRes, -1);
    }
}