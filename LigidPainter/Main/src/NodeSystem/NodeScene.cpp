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

#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"
#include "GUI/GUI.hpp"
#include "LibrarySystem/Library.hpp"
#include "NodeSystem/Node/Node.hpp"

#include <vector>
#include <string>

std::vector<Node> __nodeScene;

void NodeScene::render(glm::vec2 videoScale, Timer &timer, TextRenderer &textRenderer,  Model &model, int textureRes, Scene scene, Panel nodeEditorPanel, NodePanel& nodePanel){
    
    for (size_t i = 0; i < __nodeScene.size(); i++)
    {
        //Update the display texture of the material node
        if(__nodeScene[i].nodeIndex == MATERIAL_NODE){//Is a material node
            for (size_t matI = 0; matI < Library::getMaterialArraySize(); matI++)
            {
                if(__nodeScene[i].materialID == Library::getMaterial(matI)->uniqueID)
                    __nodeScene[i].nodePanel.sections[0].elements[0].button.texture = Texture(Library::getMaterial(matI)->displayingTexture);
            }
        }

        __nodeScene[i].render(videoScale, timer, textRenderer, nodeEditorPanel, i, nodePanel, model, textureRes, scene);
    }
}

void NodeScene::addNode(const Node node){
    if(node.nodeIndex != MESH_NODE)
        registerNodeAction("New node", Texture());
    __nodeScene.push_back(node);
}

Node* NodeScene::getNode(int index){
    return &__nodeScene[index];
}

int NodeScene::getArraySize(){
    return __nodeScene.size();
}

void NodeScene::clearArray(){
    for (size_t i = 1; i < __nodeScene.size(); i++)
    {
        NodeScene::deleteNode(i);
    }

    __nodeScene.clear();
}

std::vector<Node>* NodeScene::getNodeArrayPointer(){
    return &__nodeScene;
}

static void initTexture(unsigned int &txtr, int textureRes){
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1,&txtr);
    glBindTexture(GL_TEXTURE_2D,txtr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureRes, textureRes, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);
}

std::string cToS(glm::vec4 clr){
    if(clr == glm::vec4(1.,1.,1.,1.))
        return "white";
    if(clr == glm::vec4(0.,0.,0.,1.))
        return "black";
    if(clr == glm::vec4(1.,0.,0.,1.))
        return "red";
    if(clr == glm::vec4(1.,1.,0.,1.))
        return "yellow";
    if(clr == glm::vec4(1.,0.5,0.,1.))
        return "orange";
    if(clr == glm::vec4(0.,1.,0.,1.))
        return "green";
    if(clr == glm::vec4(0.,1.,1.,1.))
        return "cyan";
    if(clr == glm::vec4(0.,0.,1.,1.))
        return "blue";
    if(clr == glm::vec4(1.,0.,1.,1.))
        return "pink";

    return "other";
}

static void bindMaterialChannel(Mesh &mesh, int channelI){
    if(channelI == 0)
        glBindTexture(GL_TEXTURE_2D, mesh.albedo.ID);
    if(channelI == 1)
        glBindTexture(GL_TEXTURE_2D, mesh.roughness.ID);
    if(channelI == 2)
        glBindTexture(GL_TEXTURE_2D, mesh.metallic.ID);
    if(channelI == 3)
        glBindTexture(GL_TEXTURE_2D, mesh.normalMap.ID);
    if(channelI == 4)
        glBindTexture(GL_TEXTURE_2D, mesh.heightMap.ID);
    if(channelI == 5)
        glBindTexture(GL_TEXTURE_2D, mesh.ambientOcclusion.ID);
}

static void deleteMaterialChannels(Mesh &mesh){
    glDeleteTextures(1, &mesh.albedo.ID);
    glDeleteTextures(1, &mesh.roughness.ID);
    glDeleteTextures(1, &mesh.metallic.ID);
    glDeleteTextures(1, &mesh.normalMap.ID);
    glDeleteTextures(1, &mesh.heightMap.ID);
    glDeleteTextures(1, &mesh.ambientOcclusion.ID);
}

