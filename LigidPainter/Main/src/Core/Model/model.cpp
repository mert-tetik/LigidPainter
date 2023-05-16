
#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Core/Model/mesh.h"
#include "Core/Model/model.h"

#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

size_t rndrC = 0;

using namespace std;


    // model data 
    vector<TextureMs> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh>    meshes;
    string directory;

    unsigned int mVAO;
    unsigned int mVBO;

    void Model::sendObjectsToModel(unsigned int aVAO,unsigned int aVBO){
        mVAO = aVAO;
        mVBO = aVBO;
    }

    // draws the model, and thus all its meshes
    void Model::Draw(unsigned int chosenMaterialIndex,unsigned int PBRProgram,bool useOpacity,std::vector<MaterialOut> &modelMaterials,glm::mat4 view,bool paintingMode,unsigned int selectedTexture,
                    glm::vec3 viewPos,float skyboxExposureVal,float skyboxRotationVal,bool renderOnlyTheChosenMesh, std::vector<unsigned int> materialOutputs,unsigned int materialResultProgram,glm::mat4& modelMatrix,
                    bool useUV,float lightRotation)
    {
        if(meshes.size() > 0 && paintingMode){
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D,selectedTexture);
            glUniform1f(glGetUniformLocation(PBRProgram, "opacity"), 1.0f);
            meshes[chosenMaterialIndex].Draw(); 
    		glActiveTexture(GL_TEXTURE28);
        }

        for(unsigned int i = 0; i < meshes.size(); i++){
            if(!paintingMode){
                glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)glfwGetVideoMode(glfwGetPrimaryMonitor())->width / (float)glfwGetVideoMode(glfwGetPrimaryMonitor())->height, 0.1f, 1000.0f);
                //glm::mat4 projection = glm::ortho(-(1920.0f / 50.0f), 1920.0f / 50.0f, 
                                                    //1080.0f / 50.0f, -(1080.0f / 50.0f), 
                                                    //-1000.0f, 1000.0f);
                for (size_t sI = 0; sI < meshes[i].submeshes.size(); sI++){
                    if(modelMaterials[meshes[i].submeshes[sI].materialIndex].program != 0){
	                    glUseProgram(modelMaterials[meshes[i].submeshes[sI].materialIndex].program);

        	            glUniformMatrix4fv(glGetUniformLocation(modelMaterials[meshes[i].submeshes[sI].materialIndex].program, "projection"), 1,GL_FALSE, glm::value_ptr(projection));
        	            glUniformMatrix4fv(glGetUniformLocation(modelMaterials[meshes[i].submeshes[sI].materialIndex].program, "view"), 1,GL_FALSE, glm::value_ptr(view));

                        const float uniRot = (skyboxRotationVal + 0.11f) * 1636.36363636;
	                    glm::mat4 rotation = glm::mat4(1);
	                    rotation = glm::rotate(rotation, glm::radians(uniRot), glm::vec3(0.0, 1.0, 0.0));
	                    glUniform1f(glGetUniformLocation(modelMaterials[meshes[i].submeshes[sI].materialIndex].program, "lightRotation"), (lightRotation));
	                    glUniformMatrix4fv(glGetUniformLocation(modelMaterials[meshes[i].submeshes[sI].materialIndex].program, "skyboxRotation"), 1,GL_FALSE, glm::value_ptr(rotation));
	                    glUniformMatrix4fv(glGetUniformLocation(modelMaterials[meshes[i].submeshes[sI].materialIndex].program, "modelMatrix"), 1,GL_FALSE, glm::value_ptr(modelMatrix));

                        for (size_t txtI = 0; txtI < modelMaterials[meshes[i].submeshes[sI].materialIndex].textures.size(); txtI++)
                        {
                            glActiveTexture(GL_TEXTURE20 + txtI);
                            glBindTexture(GL_TEXTURE_2D,modelMaterials[meshes[i].submeshes[sI].materialIndex].textures[txtI]);

            	            glUniform1i(glGetUniformLocation(modelMaterials[meshes[i].submeshes[sI].materialIndex].program, ("input_" + std::to_string(txtI)).c_str()), 20+txtI);
                        }
                        glUniform1i(glGetUniformLocation(modelMaterials[meshes[i].submeshes[sI].materialIndex].program, "useUV"), useUV);
                        glUniform1i(glGetUniformLocation(modelMaterials[meshes[i].submeshes[sI].materialIndex].program, "is3D"), 1);
                        glUniform1i(glGetUniformLocation(modelMaterials[meshes[i].submeshes[sI].materialIndex].program, "prefilterMap"), 16);
                        glUniform1i(glGetUniformLocation(modelMaterials[meshes[i].submeshes[sI].materialIndex].program, "brdfLUT"), 15);
                        glUniform1i(glGetUniformLocation(modelMaterials[meshes[i].submeshes[sI].materialIndex].program, "blurySkybox"), 13);
            	        const float uniExpo = (skyboxExposureVal + 0.11f) * 4.54545454545 *2 ;
                        glUniform1f(glGetUniformLocation(modelMaterials[meshes[i].submeshes[sI].materialIndex].program, "skyboxExposure"), uniExpo);
                        glUniform3fv(glGetUniformLocation(modelMaterials[meshes[i].submeshes[sI].materialIndex].program, "viewPos"),1,&viewPos[0]);

                    }
                }

        
                if(meshes[i].submeshes.size() <= 1){
                    meshes[i].Draw();
                }
                else{
                    rndrC++;
                    glUseProgram(materialResultProgram);
                    if(rndrC % 1 == 0){
                        glUniformMatrix4fv(glGetUniformLocation(materialResultProgram, "projection"), 1,GL_FALSE, glm::value_ptr(projection));
        	            glUniformMatrix4fv(glGetUniformLocation(materialResultProgram, "view"), 1,GL_FALSE, glm::value_ptr(view));

                        for (size_t sI = 0; sI < meshes[i].submeshes.size(); sI++)//20 21 22 23 24 25 
                        {
                            glActiveTexture(GL_TEXTURE20+sI);
                            glBindTexture(GL_TEXTURE_2D,materialOutputs[meshes[i].submeshes[sI].materialIndex]);
                            glUniform1i(glGetUniformLocation(materialResultProgram, ("material" + std::to_string(sI)).c_str()), 20+sI);
                        }

                        for (size_t sI = 0; sI < meshes[i].submeshes.size(); sI++)//26 27 28 29 30 31
                        {
                            glActiveTexture(GL_TEXTURE26+sI);
                            glBindTexture(GL_TEXTURE_2D,meshes[i].submeshes[sI].maskTexture);
                            glUniform1i(glGetUniformLocation(materialResultProgram, ("mask" + std::to_string(sI)).c_str()), 26+sI);
                        }

                        glUniform1i(glGetUniformLocation(materialResultProgram, "submeshCount"), meshes[i].submeshes.size());
                    }
                    meshes[i].Draw();
                }
            }
            else if(useOpacity && !renderOnlyTheChosenMesh){
                glActiveTexture(GL_TEXTURE0);
	            if(i != chosenMaterialIndex){
                    glUniform1f(glGetUniformLocation(PBRProgram, "opacity"), 0.3f);
                    meshes[i].Draw();
                }
        		glActiveTexture(GL_TEXTURE28);
            }
        }
	    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	    glBindVertexArray(mVAO);
    }
    void Model::Draw()
    {
        for(unsigned int i = 0; i < meshes.size(); i++){
            if(meshes[i].submeshes.size() <= 1){
                meshes[i].Draw();
            }
        }
	    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	    glBindVertexArray(mVAO);
    }
    void Model::Draw(int i)
    {
        meshes[i].Draw();
	    
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	    glBindVertexArray(mVAO);
    }
    
    void Model::uploadModel(Model uploadedModel){
        meshes.clear();
        
        for (size_t i = 0; i < uploadedModel.meshes.size(); i++)
        {
            meshes.push_back(uploadedModel.meshes[i]);
        }
        
    }
    
    glm::vec3 Model::getMostFarVertex(){
        glm::vec3 mostFarPos = glm::vec3(0);
        for(unsigned int i = 0; i < meshes.size(); i++){
            for (size_t v = 0; v < meshes[i].vertices.size(); v++)
            {
                if(glm::length(meshes[i].vertices[v].Position) > glm::length(mostFarPos)){
                    mostFarPos = meshes[i].vertices[v].Position;
                }
            }
        }
        return mostFarPos;
    }
    
    glm::vec3 Model::getMostFarVector(){
        glm::vec3 mostFarPos = glm::vec3(0);

        for(unsigned int i = 0; i < meshes.size(); i++){
            for (size_t v = 0; v < meshes[i].vertices.size(); v++)
            {
                if(abs(meshes[i].vertices[v].Position.x) > mostFarPos.x){
                    mostFarPos.x = abs(meshes[i].vertices[v].Position.x);
                }
                if(abs(meshes[i].vertices[v].Position.y) > mostFarPos.y){
                    mostFarPos.y = abs(meshes[i].vertices[v].Position.y);
                }
                if(abs(meshes[i].vertices[v].Position.z) > mostFarPos.z){
                    mostFarPos.z = abs(meshes[i].vertices[v].Position.z);
                }
            }
        }
        return glm::vec3(max(max(mostFarPos.x,mostFarPos.y),mostFarPos.z));
    }
    
    void Model::normalizeVertices(){
        for(unsigned int i = 0; i < meshes.size(); i++){
            for (size_t v = 0; v < meshes[i].vertices.size(); v++)
            {
                meshes[i].vertices[v].Position = glm::normalize(meshes[i].vertices[v].Position); 
            }
        }
        return;
    }

    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void Model::loadModel(string const &path,bool triangulate)
    {
        meshes.clear();
        // read file via ASSIMP
        Assimp::Importer importer;

        const aiScene* scene;
        if(triangulate)
            scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals  | aiProcess_CalcTangentSpace );
        else
            scene = importer.ReadFile(path, aiProcess_GenSmoothNormals  | aiProcess_CalcTangentSpace);

        // check for errors
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        // retrieve the directory path of the filepath
        directory = path.substr(0, path.find_last_of('/'));

        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
    }

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void Model::processNode(aiNode *node, const aiScene *scene)
    {
        // process each mesh located at the current node
        for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        
            meshes.push_back(processMesh(mesh, scene));
            SubMeshMs submesh;
            meshes[i].submeshes.push_back(submesh);
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for(unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }

    }

    Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
    {
        // data to fill
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<TextureMs> textures;
        std::string materialName;
        int materialIndex = 0;

        // walk through each of the mesh's vertices
        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // normals
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }
            // TextureMs coordinates
            if(mesh->mTextureCoords[0]) // does the mesh contain TextureMs coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different TextureMs coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple TextureMs coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x; 
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
                // tangent
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
                // bitangent
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);        
        }
        // process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];   

        materialName = material->GetName().C_Str();


        return Mesh(vertices, indices, textures,materialName,materialIndex);
    }

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a TextureMs struct.
    vector<TextureMs> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
    {
        vector<TextureMs> textures;
        for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            // check if TextureMs was loaded before and if so, continue to next iteration: skip loading a new TextureMs
            bool skip = false;
            for(unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // a TextureMs with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if(!skip)
            {   // if TextureMs hasn't been loaded already, load it
                TextureMs TextureMs;
                //TextureMs.id = TextureFromFile(str.C_Str(), this->directory);
                TextureMs.type = typeName;
                TextureMs.path = str.C_Str();
                textures.push_back(TextureMs);
                textures_loaded.push_back(TextureMs);  // store it as TextureMs loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            }
        }
        return textures;
    }


// unsigned int TextureFromFile(const char *path, const string &directory, bool gamma)
// {
//     string filename = string(path);
//     filename = directory + '/' + filename;

//     unsigned int textureID;
//     glGenTextures(1, &textureID);

//     int width, height, nrComponents;
//     unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
//     if (data)
//     {
//         GLenum format;
//         if (nrComponents == 1)
//             format = GL_RED;
//         else if (nrComponents == 3)
//             format = GL_RGB;
//         else if (nrComponents == 4)
//             format = GL_RGBA;

//         glBindTexture(GL_TEXTURE_2D, textureID);
//         glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
//         glGenerateMipmap(GL_TEXTURE_2D);

//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//         stbi_image_free(data);
//     }
//     else
//     {
//         std::cout << "TextureMs failed to load at path: " << path << std::endl;
//         stbi_image_free(data);
//     }

//     return textureID;
// }