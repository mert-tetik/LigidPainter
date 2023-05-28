#ifndef LGDUINODE_HPP
#define LGDUINODE_HPP

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Shader.hpp"
#include "Renderer.h"
#include "Util.hpp"
#include "Timer.hpp"
#include "Texture.hpp"
#include "Mouse.hpp"
#include "Util.hpp"

#include "GUI/Node/NodeIO.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

//TODO Make the node editor panel movable
//TODO Prevent connection to connected input

class Node
{
private:
    void drawLine(glm::vec2 src, glm::vec2 dest,glm::vec2 videoScale,Panel nodeEditorPanel){
        glm::vec2 gap = src - dest;
        
        glm::vec2 pos = src - gap/2.f;
        glm::vec2 scale = glm::abs(gap/2.f);
        Util util;
        glm::vec3 resultPos = glm::vec3( 
                                        util.getPercent(videoScale,glm::vec2(pos.x,pos.y)) //Don't include the depth
                                        ,0.9f); 


        // scale value % of the video scale
        glm::vec2 resultScale = util.getPercent(videoScale,scale);

        singleCurveShader.use();
        nodeEditorPanel.resultPos.x = videoScale.x/2.f;
        nodeEditorPanel.resultPos.y = videoScale.y/2.f;
        nodeEditorPanel.resultScale.x = videoScale.x/2.f;
        nodeEditorPanel.resultScale.y = videoScale.y/2.f;
        
        singleCurveShader.setVec3("pos",       nodeEditorPanel.resultPos);
        singleCurveShader.setVec2("scale",     nodeEditorPanel.resultScale);
        
        singleCurveShader.setVec2("startPos",       src);
        singleCurveShader.setVec2("destPos",       dest);

        singleCurveShader.setVec2("percScale",       nodeEditorPanel.scale);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        buttonShader.use();
    }
    bool doHaveConnection(int nodeI,int outI){
        for (size_t outi = 0; outi < outputs.size(); outi++)
        {
            for (size_t coni = 0; coni < outputs[outi].connections.size(); coni++)
            {
                if(outputs[outi].connections[coni].inputIndex == outI && outputs[outi].connections[coni].nodeIndex == nodeI)
                    return true;
            }
        }
    }
public:
    std::vector<NodeIO> inputs;
    std::vector<NodeIO> outputs;

    Shader buttonShader;  
    Shader singleCurveShader;  

    //int ID; //Unique id for the node (IS NOT USED RN)
    int materialID;

    Panel nodePanel;//Node's itself
    Button barButton; //That bar on top of the nodes 

    glm::vec2 scale = glm::vec2(10,20); //Scale of the node
    glm::vec3 pos = glm::vec3(50,50,0.8f); //Position of the node


    Node(){}

