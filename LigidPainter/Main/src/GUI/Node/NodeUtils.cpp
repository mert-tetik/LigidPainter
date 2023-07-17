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

void Node::drawLine(glm::vec2 src, glm::vec2 dest,glm::vec2 videoScale,Panel nodeEditorPanel, int direction){
    glm::vec2 gap = src - dest;
    
    glm::vec2 pos = src - gap/2.f;
    glm::vec2 scale = glm::abs(gap/2.f);
    ;
    glm::vec3 resultPos = glm::vec3( 
                                    UTIL::getPercent(videoScale,glm::vec2(pos.x,pos.y)) //Don't include the depth
                                    ,0.9f); 
    // scale value % of the video scale
    glm::vec2 resultScale = UTIL::getPercent(videoScale,scale);
    connectionCurveShader.use();
    nodeEditorPanel.resultPos.x = videoScale.x/2.f;
    nodeEditorPanel.resultPos.y = videoScale.y/2.f;
    nodeEditorPanel.resultScale.x = videoScale.x/2.f;
    nodeEditorPanel.resultScale.y = videoScale.y/2.f;
    
    connectionCurveShader.setVec3("pos",       nodeEditorPanel.resultPos);
    connectionCurveShader.setVec2("scale",     nodeEditorPanel.resultScale);
    
    connectionCurveShader.setInt("direction",       direction);
    
    connectionCurveShader.setVec2("startPos",       src);
    connectionCurveShader.setVec2("destPos",       dest);
    connectionCurveShader.setVec2("percScale",       nodeEditorPanel.scale);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    buttonShader.use();
}


void Node::createConnection(int nodeIParam,int IOIParam,int currentNodeI,int currentIOI,std::vector<Node> &meshNodeScene){
    
    IOs[currentIOI].connections.push_back(NodeConnection(nodeIParam,IOIParam));
    meshNodeScene[nodeIParam].IOs[IOIParam].connections.push_back(NodeConnection(currentNodeI,currentIOI));
}

void Node::clearConnections(int nodeIParam,int IOIParam,std::vector<Node> &meshNodeScene){
    
    meshNodeScene[nodeIParam].IOs[IOIParam].connections.clear();

    //Check all the nodes in the meshNodeScene
    for (size_t nodeI = 0; nodeI < meshNodeScene.size(); nodeI++)
    {
        //Check all the IO in the node
        for (size_t IOI = 0; IOI < meshNodeScene[nodeI].IOs.size(); IOI++)
        {
            //Check all the connections in the IO
            for (size_t conI = 0; conI < meshNodeScene[nodeI].IOs[IOI].connections.size(); conI++)
            {
                //Does the node matches
                bool nodeMatches = false;
                
                if(meshNodeScene[nodeI].IOs[IOI].connections[conI].nodeIndex == nodeIParam)
                    nodeMatches = true;
                
                
                //Does the IO matches
                bool IOMatches = false;
                
                if(meshNodeScene[nodeI].IOs[IOI].connections[conI].inputIndex == IOIParam)
                    IOMatches = true;

                
                //If the both node & IOI indices matches than the input has connection
                if(nodeMatches && IOMatches)
                    meshNodeScene[nodeI].IOs[IOI].connections.erase(meshNodeScene[nodeI].IOs[IOI].connections.begin() + conI);
            }
        }
    }
}


bool Node::doHaveConnection(int nodeIParam,int IOIParam,std::vector<Node> meshNodeScene){
    
    //Check all the nodes in the meshNodeScene
    for (size_t nodeI = 0; nodeI < meshNodeScene.size(); nodeI++)
    {
        //Check all the IO in the node
        for (size_t IOI = 0; IOI < meshNodeScene[nodeI].IOs.size(); IOI++)
        {
            //Check all the connections in the IO
            for (size_t conI = 0; conI < meshNodeScene[nodeI].IOs[IOI].connections.size(); conI++)
            {
                //Does the node matches
                bool nodeMatches = false;
                
                if(meshNodeScene[nodeI].IOs[IOI].connections[conI].nodeIndex == nodeIParam)
                    nodeMatches = true;
                
                
                //Does the IO matches
                bool IOMatches = false;
                
                if(meshNodeScene[nodeI].IOs[IOI].connections[conI].inputIndex == IOIParam)
                    IOMatches = true;

                
                //If the both node & IOI indices matches than the input has connection
                if(nodeMatches && IOMatches)
                    return true;
            }
        }
    }

    return false;
}

int Node::getStateData(int nodeI, int IOI,std::vector<Node> meshNodeScene){
    return meshNodeScene[nodeI].IOs[IOI].state;
}

void Node::getHoveredInputs(int &nodeIParam, int &IOIndexParam, std::vector<Node> meshNodeScene){
    //Check all the nodes in the meshNodeScene
    for (size_t nodeI = 0; nodeI < meshNodeScene.size(); nodeI++)
    {
        //Check all the IO in the node
        for (size_t IOI = 0; IOI < meshNodeScene[nodeI].IOs.size(); IOI++)
        {
            //Check if IO circle is hovered
            if(meshNodeScene[nodeI].IOs[IOI].IOCircle.hover){
                nodeIParam = nodeI;
                IOIndexParam = IOI;
            }
        }
    }
}

