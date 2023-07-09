/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef THREED_HPP
#define THREED_HPP


#include "UTIL/Util.hpp"

#include "3D/Material/Material.hpp"

//Front decleration
struct Section;

struct Vertex {
    // position
    glm::vec3 Position;
    // texCoords
    glm::vec2 TexCoords;
    // normal
    glm::vec3 Normal;
    // tangent
    glm::vec3 Tangent = glm::vec3(0);
    // bitangent
    glm::vec3 Bitangent = glm::vec3(0);
};


class Skybox{
public:
    unsigned int ID;
    unsigned int IDPrefiltered = 0;
    unsigned int VBO;
    unsigned int VAO;
	unsigned int displayingTexture = 0;
	glm::mat4 transformMatrix = glm::mat4(1);//Used to rotate the skybox
	float lod = 0.; //Used to blur the skybox
	glm::vec3 bgColor = glm::vec3(0.1);
	float opacity = 0.f;

    //Constructor
    Skybox();

    //Public member functions    
    void init();
    void load(std::string path);
    void draw(bool bindTxtr);
	unsigned int createPrefilterMap(Shader prefilteringShader,glm::vec2 videoScale);
	void createDisplayingTxtr(Shader skyboxBall,Model &sphereModel,glm::vec2 windowScale);
};

class Mesh {
public:
    // mesh Data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO;
    std::string materialName;
    int materialIndex;
    float modelMaterialButtonMixVal = 0.f;

    //Default constructor
    Mesh(){}
    
    //Constructor
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::string materialName,int materialIndex);

    void Draw();

private:
    // render data 
    unsigned int VBO, EBO;
    unsigned int oVBO, oVAO;

    // initializes all the buffer objects/arrays
    void setupMesh();
};



class Model{
private:

    std::string directory;
public:
    // model data 
    std::vector<Mesh>    meshes;
    std::string filePath;
    std::string title;
    unsigned int mVAO;
    unsigned int mVBO;

    bool newModelAdded = false; //Returns true in the same frame a new model is imported

    //Public member functions
    void Draw();
    void exportModel(std::string path);
    void loadModel(std::string const &path,bool triangulate);
};

#endif