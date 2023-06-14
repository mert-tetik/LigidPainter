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
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"
#include "GUI/Elements.hpp"

#include <string>
#include <iostream>
#include <vector>

//TODO Make the node editor panel movable
//TODO Prevent connection to connected input


NodeConnection::NodeConnection(int nodeIndex,int inputIndex){
    this->nodeIndex = nodeIndex;
    this->inputIndex = inputIndex;
}

Node::Node(){}

void Node::loadIO(std::vector<NodeIO> inputs, std::vector<NodeIO> outputs, Shader buttonShader,Shader singleCurveShader,ColorPalette colorPalette,int materialID,int nodeIndex){
    this->buttonShader = buttonShader;
    this->singleCurveShader = singleCurveShader;

    addVectors(this->IOs,inputs);
    addVectors(this->IOs,outputs);
    this->materialID = materialID;
    this->nodeIndex = nodeIndex;
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
        20.f
    );
    
    //Don't clear the depth buffer right after rendering the panel
    nodePanel.clearDepthBuffer = false;
    
    //Create the barbutton
    this->barButton = Button(2,nodePanel.scale,colorPalette,buttonShader,"Node",Texture(),0.f,false);

    //Create the elements of the node panel
    createPanelUsingIOs();
}



void Node::render(glm::vec2 videoScale,Mouse& mouse,Timer &timer,TextRenderer &textRenderer,Panel nodeEditorPanel,std::vector<Node> &nodeScene,int currentNodeIndex){
    
    //Barriers (In order to prevent the overflow)
    bool cursorOnBarriers = renderBarriers(nodeEditorPanel,mouse);

    //Render the node panel which contains the input buttons and stuff
    nodePanel.render(videoScale,mouse,timer,textRenderer,!cursorOnBarriers);
    
    //Auto scaling the node panel compatible with it's elements
    const float nodePanelTopPos = nodePanel.pos.y - nodePanel.scale.y; //In y axis ofc 
    const float nodePanelLastElementPos = nodePanel.sections[0].elements[nodePanel.sections[0].elements.size() - 1].button.pos.y; //In y axis
    const float nodePanelLastElementScale = nodePanel.sections[0].elements[nodePanel.sections[0].elements.size() - 1].button.scale.y; //In y axis
    nodePanel.scale.y = (nodePanelLastElementPos + nodePanelLastElementScale) - nodePanelTopPos; 
    
    //Render & process all the inputs & outputs
    for (size_t i = 0; i < IOs.size(); i++)
    {
        //Move the IO Circle to the left side of the the input element
        IOs[i].IOCircle.pos = nodePanel.sections[0].elements[i].pos;
        IOs[i].IOCircle.pos.x -= nodePanel.sections[0].elements[i].scale.x;
        IOs[i].IOCircle.pos.z = nodePanel.sections[0].elements[i].pos.z + 0.01f;        

        IOs[i].IOCircle.render(videoScale,mouse,timer,textRenderer,true);
    }


    //Position the bar button        
    barButton.pos = nodePanel.pos;
    barButton.scale = nodePanel.scale;
    barButton.scale.y = 1.5f;
    barButton.pos.z += 0.02f;
    barButton.pos.y = nodePanel.pos.y - nodePanel.scale.y - barButton.scale.y; 
    //Render the bar button
    barButton.render(videoScale,mouse,timer,textRenderer,!cursorOnBarriers);
    //Move the node panel if bar button is pressed
    if(barButton.clickState1){ //Pressed
        nodePanel.pos.x += mouse.mouseOffset.x/videoScale.x * 100.f;
        nodePanel.pos.y += mouse.mouseOffset.y/videoScale.y * 100.f;
    }
    glClear(GL_DEPTH_BUFFER_BIT);
}