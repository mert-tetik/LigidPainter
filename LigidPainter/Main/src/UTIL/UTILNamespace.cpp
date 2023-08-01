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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <filesystem>

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"


float UTIL::getPercent(float value, float percent){
    return value / 100.f * percent;
}
glm::vec2 UTIL::getPercent(glm::vec2 value, glm::vec2 percent){
    return glm::vec2(value.x / 100.f * percent.x,value.y / 100.f * percent.y);
}
glm::vec3 UTIL::getPercent(glm::vec3 value, glm::vec3 percent){
    return glm::vec3(value.x / 100.f * percent.x,value.y / 100.f * percent.y,value.z / 100.f * percent.z);
}

std::string UTIL::getLastWordBySeparatingWithChar(std::string s, char del)
{
	if(s == "" || s.size() == 1 || s.size() == 2)
		return "";

	std::vector<std::string> words;
	std::stringstream ss(s);
	std::string word;
	while (!ss.eof()) {
		std::getline(ss, word, del);
		words.push_back(word);
	}
	return words[words.size() - 1];
}

std::string UTIL::removeLastWordBySeparatingWithChar(std::string s, char del)
{
	for (int i = s.size()-1; i >= 0; i--)
	{
		if(s[i] == del){
			s.erase(s.begin()+i,s.end());
			break;
		}
	}
	return s;
}

std::string UTIL::removeExtension(std::string s){
	int i = 1;
	while (s[s.size() - i] != '.'){i++;}
	for (size_t j = 0; j < i; j++){s.pop_back();}
	return s;
}

std::string UTIL::getFileExtension(std::string s) {
    size_t dotIndex = s.find_last_of('.');
    if (dotIndex != std::string::npos) {
        return s.substr(dotIndex + 1);
    }
    return "";
}

std::string UTIL::toLowercase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return result;
}


std::vector<Material> UTIL::getTheMaterialsConnectedToTheMeshNode(std::vector<Node> &meshNodeScene,Library &library,int textureRes, AppTextures appTextures){
    std::vector<Material> materials;
    
    // //Check all the inputs of the mesh node
    // for (size_t i = 0; i < meshNodeScene[0].IOs.size(); i++)
    // {
    //     int materialID = 1000;
    //     if(meshNodeScene[0].IOs[i].connections.size())
    //         materialID = meshNodeScene[meshNodeScene[0].IOs[i].connections[0].nodeIndex].materialID;
    //     else{
	// 		if(textureRes == 0){
    //         	materials.push_back(Material());

	// 			materials[materials.size()-1].albedo = appTextures.noMaterialConnectedToTheMeshWarningImage;
	// 			materials[materials.size()-1].roughness = appTextures.noMaterialConnectedToTheMeshWarningImage;
	// 			materials[materials.size()-1].metallic = appTextures.noMaterialConnectedToTheMeshWarningImage;
	// 			materials[materials.size()-1].normalMap = appTextures.noMaterialConnectedToTheMeshWarningImage;
	// 			materials[materials.size()-1].heightMap = appTextures.noMaterialConnectedToTheMeshWarningImage;
	// 			materials[materials.size()-1].ambientOcclusion = appTextures.noMaterialConnectedToTheMeshWarningImage;
	// 		}
	// 		else
    //         	materials.push_back(Material(textureRes,"autoGenerated",123456));
	// 	}

    //     if(materialID != 1000){
    //         for (size_t matI = 0; matI < library.materials.size(); matI++)
    //         {
    //             if(library.materials[matI].uniqueID == materialID)
    //                 materials.push_back(library.materials[matI]);
    //         }
    //     }
    // }

    return materials;
}

void UTIL::updateAllTheNodeConnections(std::vector<Node> &meshNodeScene){
	//Check all the nodes
	for (size_t ii = 0; ii < meshNodeScene.size(); ii++)
	{
		for (size_t IOI = 0; IOI < meshNodeScene[ii].IOs.size(); IOI++)
		{
			for (size_t conI = 0; conI < meshNodeScene[ii].IOs[IOI].connections.size(); conI++)
			{
				NodeConnection connection = meshNodeScene[ii].IOs[IOI].connections[conI];
				
				//If the input or the node is disapeared 
				if(connection.inputIndex >= meshNodeScene[connection.nodeIndex].IOs.size() || connection.nodeIndex >= meshNodeScene.size()){
					//Severe the connection
					meshNodeScene[ii].IOs[IOI].connections.erase(meshNodeScene[ii].IOs[IOI].connections.begin() + conI);
					conI--;
				}
			}
		}
	}
}

