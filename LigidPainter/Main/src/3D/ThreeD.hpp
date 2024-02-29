/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef THREED_HPP
#define THREED_HPP


#include "UTIL/Util.hpp"

#include "3D/Material/Material.hpp"

//Forward decleration
struct Section;

struct Vertex {
    // position
    glm::vec3 Position = glm::vec3(0.f);
    // texCoords
    glm::vec2 TexCoords = glm::vec2(0.f);
    // normal
    glm::vec3 Normal = glm::vec3(0.f);
    // tangent
    glm::vec3 Tangent = glm::vec3(0);
    // bitangent
    glm::vec3 Bitangent = glm::vec3(0);

    Vertex(){}
    Vertex(glm::vec3 Position, glm::vec2 TexCoords, glm::vec3 Normal, glm::vec3 Tangent, glm::vec3 Bitangent){
        this->Position = Position;
        this->TexCoords = TexCoords;
        this->Normal = Normal;
        this->Tangent = Tangent;
        this->Bitangent = Bitangent;
    }

    bool operator==(const Vertex& other) const
    {
        return Position == other.Position && TexCoords == other.TexCoords && Normal == other.Normal;
    }
};


class Skybox{
public:
    /*! @brief Skybox CubeMap OpenGL texture buffer object ID */
    unsigned int ID;

    /*! @brief Prefiltered Skybox CubeMap OpenGL texture buffer object ID */
    unsigned int IDPrefiltered = 0;
    
    /*! @brief Displaying texture OpenGL texture buffer object ID (Captures a skybox ball)*/
	unsigned int displayingTexture = 0;
	
    /*! @brief Used to rotate the skybox */
    glm::mat4 transformMatrix = glm::mat4(1);
	
    /*! @brief Used to blur the skybox */
    float lod = 0.;
    
    /*! @brief What color replaces the skybox */
	glm::vec3 bgColor = glm::vec3(0.1);
	
    /*! @brief Mixes the bgColor value and the skybox color value */
    float opacity = 0.f;

    /*! @brief Default constructor*/
    Skybox();

    /*! @brief Init @ref VAO & @ref VBO */
    void init();

    /*! 
        @brief Load skybox cubemap texture 
        @param path Path to the folder holds the faces of the each cubemap

        The folder must contain these images : 
        px.png 
		nx.png
		ny.png
		py.png
		pz.png
		nz.png
    */
    void load(std::string path);

    /*!
        @brief Renders the skybox
        @param bindTxtr binds the prefiltered texture before rendering if true.
                        Is set to false when rendering while prefiltering 
                        the skybox texture ( @ref ID )
    */
    void draw(bool bindTxtr);
    
    /*!
        @brief Creates the @ref IDPrefiltered. (Prefilters the skybox texture ( @ref ID ))
        @param videoScale Primary monitor resolution value
    */
	unsigned int createPrefilterMap();
	
    /*!
        @brief Creates the @ref displayingTexture. Renders a sphere using skyboxBall 
                shader and captures it into the displayingTexture.
    */
    void createDisplayingTxtr();

private:
    /*! @brief Skybox vertex buffer object ID*/
    unsigned int VBO;
    
    /*! @brief Skybox vertex array object ID*/
    unsigned int VAO;
};

struct MeshObject{
    std::string title;
    /// @brief start & end
    glm::ivec2 vertIndices; 

    MeshObject(){}
    MeshObject(std::string title, glm::ivec2 vertIndices){
        this->title = title; 
        this->vertIndices = vertIndices; 
    }
};

struct MaterialIDColor{
    glm::vec3 color;
    Texture grayScaleTxtr;
};

class Mesh {
public:
    
    /*----- Channels of the material -----

        Only manipulated by the material 
          modifiers and no other stuff
       (results of the material modifiers)
      (don't try to equate to another txtr) 
    
    */
    Texture albedo;
    Texture roughness;
    Texture metallic; 
    Texture normalMap;
    Texture heightMap;
    Texture ambientOcclusion;
    Texture materialIDTxtr;
    std::vector<MaterialIDColor> materialIDColors;
    std::string materialIDTxtrPath;

    Texture uvMask;

    Texture displayingTxtr;

    std::vector<int> selectedObjectIndices;
    Texture selectedObjectPrimitivesTxtr;

    std::vector<MeshObject> objects;

    /*! @brief Vertices of the mesh */
    std::vector<Vertex>       vertices;

    /*! @brief Indices of the mesh */
    std::vector<unsigned int> indices;
    
    /*! @brief Title of the mesh */
    std::string materialName;
    
    /*! Default constructor */
    Mesh(){}
    
