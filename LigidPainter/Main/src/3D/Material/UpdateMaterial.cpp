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

#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"

#include <string>
#include <iostream>
#include <vector>


void Material::updateMaterial(
                                float textureRes,
                                Box box,
                                Context context,
                                Shader buttonShader,
                                Shader tdModelShader,
                                Model sphereModel
                            ){ 
    //Set the OpenGL viewport to the texture resolution
    glViewport(0,0,textureRes,textureRes);
    
    //Set the orthographic projection to the texture resolution
    glm::mat4 projection = glm::ortho(0.f,(float)textureRes,(float)textureRes,0.f);
    
    //Transform values to take the texture in to the middle of the screen and cover it completely
    glm::vec2 fragScale = glm::vec2((float)textureRes/2.f,(float)textureRes/2.f);
    glm::vec3 fragPos = glm::vec3((float)textureRes/2.f,(float)textureRes/2.f,1.0f);
    
    //For every modifier the material has (Output every modifier the material has)
    for (int i = this->materialModifiers.size() - 1; i >= 0; --i)    
    {
        //For all the material channels
        for (int channelI = 0; channelI < 6; channelI++) 
        {
            //Disable the depth test (just in case)
            glDisable(GL_DEPTH_TEST);

            //That framebuffer will be used to get the results of the shader 
            unsigned int FBO; 
            glGenFramebuffers(1,&FBO);
            glBindFramebuffer(GL_FRAMEBUFFER,FBO);
        
            //Get the channel's texture from material
            unsigned int textureBuffer; //Material's texture
            if(channelI == 0){
                textureBuffer = this->albedo.ID;
            }
            if(channelI == 1){
                textureBuffer = this->roughness.ID;
            }
            if(channelI == 2){
                textureBuffer = this->metallic.ID;
            }
            if(channelI == 3){
                textureBuffer = this->normalMap.ID;
            }
            if(channelI == 4){
                textureBuffer = this->heightMap.ID;
            }
            if(channelI == 5){
                textureBuffer = this->ambientOcclusion.ID;
            }
            
            //Bind the channel texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D,textureBuffer);
            
            //Params for the channel texture
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
            
            //Refresh the channel texture
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureRes, textureRes, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glGenerateMipmap(GL_TEXTURE_2D);

            //Bind the channel texture to the capture framebuffer
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureBuffer, 0);
            
            //Make the background pink (cause why not)
            glClearColor(1,0,1,1);
            glClear(GL_COLOR_BUFFER_BIT);
            
            //Set the uniforms of the modifier's shader
            this->materialModifiers[i].shader.use(); //Use the shader of the modifier
            this->materialModifiers[i].shader.setMat4("projection",projection); //Set the projection
            this->materialModifiers[i].shader.setVec2("scale",fragScale); //Set the scale
            this->materialModifiers[i].shader.setVec3("pos",fragPos); //Set the position
            this->materialModifiers[i].shader.setInt("theTexture",0); //Set the texture slot
            this->materialModifiers[i].shader.setInt("state",channelI); //Set the texture slot


            //Bind the texture (bind the channel textures if rendering a texture modifier & bind the result of the previous modifier)
            glActiveTexture(GL_TEXTURE0);
            
            //If the modifier is a texture modifier bind the texture from the materialModifier's button element texture
            if(this->materialModifiers[i].modifierIndex == 0) 
                glBindTexture(GL_TEXTURE_2D,this->materialModifiers[i].sections[0].elements[channelI].button.texture.ID);
            //else
            //  glBindTexture(GL_TEXTURE_2D,previousTexture);
            
            //Render the result to the framebuffer
            glDrawArrays(GL_TRIANGLES, 0, 6);
            
            //Just in case 🤫😁🤑 
            glGenerateMipmap(GL_TEXTURE_2D);
            
            //Delete the framebuffer after completing the channel
            glDeleteFramebuffers(1,&FBO);
            glEnable(GL_DEPTH_TEST);
        }
    }
    

    
    //!Update the material displaying texture
    
    //Capturing framebuffer
    unsigned int FBO; 
    glGenFramebuffers(1,&FBO);
    glBindFramebuffer(GL_FRAMEBUFFER,FBO);
    
    //Resolution of the material displaying texture
    const int displayRes = 2048;

    //Set the OpenGL viewport to the resolution of the material displaying texture
    glViewport(0,0,displayRes,displayRes);

    //Render buffer to render the sphere 3D model (for depth testing)
    unsigned int RBO;
	glGenRenderbuffers(1,&RBO);
	glBindRenderbuffer(GL_RENDERBUFFER,RBO);
	
    //Set the renderbuffer to store depth
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, displayRes, displayRes);
	
    //Give the renderbuffer to the framebuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);
    
    //Bind the displaying texture of the material to the capture framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->displayingTexture, 0);
    
    //Clear the capture framebuffer (displaying texture) with alpha zero color
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Use the 3D model rendering shader
    tdModelShader.use();

    //Move the camera to the side
    glm::vec3 viewPos = glm::vec3(3.f,0,0);
    glm::mat4 view = glm::lookAt(viewPos, 
                                 glm::vec3(0), 
                                 glm::vec3(0.0, 1.0, 0.0));
    //The perspective projection matrix    
    glm::mat4 projectionMatrix = glm::perspective(
                                                    glm::radians(90.f), //Fov  
                                                    1.f,  //Ratio (is 1 since the width & the height is equal to displayRes)
                                                    100.f,  //Near (the material is pretty close to the camera actually  ) 
                                                    0.1f    //Far
                                                );

    //Throw the camera data to the shader
    tdModelShader.setVec3("viewPos",viewPos);
    tdModelShader.setMat4("view",view);
    tdModelShader.setMat4("projection",projectionMatrix);
    
    //Bind the channels of the material
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D,this->albedo.ID);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D,this->roughness.ID);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D,this->metallic.ID);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D,this->normalMap.ID);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D,this->heightMap.ID);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D,this->ambientOcclusion.ID);
    

    //Draw the sphere
    sphereModel.Draw();
    
    //Just in case (Is not necessary (probably (I guess))) !!Actually I'm 100% sure that's not necessary but u know. Just in case. lol
    glGenerateMipmap(GL_TEXTURE_2D);
    
    //!Finish (prepeare rendering the GUI)

    //Use the button shader (Is necessary since that process is done in the middle of GUI rendering) 
    buttonShader.use();

    //Bind the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    
    //Our work is done with that capture framebuffer (delete it)
    glDeleteFramebuffers(1,&FBO);

    //Set the OpenGL viewport to default
    glViewport(0,0,context.windowScale.x,context.windowScale.y);
}