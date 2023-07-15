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
#include "GUI/Elements/Elements.hpp"

#include <string>
#include <iostream>
#include <vector>


NodeConnection::NodeConnection(int nodeIndex,int inputIndex){
    this->nodeIndex = nodeIndex;
    this->inputIndex = inputIndex;
}

Node::Node(){}

Node::Node(int nodeIndex, int materialID, Shader buttonShader, Shader connectionCurveShader, ColorPalette colorPalette, AppTextures appTextures, glm::vec2 videoScale){
    
    //Get the shaders to the member variables
    this->buttonShader = buttonShader;
    this->connectionCurveShader = connectionCurveShader;

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
                        NodeIO("Input1",Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,8),colorPalette,buttonShader,"Input1",appTextures.TDModelIcon,2.f,false)),colorPalette.mainColor,colorPalette,buttonShader,videoScale,1),
                    };
            
        outputs =   {
                        NodeIO("Input1",Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1),colorPalette,buttonShader,"Input1",appTextures.TDModelIcon,2.f,false)),colorPalette.mainColor,colorPalette,buttonShader,videoScale,2),
                    };
    }    
    else if(nodeIndex == MESH_NODE){
        //Insert the inputs later
    }

    //Add inputs & outputs to the IOs vector
    this->addVectors(this->IOs, inputs);
    this->addVectors(this->IOs, outputs);

    this->nodePanel = Panel(
        buttonShader,
        colorPalette,
        {
            //Sections
        },
        this->scale, //Scale of the node
        this->pos, //Pos of the node
        colorPalette.mainColor,
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
    this->barButton = Button(ELEMENT_STYLE_BASIC,nodePanel.scale,colorPalette,buttonShader,"Node",Texture(),0.f,false);
    
    /*
    //Coloring & renaming the bar button
    if(nodeIndex == MATERIAL_NODE){
        this->barButton.color = glm::vec4(100.f/255.f, 32.f/255.f, 125.f/255.f, 1.f);
    }

    else if(nodeIndex == MESH_NODE){
        this->barButton.color = glm::vec4(214.f/255.f, 65.f/255.f, 65.f/255.f, 1.f);
    }
    */

    //Create the elements of the node panel
    createPanelUsingIOs();
}