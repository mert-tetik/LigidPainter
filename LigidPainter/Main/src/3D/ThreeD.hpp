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

#ifndef MAX_BONE_INFLUENCE
#define MAX_BONE_INFLUENCE 4
#endif

#include "assimp/Exporter.hpp"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "UTIL/Util.hpp"

struct Section;

struct Vertex {
    // position
    glm::vec3 Position;
    // texCoords
    glm::vec2 TexCoords;
    // normal
    glm::vec3 Normal;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
	//bone indexes which will influence this vertex
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	//weights from each bone
	float m_Weights[MAX_BONE_INFLUENCE];
};

struct TextureMs {
    unsigned int id;
    std::string type;
    std::string path;
};

struct SubMeshMs{
    unsigned int maskTexture = 0;
    bool textureSelectionState = false;
    int materialIndex = 0;
    float modelMaterialButtonMixVal = 0.f;
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
    std::vector<SubMeshMs> submeshes;
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<TextureMs>      textures;
    unsigned int VAO;
    std::string materialName;
    int materialIndex;
    float modelMaterialButtonMixVal = 0.f;

    //Constructor
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<TextureMs> textures,std::string materialName,int materialIndex);

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
    #if defined(_WIN32) || defined(_WIN64)
		    char folderDistinguisher = '\\';
	#else
			char folderDistinguisher = '/'; 
	#endif

    std::string directory;
public:
    // model data 
    std::vector<TextureMs> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Mesh>    meshes;
    std::string filePath;
    std::string title;
    unsigned int mVAO;
    unsigned int mVBO;

    bool newModelAdded = false; //Returns true in the same frame a new model is imported

    //Public member functions
    void Draw();
    void exportModel(std::string path);
    aiScene* createAssimpScene();
    void loadModel(std::string const &path,bool triangulate);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<TextureMs> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};

//-------------- MATERIAL MODIFIER ------------

#define TEXTURE_MATERIAL_MODIFIER 0

class MaterialModifier
{
public:
    //0 = albedo
    //1 = roughness
    //2 = metallic 
    //3 = normal map
    //4 = height map
    //5 = ambient Occlusion

    Shader shader;  
    int modifierIndex;
    std::vector<Section> sections;   
    std::string title;

    //Constructors
    MaterialModifier();
    MaterialModifier(ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures,int modifierIndex);

    //Public member functions
    std::vector<Section> createTextureModifier(ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures);
};


struct AppMaterialModifiers{
    MaterialModifier textureModifier;
};



class Material 
{
private:
    #if defined(_WIN32) || defined(_WIN64)
		    char folderDistinguisher = '\\';
	#else
			char folderDistinguisher = '/'; 
	#endif

    void initTexture(Texture &txtr,int textureRes);
public:
    std::string title;

    Texture albedo;
    Texture roughness;
    Texture metallic; 
    Texture normalMap;
    Texture heightMap;
    Texture ambientOcclusion;
    unsigned int displayingTexture;
    int ID; //Unique ID
    std::vector<MaterialModifier> materialModifiers;

    //Constructors
    Material();
    Material(int textureRes,std::string title,int ID);

    //Public member functions
    void writeFile(std::string path);
    void readFile(std::string path,ColorPalette colorPalette ,Shader buttonShader ,AppTextures appTextures);
};

#endif //THREED_HPP