Mesh NodeScene::processNode(Node &node, Mesh& mesh,   Scene scene, int textureRes){
    Mesh msh = mesh;
    initTexture(msh.albedo.ID, textureRes);
    initTexture(msh.roughness.ID, textureRes);
    initTexture(msh.metallic.ID, textureRes);
    initTexture(msh.normalMap.ID, textureRes);
    initTexture(msh.heightMap.ID, textureRes);
    initTexture(msh.ambientOcclusion.ID, textureRes);

    if(node.nodeIndex == MATERIAL_ID_NODE){        
        // Load the id masking shader
        
        bool detectedWhite = false; 
        bool detectedRed = false; 
        bool detectedGreen = false; 
        bool detectedBlue = false; 
        bool detectedPink = false; 
        bool detectedYellow = false; 
        bool detectedOrange = false; 
        bool detectedCyan = false; 
        bool detectedBlack = false;
        
        
        std::map<std::string, Mesh> retrievedMeshes;
        for (size_t i = 0; i < node.IOs.size(); i++)
        {
            // 0 : input
            // 1 : input without connections 
            // 2 : output
            // 3 : output without connections 
            if(node.IOs[i].state == 0){
                //An input can only have one connection unlike the outputs
                
                if(node.IOs[i].connections.size())
                    retrievedMeshes[cToS(node.IOs[i].element.button.color)] = NodeScene::processNode(__nodeScene[node.IOs[i].connections[0].nodeIndex], mesh, scene, textureRes);
                else{
                    retrievedMeshes[cToS(node.IOs[i].element.button.color)] = Mesh();
                    initTexture(retrievedMeshes[cToS(node.IOs[i].element.button.color)].albedo.ID, 100);
                    initTexture(retrievedMeshes[cToS(node.IOs[i].element.button.color)].roughness.ID, 100);
                    initTexture(retrievedMeshes[cToS(node.IOs[i].element.button.color)].metallic.ID, 100);
                    initTexture(retrievedMeshes[cToS(node.IOs[i].element.button.color)].normalMap.ID, 100);
                    initTexture(retrievedMeshes[cToS(node.IOs[i].element.button.color)].heightMap.ID, 100);
                    initTexture(retrievedMeshes[cToS(node.IOs[i].element.button.color)].ambientOcclusion.ID, 100);
                }
            }
        }

        ShaderSystem::colorIDMaskingShader().use();
        ShaderSystem::colorIDMaskingShader().setMat4("projection", glm::ortho(0.f,1.f,0.f,1.f));
        ShaderSystem::colorIDMaskingShader().setMat4("projectedPosProjection", glm::ortho(0.f,1.f,0.f,1.f));
        ShaderSystem::colorIDMaskingShader().setVec3("pos", glm::vec3(0.5f));
        ShaderSystem::colorIDMaskingShader().setVec2("scale", glm::vec2(0.5f));

        ShaderSystem::colorIDMaskingShader().setInt("IDTexture", 0);
        ShaderSystem::colorIDMaskingShader().setInt("texture_black", 1);
        ShaderSystem::colorIDMaskingShader().setInt("texture_white", 2);
        ShaderSystem::colorIDMaskingShader().setInt("texture_red", 3);
        ShaderSystem::colorIDMaskingShader().setInt("texture_green", 4);
        ShaderSystem::colorIDMaskingShader().setInt("texture_blue", 5);
        ShaderSystem::colorIDMaskingShader().setInt("texture_yellow", 6);
        ShaderSystem::colorIDMaskingShader().setInt("texture_orange", 7);
        ShaderSystem::colorIDMaskingShader().setInt("texture_cyan", 8);
        ShaderSystem::colorIDMaskingShader().setInt("texture_pink", 9);

        unsigned int proceduralTxtr = 0;
        
        // Bind the id mask
        glActiveTexture(GL_TEXTURE0);
        proceduralTxtr = node.IOs[0].element.button.texture.generateProceduralTexture(mesh, scene, textureRes);
        glBindTexture(GL_TEXTURE_2D, proceduralTxtr);
        ShaderSystem::colorIDMaskingShader().use();
                
        Box box;
        box.init();
        box.bindBuffers();

        for (size_t channelI = 0; channelI < 6; channelI++)
        {
            
            /* Black */
            glActiveTexture(GL_TEXTURE1);
            bindMaterialChannel(retrievedMeshes[cToS(glm::vec4(0.f, 0.f, 0.f , 1.f))], channelI);
            
            /* White */
            glActiveTexture(GL_TEXTURE2);
            bindMaterialChannel(retrievedMeshes[cToS(glm::vec4(1.f, 1.f, 1.f , 1.f))], channelI);
            
            /* Red */
            glActiveTexture(GL_TEXTURE3);
            bindMaterialChannel(retrievedMeshes[cToS(glm::vec4(1.f, 0.f, 0.f , 1.f))], channelI);
            
            /* Green */
            glActiveTexture(GL_TEXTURE4);
            bindMaterialChannel(retrievedMeshes[cToS(glm::vec4(0.f, 1.f, 0.f , 1.f))], channelI);
            
            /* Blue */
            glActiveTexture(GL_TEXTURE5);
            bindMaterialChannel(retrievedMeshes[cToS(glm::vec4(0.f, 0.f, 1.f , 1.f))], channelI);
            
            /* Yellow */
            glActiveTexture(GL_TEXTURE6);
            bindMaterialChannel(retrievedMeshes[cToS(glm::vec4(1.f, 1.f, 0.f , 1.f))], channelI);
            
            /* Orange */
            glActiveTexture(GL_TEXTURE7);
            bindMaterialChannel(retrievedMeshes[cToS(glm::vec4(1.f, 0.5f, 0.f , 1.f))], channelI);
            
            /* Cyan */
            glActiveTexture(GL_TEXTURE8);
            bindMaterialChannel(retrievedMeshes[cToS(glm::vec4(0.f, 1.f, 1.f , 1.f))], channelI);
            
            /* Pink */
            glActiveTexture(GL_TEXTURE9);
            bindMaterialChannel(retrievedMeshes[cToS(glm::vec4(1.f, 0.f, 1.f , 1.f))], channelI);
        
            unsigned int mshID;
            if(channelI == 0)
                mshID = msh.albedo.ID;
            if(channelI == 1)
                mshID = msh.roughness.ID;
            if(channelI == 2)
                mshID = msh.metallic.ID;
            if(channelI == 3)
                mshID = msh.normalMap.ID;
            if(channelI == 4)
                mshID = msh.heightMap.ID;
            if(channelI == 5)
                mshID = msh.ambientOcclusion.ID;
            
            
            unsigned int FBO;
            glGenFramebuffers(1,&FBO);
            glBindFramebuffer(GL_FRAMEBUFFER,FBO);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mshID, 0);
            glViewport(0, 0, textureRes, textureRes);
            
            glClearColor(0,0,0,0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glDrawArrays(GL_TRIANGLES, 0 , 6);
            
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDeleteFramebuffers(0, &FBO);
        }



        // DELETE ALL THE MESH CHANNEL TEXTURES
        deleteMaterialChannels(retrievedMeshes[cToS(glm::vec4(0.f, 0.f, 0.f , 1.f))]);
        
        /* White */
        deleteMaterialChannels(retrievedMeshes[cToS(glm::vec4(1.f, 1.f, 1.f , 1.f))]);
        
        /* Red */
        deleteMaterialChannels(retrievedMeshes[cToS(glm::vec4(1.f, 0.f, 0.f , 1.f))]);
        
        /* Green */
        deleteMaterialChannels(retrievedMeshes[cToS(glm::vec4(0.f, 1.f, 0.f , 1.f))]);
        
        /* Blue */
        deleteMaterialChannels(retrievedMeshes[cToS(glm::vec4(0.f, 0.f, 1.f , 1.f))]);
        
        /* Yellow */
        deleteMaterialChannels(retrievedMeshes[cToS(glm::vec4(1.f, 1.f, 0.f , 1.f))]);
        
        /* Orange */
        deleteMaterialChannels(retrievedMeshes[cToS(glm::vec4(1.f, 0.5f, 0.f , 1.f))]);
        
        /* Cyan */
        deleteMaterialChannels(retrievedMeshes[cToS(glm::vec4(0.f, 1.f, 1.f , 1.f))]);
        
        /* Pink */
        deleteMaterialChannels(retrievedMeshes[cToS(glm::vec4(1.f, 0.f, 1.f , 1.f))]);
            
        if(proceduralTxtr != 0)
            glDeleteTextures(1, &proceduralTxtr);
        
        //Return mesh by masking the retrieved meshes
        return msh;
    }
    else if(node.nodeIndex == MATERIAL_MASK_NODE){
        
        Mesh blackMesh;
        if(node.IOs[2].connections.size())
            blackMesh = NodeScene::processNode(__nodeScene[node.IOs[2].connections[0].nodeIndex], mesh, scene, textureRes);
        else{
            initTexture(blackMesh.albedo.ID, 100);
            initTexture(blackMesh.roughness.ID, 100);
            initTexture(blackMesh.metallic.ID, 100);
            initTexture(blackMesh.normalMap.ID, 100);
            initTexture(blackMesh.heightMap.ID, 100);
            initTexture(blackMesh.ambientOcclusion.ID, 100);
        }
        Mesh whiteMesh;
        if(node.IOs[2].connections.size()) 
            whiteMesh = NodeScene::processNode(__nodeScene[node.IOs[3].connections[0].nodeIndex], mesh, scene, textureRes);
        else{
            initTexture(whiteMesh.albedo.ID, 100);
            initTexture(whiteMesh.roughness.ID, 100);
            initTexture(whiteMesh.metallic.ID, 100);
            initTexture(whiteMesh.normalMap.ID, 100);
            initTexture(whiteMesh.heightMap.ID, 100);
            initTexture(whiteMesh.ambientOcclusion.ID, 100);
        }

        ShaderSystem::grayScaleIDMaskingShader().use();
        ShaderSystem::grayScaleIDMaskingShader().setMat4("projection", glm::ortho(0.f,1.f,0.f,1.f));
        ShaderSystem::grayScaleIDMaskingShader().setMat4("projectedPosProjection", glm::ortho(0.f,1.f,0.f,1.f));
        ShaderSystem::grayScaleIDMaskingShader().setVec3("pos", glm::vec3(0.5f));
        ShaderSystem::grayScaleIDMaskingShader().setVec2("scale", glm::vec2(0.5f));

        ShaderSystem::grayScaleIDMaskingShader().setFloat("offset", node.nodePanel.sections[0].elements[1].rangeBar.value / 100.f);

        ShaderSystem::grayScaleIDMaskingShader().setInt("maskTexture", 0);
        ShaderSystem::grayScaleIDMaskingShader().setInt("texture_black", 1);
        ShaderSystem::grayScaleIDMaskingShader().setInt("texture_white", 2);
        
        unsigned int proceduralTxtr = 0;
        
        // Bind the mask
        glActiveTexture(GL_TEXTURE0);
        proceduralTxtr = node.nodePanel.sections[0].elements[0].button.texture.generateProceduralTexture(mesh, scene, textureRes);
        glBindTexture(GL_TEXTURE_2D, proceduralTxtr);
        ShaderSystem::grayScaleIDMaskingShader().use();
                
        Box box;
        box.init();
        box.bindBuffers();

        for (size_t channelI = 0; channelI < 6; channelI++)
        {
            /* Black */
            glActiveTexture(GL_TEXTURE1);
            bindMaterialChannel(blackMesh, channelI);
            
            /* White */
            glActiveTexture(GL_TEXTURE2);
            bindMaterialChannel(whiteMesh, channelI);
            
            unsigned int mshID;

            if(channelI == 0)
                mshID = msh.albedo.ID;
            if(channelI == 1)
                mshID = msh.roughness.ID;
            if(channelI == 2)
                mshID = msh.metallic.ID;
            if(channelI == 3)
                mshID = msh.normalMap.ID;
            if(channelI == 4)
                mshID = msh.heightMap.ID;
            if(channelI == 5)
                mshID = msh.ambientOcclusion.ID;
            
            unsigned int FBO;
            glGenFramebuffers(1,&FBO);
            glBindFramebuffer(GL_FRAMEBUFFER,FBO);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mshID, 0);
            glViewport(0, 0, textureRes, textureRes);
            
            glClearColor(0,0,0,0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glDrawArrays(GL_TRIANGLES, 0 , 6);
            
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDeleteFramebuffers(0, &FBO);
        }
        
        // DELETE ALL THE MESH CHANNEL TEXTURES
        deleteMaterialChannels(blackMesh);
        
        /* White */
        deleteMaterialChannels(whiteMesh);

        if(proceduralTxtr != 0)
            glDeleteTextures(1, &proceduralTxtr);

        //Return masked node
        return msh;
    }
    else if(node.nodeIndex == MATERIAL_NODE){
        
        Material material;
        for (size_t i = 0; i < Library::getMaterialArraySize(); i++)
        {
            if(node.materialID == Library::getMaterial(i)->uniqueID)
                material = *Library::getMaterial(i);
        }
        
        for (int i = material.materialModifiers.size() - 1; i >= 0; --i)    
        {
            material.materialModifiers[i].updateMaterialChannels(material, msh, textureRes, i, scene.projectionMatrix, scene.viewMatrix);
        }

        
        //Return mesh by creating using material modifiers
        return msh;
    }

    return msh;
}

