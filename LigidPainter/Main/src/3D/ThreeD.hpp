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
	unsigned int createPrefilterMap(glm::vec2 videoScale);
	
    /*!
        @brief Creates the @ref displayingTexture. Renders a sphere using skyboxBall 
                shader and captures it into the displayingTexture.
        
        @param sphereModel The skybox ball 3D Model
        @param windowScale Resolution of the main window
    */
    void createDisplayingTxtr(Model &sphereModel, glm::vec2 windowScale);

private:
    /*! @brief Skybox vertex buffer object ID*/
    unsigned int VBO;
    
    /*! @brief Skybox vertex array object ID*/
    unsigned int VAO;
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

    unsigned int uvMask;

    /*! @brief Vertices of the mesh */
    std::vector<Vertex>       vertices;

    /*! @brief Indices of the mesh */
    std::vector<unsigned int> indices;
    
    /*! @brief Title of the mesh */
    std::string materialName;
    
    /*! Default constructor */
    Mesh(){}
    
    /*! @brief Generates the buffer objects using the first two parameters */
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::string materialName);
    
    /*! @brief Renders the mesh*/
    void Draw();

    unsigned int VBO, EBO, VAO; //Vertex buffer object, element buffer object

private:
    /*! @brief initializes all the buffer objects/arrays */
    void setupMesh();

    void generateUVMask();
};


class Model{
public:
    /*! @brief Meshes of the model */
    std::vector<Mesh>    meshes;

    /*! @brief Title of the model*/    
    std::string title;

    /*! 
        @brief Is true in the same frame a new model is imported.
                And is set to false back in the renderer.render  
    */
    bool newModelAdded = false; 

    //Public member functions
    
    void Draw();
    void exportModel(std::string path);
    void loadModel(std::string const &path,bool triangulate);
};

#endif