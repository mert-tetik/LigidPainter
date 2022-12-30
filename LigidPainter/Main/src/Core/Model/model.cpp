
#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Core/Model/mesh.h"
#include "Core/Model/model.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std;


    // model data 
    vector<TextureMs> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh>    meshes;
    string directory;


    // draws the model, and thus all its meshes
    void Model::Draw(unsigned int chosenMaterialIndex,unsigned int PBRProgram,bool useOpacity,vector<unsigned int> albedoTextures)
    {
        if(meshes.size() > 0){
            glUniform1f(glGetUniformLocation(PBRProgram, "opacity"), 1.0f);
            meshes[chosenMaterialIndex].Draw(); 
        }

        for(unsigned int i = 0; i < meshes.size(); i++){
            
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D,albedoTextures[i]);

            if(useOpacity && chosenMaterialIndex != i){
	            glUniform1f(glGetUniformLocation(PBRProgram, "opacity"), 0.1f);
                meshes[i].Draw();
            }            
        }
        if(meshes.size() > 0)
            glBindTexture(GL_TEXTURE_2D,albedoTextures[chosenMaterialIndex]);
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

        //cout << "Materials are : " << material->GetName().C_Str() << '\n'; 
        
        // we assume a convention for sampler names in the shaders. Each diffuse TextureMs should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
        // Same applies to other TextureMs as the following list summarizes:
        // diffuse: texture_diffuseN
        // specular: texture_specularN
        // normal: texture_normalN

        // // 1. diffuse maps
        // vector<TextureMs> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        // textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // // 2. specular maps
        // vector<TextureMs> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        // textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // // 3. normal maps
        // std::vector<TextureMs> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        // textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // // 4. height maps
        // std::vector<TextureMs> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        //textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        
        // return a mesh object created from the extracted mesh data
        materialName = material->GetName().C_Str();


        return Mesh(vertices, indices, textures,materialName);
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