std::vector<int> findNodeIndexConnectedToMesh(int i){
    
    std::vector<int> res;

    for (size_t IOI = 0; IOI < __nodeScene[i].IOs.size(); IOI++)
    {
        if(__nodeScene[i].IOs[IOI].state == 2){
            for (size_t conI = 0; conI < __nodeScene[i].IOs[conI].connections.size(); conI++)
            {
                if(__nodeScene[i].IOs[IOI].connections[conI].nodeIndex == 0){
                    res.push_back(i);
                }
                else{
                    std::vector<int> rec = findNodeIndexConnectedToMesh(__nodeScene[i].IOs[IOI].connections[conI].nodeIndex);
                    for (size_t i = 0; i < rec.size(); i++)
                    {
                        res.push_back(rec[i]);
                    }
                }
            }
        }
    }
    
    return res;
}

void NodeScene::updateNodeResults( Model& model, Scene scene, int textureRes, int updateNodeI){
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    int viewportWidth = viewport[2];
    int viewportHeight = viewport[3];
    
    if(updateNodeI == -1){
        for (size_t nodeI = 0; nodeI < __nodeScene.size(); nodeI++)
        {
            for (size_t IOI = 0; IOI < __nodeScene[nodeI].IOs.size(); IOI++)
            {
                if(__nodeScene[nodeI].IOs[IOI].state == 2){
                    for (size_t conI = 0; conI < __nodeScene[nodeI].IOs[IOI].connections.size(); conI++)
                    {
                        if(__nodeScene[nodeI].IOs[IOI].connections[conI].nodeIndex == 0){
                            Mesh retMesh = NodeScene::processNode(__nodeScene[nodeI], model.meshes[__nodeScene[nodeI].IOs[IOI].connections[conI].inputIndex], scene, textureRes);
                            
                            deleteMaterialChannels(model.meshes[__nodeScene[nodeI].IOs[IOI].connections[conI].inputIndex]);
                            
                            model.meshes[__nodeScene[nodeI].IOs[IOI].connections[conI].inputIndex].albedo.ID = retMesh.albedo.ID; 
                            model.meshes[__nodeScene[nodeI].IOs[IOI].connections[conI].inputIndex].roughness.ID = retMesh.roughness.ID; 
                            model.meshes[__nodeScene[nodeI].IOs[IOI].connections[conI].inputIndex].metallic.ID = retMesh.metallic.ID; 
                            model.meshes[__nodeScene[nodeI].IOs[IOI].connections[conI].inputIndex].normalMap.ID = retMesh.normalMap.ID; 
                            model.meshes[__nodeScene[nodeI].IOs[IOI].connections[conI].inputIndex].heightMap.ID = retMesh.heightMap.ID; 
                            model.meshes[__nodeScene[nodeI].IOs[IOI].connections[conI].inputIndex].ambientOcclusion.ID = retMesh.ambientOcclusion.ID;

                        }
                    }
                }
            } 
        }
    }
    else{
        std::vector<int> indices = findNodeIndexConnectedToMesh( updateNodeI);

        for (size_t i = 0; i < indices.size(); i++)
        {
            for (size_t IOI = 0; IOI < __nodeScene[indices[i]].IOs.size(); IOI++)
            {
                if(__nodeScene[indices[i]].IOs[IOI].state == 2){
                    for (size_t conI = 0; conI < __nodeScene[indices[i]].IOs[IOI].connections.size(); conI++)
                    {
                        if(__nodeScene[indices[i]].IOs[IOI].connections[conI].nodeIndex == 0){
                            Mesh retMesh = NodeScene::processNode(__nodeScene[indices[i]], model.meshes[__nodeScene[indices[i]].IOs[IOI].connections[conI].inputIndex],  scene, textureRes);
                            
                            deleteMaterialChannels(model.meshes[__nodeScene[indices[i]].IOs[IOI].connections[conI].inputIndex]);
                            
                            model.meshes[__nodeScene[indices[i]].IOs[IOI].connections[conI].inputIndex].albedo.ID = retMesh.albedo.ID; 
                            model.meshes[__nodeScene[indices[i]].IOs[IOI].connections[conI].inputIndex].roughness.ID = retMesh.roughness.ID; 
                            model.meshes[__nodeScene[indices[i]].IOs[IOI].connections[conI].inputIndex].metallic.ID = retMesh.metallic.ID; 
                            model.meshes[__nodeScene[indices[i]].IOs[IOI].connections[conI].inputIndex].normalMap.ID = retMesh.normalMap.ID; 
                            model.meshes[__nodeScene[indices[i]].IOs[IOI].connections[conI].inputIndex].heightMap.ID = retMesh.heightMap.ID; 
                            model.meshes[__nodeScene[indices[i]].IOs[IOI].connections[conI].inputIndex].ambientOcclusion.ID = retMesh.ambientOcclusion.ID;

                        }
                    }
                }
            }
        }
        

        //TODO : Delete previous textures   
    }

    glViewport(0, 0, viewportWidth, viewportHeight);
}

