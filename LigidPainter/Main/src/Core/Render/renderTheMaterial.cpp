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

unsigned int lastProgram = 0;

MaterialOut Render::renderTheNodes(NodeScene &material,Model &model,glm::mat4 perspectiveProjection,glm::mat4 view,int maxScreenWidth,int screenSizeX,int maxScreenHeight,int screenSizeY,std::vector<Node>appNodes){
    MaterialOut resultOut;
    if(lastProgram == 0){
        lastProgram = appNodes[0].program;
    }
    for (size_t i = 0; i < material.renderingPipeline.size(); i++)
    {
        //Removal of the previous output textures
        for (size_t outI = 0; outI < material.renderingPipeline[i].outputs.size(); outI++)
        {
            glDeleteTextures(1,&material.renderingPipeline[i].outputs[outI].result);
        }
    }

    material.renderingPipeline.clear();

    GlSet glset;

    glset.viewport(1920,1080);

    for (size_t i = 0; i < material.nodes.size(); i++)
    {
        //Default indexes
        material.nodes[i].renderingIndex = 10000;        
    }
    

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
            material.renderingPipeline.push_back(material.nodes[i]);
            material.nodes[i].renderingIndex = material.renderingPipeline.size()-1; 
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
                material.renderingPipeline.push_back(material.nodes[nodeI]);
                material.nodes[nodeI].renderingIndex = material.renderingPipeline.size()-1; 
                isChangesAreMade = true;
            }
        }
        if(!isChangesAreMade){
            break;
        }
    }

    


    for (size_t nodeI = 0; nodeI < material.renderingPipeline.size(); nodeI++)
    {
        unsigned int nodeProgram = material.renderingPipeline[nodeI].program;

        for (size_t inputI = 0; inputI < material.renderingPipeline[nodeI].inputs.size(); inputI++)
        {
            //------CREATE THE TEXTURE------

            unsigned int texture;

            if(material.renderingPipeline[nodeI].inputs[inputI].element == "range"){
                if(material.renderingPipeline[nodeI].inputs[inputI].nodeConnectionIndex == 10000){
                    glActiveTexture(GL_TEXTURE28);
                    glGenTextures(1, &texture);
                    glBindTexture(GL_TEXTURE_2D,texture);

                    unsigned int channels;
                    int channelSize;

                    if(material.renderingPipeline[nodeI].inputs[inputI].type == "float"){
                        channels = GL_RED;
                        channelSize = 1;
                    }

                    if(material.renderingPipeline[nodeI].inputs[inputI].type == "vec2"){
                        channels = GL_RG;
                        channelSize = 2;
                    }

                    if(material.renderingPipeline[nodeI].inputs[inputI].type == "vec3"){
                        channels = GL_RGB;
                        channelSize = 3;
                    }

                    GLubyte* data = new GLubyte[channelSize];

                    for (size_t i = 0; i < channelSize; i++)
                    {
                        data[i] = material.renderingPipeline[nodeI].inputs[inputI].value[i]*255;
                    }

                    glset.texImage(data,1,1,channels);
                }
                else{
                    texture = material.renderingPipeline[material.nodes[material.renderingPipeline[nodeI].inputs[inputI].nodeConnectionIndex].renderingIndex].outputs[material.renderingPipeline[nodeI].inputs[inputI].inputConnectionIndex].result;
                }
            }
            else if(material.renderingPipeline[nodeI].inputs[inputI].element == "image"){
                if(material.renderingPipeline[nodeI].inputs[inputI].nodeConnectionIndex == 10000){
                    if(material.renderingPipeline[nodeI].inputs[inputI].selectedTextureIndex != 10000)
                        texture = material.renderingPipeline[nodeI].inputs[inputI].selectedTexture;
                    else{
                        glActiveTexture(GL_TEXTURE28);
                        glGenTextures(1, &texture);
                        glBindTexture(GL_TEXTURE_2D,texture);

                        const unsigned int channels = GL_RGB;
                        const int channelSize = 3;


                        GLubyte* data = new GLubyte[channelSize];

                        for (size_t i = 0; i < channelSize; i++)
                        {
                            data[i] = material.renderingPipeline[nodeI].inputs[inputI].color[i];
                        }

                        glset.texImage(data,1,1,channels);
                    }
                }
                else{
                    texture = material.renderingPipeline[material.nodes[material.renderingPipeline[nodeI].inputs[inputI].nodeConnectionIndex].renderingIndex].outputs[material.renderingPipeline[nodeI].inputs[inputI].inputConnectionIndex].result;
                }
            }
            else if(material.renderingPipeline[nodeI].inputs[inputI].element == "color"){
                if(material.renderingPipeline[nodeI].inputs[inputI].nodeConnectionIndex == 10000){
                    
                    glActiveTexture(GL_TEXTURE28);
                    glGenTextures(1, &texture);
                    glBindTexture(GL_TEXTURE_2D,texture);

                    const unsigned int channels = GL_RGB;
                    const int channelSize = 3;


                    GLubyte* data = new GLubyte[channelSize];

                    for (size_t i = 0; i < channelSize; i++)
                    {
                        data[i] = material.renderingPipeline[nodeI].inputs[inputI].color[i];
                    }

                    glset.texImage(data,1,1,channels);
                }
                else{
                    texture = material.renderingPipeline[material.nodes[material.renderingPipeline[nodeI].inputs[inputI].nodeConnectionIndex].renderingIndex].outputs[material.renderingPipeline[nodeI].inputs[inputI].inputConnectionIndex].result;
                }
            }
            else if(material.renderingPipeline[nodeI].inputs[inputI].element == "none"){
                if(material.renderingPipeline[nodeI].inputs[inputI].nodeConnectionIndex == 10000){
                    glGenTextures(1, &texture);
                }
                else{
                    texture = material.renderingPipeline[material.nodes[material.renderingPipeline[nodeI].inputs[inputI].nodeConnectionIndex].renderingIndex].outputs[material.renderingPipeline[nodeI].inputs[inputI].inputConnectionIndex].result;
                }
            }

            //------PROCESS------

            glUseProgram(nodeProgram);
            
            glActiveTexture(GL_TEXTURE20 + inputI);
            glBindTexture(GL_TEXTURE_2D,texture);
            
            glset.uniform1i(nodeProgram,("input_" + std::to_string(inputI)).c_str(),20+inputI);
            resultOut.textures.push_back(texture);
        }

        glset.uniform1i(nodeProgram,"is3D",0);
		
        glm::mat4 projection = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);
        glset.uniformMatrix4fv(nodeProgram,"projection",projection);

        glActiveTexture(GL_TEXTURE28);

        for (int outI = 0; outI < material.renderingPipeline[nodeI].outputs.size(); outI++)
        {
            if(material.renderingPipeline[nodeI].outputs[outI].isConnectedToShaderInput){
                resultOut.program = material.renderingPipeline[nodeI].program; 
                lastProgram = resultOut.program;
                glset.uniformMatrix4fv(nodeProgram,"projection",perspectiveProjection);
                glset.uniformMatrix4fv(nodeProgram,"view",view);

                goto end;
            }
            else{
                glActiveTexture(GL_TEXTURE28);
                
                unsigned int resultTexture;
                glset.genTextures(resultTexture);
                glset.bindTexture(resultTexture);

                glset.texImage(nullptr,1920,1080,GL_RGBA);
                glset.generateMipmap();
                
                unsigned int FBO; 
                glset.genFramebuffers(FBO);
                glset.bindFramebuffer(FBO);

    	        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + outI, GL_TEXTURE_2D,  resultTexture, 0);
                
                GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5,GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7 };
                glDrawBuffers(8, drawBuffers);

                glReadBuffer(GL_COLOR_ATTACHMENT0 + outI);
                
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
                glset.drawArrays(renderVertices,false);
                glset.generateMipmap();

                GLubyte* data = new GLubyte[1920*1080*4];
                glReadPixels(0,0,1920,1080,GL_RGBA,GL_UNSIGNED_BYTE,data);

                glset.texImage(data,1920,1080,GL_RGBA);
                glset.generateMipmap();

                delete[] data;
                
                glset.bindFramebuffer(0);
                glset.deleteFramebuffers(FBO);

                material.renderingPipeline[nodeI].outputs[outI].result = resultTexture;
                
                resultOut.textures.clear();
            }
        }
    }
    
    
    end:
    glset.bindFramebuffer(0);
	glset.bindRenderBuffer(0);
    
    resultOut.program = lastProgram;
    glset.uniform1i(resultOut.program,"is3D",1);

    glViewport(-(maxScreenWidth - screenSizeX)/2, -(maxScreenHeight - screenSizeY), maxScreenWidth, maxScreenHeight);


    return resultOut;
}