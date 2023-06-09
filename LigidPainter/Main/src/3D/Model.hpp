/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

Model.hpp : Model class stores a 3D Model's data

*/

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "assimp/Exporter.hpp"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Mesh.hpp"
#include "Shader.hpp"
#include "Util.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <map>
#include <vector>
#include <cstdlib>

using namespace std;

class Model{
private:
    #if defined(_WIN32) || defined(_WIN64)
		    char folderDistinguisher = '\\';
	#else
			char folderDistinguisher = '/'; 
	#endif

    string directory;
public:
    // model data 
    vector<TextureMs> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh>    meshes;
    string filePath;
    string title;

    unsigned int mVAO;
    unsigned int mVBO;

    void Draw()
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        for(unsigned int i = 0; i < meshes.size(); i++){
            if(meshes[i].submeshes.size() <= 1){
                meshes[i].Draw();
            }
        }
	    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	    glBindVertexArray(mVAO);
    }

    void exportModel(std::string ){
        // Create an instance of the Assimp Exporter
        Assimp::Exporter exporter;

        // Set the export format to FBX
        const aiExportFormatDesc* exportFormat = exporter.GetExportFormatDescription(1);
        
        if (!exportFormat) {
            // FBX format is not available, handle the error
            return;
        }

        // Define the export file name
        std::string exportFileName = "output_file.fbx";

        aiScene* scene = createAssimpScene();

        // Export the scene to FBX format
        exporter.Export(scene, exportFileName, exportFormat->id, aiProcess_Triangulate);
    }

    aiScene* createAssimpScene() {
    // Create an instance of aiScene
    aiScene* scene = new aiScene();

    // Create an array of aiMesh pointers
    scene->mMeshes = new aiMesh*[meshes.size()];
    scene->mNumMeshes = meshes.size();

    // Convert each Mesh to Assimp-compatible structures
    for (unsigned int i = 0; i < meshes.size(); ++i) {
        const Mesh& meshData = meshes[i];

        // Create an instance of aiMesh
        aiMesh* mesh = new aiMesh();
        scene->mMeshes[i] = mesh;

        // Convert Vertex data to Assimp-compatible structures
        std::vector<aiVector3D> positions;
        std::vector<aiVector2D> texCoords;
        std::vector<aiVector3D> normals;
        std::vector<aiVector3D> tangents;
        std::vector<aiVector3D> bitangents;
        std::vector<unsigned int> boneIDs;
        std::vector<float> weights;

        for (const auto& vertex : meshData.vertices) {
            positions.push_back(aiVector3D(vertex.Position.x, vertex.Position.y, vertex.Position.z));
            texCoords.push_back(aiVector2D(vertex.TexCoords.x, vertex.TexCoords.y));
            normals.push_back(aiVector3D(vertex.Normal.x, vertex.Normal.y, vertex.Normal.z));
            tangents.push_back(aiVector3D(vertex.Tangent.x, vertex.Tangent.y, vertex.Tangent.z));
            bitangents.push_back(aiVector3D(vertex.Bitangent.x, vertex.Bitangent.y, vertex.Bitangent.z));

            for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
                boneIDs.push_back(vertex.m_BoneIDs[i]);
                weights.push_back(vertex.m_Weights[i]);
            }
        }

        // Set the vertex positions in the mesh
        mesh->mVertices = new aiVector3D[positions.size()];
        std::copy(positions.begin(), positions.end(), mesh->mVertices);
        mesh->mNumVertices = positions.size();

        // Set the texture coordinates in the mesh
        mesh->mTextureCoords[0] = new aiVector3D[texCoords.size()];
        std::copy(texCoords.begin(), texCoords.end(), mesh->mTextureCoords[0]);

        // Set the normals in the mesh
        mesh->mNormals = new aiVector3D[normals.size()];
        std::copy(normals.begin(), normals.end(), mesh->mNormals);

        // Set the tangents in the mesh
        mesh->mTangents = new aiVector3D[tangents.size()];
        std::copy(tangents.begin(), tangents.end(), mesh->mTangents);

        // Set the bitangents in the mesh
        mesh->mBitangents = new aiVector3D[bitangents.size()];
        std::copy(bitangents.begin(), bitangents.end(), mesh->mBitangents);

        // Set the bone IDs and weights in the mesh
        mesh->mBones = new aiBone*[1];
        mesh->mBones[0] = new aiBone();
        mesh->mBones[0]->mNumWeights = weights.size();
        mesh->mBones[0]->mWeights = new aiVertexWeight[weights.size()];

        for (unsigned int j = 0; j < weights.size(); ++j) {
            mesh->mBones[0]->mWeights[j].mVertexId = j;
            mesh->mBones[0]->mWeights[j].mWeight = weights[j];
        }
    }

    return scene;
}

    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const &path,bool triangulate)
    {
        Util util;
        filePath = path;
        title = util.getLastWordBySeparatingWithChar(filePath,folderDistinguisher);
        title = util.removeExtension(title);
        
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
    void processNode(aiNode *node, const aiScene *scene)
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

    Mesh processMesh(aiMesh *mesh, const aiScene *scene)
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
    vector<TextureMs> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
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
};