void NodeScene::updateAllTheNodeConnections(){
	//Check all the nodes
	for (size_t ii = 0; ii < __nodeScene.size(); ii++)
	{
		for (size_t IOI = 0; IOI < __nodeScene[ii].IOs.size(); IOI++)
		{
			for (size_t conI = 0; conI < __nodeScene[ii].IOs[IOI].connections.size(); conI++)
			{
				NodeConnection connection = __nodeScene[ii].IOs[IOI].connections[conI];
				
				//If the input or the node is disapeared 
				if(connection.inputIndex >= __nodeScene[connection.nodeIndex].IOs.size() || connection.nodeIndex >= __nodeScene.size()){
					//Severe the connection
					__nodeScene[ii].IOs[IOI].connections.erase(__nodeScene[ii].IOs[IOI].connections.begin() + conI);
					conI--;
				}
			}
		}
	}
}

void NodeScene::deleteNode(int index){
	//If the deleted node is a material node
    if(__nodeScene[index].nodeIndex != MESH_NODE){
        
        if(__nodeScene[index].nodeIndex == MATERIAL_ID_NODE | MATERIAL_MASK_NODE){
            if(__nodeScene[index].nodePanel.sections[0].elements[0].button.texture.ID != 0)
                glDeleteTextures(1, &__nodeScene[index].nodePanel.sections[0].elements[0].button.texture.ID);
        }

        //Remove the related input connections
        for (size_t IOI = 0; IOI < __nodeScene[index].IOs.size(); IOI++)
        {
            for (size_t conI = 0; conI < __nodeScene[index].IOs[IOI].connections.size(); conI++)
            {
                int nodeI = __nodeScene[index].IOs[IOI].connections[conI].nodeIndex; 
                int inputI = __nodeScene[index].IOs[IOI].connections[conI].inputIndex; 
                
                //Remove the connection from the connected node/IO
                __nodeScene[nodeI].IOs[inputI].connections.clear();
            }
        }
        
        __nodeScene.erase(__nodeScene.begin() + index);
    }
}