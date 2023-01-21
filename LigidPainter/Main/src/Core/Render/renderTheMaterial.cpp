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

std::vector<float> renderVertices = { 
	// first triangle
	 1.0f,  1.0f, 0.0f,1,1,0,0,0,  // top right
	 1.0f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
	 0.0f,  1.0f, 0.0f,0,1,0,0,0,  // top left 
	// second triangle	  ,0,0,0,
	 1.0f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
	 0.0f,  0.0f, 0.0f,0,0,0,0,0,  // bottom left
	 0.0f,  1.0f, 0.0f,0,1,0,0,0   // top left
};

NodeResult Render::renderTheNodes(NodeScene material,Model &model,glm::mat4 perspectiveProjection,glm::mat4 view){
    
    NodeResult nodeResult;
    nodeResult.program = 18;
    nodeResult.colorAttachment = 0;

    std::vector<Node> renderingPipeline;
    GlSet glset;

    //TODO : Create before the while loop

    unsigned int FBO; 
    glset.genFramebuffers(FBO);
    glset.bindFramebuffer(FBO);


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

    


    for (size_t nodeI = 0; nodeI < renderingPipeline.size(); nodeI++)
    {
        Node node = renderingPipeline[nodeI];
        unsigned int nodeProgram = node.program;

        for (size_t inputI = 0; inputI < node.inputs.size(); inputI++)
        {
            //------CREATE THE TEXTURE------


            NodeInput input = node.inputs[inputI];
            
            unsigned int texture;

            if(input.element == "range"){
                if(input.nodeConnectionIndex == 10000){
                    glActiveTexture(GL_TEXTURE28);
                    glGenTextures(1, &texture);
                    glBindTexture(GL_TEXTURE_2D,texture);

                    unsigned int channels;
                    int channelSize;

                    if(input.type == "float"){
                        channels = GL_RED;
                        channelSize = 1;
                    }

                    if(input.type == "vec2"){
                        channels = GL_RG;
                        channelSize = 2;
                    }

                    if(input.type == "vec3"){
                        channels = GL_RGB;
                        channelSize = 3;
                    }

                    GLubyte* data = new GLubyte[channelSize];

                    for (size_t i = 0; i < channelSize; i++)
                    {
                        data[i] = input.value[i]*255;
                    }

                    glset.texImage(data,1,1,channels);
                }
                else{
                    texture = material.nodes[input.nodeConnectionIndex].outputs[input.inputConnectionIndex].result;
                }
            }
            else if(input.element == "image"){
                if(input.nodeConnectionIndex == 10000){
                    texture = input.selectedTexture;
                }
                else{
                    texture = material.nodes[input.nodeConnectionIndex].outputs[input.inputConnectionIndex].result;
                }
            }
            else if(input.element == "none"){
                if(input.nodeConnectionIndex == 10000){
                    glActiveTexture(GL_TEXTURE28);
                    glGenTextures(1, &texture);
                }
                else{
                    texture = material.nodes[input.nodeConnectionIndex].outputs[input.inputConnectionIndex].result;
                }
            }

            //------PROCESS------

            glUseProgram(nodeProgram);
            
            glActiveTexture(GL_TEXTURE20 + inputI);
            glBindTexture(GL_TEXTURE_2D,texture);
            
            glset.uniform1i(nodeProgram,("input_" + std::to_string(inputI)).c_str(),20+inputI);
        }

        glset.uniform1i(nodeProgram,"is3D",0);
		
        glm::mat4 projection = glm::ortho(0.0f, 1.77777777778f, 0.0f, 1.0f);
        glset.uniformMatrix4fv(nodeProgram,"projection",projection);

        glActiveTexture(GL_TEXTURE28);

        for (int outI = 0; outI < node.outputs.size(); outI++)
        {
            if(node.outputs[outI].isConnectedToShaderInput){
                nodeResult.colorAttachment = GL_COLOR_ATTACHMENT0 + outI;
                nodeResult.program = node.program; 
                glset.uniform1i(nodeProgram,"is3D",1);
                glset.uniformMatrix4fv(nodeProgram,"projection",perspectiveProjection);
                glset.uniformMatrix4fv(nodeProgram,"view",view);

                goto end;
            }
            else{
                unsigned int resultTexture;
                glset.genTextures(resultTexture);
    	        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + outI, GL_TEXTURE_2D,  resultTexture, 0);

                glDrawBuffer(GL_COLOR_ATTACHMENT0 + outI);

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glset.drawArrays(renderVertices,false);

                node.outputs[outI].result = resultTexture;
                
            }
        }
    }
    
    end:
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glset.bindFramebuffer(0);
    return nodeResult;
}