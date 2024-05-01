/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef THREED_SKYBOX_HPP
#define THREED_SKYBOX_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <string>

class Skybox{
public:
    /*! @brief Skybox CubeMap OpenGL texture buffer object ID */
    unsigned int ID;

    /*! @brief Prefiltered Skybox CubeMap OpenGL texture buffer object ID */
    unsigned int IDPrefiltered = 0;
    
    /*! @brief Displaying texture OpenGL texture buffer object ID (Captures a skybox ball)*/
	Texture displayingTexture;
	
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
    */
    void draw();
    
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

#endif