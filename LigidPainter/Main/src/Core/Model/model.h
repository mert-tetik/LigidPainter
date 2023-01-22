
#ifndef LGDMODEL_H
#define LGDMODEL_H

#include <vector>
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "mesh.h"

using namespace std;

class Model 
{
public:
    // model data 
    std::vector<TextureMs> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Mesh>    meshes;
    std::string directory;


    // draws the model, and thus all its meshes
    void Draw(unsigned int chosenMaterialIndex,unsigned int PBRProgram,bool useOpacity,std::vector<unsigned int> &modelMaterialPrograms,glm::mat4 view);
    
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(std::string const &path,bool triangulate);

private:

    void processNode(aiNode *node, const aiScene *scene);

    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
   
    std::vector<TextureMs> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};
#endif