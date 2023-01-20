#include <iostream>

#include <string>
#include <vector>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Core/LigidPainter.h"
#include "Core/Render/Render.h"
#include "Core/UI/UserInterface.h"
#include "Core/gl.h"
#include "Core/Utilities.h"
#include "Core/Load.hpp"

void Render::renderTheNodes(NodeScene material){


    std::vector<Node> renderingPipeline;


    //Default indexes
    for (size_t i = 0; i < material.nodes.size(); i++)
    {
        material.nodes[i].renderingIndex = 10000;
    }
    
    int renderingIndex = 0;

    //Process indexes independant nodes
    for (size_t i = 0; i < material.nodes.size(); i++)
    {
        bool nodeHasOutputConnection = false;
        bool nodeHasInputConnection = false;
        
        for (size_t inputI = 0; inputI < material.nodes[i].inputs.size(); inputI++)
        {
            if(material.nodes[i].inputs[inputI].nodeConnectionIndex != 10000 && material.nodes[i].inputs[inputI].inputConnectionIndex != 10000){
                nodeHasInputConnection = true;
            }
        }
        for (size_t outputI = 0; outputI < material.nodes[i].outputs.size(); outputI++)
        {
            if(material.nodes[i].outputs[outputI].nodeConnectionIndex != 10000 && material.nodes[i].outputs[outputI].inputConnectionIndex != 10000){
                nodeHasOutputConnection = true;
            }
        }
        
        if(!nodeHasInputConnection){
            renderingPipeline.push_back(material.nodes[i]);
            material.nodes[i].renderingIndex = renderingIndex; 
            renderingIndex++;
        }
    }
    for (int i = 0; i < 1000; i++)
    {
        bool isChangesAreMade = false;
        for (int nodeI = 0; nodeI < material.nodes.size(); nodeI++)
        {
            bool addToPipeline = false;
            for (int inputI = 0; inputI < material.nodes[nodeI].inputs.size(); inputI++)
            {
                if(material.nodes[nodeI].inputs[inputI].nodeConnectionIndex != 10000 && material.nodes[nodeI].inputs[inputI].inputConnectionIndex != 10000 && material.nodes[nodeI].renderingIndex == 10000){
                    NodeInput input = material.nodes[nodeI].inputs[inputI];

                    if(material.nodes[input.nodeConnectionIndex].renderingIndex != 10000){
                        addToPipeline = true;
                    }
                    else{
                        addToPipeline = false;
                        break;
                    }
                }
            }
            if(addToPipeline){
                renderingPipeline.push_back(material.nodes[nodeI]);
                material.nodes[nodeI].renderingIndex = renderingIndex; 
                renderingIndex++;
                isChangesAreMade = true;
            }
        }
        if(!isChangesAreMade){
            break;
        }
    }

    std::cout << renderingPipeline.size() << ' ';
}