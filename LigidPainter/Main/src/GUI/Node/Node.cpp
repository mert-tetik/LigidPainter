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
        20.f,
        false
    );
    
    //Don't clear the depth buffer right after rendering the panel
    nodePanel.clearDepthBuffer = false;
    
    //Create the barbutton
    this->barButton = Button(BUTTON_STYLE_BASIC,nodePanel.scale,colorPalette,buttonShader,"Node",Texture(),0.f,false);

    //Create the elements of the node panel
    createPanelUsingIOs();
}



void Node::render(glm::vec2 videoScale,Mouse& mouse,Timer &timer,TextRenderer &textRenderer,Panel nodeEditorPanel,std::vector<Node> &nodeScene,int currentNodeIndex){
    
    //Barriers (In order to prevent the overflow)
    bool cursorOnBarriers = renderBarriers(nodeEditorPanel,mouse);

    //Render the node panel which contains the input buttons and stuff
    nodePanel.render(videoScale,mouse,timer,textRenderer,!cursorOnBarriers);
    
    //Auto scaling the node panel compatible with it's elements
    if(nodePanel.sections.size()){
        
        nodePanel.scale.y = 0;
        for (size_t i = 0; i < nodePanel.sections[0].elements.size(); i++)
        {
            nodePanel.scale.y += nodePanel.sections[0].elements[i].scale.y;
            nodePanel.scale.y += nodePanel.sections[0].elements[i].panelOffset/2.f;
        }

        nodePanel.scale.y += 2;

    }
    
    //Render & process all the inputs & outputs
    for (size_t i = 0; i < IOs.size(); i++)
    {
        //IO state 0 = input
        //IO state 1 = input without connection
        //IO state 2 = output
        //IO state 3 = output without connection

        //Don't render the IO circle if the IO has no connection
        if(IOs[i].state != 1 && IOs[i].state != 3){

            //Move the IO circle to the node panel IO button             
            IOs[i].IOCircle.pos = nodePanel.sections[0].elements[i].pos;
            IOs[i].IOCircle.pos.z += 0.01f;        
        
            
            //Move the IO Circle to the left side of the the IO element if is input
            if(IOs[i].state == 0){//Is input
                IOs[i].IOCircle.pos.x -= nodePanel.sections[0].elements[i].scale.x;
            }

            //Move the IO Circle to the right side of the the IO element if is input
            else if(IOs[i].state == 2){//Is output
                IOs[i].IOCircle.pos.x += nodePanel.sections[0].elements[i].scale.x;
            }

            //If pressed to the circle
            if(IOs[i].IOCircle.clickState1){

                nodePanel.leftSide.pressed = false;
                nodePanel.rightSide.pressed = false;
                
                //If an input is pressed that already has a connection
                if(IOs[i].state == 0 && IOs[i].connections.size()){
                    int resNodeI = 1000;
                    int resIOI = 1000;
                    
                    getTheIOConnectedToTheInput(resNodeI,resIOI,currentNodeIndex,i,nodeScene);

                    if(resNodeI != 1000 && resIOI != 1000){
                        nodeScene[resNodeI].IOs[resIOI].IOCircle.clickState1 = true;
                        IOs[i].IOCircle.clickState1 = false;
                    }
                    
                    clearConnections(currentNodeIndex,i,nodeScene);

                }

                //Draw a line from the circle to the cursor
                drawLine(
                            glm::vec2(IOs[i].IOCircle.pos.x,IOs[i].IOCircle.pos.y), //Circle pos
                            mouse.cursorPos/videoScale * 100.f, //Cursor pos in the range of 0 - 100
                            videoScale,
                            nodeEditorPanel);

                //First frame line released
                if(!mouse.LPressed){
                    
                    //Those values will remain 1000 if no IO circle is hovered
                    int hoveredNodeI = 1000;
                    int hoveredIOI = 1000;

                    getHoveredInputs(hoveredNodeI,hoveredIOI,nodeScene);

                    //A IO circle is hovered
                    if(hoveredNodeI != 1000 && hoveredIOI != 1000){
                        
                        //Delete the previous connection if an input IO is connected
                        if(IOs[i].state == 0)
                            clearConnections(currentNodeIndex,i,nodeScene);

                        //If the circle hovered already has a connection & is an input
                        if(doHaveConnection(hoveredNodeI,hoveredNodeI,nodeScene) && getStateData(hoveredNodeI,hoveredNodeI,nodeScene) == 0){
                           //Than remove the connections of the circle hovered
                            clearConnections(hoveredNodeI,hoveredIOI,nodeScene);
                        }

                        //Create a connection 
                        createConnection(hoveredNodeI,hoveredIOI,currentNodeIndex,i,nodeScene);
                    }
                }
            }

            //Render the connections of the IO if is an output
            if(IOs[i].state == 2){

                for (size_t conI = 0; conI < IOs[i].connections.size(); conI++)
                {

                    //Which node the connection is connected to
                    Node connectedNode = nodeScene[IOs[i].connections[conI].nodeIndex];

                    //Which IO circle the connection is connected to
                    NodeIO connectedIO = connectedNode.IOs[IOs[i].connections[conI].inputIndex]; 

                    drawLine(
                                glm::vec2(IOs[i].IOCircle.pos.x,IOs[i].IOCircle.pos.y), //Circle pos
                                glm::vec2(connectedIO.IOCircle.pos.x,connectedIO.IOCircle.pos.y), //Connected Circle pos
                                videoScale,
                                nodeEditorPanel);

                }
            }
            


            //Render the IO circle
            IOs[i].IOCircle.render(videoScale,mouse,timer,textRenderer,true);
        }
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