    void loadIO(std::vector<NodeIO> inputs, std::vector<NodeIO> outputs, Shader buttonShader,Shader singleCurveShader,ColorPalette colorPalette,float heigth,int materialID){
        this->buttonShader = buttonShader;
        this->singleCurveShader = singleCurveShader;
        this->inputs = inputs;
        this->outputs = outputs;
        this->scale.y = heigth;
        this->materialID = materialID;


        this->nodePanel = Panel(
            buttonShader,
            colorPalette,
            {
                //Sections
            },
            scale, //Scale of the node
            pos, //Pos of the node
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
        nodePanel.clearDepthBuffer = false;
        this->barButton = Button(2,nodePanel.scale,colorPalette,buttonShader,"Node",Texture(),0.f,false);

        Section section;
        section.header = Element(Button());
        
        for (size_t i = 0; i < inputs.size(); i++)
        {
            section.elements.push_back(inputs[i].element);
        }
        
        for (size_t i = 0; i < outputs.size(); i++)
        {
            section.elements.push_back(outputs[i].element);
        }

        nodePanel.sections.push_back(section);
        
    }



    void render(glm::vec2 videoScale,Mouse& mouse,Timer &timer,TextRenderer &textRenderer,Panel nodeEditorPanel,std::vector<Node> &nodeScene,int currentNodeIndex){
        GLFWcursor* lastCursor = mouse.activeCursor;
        
        //Barriers (In order to prevent the overflow)
        bool cursorOnBarriers = renderBarriers(nodeEditorPanel,mouse);

        //Render the node panel which contains the input buttons and stuff
        nodePanel.render(videoScale,mouse,timer,textRenderer);
        
        if(cursorOnBarriers){ //Righ after rendering cancel mouse states regarding the panel
            nodePanel.hover = false;
            nodePanel.leftSide.hover = false;
            nodePanel.leftSide.pressed = false;
            nodePanel.rightSide.hover = false;
            nodePanel.rightSide.pressed = false;
        }
        
        for (size_t i = 0; i < nodePanel.sections[0].elements.size(); i++) //Render IO circles and check them etc.
        {
            if(nodePanel.sections[0].elements[i].nodeState == 0){
                //----------Inputs-----------

                //Move the IO Circle to the left side of the the input element
                inputs[i].IOCircle.pos = nodePanel.sections[0].elements[i].pos;
                inputs[i].IOCircle.pos.x -= nodePanel.sections[0].elements[i].scale.x;
                inputs[i].IOCircle.pos.z = nodePanel.sections[0].elements[i].pos.z + 0.01f;

                //Render the IO circle
                inputs[i].IOCircle.render(videoScale,mouse,timer,textRenderer);
                if(cursorOnBarriers)
                    inputs[i].IOCircle.hover = false;


                //Check all the nodes if an output is connected to the input
                for (size_t nodI = 0; nodI < nodeScene.size(); nodI++){
                    for (size_t IOi = 0; IOi < nodeScene[nodI].outputs.size(); IOi++){
                        for (size_t conI = 0; conI < nodeScene[nodI].outputs[IOi].connections.size(); conI++)
                        {
                            //If an output is connected to the input
                            if(nodeScene[nodI].outputs[IOi].connections[conI].nodeIndex == currentNodeIndex && nodeScene[nodI].outputs[IOi].connections[conI].inputIndex == i){
                                //Add the connection
                                if(inputs[i].connections.size())
                                    inputs[i].connections[0] = NodeConnection(nodI,IOi);
                                else
                                    inputs[i].connections.push_back(NodeConnection(nodI,IOi));
                            }
                            else{
                                //Remove the connection if there are any (if an output is not connected to the input)
                                inputs[i].connections.clear();
                            }
                        }
                    }
                }
                
                //Don't scale the panel if IO circle is pressed
                if(inputs[i].IOCircle.clickState1){
                    nodePanel.leftSide.pressed = false;
                    nodePanel.rightSide.pressed = false;
                }
            }
            if(nodePanel.sections[0].elements[i].nodeState == 2){
                //----------Outputs-----------

                //Move the IO Circle to the right side of the the output element
                outputs[i-inputs.size()].IOCircle.pos = nodePanel.sections[0].elements[i].pos;
                outputs[i-inputs.size()].IOCircle.pos.x += nodePanel.sections[0].elements[i].scale.x;
                outputs[i-inputs.size()].IOCircle.pos.z = nodePanel.sections[0].elements[i].pos.z + 0.01f;

                
                if(outputs[i-inputs.size()].IOCircle.clickState1){//Pressed to IO circle
                    Util util;
                    
                    //Don't scale the panel if IO circle is pressed
                    nodePanel.leftSide.pressed = false;
                    nodePanel.rightSide.pressed = false;
                    
                    //Render the line (starting point : IO circle pos , destination point : cursor pos)
                    drawLine(glm::vec2(outputs[i-inputs.size()].IOCircle.pos.x,outputs[i-inputs.size()].IOCircle.pos.y),mouse.cursorPos/videoScale * 100.f,videoScale,nodeEditorPanel);
                }

                //Check all the nodes if released on top of the IO button
                for (size_t nodI = 0; nodI < nodeScene.size(); nodI++)
                {
                    for (size_t IOi = 0; IOi < nodeScene[nodI].inputs.size(); IOi++)
                    {
                        if(nodeScene[nodI].inputs[IOi].IOCircle.hover){
                            if(outputs[i-inputs.size()].IOCircle.clickState1 && !mouse.LPressed){//Released the IO circle
                                outputs[i-inputs.size()].connections.push_back(NodeConnection(nodI,IOi));//Connect to the input button
                            }
                            if(doHaveConnection(nodI,IOi) && mouse.LClick){//If pressed to a IO circle that already is connected
                                //Severe the current connection & make the output IO circle pressed (move connection)

                                outputs[i-inputs.size()].IOCircle.clickState1 = true; //Move that connection
                                
                                for (size_t coni = 0; coni < outputs[i-inputs.size()].connections.size(); coni++)//Severe that connection
                                {
                                    if(outputs[i-inputs.size()].connections[coni].inputIndex == IOi && outputs[i-inputs.size()].connections[coni].nodeIndex == nodI){
                                        outputs[i-inputs.size()].connections.erase(outputs[i-inputs.size()].connections.begin() + coni);
                                    }
                                }
                            }
                        }
                    }
                }
                    
                //Render the lines of the connected connections
                for (size_t conI = 0; conI < outputs[i-inputs.size()].connections.size(); conI++)
                {
                    glm::vec3 destPos = nodeScene[
                                                    outputs[i-inputs.size()].connections[conI].nodeIndex //Which node the output is connected
                                                 ].inputs
                                                 [
                                                    outputs[i-inputs.size()].connections[conI].inputIndex //Which input the output is connected
                                                 ].IOCircle.pos; //Position of the input button
                    drawLine(
                                glm::vec2(outputs[i-inputs.size()].IOCircle.pos.x,outputs[i-inputs.size()].IOCircle.pos.y), //Source (Position of the output)
                                glm::vec2(destPos.x,destPos.y), //Destination
                                videoScale,
                                nodeEditorPanel
                            );
                }
                
                //Render the output circle
                outputs[i-inputs.size()].IOCircle.render(videoScale,mouse,timer,textRenderer);
                
                if(cursorOnBarriers)
                    outputs[i-inputs.size()].IOCircle.hover = false;
            }
        }

        //Position the bar button        
        barButton.pos = nodePanel.pos;
        barButton.scale = nodePanel.scale;
        barButton.scale.y = 1.5f;
        barButton.pos.z += 0.02f;
        barButton.pos.y = nodePanel.pos.y - nodePanel.scale.y - barButton.scale.y; 

        //Render the bar button
        barButton.render(videoScale,mouse,timer,textRenderer);
        if(cursorOnBarriers){
            barButton.clickState1 = false;
            barButton.hover = false;
        }

        //Move the node panel if bar button is pressed
        if(barButton.clickState1){ //Pressed
            nodePanel.pos.x += mouse.mouseOffset.x/videoScale.x * 100.f;
            nodePanel.pos.y += mouse.mouseOffset.y/videoScale.y * 100.f;
        }

        if(cursorOnBarriers){ //If mouse is on the barriers don't change the cursor 
            mouse.setCursor(lastCursor);
        }

        glClear(GL_DEPTH_BUFFER_BIT);
    }

private:
    bool renderBarriers(Panel &nodeEditorPanel,Mouse &mouse){
        //Renders the barriers and returns if the cursor is inside of the barriers
        //Barriers prevents the node rendering outside of the node editor panel
        //Barriers are covering outside of the node editor panel

        bool cursorOnBarriers = false;

        //Bottom
        buttonShader.setVec3("pos",       glm::vec3(nodeEditorPanel.resultPos.x,nodeEditorPanel.resultPos.y + nodeEditorPanel.resultScale.y + 5000,1.f)); //To the bottom
        buttonShader.setVec2("scale",     glm::vec2(5000));
        buttonShader.setFloat("radius",     0.f   ); 
        buttonShader.setInt("outlineExtra" ,    false     ); 
        buttonShader.setVec4("color",     glm::vec4(0)   ); //Invisible
        buttonShader.setVec4("color2",     glm::vec4(0)   ); //Invisible
        buttonShader.setFloat("colorMixVal"  ,   0.f );
        buttonShader.setVec3("outlineColor" ,    glm::vec4(0)     ); //Invisible
        
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
};

#endif