    /*! @brief Generates the buffer objects using the first two parameters */
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::string materialName, bool initTxtrs);
    
    /*! @brief Renders the mesh*/
    void Draw(bool displayWireframe);

    unsigned int VBO, EBO, VAO; //Vertex buffer object, element buffer object

    void processHeightMap();

    void generateDisplayingTexture();

    glm::vec3 getCenterPosition();

    glm::vec3 getMostDistantPointFromAPoint(glm::vec3 point);

    float getBiggestFloatValue(glm::vec3 origin);

private:
    /*! @brief initializes all the buffer objects/arrays */
    void setupMesh();

    void generateUVMask();

};

#include <assimp/scene.h>

class Model{
public:
    /*! @brief Meshes of the model */
    std::vector<Mesh>    meshes;

    /*! @brief Title of the model*/    
    std::string title;
    
    /*! @brief Path of the model*/    
    std::string path;

    /*! 
        @brief Is true in the same frame a new model is imported.
                And is set to false back in the renderer.render  
    */
    bool newModelAdded = false;

    Texture displayingTxtr; 
    
    /*! @brief 3D Model rendered with different color values for each object
            Created in the updateObjectIDsTexture function 
            Params : GL_NEAREST, GL_RED, GL_R32F
    */
    Texture objectIDs;
    
    void Draw();
    void exportModel(std::string path);
    bool loadModel(std::string const &path, bool triangulate, bool initTxtrs);
    void updateObjectIDsTexture();

    /*! 
        @brief Call this function as the mouse is clicked
               Detects the selected object (based on the cursor position)
               and applies it's index to the @ref selectedObjectIndices array
    */
    void selectObject(Panel& objectsPanel);
};

class SceneTiles
{
public:
    unsigned int VBO;
    unsigned int VAO;

    /// @brief Default constructor (does nothing) (use @ref init instead to init the OpenGL objects)
    SceneTiles();

    /// @brief Initializes / Creates OpenGL vertex objects regarding 2D square rendering
    void init();

    /// @brief Binds the 2D square vertex objects
    void bindBuffers();
    
    /// @brief Unbinds the 2D square vertex objects
    void unbindBuffers();
    
    /// @brief Draw the square (is not used) 
    void draw();
};

class SceneAxisDisplayer
{
public:
    unsigned int VBO;
    unsigned int VAO;

    /// @brief Default constructor (does nothing) (use @ref init instead to init the OpenGL objects)
    SceneAxisDisplayer();

    /// @brief Initializes / Creates OpenGL vertex objects regarding 2D square rendering
    void init();

    /// @brief Binds the 2D square vertex objects
    void bindBuffers();
    
    /// @brief Unbinds the 2D square vertex objects
    void unbindBuffers();
    
    /// @brief Draw the square (is not used) 
    void draw();
};

class ThreeDBox
{
public:
    unsigned int VBO = 0;
    unsigned int VAO = 0;
    unsigned int EBO = 0;

    glm::vec3 pos_topLeft;
    glm::vec3 pos_topRight;
    glm::vec3 pos_bottomLeft;
    glm::vec3 pos_bottomRight;
    glm::vec3 normal;

    std::vector<Vertex> boxVertices;
    std::vector<unsigned int> boxIndices;

    bool flipX = false;
    bool flipY = false;

    ThreeDBox();

    void init(glm::vec3 pos_topLeft, glm::vec3 pos_topRight, glm::vec3 pos_bottomLeft, glm::vec3 pos_bottomRight, glm::vec3 normal, Painter& painter);
    void update(glm::vec3 pos_topLeft, glm::vec3 pos_topRight, glm::vec3 pos_bottomLeft, glm::vec3 pos_bottomRight, glm::vec3 normal, Painter& painter);
    void projectToModel(std::vector<Vertex>& vertices, glm::vec3 center, Painter& painter);

