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
#include "LibrarySystem/Library.hpp"
#include "NodeSystem/Node/Node.hpp"
#include "MouseSystem/Mouse.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <map>

void Node::render(  
                    Timer &timer,
                    Panel nodeEditorPanel,
                    int currentNodeIndex,
                    NodePanel& nodePanelData,
                    int textureRes,
                    bool doMouseTracking
                )
{
    

    //Barriers (In order to prevent the overflow)
    this->cursorOnBarriers = renderBarriers(nodeEditorPanel);

    this->nodePanel.additionalPos.x = nodePanelData.position.x + nodePanelData.mixVal.x;
    this->nodePanel.additionalPos.y = nodePanelData.position.y + nodePanelData.mixVal.y - nodeEditorPanel.scale.y * 2;

    //Render the node panel which contains the input buttons and stuff
    nodePanel.render(timer, (this->nodeIndex == MATERIAL_ID_NODE || this->nodeIndex == MATERIAL_MASK_NODE) && doMouseTracking && !cursorOnBarriers);

    if(nodePanel.sections[0].elements[0].button.clicked){        
        if(this->nodeIndex == MATERIAL_ID_NODE){
            std::vector<NodeIO> inputs;
            std::vector<NodeIO> outputs;
            std::vector<glm::vec3> palette;

            inputs.push_back(NodeIO("Texture", nodePanel.sections[0].elements[0].button, ColorPalette::mainColor,1));

            for (size_t i = 0; i < palette.size(); i++)
            {
                inputs.push_back(NodeIO("Input1",Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1), "Input1", Texture(), 2.f,false)),ColorPalette::mainColor,0));
                inputs[inputs.size() - 1].element.button.color = glm::vec4(palette[i], 1.f);
            }

            outputs.push_back(NodeIO("Output",Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1), "Output", Texture(), 2.f,false)),ColorPalette::mainColor,2));

            this->uploadNewIOs(inputs, outputs);
        }

        NodeScene::updateNodeResults( textureRes, -1);
    }

    if(this->nodeIndex == MATERIAL_MASK_NODE){
        if(nodePanel.sections[0].elements[1].rangeBar.valueDoneChanging){
            NodeScene::updateNodeResults( textureRes, -1);
        }
    }


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
                    registerNodeAction("Severed node connection", Texture());
                    
                    int resNodeI = 1000;
                    int resIOI = 1000;
                    
                    getTheIOConnectedToTheInput(resNodeI,resIOI,currentNodeIndex,i);

                    if(resNodeI != 1000 && resIOI != 1000){
                        NodeScene::getNode(resNodeI)->IOs[resIOI].IOCircle.clickState1 = true;
                        IOs[i].IOCircle.clickState1 = false;
                    }
                    
                    clearConnections(currentNodeIndex,i);
                    NodeScene::updateNodeResults( textureRes, -1);
                }

                //Draw a line from the circle to the cursor
                drawLine(
                            glm::vec2(IOs[i].IOCircle.pos.x,IOs[i].IOCircle.pos.y), //Circle pos
                            *Mouse::cursorPos() / *Settings::videoScale() * 100.f, //Cursor pos in the range of 0 - 100
                            
                            nodeEditorPanel,
                            IOs[i].state == 2   
                        );

                //First frame line released
                if(!*Mouse::LPressed()){
                    
                    //Those values will remain 1000 if no IO circle is hovered
                    int hoveredNodeI = 1000;
                    int hoveredIOI = 1000;

                    getHoveredInputs(hoveredNodeI,hoveredIOI);

                    //A IO circle is hovered
                    if(hoveredNodeI != 1000 && hoveredIOI != 1000 && hoveredNodeI != currentNodeIndex){
                        if((NodeScene::getNode(hoveredNodeI)->IOs[hoveredIOI].state == 0 && IOs[i].state == 2) || (NodeScene::getNode(hoveredNodeI)->IOs[hoveredIOI].state == 2 && IOs[i].state == 0)){
                            registerNodeAction("New node connection", Texture());
                            
                            //Delete the previous connection if an input IO is connected
                            if(IOs[i].state == 0)
                                clearConnections(currentNodeIndex,i);

                            //If the circle hovered already has a connection & is an input
                            if(doHaveConnection(hoveredNodeI,hoveredNodeI) && getStateData(hoveredNodeI,hoveredNodeI) == 0){
                            //Than remove the connections of the circle hovered
                                clearConnections(hoveredNodeI,hoveredIOI);
                            }

                            //Create a connection 
                            createConnection(hoveredNodeI,hoveredIOI,currentNodeIndex,i);

                            Material material;
                            for (size_t i = 0; i < Library::getMaterialArraySize(); i++)
                            {
                                if(NodeScene::getNode(hoveredNodeI)->nodeIndex == MATERIAL_NODE){
                                    if(NodeScene::getNode(hoveredNodeI)->materialID == Library::getMaterial(i)->uniqueID)
                                        material = *Library::getMaterial(i);
                                }
                                else if(NodeScene::getNode(currentNodeIndex)->nodeIndex == MATERIAL_NODE){
                                    if(NodeScene::getNode(currentNodeIndex)->materialID == Library::getMaterial(i)->uniqueID)
                                        material = *Library::getMaterial(i);
                                }
                            }

                            NodeScene::updateNodeResults( textureRes, -1);
                        }
                    }
                }
            }

            //Render the connections of the IO if is an output
            if(IOs[i].state == 2){

                for (size_t conI = 0; conI < IOs[i].connections.size(); conI++)
                {

                    //Which node the connection is connected to
                    Node connectedNode = *NodeScene::getNode(IOs[i].connections[conI].nodeIndex);


                    //Which IO circle the connection is connected to
                    NodeIO connectedIO = connectedNode.IOs[IOs[i].connections[conI].inputIndex]; 

                    drawLine(
                                glm::vec2(IOs[i].IOCircle.pos.x,IOs[i].IOCircle.pos.y), //Circle pos
                                glm::vec2(connectedIO.IOCircle.pos.x,connectedIO.IOCircle.pos.y), //Connected Circle pos
                                
                                nodeEditorPanel,
                                1
                            );

                }
            }
            


            //Render the IO circle
            IOs[i].IOCircle.render(timer, !cursorOnBarriers && doMouseTracking);
        }
    }


    //Position the bar button        
    barButton.pos = nodePanel.pos;
    
    barButton.scale = nodePanel.scale;
    barButton.scale.y = 1.5f;
    
    barButton.pos.x += nodePanelData.position.x + nodePanelData.mixVal.x;
    barButton.pos.y = nodePanel.pos.y + nodePanelData.position.y + nodePanelData.mixVal.y - nodeEditorPanel.scale.y * 2 - nodePanel.scale.y; 
    barButton.pos.z += 0.02f;
    //Render the bar button
    barButton.render(timer,!cursorOnBarriers && doMouseTracking);
    
    //Move the node panel if bar button is pressed
    if(barButton.clickState1 && !nodePanel.topSide.pressed){ //Pressed

        //Prevent moving multiple nodes
        for (size_t i = 0; i < NodeScene::getArraySize(); i++)
        {
            if(i != currentNodeIndex)
                NodeScene::getNode(i)->barButton.clickState1 = false;
        }

        nodePanel.pos.x += Mouse::mouseOffset()->x/Settings::videoScale()->x * 100.f;
        nodePanel.pos.y += Mouse::mouseOffset()->y/Settings::videoScale()->y * 100.f;
    }
    
    glClear(GL_DEPTH_BUFFER_BIT);

    //buttonShader.setVec2("groupPos", glm::vec2(0));
    //buttonShader.setFloat("groupScale", 1);


}