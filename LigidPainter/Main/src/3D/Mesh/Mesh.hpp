/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef THREED_MESH_HPP
#define THREED_MESH_HPP

#include "UTIL/Util.hpp"
#include "Toolkit/Layers/Layers.hpp"
#include "3D/ThreeDPoint/ThreeDPoint.hpp"

struct MaterialIDColor{
    glm::vec3 color;
    Texture grayScaleTxtr;
};

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

    bool operator!=(const Vertex& other) const
    {
        return !(*this == other);
    }
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

class Mesh {
public:
    struct FaceSelection{
        /// @brief Painting the selected mesh mode activated flag 
        bool activated = false;
        /// @brief Selecting faces mode flag 
        bool editMode = false;
        /// @brief 0 : circle | 1 : box  
        int selectionModeIndex = 0;
        /// @brief radius value of the circle selection mode
        int radius = 10;
        /*! @brief Unselected faces will not be rendered in the scene */
        bool hideUnselected = false;

        glm::vec2 boxSelectionStart = glm::vec2(0.f);
        glm::vec2 boxSelectionEnd = glm::vec2(0.f);

        /// @brief Contains the indices of the selected faces
        ///        Being processed in the PBR shader
        std::vector<byte> selectedPrimitiveIDs;
        std::vector<int> changedIndices;

        /*! @brief This Texture is used for masking the mesh using a texture and not primitive IDs*/
        Texture meshMask;

        /*! @brief Contains selected face data to be intercepted in shaders
                    Resolution : x = sqrt(mesh's indices size), y = sqrt(mesh's indices size)      
                    If a primitive is selected corresponding pixel is set to white
        */
        Texture selectedFaces;

        /// @brief 3D model rendered with primitive ID rendering shader
        Texture modelPrimitives;
    };

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

    FaceSelection face_selection_data;

    LayerScene layerScene;
    
    Texture uvMask;

    Texture displayingTxtr;

    Texture meshPosTxtr;
    Texture meshNormalTxtr;

    Texture objectIDs;
    void updateObjectIDsTexture();
    void updateModelPrimitivesTexture();


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
    void Draw();

    unsigned int VBO, EBO, VAO; //Vertex buffer object, element buffer object

    void processHeightMap();

    void generateDisplayingTexture();

    glm::vec3 getCenterPosition();

    glm::vec3 getMostDistantPointFromAPoint(glm::vec3 point);

    float getBiggestFloatValue(glm::vec3 origin);

    void getPosNormalOverPoint(glm::vec2 pointPos, float*& posData, float*& normalData, bool readNormal);
    /*! @brief Returns the 3D point value of the model on the cursor
               Returns the pos & normal value -1000 if the point is not valid
    */
    ThreeDPoint getCurrentPosNormalDataOverCursor();
    void updatePosNormalTexture();    

    void update_vertex_buffers();

private:
    /*! @brief initializes all the buffer objects/arrays */
    void setupMesh();

    void generateUVMask();
};

#endif