void UTIL::deleteNode(std::vector<Node>& meshNodeScene, int index){
	//If the deleted node is a material node
    if(meshNodeScene[index].nodeIndex == MATERIAL_NODE){
        
        //Remove the related input connections
        for (size_t IOI = 0; IOI < meshNodeScene[index].IOs.size(); IOI++)
        {
            for (size_t conI = 0; conI < meshNodeScene[index].IOs[IOI].connections.size(); conI++)
            {
                int nodeI = meshNodeScene[index].IOs[IOI].connections[conI].nodeIndex; 
                int inputI = meshNodeScene[index].IOs[IOI].connections[conI].inputIndex; 
                
                //Remove the connection from the connected node/IO
                meshNodeScene[nodeI].IOs[inputI].connections.clear();
            }
        }
        
        meshNodeScene.erase(meshNodeScene.begin() + index);
    }
}
static void initTexture(unsigned int &txtr, int textureRes){
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1,&txtr);
    glBindTexture(GL_TEXTURE_2D,txtr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

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

Mesh UTIL::processNode(Node &node, std::vector<Node> &nodeScene, Library library, Mesh& mesh, Shader heightToNormalShader, Scene scene, int textureRes){
    
    
    Mesh msh = mesh;
    initTexture(msh.albedo.ID, textureRes);
    initTexture(msh.roughness.ID, textureRes);
    initTexture(msh.metallic.ID, textureRes);
    initTexture(msh.normalMap.ID, textureRes);
    initTexture(msh.heightMap.ID, textureRes);
    initTexture(msh.ambientOcclusion.ID, textureRes);

    if(node.nodeIndex == MATERIAL_ID_NODE){
        
        Box box;
        box.init();
        box.bindBuffers();
        
        // Load the id masking shader
        Shader idMaskingShader = Shader("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/MaterialID.frag" ,nullptr, nullptr, nullptr);
        
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
                    retrievedMeshes[cToS(node.IOs[i].element.button.color)] = UTIL::processNode(nodeScene[node.IOs[i].connections[0].nodeIndex], nodeScene, library, mesh, heightToNormalShader, scene, textureRes);
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

        idMaskingShader.use();
        idMaskingShader.setMat4("projection", glm::ortho(0.f,1.f,0.f,1.f));
        idMaskingShader.setMat4("projectedPosProjection", glm::ortho(0.f,1.f,0.f,1.f));
        idMaskingShader.setVec3("pos", glm::vec3(0.5f));
        idMaskingShader.setVec2("scale", glm::vec2(0.5f));

        idMaskingShader.setInt("IDTexture", 0);
        idMaskingShader.setInt("texture_black", 1);
        idMaskingShader.setInt("texture_white", 2);
        idMaskingShader.setInt("texture_red", 3);
        idMaskingShader.setInt("texture_green", 4);
        idMaskingShader.setInt("texture_blue", 5);
        idMaskingShader.setInt("texture_yellow", 6);
        idMaskingShader.setInt("texture_orange", 7);
        idMaskingShader.setInt("texture_cyan", 8);
        idMaskingShader.setInt("texture_pink", 9);

        // Bind the id mask
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, node.IOs[0].element.button.texture.ID);
        
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
            
            glClearColor(0.5,1,0,1);
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
            
        //Return mesh by masking the retrieved meshes
        return msh;
    }
    else if(node.nodeIndex == MATERIAL_MASK_NODE){
        
        Mesh blackMesh;
        if(node.IOs[2].connections.size())
            blackMesh = UTIL::processNode(nodeScene[node.IOs[2].connections[0].nodeIndex], nodeScene, library, mesh, heightToNormalShader, scene, textureRes);
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
            whiteMesh = UTIL::processNode(nodeScene[node.IOs[3].connections[0].nodeIndex], nodeScene, library, mesh, heightToNormalShader, scene, textureRes);
        else{
            initTexture(whiteMesh.albedo.ID, 100);
            initTexture(whiteMesh.roughness.ID, 100);
            initTexture(whiteMesh.metallic.ID, 100);
            initTexture(whiteMesh.normalMap.ID, 100);
            initTexture(whiteMesh.heightMap.ID, 100);
            initTexture(whiteMesh.ambientOcclusion.ID, 100);
        }

        Shader maskingShader = Shader("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/GrayScaleMasking.frag" ,nullptr, nullptr, nullptr);

        maskingShader.use();
        maskingShader.setMat4("projection", glm::ortho(0.f,1.f,0.f,1.f));
        maskingShader.setMat4("projectedPosProjection", glm::ortho(0.f,1.f,0.f,1.f));
        maskingShader.setVec3("pos", glm::vec3(0.5f));
        maskingShader.setVec2("scale", glm::vec2(0.5f));

        maskingShader.setFloat("offset", node.nodePanel.sections[0].elements[1].rangeBar.value / 100.f);

        maskingShader.setInt("maskTexture", 0);
        maskingShader.setInt("texture_black", 1);
        maskingShader.setInt("texture_white", 2);
        
        // Bind the mask
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, node.nodePanel.sections[0].elements[0].button.texture.ID);

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
            
            glClearColor(0.5,1,0,1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glDrawArrays(GL_TRIANGLES, 0 , 6);
            
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDeleteFramebuffers(0, &FBO);
        }
        
        // DELETE ALL THE MESH CHANNEL TEXTURES
        deleteMaterialChannels(blackMesh);
        
        /* White */
        deleteMaterialChannels(whiteMesh);

        //Return masked node
        return msh;
    }
    else if(node.nodeIndex == MATERIAL_NODE){
        
        Material material;
        for (size_t i = 0; i < library.materials.size(); i++)
        {
            if(node.materialID == library.materials[i].uniqueID)
                material = library.materials[i];
        }
        
        for (int i = material.materialModifiers.size() - 1; i >= 0; --i)    
        {
            material.materialModifiers[i].updateMaterialChannels(material, msh, textureRes, i, scene.projectionMatrix, scene.viewMatrix, heightToNormalShader);
        }

        
        //Return mesh by creating using material modifiers
        return msh;
    }

    return msh;
}

