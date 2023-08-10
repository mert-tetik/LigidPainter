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
#include "GUI/Elements/Elements.hpp"

#include <string>
#include <iostream>
#include <vector>


NodeConnection::NodeConnection(int nodeIndex,int inputIndex){
    this->nodeIndex = nodeIndex;
    this->inputIndex = inputIndex;
}

Node::Node(){}

Node::Node(int nodeIndex, int materialID, ColorPalette colorPalette, AppTextures appTextures, glm::vec2 videoScale){
    
    //Get the shaders to the member variables
    this->colorPalette = colorPalette;

    //Get the other stuff to the member variables
    this->appTextures = appTextures; 
    this->videoScale = videoScale; 

    //Get the material ID data to a member variable 
    this->materialID = materialID;

    //Get the node index data to a member variable 
    this->nodeIndex = nodeIndex;

    std::vector<NodeIO> inputs;
    std::vector<NodeIO> outputs;
    
    if(nodeIndex == MATERIAL_NODE){
        //Material node
        inputs =    {
                        NodeIO("Input1",Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,8),colorPalette,"Input1",Texture(),2.f,false)),colorPalette.mainColor,colorPalette,videoScale,1),
                    };
            
        outputs =   {
                        NodeIO("Input1",Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1),colorPalette,"Input1",Texture(),2.f,false)),colorPalette.mainColor,colorPalette,videoScale,2),
                    };
    }    
    else if(nodeIndex == MESH_NODE){
        //Insert the inputs later
    }
    else if(nodeIndex == MATERIAL_MASK_NODE){
        inputs =    {
            NodeIO("Put a Texture",Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,8),colorPalette,"Put a Texture",Texture(),2.f,false)),colorPalette.mainColor,colorPalette,videoScale,1),
            NodeIO("Offset",Element(RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1),colorPalette,"Offset",Texture(),2.f,0.f,100.f,50.f, appTextures)),colorPalette.mainColor,colorPalette,videoScale,1),
            NodeIO("Black Side",Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1),colorPalette,"Black Side",Texture(),2.f,false)),colorPalette.mainColor,colorPalette,videoScale,0),
            NodeIO("White Side",Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1),colorPalette,"White Side",Texture(),2.f,false)),colorPalette.mainColor,colorPalette,videoScale,0),
        };
        outputs.push_back(NodeIO("Output",Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1), this->colorPalette, "Output", Texture(), 2.f,false)),colorPalette.mainColor,colorPalette,videoScale,2));
        inputs[0].element.button.textureSelection = true;
        
        inputs[2].element.button.color = glm::vec4(glm::vec3(0.f), 1.f);
        inputs[3].element.button.color = glm::vec4(1.f);
    }
    else if(nodeIndex == MATERIAL_ID_NODE){
            inputs =    {
                NodeIO("Put a Texture",Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,8),colorPalette,"Put a Texture",Texture(),2.f,false)),colorPalette.mainColor,colorPalette,videoScale,1),
            };
            inputs[0].element.button.textureSelection = true;
    }
    else
        std::cout << "WARNING : Unknown node type is requested : " << nodeIndex << std::endl;

    //Add inputs & outputs to the IOs vector
    this->addVectors(this->IOs, inputs);
    this->addVectors(this->IOs, outputs);

    glm::vec3 panelColor;
    if(nodeIndex == MATERIAL_NODE){
        panelColor = glm::vec3(53.f/255.f, 73.f/255.f, 74.f/255.f);
    }
    else if(nodeIndex == MESH_NODE){
        panelColor = glm::vec3(67.f/255.f, 74.f/255.f, 54.f/255.f);
    }
    else if(nodeIndex == MATERIAL_ID_NODE){
        panelColor = glm::vec3(84.f/255.f, 69.f/255.f, 84.f/255.f);
    }
    else if(nodeIndex == MATERIAL_MASK_NODE){
        panelColor = glm::vec3(52.f/255.f, 52.f/255.f, 63.f/255.f);
    }


    this->nodePanel = Panel(
        
        colorPalette,
        {
            //Sections
        },
        this->scale, //Scale of the node
        this->pos, //Pos of the node 
        glm::vec4(panelColor, 0.5f),
        colorPalette.secondColor,
        true,
        false,
        false,
        true,
        true,
        0.5f,
        1,
        {},
        20.f,
        false
    );
    
    //Don't clear the depth buffer right after rendering the panel
    nodePanel.clearDepthBuffer = false;
    
    //Create the barbutton
    this->barButton = Button(ELEMENT_STYLE_BASIC,nodePanel.scale,colorPalette,"Node",Texture(),0.f,false);
    if(nodeIndex == MATERIAL_ID_NODE){
        this->barButton.text = "Material ID Node";
    }
    else if(nodeIndex == MATERIAL_MASK_NODE){
        this->barButton.text = "Material Mask Node";
    }
    
    this->barButton.color = glm::vec4(panelColor, 0.7f);
    
    //Coloring & renaming the bar button
    if(nodeIndex == MATERIAL_NODE){
        /*Positioning the node*/
        this->nodePanel.pos.x = 40;
        this->nodePanel.pos.y = 120.7222;
    }

    else if(nodeIndex == MESH_NODE){
        /*Positioning the node*/
        this->nodePanel.pos.x = 55;
        this->nodePanel.pos.y = 110.7222;
    }


    //Create the elements of the node panel
    createPanelUsingIOs();
}