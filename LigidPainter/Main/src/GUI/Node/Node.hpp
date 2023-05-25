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


class Node
{
private:
    void drawLine(glm::vec2 src, glm::vec2 dest,glm::vec2 videoScale){
        glm::vec2 gap = src - dest;
        
        glm::vec2 pos = src - gap/2.f;
        glm::vec2 scale = glm::abs(gap/2.f);
        Util util;
        glm::vec3 resultPos = glm::vec3( 
                                        util.getPercent(videoScale,glm::vec2(pos.x,pos.y)) //Don't include the depth
                                        ,0.9f); 


        // scale value % of the video scale
        glm::vec2 resultScale = util.getPercent(videoScale,scale);

        buttonShader.setVec3("pos",       resultPos);
        buttonShader.setVec2("scale",     resultScale);
        buttonShader.setVec4("color",     glm::vec4(1)   );
        buttonShader.setVec4("color2"  ,     glm::vec4(1)     );
        buttonShader.setFloat("colorMixVal"  ,   0.f );
        
        buttonShader.setFloat("width",     resultScale.x  );
        buttonShader.setFloat("height",     resultScale.y   );
        
        buttonShader.setFloat("radius",     0.f   );
        buttonShader.setInt("outline",     false   );
        buttonShader.setInt("outlineExtra" ,    false     ); 

        buttonShader.setVec3("outlineColor" ,    glm::vec3(0,0,0)     ); 
        buttonShader.setVec3("outlineColor2" ,    glm::vec3(0,0,0)     ); 

        buttonShader.setFloat("thickness" ,    1.f  );
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
public:
    std::vector<NodeIO> inputs;
    std::vector<NodeIO> outputs;

    Shader shader; //This shader will be used to output 
    Shader buttonShader;  

    int ID; //Unique id for the node

    Panel nodePanel;//Node's itself
    Button barButton; //That bar on top of the nodes 

    glm::vec2 scale = glm::vec2(10,20); //Scale of the node
    glm::vec3 pos = glm::vec3(50,50,0.8f); //Position of the node


    Node(){}

    void loadIO(std::vector<NodeIO> inputs, std::vector<NodeIO> outputs, Shader buttonShader,ColorPalette colorPalette,float heigth){
        this->buttonShader = buttonShader;
        this->inputs = inputs;
        this->outputs = outputs;
        this->scale.y = heigth;


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
            {}
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



    void render(glm::vec2 videoScale,Mouse& mouse,Timer &timer,TextRenderer &textRenderer,Panel nodeEditorPanel){
        //Barriers (In order to prevent the overflow)
        
        //Bottom
        buttonShader.setVec3("pos",       glm::vec3(nodeEditorPanel.resultPos.x,nodeEditorPanel.resultPos.y + nodeEditorPanel.resultScale.y + 5000,1.f)); //To the bottom
        buttonShader.setVec2("scale",     glm::vec2(5000));
        buttonShader.setFloat("radius",     0.f   ); 
        buttonShader.setInt("outlineExtra" ,    false     ); 
        buttonShader.setVec4("color",     glm::vec4(0)   ); //Invisible
        buttonShader.setVec4("color2",     glm::vec4(0)   ); //Invisible
        buttonShader.setFloat("colorMixVal"  ,   0.f );
        buttonShader.setVec3("outlineColor" ,    glm::vec4(0)     ); //Invisible
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        //Top
        buttonShader.setVec3("pos",       glm::vec3(nodeEditorPanel.resultPos.x,nodeEditorPanel.resultPos.y - nodeEditorPanel.resultScale.y - 5000,1.f)); //To the Top
        buttonShader.setVec2("scale",     glm::vec2(5000));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        //Left
        buttonShader.setVec3("pos",       glm::vec3(nodeEditorPanel.resultPos.x - nodeEditorPanel.resultScale.x - 5000,nodeEditorPanel.resultPos.y,1.f));
        buttonShader.setVec2("scale",     glm::vec2(5000));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        //Right
        buttonShader.setVec3("pos",       glm::vec3(nodeEditorPanel.resultPos.x + nodeEditorPanel.resultScale.x + 5000,nodeEditorPanel.resultPos.y,1.f));
        buttonShader.setVec2("scale",     glm::vec2(5000));
        glDrawArrays(GL_TRIANGLES, 0, 6);


        nodePanel.render(videoScale,mouse,timer,textRenderer);
        
        for (size_t i = 0; i < nodePanel.sections[0].elements.size(); i++)
        {
            if(nodePanel.sections[0].elements[i].nodeState == 0){//Input

                //Move the IO Circle to the left side of the the input element
                inputs[i].IOCircle.pos = nodePanel.sections[0].elements[i].pos;
                inputs[i].IOCircle.pos.x -= nodePanel.sections[0].elements[i].scale.x;
                inputs[i].IOCircle.pos.z = nodePanel.sections[0].elements[i].pos.z + 0.01f;

                //Render the IO circle
                inputs[i].IOCircle.render(videoScale,mouse,timer,textRenderer);
            }
            if(nodePanel.sections[0].elements[i].nodeState == 2){//Outputs

                //Move the IO Circle to the right side of the the output element
                outputs[i-inputs.size()].IOCircle.pos = nodePanel.sections[0].elements[i].pos;
                outputs[i-inputs.size()].IOCircle.pos.x += nodePanel.sections[0].elements[i].scale.x;
                outputs[i-inputs.size()].IOCircle.pos.z = nodePanel.sections[0].elements[i].pos.z + 0.01f;

                outputs[i-inputs.size()].IOCircle.render(videoScale,mouse,timer,textRenderer);
                
                if(outputs[i-inputs.size()].IOCircle.clickState1){//Pressed to IO circle
                    Util util;
                    
                    //Don't scale the panel if IO circle is pressed
                    nodePanel.leftSide.pressed = false;
                    nodePanel.rightSide.pressed = false;
                    
                    //Render the line (starting point : IO circle pos , destination point : cursor pos)
                    drawLine(glm::vec2(outputs[i-inputs.size()].IOCircle.pos.x,outputs[i-inputs.size()].IOCircle.pos.y),mouse.cursorPos/videoScale * 100.f,videoScale);
                }
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

        //Move the node panel if bar button is pressed
        if(barButton.clickState1){ //Pressed
            nodePanel.pos.x += mouse.mouseOffset.x/videoScale.x * 100.f;
            nodePanel.pos.y += mouse.mouseOffset.y/videoScale.y * 100.f;
        }
        glClear(GL_DEPTH_BUFFER_BIT);
    }
};

#endif