std::vector<int> findNodeIndexConnectedToMesh(std::vector<Node> meshNodeScene, int i){
    
    std::vector<int> res;

    for (size_t IOI = 0; IOI < meshNodeScene[i].IOs.size(); IOI++)
    {
        if(meshNodeScene[i].IOs[IOI].state == 2){
            for (size_t conI = 0; conI < meshNodeScene[i].IOs[conI].connections.size(); conI++)
            {
                if(meshNodeScene[i].IOs[IOI].connections[conI].nodeIndex == 0){
                    res.push_back(i);
                }
                else{
                    std::vector<int> rec = findNodeIndexConnectedToMesh(meshNodeScene,meshNodeScene[i].IOs[IOI].connections[conI].nodeIndex);
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

void UTIL::updateNodeResults(std::vector<Node>& meshNodeScene, Model& model, Library library, Shader heightToNormalShader, Scene scene, int textureRes, int updateNodeI){
    if(updateNodeI == -1){
        for (size_t nodeI = 0; nodeI < meshNodeScene.size(); nodeI++)
        {
            for (size_t IOI = 0; IOI < meshNodeScene[nodeI].IOs.size(); IOI++)
            {
                if(meshNodeScene[nodeI].IOs[IOI].state == 2){
                    for (size_t conI = 0; conI < meshNodeScene[nodeI].IOs[IOI].connections.size(); conI++)
                    {
                        if(meshNodeScene[nodeI].IOs[IOI].connections[conI].nodeIndex == 0){
                            Mesh retMesh = UTIL::processNode(meshNodeScene[nodeI], meshNodeScene, library, model.meshes[meshNodeScene[nodeI].IOs[IOI].connections[conI].inputIndex], heightToNormalShader, scene, textureRes);
                            
                            deleteMaterialChannels(model.meshes[meshNodeScene[nodeI].IOs[IOI].connections[conI].inputIndex]);
                            
                            model.meshes[meshNodeScene[nodeI].IOs[IOI].connections[conI].inputIndex].albedo.ID = retMesh.albedo.ID; 
                            model.meshes[meshNodeScene[nodeI].IOs[IOI].connections[conI].inputIndex].roughness.ID = retMesh.roughness.ID; 
                            model.meshes[meshNodeScene[nodeI].IOs[IOI].connections[conI].inputIndex].metallic.ID = retMesh.metallic.ID; 
                            model.meshes[meshNodeScene[nodeI].IOs[IOI].connections[conI].inputIndex].normalMap.ID = retMesh.normalMap.ID; 
                            model.meshes[meshNodeScene[nodeI].IOs[IOI].connections[conI].inputIndex].heightMap.ID = retMesh.heightMap.ID; 
                            model.meshes[meshNodeScene[nodeI].IOs[IOI].connections[conI].inputIndex].ambientOcclusion.ID = retMesh.ambientOcclusion.ID;

                        }
                    }
                }
            }
        }
    }
    else{
        std::vector<int> indices = findNodeIndexConnectedToMesh(meshNodeScene, updateNodeI);

        for (size_t i = 0; i < indices.size(); i++)
        {
            for (size_t IOI = 0; IOI < meshNodeScene[indices[i]].IOs.size(); IOI++)
            {
                if(meshNodeScene[indices[i]].IOs[IOI].state == 2){
                    for (size_t conI = 0; conI < meshNodeScene[indices[i]].IOs[IOI].connections.size(); conI++)
                    {
                        if(meshNodeScene[indices[i]].IOs[IOI].connections[conI].nodeIndex == 0){
                            Mesh retMesh = UTIL::processNode(meshNodeScene[indices[i]], meshNodeScene, library, model.meshes[meshNodeScene[indices[i]].IOs[IOI].connections[conI].inputIndex], heightToNormalShader, scene, textureRes);
                            
                            deleteMaterialChannels(model.meshes[meshNodeScene[indices[i]].IOs[IOI].connections[conI].inputIndex]);
                            
                            model.meshes[meshNodeScene[indices[i]].IOs[IOI].connections[conI].inputIndex].albedo.ID = retMesh.albedo.ID; 
                            model.meshes[meshNodeScene[indices[i]].IOs[IOI].connections[conI].inputIndex].roughness.ID = retMesh.roughness.ID; 
                            model.meshes[meshNodeScene[indices[i]].IOs[IOI].connections[conI].inputIndex].metallic.ID = retMesh.metallic.ID; 
                            model.meshes[meshNodeScene[indices[i]].IOs[IOI].connections[conI].inputIndex].normalMap.ID = retMesh.normalMap.ID; 
                            model.meshes[meshNodeScene[indices[i]].IOs[IOI].connections[conI].inputIndex].heightMap.ID = retMesh.heightMap.ID; 
                            model.meshes[meshNodeScene[indices[i]].IOs[IOI].connections[conI].inputIndex].ambientOcclusion.ID = retMesh.ambientOcclusion.ID;

                        }
                    }
                }
            }
        }
        

        //TODO : Delete previous textures   
    }
}


std::string UTIL::rmvPath(std::string startingPath, std::string fullPath) 
{
	// look for starting path in the fullpath
	int index = fullPath.find(startingPath);

	if (index != std::string::npos)
	{
		fullPath = fullPath.erase(0, startingPath.length());
	}

	return fullPath;
}


void UTIL::openInFileExplorer(const char* path) {
#ifdef _WIN32
    // Windows
    std::string command = std::string("explorer ") + path;
    std::system(command.c_str());
#elif __linux__
    // Linux
    std::string command = std::string("xdg-open ") + path;
    std::system(command.c_str());
#elif __APPLE__
    // macOS
    std::string command = std::string("open ") + path;
    std::system(command.c_str());
#else
    // Unsupported platform
    // Add error handling or alternative implementation here
#endif
}

void UTIL::giveUniqueId(int &ID ,const std::vector<int> otherIDs){
	int newID = 0;

    // Find the next available ID that is not in the otherIDs vector
    while (std::find(otherIDs.begin(), otherIDs.end(), newID) != otherIDs.end()) {
        newID++;
    }

    // Assign the unique ID to the reference variable
    ID = newID;
}

void UTIL::giveUniqueId(int &ID ,const std::vector<Texture> textures){
	std::vector<int> IDArray;

	for (size_t i = 0; i < textures.size(); i++)
	{
		IDArray.push_back(textures[i].uniqueId);
	}
	
	giveUniqueId(ID, IDArray);
}

void UTIL::giveUniqueId(int &ID ,const std::vector<Material> materials){
	std::vector<int> IDArray;

	for (size_t i = 0; i < materials.size(); i++)
	{
		IDArray.push_back(materials[i].uniqueID);
	}

	giveUniqueId(ID, IDArray);
}

char* const* UTIL::convertStringArray(const std::vector<std::string> strings){
    char** convertedArray = new char*[strings.size()];

    for (size_t i = 0; i < strings.size(); i++)
    {
        convertedArray[i] = const_cast<char*>(strings[i].c_str());
    }

    return convertedArray;
}

char UTIL::folderDistinguisher(){
	//If the user is on a windows OS
	#if defined(_WIN32) || defined(_WIN64)
		return '\\';
	//If the user is on a unix based OS
	#else
		return '/'; 
	#endif
}

void UTIL::deleteFilesInFolder(const std::string folderPath) {
    for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            std::filesystem::remove(entry.path());
        }
    }
}

void UTIL::duplicateFolder(const std::string src, const std::string dest){
	for (const auto& entry : std::filesystem::recursive_directory_iterator(src)){
        
		std::string file = entry.path().string();  

        std::string dst = dest + UTIL::folderDistinguisher() + UTIL::rmvPath(src, file);
        
        //If a folder then create a folder
        if(std::filesystem::is_directory(file)){
            
            std::filesystem::create_directories(dst);
        
        }

        //If a file then duplicate the file
        else{

            std::filesystem::copy_file(file, dst);

        }
    }
}