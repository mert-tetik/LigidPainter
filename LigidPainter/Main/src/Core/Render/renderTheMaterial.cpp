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

MaterialOut Render::renderTheNodes(NodeScene &material,Model &model,glm::mat4 perspectiveProjection,glm::mat4 view,int maxScreenWidth,int screenSizeX,int maxScreenHeight,int screenSizeY,std::vector<Node>appNodes,int chosenTextureResIndex,bool& bakeTheMaterial,std::vector<aTexture> &albedoTextures,int currentMaterialIndex,std::vector<NodeScene> &nodeScenesHistory,int chosenNodeResIndex,unsigned int materialFBO){
    glClearColor(0,0,0,1);
    int txtrRes = 256;

    int nodeTxtrResIndex;
    if(bakeTheMaterial)
        nodeTxtrResIndex = chosenTextureResIndex;
    else 
        nodeTxtrResIndex = chosenNodeResIndex;

	for (size_t i = 0; i < chosenTextureResIndex; i++)
	{
		txtrRes*=2;
	}
    int nodeRes = 256;
	for (size_t i = 0; i < nodeTxtrResIndex; i++)
	{
		nodeRes*=2;
	}
    
    
    MaterialOut resultOut;
    if(lastProgram == 0){
        lastProgram = appNodes[0].program;
    }
    for (size_t i = 0; i < material.renderingPipeline.size(); i++)
    {
        if(material.renderingPipeline[i].stateChanged){
            //Removal of the previous output textures
            for (size_t outI = 0; outI < material.renderingPipeline[i].outputs.size(); outI++)
            {
                glDeleteTextures(1,&material.renderingPipeline[i].outputs[outI].result);
            }
        }
    }

    material.renderingPipeline.clear();

    GlSet glset;

    glset.viewport(nodeRes,nodeRes);

    for (size_t i = 0; i < material.nodes.size(); i++)
    {
        //Default indexes
        material.nodes[i].renderingIndex = 10000;        
    }
    
    //Process indexes independant nodes
    for (size_t i = 0; i < material.nodes.size(); i++)
    {
        if(material.nodes[i].stateChanged){
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
                if(material.nodes[i].outputs[outputI].connections.size()){
                    nodeHasOutputConnection = true;
                }
            }

            if(!nodeHasInputConnection){
                material.renderingPipeline.push_back(material.nodes[i]);
                material.nodes[i].renderingIndex = material.renderingPipeline.size()-1; 
            }
        }

    }

    for (int i = 0; i < 1000; i++)
    {
        bool isChangesAreMade = false;
        for (int nodeI = 0; nodeI < material.nodes.size(); nodeI++)
        {
            if(material.nodes[nodeI].stateChanged){
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

            if(material.renderingPipeline[nodeI].inputs[inputI].element == "ramp"){
                if(material.renderingPipeline[nodeI].inputs[inputI].nodeConnectionIndex == 10000){
                    glActiveTexture(GL_TEXTURE28);
                    glGenTextures(1, &texture);
                    glBindTexture(GL_TEXTURE_2D,texture);

                    glset.texImage(nullptr,1,1,1);
                }
                else
                    texture = material.renderingPipeline[material.nodes[material.renderingPipeline[nodeI].inputs[inputI].nodeConnectionIndex].renderingIndex].outputs[material.renderingPipeline[nodeI].inputs[inputI].inputConnectionIndex].result;                    
            }
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
                    delete[] data;
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
                        delete[] data;
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
                    delete[] data;
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
            if( material.renderingPipeline[nodeI].title == "22 Repeat"){
                if(material.renderingPipeline[nodeI].listBoxes[0].chosenIndex == 0){
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                } 
                else{
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
                }

            }
            glset.uniformMatrix4fv(nodeProgram,"view",view);
            
            if(material.renderingPipeline[nodeI].inputs[inputI].element == "ramp"){
                for (size_t rampi = 0; rampi < 10; rampi++)
			    {
			    	GlSet gl;
			    	std::string targetPoint = "point1D[" + std::to_string(rampi) + "]";
			    	std::string targetColor = "color1D[" + std::to_string(rampi) + "]";
			    	if(rampi < material.renderingPipeline[nodeI].inputs[inputI].rampPos.size()){
			    		gl.uniform1f(nodeProgram,targetPoint.c_str(),material.renderingPipeline[nodeI].inputs[inputI].rampPos[rampi]);
			    		gl.uniform3fv(nodeProgram,targetColor.c_str(),material.renderingPipeline[nodeI].inputs[inputI].rampClr[rampi]);
			    	}
			    	else{
			    		gl.uniform1f(nodeProgram,targetPoint.c_str(),5.0f);
					    glm::vec3 emptyVec3 = glm::vec3(0);			    		
                        gl.uniform3fv(nodeProgram,targetColor.c_str(),emptyVec3);
			    	}
			    }
            }

            glset.uniform1i(nodeProgram,("input_" + std::to_string(inputI)).c_str(),20+inputI);
            resultOut.textures.push_back(texture);
        }

        for (size_t listI = 0; listI < material.renderingPipeline[nodeI].listBoxes.size(); listI++)
        {
            glset.uniform1i(nodeProgram,("list_" + std::to_string(listI)).c_str(),material.renderingPipeline[nodeI].listBoxes[listI].chosenIndex);
        }
        
        
        glset.uniform1i(nodeProgram,"is3D",0);
		
        glm::mat4 projection = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);
        glset.uniformMatrix4fv(nodeProgram,"projection",projection);

        glActiveTexture(GL_TEXTURE28);

        for (int outI = 0; outI < material.renderingPipeline[nodeI].outputs.size(); outI++)
        {
            if(material.renderingPipeline[nodeI].outputs[outI].isConnectedToShaderInput){
                if(bakeTheMaterial && material.renderingPipeline[nodeI].marked){
                    
                    //Bakes the PBR node

                    glset.viewport(txtrRes,txtrRes);
                    
                    glActiveTexture(GL_TEXTURE28);
                
                    unsigned int copyTxtr;
                    glset.genTextures(copyTxtr);
                    glset.bindTexture(copyTxtr);


                    glset.texImage(nullptr,txtrRes,txtrRes,GL_RGBA);
                    glset.generateMipmap();

                    unsigned int copyFBO; 
                    glset.genFramebuffers(copyFBO);
                    glset.bindFramebuffer(copyFBO);

    	            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + outI, GL_TEXTURE_2D,  copyTxtr, 0);

                    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5,GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7 };
                    glDrawBuffers(8, drawBuffers);

                    glReadBuffer(GL_COLOR_ATTACHMENT0 + outI);

                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


                    model.meshes[currentMaterialIndex].Draw(); 


                    glset.generateMipmap();

                    GLubyte* copyData = new GLubyte[txtrRes*txtrRes*4];
                    glReadPixels(0,0,txtrRes,txtrRes,GL_RGBA,GL_UNSIGNED_BYTE,copyData);

                    glset.texImage(copyData,txtrRes,txtrRes,GL_RGBA);
                    glset.generateMipmap();

                    delete[] copyData;

                    glset.bindFramebuffer(0);
                    glset.deleteFramebuffers(copyFBO);
                    
                    aTexture txtr;
                    txtr.id = copyTxtr;
                    txtr.name = material.renderingPipeline[nodeI].outputs[outI].text;;
                    Utilities util;
                    std::vector<std::string> textureNames;
			        for (size_t nameI = 0; nameI < albedoTextures.size(); nameI++)
			        {
				        textureNames.push_back(albedoTextures[nameI].name);
			        }
			
                    txtr.name = util.uniqueName(txtr.name,textureNames);

                    albedoTextures.push_back(txtr);
                }
                if(true){
                    
                    //Output the material

                    glset.viewport(txtrRes,txtrRes);
                    
                    glActiveTexture(GL_TEXTURE28);

                    unsigned int AFBO;
                    glset.genFramebuffers(AFBO);
                    glset.bindFramebuffer(AFBO);
                    glClear(GL_COLOR_BUFFER_BIT);

    	            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, material.outTexture, 0);

                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                    model.meshes[currentMaterialIndex].Draw(); 

                    glset.generateMipmap();

                    glset.bindFramebuffer(0);
                }
                
                
                resultOut.program = material.renderingPipeline[nodeI].program; 
                lastProgram = resultOut.program;
                glset.uniformMatrix4fv(nodeProgram,"projection",perspectiveProjection);
                glset.uniformMatrix4fv(nodeProgram,"view",view);

                goto end;
            }
            else{
                glset.viewport(nodeRes,nodeRes);

                glActiveTexture(GL_TEXTURE28);
                
                unsigned int resultTexture;
                glset.genTextures(resultTexture);
                glset.bindTexture(resultTexture);
                

                glset.texImage(nullptr,nodeRes,nodeRes,GL_RGBA);
                glset.generateMipmap();
                
                unsigned int FBO; 
                glset.genFramebuffers(FBO);
                glset.bindFramebuffer(FBO);

    	        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + outI, GL_TEXTURE_2D,  resultTexture, 0);
                
                GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5,GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7 };
                glDrawBuffers(8, drawBuffers);

                glReadBuffer(GL_COLOR_ATTACHMENT0 + outI);
                
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                
                if(!material.renderingPipeline[nodeI].useModel)
                    glset.drawArrays(renderVertices,false);
                else
                    model.meshes[currentMaterialIndex].Draw(); 


                glset.generateMipmap();

                GLubyte* data = new GLubyte[nodeRes*nodeRes*4];
                glReadPixels(0,0,nodeRes,nodeRes,GL_RGBA,GL_UNSIGNED_BYTE,data);

                glset.texImage(data,nodeRes,nodeRes,GL_RGBA);
                glset.generateMipmap();

                delete[] data;
                
                glset.bindFramebuffer(0);
                glset.deleteFramebuffers(FBO);

                material.renderingPipeline[nodeI].outputs[outI].result = resultTexture;
                
                
                if(bakeTheMaterial && material.renderingPipeline[nodeI].marked){
                    glset.viewport(txtrRes,txtrRes);
                    
                    glActiveTexture(GL_TEXTURE28);
                
                    unsigned int copyTxtr;
                    glset.genTextures(copyTxtr);
                    glset.bindTexture(copyTxtr);


                    glset.texImage(nullptr,txtrRes,txtrRes,GL_RGBA);
                    glset.generateMipmap();

                    unsigned int copyFBO; 
                    glset.genFramebuffers(copyFBO);
                    glset.bindFramebuffer(copyFBO);

    	            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + outI, GL_TEXTURE_2D,  copyTxtr, 0);

                    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5,GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7 };
                    glDrawBuffers(8, drawBuffers);

                    glReadBuffer(GL_COLOR_ATTACHMENT0 + outI);

                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                    if(!material.renderingPipeline[nodeI].useModel)
                        glset.drawArrays(renderVertices,false);
                    else
                        model.meshes[currentMaterialIndex].Draw(); 


                    glset.generateMipmap();

                    GLubyte* copyData = new GLubyte[txtrRes*txtrRes*4];
                    glReadPixels(0,0,txtrRes,txtrRes,GL_RGBA,GL_UNSIGNED_BYTE,copyData);

                    glset.texImage(copyData,txtrRes,txtrRes,GL_RGBA);
                    glset.generateMipmap();

                    delete[] copyData;

                    glset.bindFramebuffer(0);
                    glset.deleteFramebuffers(copyFBO);
                    
                    aTexture txtr;
                    txtr.id = copyTxtr;
                    txtr.name = material.renderingPipeline[nodeI].outputs[outI].text;;

                    Utilities util;
                    std::vector<std::string> textureNames;
			        for (size_t nameI = 0; nameI < albedoTextures.size(); nameI++)
			        {
				        textureNames.push_back(albedoTextures[nameI].name);
			        }
			
                    txtr.name = util.uniqueName(txtr.name,textureNames);

                    albedoTextures.push_back(txtr);
                }
                resultOut.textures.clear();
            }
        }
    }
    
    
    end:
    glset.bindFramebuffer(0);
	glset.bindRenderBuffer(0);
    
    resultOut.program = lastProgram;
    glset.uniform1i(resultOut.program,"is3D",1);

    LigidPainter lp;
	lp.setViewportToDefault();

    bakeTheMaterial = false;
    return resultOut;
}