void Node::getTheIOConnectedToTheInput(int &nodeIParam, int &IOIParam,int currentNodeI, int currentIOI, std::vector<Node> meshNodeScene){
    //Check all the nodes in the meshNodeScene
    for (size_t nodeI = 0; nodeI < meshNodeScene.size(); nodeI++)
    {
        //Check all the IO in the node
        for (size_t IOI = 0; IOI < meshNodeScene[nodeI].IOs.size(); IOI++)
        {
            //Check all the connections in the IO
            for (size_t conI = 0; conI < meshNodeScene[nodeI].IOs[IOI].connections.size(); conI++)
            {
                //Does the node matches
                bool nodeMatches = false;
                
                if(meshNodeScene[nodeI].IOs[IOI].connections[conI].nodeIndex == currentNodeI)
                    nodeMatches = true;
                
                
                //Does the IO matches
                bool IOMatches = false;
                
                if(meshNodeScene[nodeI].IOs[IOI].connections[conI].inputIndex == currentIOI)
                    IOMatches = true;

                
                //If the both node & IOI indices matches than the input has connection
                if(nodeMatches && IOMatches){
                    nodeIParam = nodeI;
                    IOIParam = IOI;
                }   
            }
        }
    }
}

bool Node::renderBarriers(Panel &nodeEditorPanel,Mouse &mouse){
    //Renders the barriers and returns if the cursor is inside of the barriers
    //Barriers prevents the node rendering outside of the node editor panel
    //Barriers are covering outside of the node editor panel
    bool cursorOnBarriers = false;
    //Bottom
    buttonShader.setVec3("pos",       glm::vec3(nodeEditorPanel.resultPos.x,nodeEditorPanel.resultPos.y + nodeEditorPanel.resultScale.y + 5000,1.f)); //To the bottom
    buttonShader.setVec2("scale",     glm::vec2(5000));
    buttonShader.setFloat("properties.radius",     0.f   ); 
    buttonShader.setInt("outlineExtra" ,    false     ); 
    buttonShader.setVec4("properties.color",     glm::vec4(0)   ); //Invisible
    buttonShader.setVec4("properties.color2",     glm::vec4(0)   ); //Invisible
    buttonShader.setFloat("properties.colorMixVal"  ,   0.f );
    buttonShader.setVec3("properties.outline.color" ,    glm::vec4(0)     ); //Invisible
    
    if(mouse.isMouseHover(glm::vec2(5000),glm::vec3(nodeEditorPanel.resultPos.x,nodeEditorPanel.resultPos.y + nodeEditorPanel.resultScale.y + 5000,1.f))){
        cursorOnBarriers = true;
    }
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    //Top
    buttonShader.setVec3("pos",       glm::vec3(nodeEditorPanel.resultPos.x,nodeEditorPanel.resultPos.y - nodeEditorPanel.resultScale.y - 5000,1.f)); //To the Top
    buttonShader.setVec2("scale",     glm::vec2(5000));
    if(mouse.isMouseHover(glm::vec2(5000),glm::vec3(nodeEditorPanel.resultPos.x,nodeEditorPanel.resultPos.y - nodeEditorPanel.resultScale.y - 5000,1.f))){
        cursorOnBarriers = true;
    }
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    //Left
    buttonShader.setVec3("pos",       glm::vec3(nodeEditorPanel.resultPos.x - nodeEditorPanel.resultScale.x - 5000,nodeEditorPanel.resultPos.y,1.f));
    buttonShader.setVec2("scale",     glm::vec2(5000));
    if(mouse.isMouseHover(glm::vec2(5000),glm::vec3(nodeEditorPanel.resultPos.x - nodeEditorPanel.resultScale.x - 5000,nodeEditorPanel.resultPos.y,1.f))){
        cursorOnBarriers = true;
    }
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    //Right
    buttonShader.setVec3("pos",       glm::vec3(nodeEditorPanel.resultPos.x + nodeEditorPanel.resultScale.x + 5000,nodeEditorPanel.resultPos.y,1.f));
    buttonShader.setVec2("scale",     glm::vec2(5000));
    if(mouse.isMouseHover(glm::vec2(5000),glm::vec3(nodeEditorPanel.resultPos.x + nodeEditorPanel.resultScale.x + 5000,nodeEditorPanel.resultPos.y,1.f)))
        cursorOnBarriers = true;
    glDrawArrays(GL_TRIANGLES, 0, 6);
    return cursorOnBarriers;
}


void Node::addVectors(std::vector<NodeIO>& orgVec, std::vector<NodeIO>& addedVec) {
    for (size_t i = 0; i < addedVec.size(); i++)
    {
        orgVec.push_back(addedVec[i]);
    }
}

void Node::createPanelUsingIOs(){
    nodePanel.sections.clear();
    
    Section section;
    section.header = Element(Button());
    
    for (size_t i = 0; i < IOs.size(); i++)
    {
        section.elements.push_back(IOs[i].element);
    }
    
    nodePanel.sections.push_back(section);
}

void Node::uploadNewIOs(std::vector<NodeIO> inputs, std::vector<NodeIO> outputs){
    
    //Clear the IOs vector
    this->IOs.clear();

    //Add inputs & outputs to the IOs vector
    this->addVectors(this->IOs, inputs);
    this->addVectors(this->IOs, outputs);

    createPanelUsingIOs();
}

void Node::uploadNewIOs(Model &model, ColorPalette colorPalette){
    
    std::vector <NodeIO> meshOutputNodeInputElements;
    for (size_t i = 0; i < model.meshes.size(); i++)
    {
        meshOutputNodeInputElements.push_back(NodeIO(model.meshes[i].materialName,Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1),colorPalette,buttonShader,model.meshes[i].materialName,Texture(),2.f,false)),colorPalette.mainColor,colorPalette,buttonShader,videoScale,0));
    }

    //Clear the IOs vector
    this->IOs.clear();

    //Add inputs & outputs to the IOs vector
    this->addVectors(this->IOs, meshOutputNodeInputElements);

    createPanelUsingIOs();
}