    void updateDetailed(
                            glm::vec3 pos_topLeft, 
                            glm::vec3 pos_topRight, 
                            glm::vec3 pos_bottomLeft, 
                            glm::vec3 pos_bottomRight, 
                            glm::vec3 normal,
                            
                            glm::vec3 detailed_threeDPoint_r1_c2, int detailed_threeDPoint_r1_c2_index, 
                            glm::vec3 detailed_threeDPoint_r1_c3, int detailed_threeDPoint_r1_c3_index, 
                            glm::vec3 detailed_threeDPoint_r1_c4, int detailed_threeDPoint_r1_c4_index, 
                            glm::vec3 detailed_threeDPoint_r2_c1, int detailed_threeDPoint_r2_c1_index, 
                            glm::vec3 detailed_threeDPoint_r2_c2, int detailed_threeDPoint_r2_c2_index, 
                            glm::vec3 detailed_threeDPoint_r2_c3, int detailed_threeDPoint_r2_c3_index, 
                            glm::vec3 detailed_threeDPoint_r2_c4, int detailed_threeDPoint_r2_c4_index, 
                            glm::vec3 detailed_threeDPoint_r2_c5, int detailed_threeDPoint_r2_c5_index, 
                            glm::vec3 detailed_threeDPoint_r3_c1, int detailed_threeDPoint_r3_c1_index, 
                            glm::vec3 detailed_threeDPoint_r3_c2, int detailed_threeDPoint_r3_c2_index, 
                            glm::vec3 detailed_threeDPoint_r3_c3, int detailed_threeDPoint_r3_c3_index, 
                            glm::vec3 detailed_threeDPoint_r3_c4, int detailed_threeDPoint_r3_c4_index, 
                            glm::vec3 detailed_threeDPoint_r3_c5, int detailed_threeDPoint_r3_c5_index, 
                            glm::vec3 detailed_threeDPoint_r4_c1, int detailed_threeDPoint_r4_c1_index, 
                            glm::vec3 detailed_threeDPoint_r4_c2, int detailed_threeDPoint_r4_c2_index, 
                            glm::vec3 detailed_threeDPoint_r4_c3, int detailed_threeDPoint_r4_c3_index, 
                            glm::vec3 detailed_threeDPoint_r4_c4, int detailed_threeDPoint_r4_c4_index, 
                            glm::vec3 detailed_threeDPoint_r4_c5, int detailed_threeDPoint_r4_c5_index, 
                            glm::vec3 detailed_threeDPoint_r5_c2, int detailed_threeDPoint_r5_c2_index, 
                            glm::vec3 detailed_threeDPoint_r5_c3, int detailed_threeDPoint_r5_c3_index, 
                            glm::vec3 detailed_threeDPoint_r5_c4, int detailed_threeDPoint_r5_c4_index,
                            Painter& painter
                        );
    
    void getDetailedVertices(
                                glm::vec3* detailed_threeDPoint_r1_c2, int* detailed_threeDPoint_r1_c2_index, 
                                glm::vec3* detailed_threeDPoint_r1_c3, int* detailed_threeDPoint_r1_c3_index, 
                                glm::vec3* detailed_threeDPoint_r1_c4, int* detailed_threeDPoint_r1_c4_index, 
                                glm::vec3* detailed_threeDPoint_r2_c1, int* detailed_threeDPoint_r2_c1_index, 
                                glm::vec3* detailed_threeDPoint_r2_c2, int* detailed_threeDPoint_r2_c2_index, 
                                glm::vec3* detailed_threeDPoint_r2_c3, int* detailed_threeDPoint_r2_c3_index, 
                                glm::vec3* detailed_threeDPoint_r2_c4, int* detailed_threeDPoint_r2_c4_index, 
                                glm::vec3* detailed_threeDPoint_r2_c5, int* detailed_threeDPoint_r2_c5_index, 
                                glm::vec3* detailed_threeDPoint_r3_c1, int* detailed_threeDPoint_r3_c1_index, 
                                glm::vec3* detailed_threeDPoint_r3_c2, int* detailed_threeDPoint_r3_c2_index, 
                                glm::vec3* detailed_threeDPoint_r3_c3, int* detailed_threeDPoint_r3_c3_index, 
                                glm::vec3* detailed_threeDPoint_r3_c4, int* detailed_threeDPoint_r3_c4_index, 
                                glm::vec3* detailed_threeDPoint_r3_c5, int* detailed_threeDPoint_r3_c5_index, 
                                glm::vec3* detailed_threeDPoint_r4_c1, int* detailed_threeDPoint_r4_c1_index, 
                                glm::vec3* detailed_threeDPoint_r4_c2, int* detailed_threeDPoint_r4_c2_index, 
                                glm::vec3* detailed_threeDPoint_r4_c3, int* detailed_threeDPoint_r4_c3_index, 
                                glm::vec3* detailed_threeDPoint_r4_c4, int* detailed_threeDPoint_r4_c4_index, 
                                glm::vec3* detailed_threeDPoint_r4_c5, int* detailed_threeDPoint_r4_c5_index, 
                                glm::vec3* detailed_threeDPoint_r5_c2, int* detailed_threeDPoint_r5_c2_index, 
                                glm::vec3* detailed_threeDPoint_r5_c3, int* detailed_threeDPoint_r5_c3_index, 
                                glm::vec3* detailed_threeDPoint_r5_c4, int* detailed_threeDPoint_r5_c4_index,
                                Painter& painter
                            );

    void draw();
};

#endif