#ifndef LGDSKYBOX_HPP
#define LGDSKYBOX_HPP


#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Mesh.hpp"
#include "Shader.hpp"

#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "../../thirdparty/stb_image.h"

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../thirdparty/stb_image_write.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>


using namespace std;

class Skybox{
public:
    unsigned int ID;

    unsigned int VBO;
    unsigned int VAO;

    Skybox(){}
    
    void init(){
        //Couldn't use the constructor since the glad is not initialized before definition
        
        std::vector<float> skyboxVertices = { //Array size is 108
	        -1.0f,  1.0f, -1.0f,
	        -1.0f, -1.0f, -1.0f,
	         1.0f, -1.0f, -1.0f,
	         1.0f, -1.0f, -1.0f,
	         1.0f,  1.0f, -1.0f,
	        -1.0f,  1.0f, -1.0f,
	        -1.0f, -1.0f,  1.0f,
	        -1.0f, -1.0f, -1.0f,
	        -1.0f,  1.0f, -1.0f,
	        -1.0f,  1.0f, -1.0f,
	        -1.0f,  1.0f,  1.0f,
	        -1.0f, -1.0f,  1.0f,
	         1.0f, -1.0f, -1.0f,
	         1.0f, -1.0f,  1.0f,
	         1.0f,  1.0f,  1.0f,
	         1.0f,  1.0f,  1.0f,
	         1.0f,  1.0f, -1.0f,
	         1.0f, -1.0f, -1.0f,
	        -1.0f, -1.0f,  1.0f,
	        -1.0f,  1.0f,  1.0f,
	         1.0f,  1.0f,  1.0f,
	         1.0f,  1.0f,  1.0f,
	         1.0f, -1.0f,  1.0f,
	        -1.0f, -1.0f,  1.0f,
	        -1.0f,  1.0f, -1.0f,
	         1.0f,  1.0f, -1.0f,
	         1.0f,  1.0f,  1.0f,
	         1.0f,  1.0f,  1.0f,
	        -1.0f,  1.0f,  1.0f,
	        -1.0f,  1.0f, -1.0f,
	        -1.0f, -1.0f, -1.0f,
	        -1.0f, -1.0f,  1.0f,
	         1.0f, -1.0f, -1.0f,
	         1.0f, -1.0f, -1.0f,
	        -1.0f, -1.0f,  1.0f,
	         1.0f, -1.0f,  1.0f
	    };

        //Generate the cubemap texture
	    glGenTextures(1,&ID);

        //Generate vertex objects
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        
        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(VAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(float), &skyboxVertices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0); 

        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        glBindVertexArray(0);

    }

    void load(std::string path) //Path to the folder
	{
		std::vector<std::string> faces //Each face of the skybox 
		{
		    path + "/px.png", 
		    path + "/nx.png",
		    path + "/py.png",
		    path + "/ny.png",
		    path + "/pz.png",
		    path + "/nz.png"
		};

	    for (unsigned int i = 0; i < faces.size(); i++)
	    {
	        int width, height, nrChannels;//Of the face of the skybox

            //Cube's faces are determined that way cause macOS having trouble indexing cubemap's faces.
            //That doesn't solve the problem tho
		    unsigned int cubePos;
			if(i == 0)
				cubePos = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
			if(i == 1)
				cubePos = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
			if(i == 2)
				cubePos = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
			if(i == 3)
				cubePos = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
			if(i == 4)
				cubePos = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
			if(i == 5)
				cubePos = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;


	        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0); //Load the face

	        if (data) //Loaded
	        {
                glActiveTexture(GL_TEXTURE0);
	            glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	            
                glTexImage2D(cubePos, 
	                         0, 
                             GL_RGB8, 
                             width, 
                             height,
                             0, 
                             GL_RGB, 
                             GL_UNSIGNED_BYTE, 
                             data
	            );
	            
                std::cout << "Loaded " << faces[i] << std::endl;
                stbi_image_free(data);
	        }
	        else //An error occured while loading
	        {
	            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
	            stbi_image_free(data);
	        }
	    }
	}

    void draw(){ //Draw the skybox
        //Bind the skybox texture to the texture slot 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP,ID);
        
        //Disable the depth testing
        glDisable(GL_DEPTH_TEST);
	    glDepthMask(GL_FALSE);
	    glDepthFunc(GL_LEQUAL);

        //Bind vertex objects
	    glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        //Draw the skybox
        glDrawArrays(GL_TRIANGLES, 0, 108);

        //Bind default buffers
        glBindBuffer(GL_ARRAY_BUFFER, 0); 
        glBindVertexArray(0);
	
        //Enable the depth testing
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
	    glDepthMask(GL_TRUE);
    }
};

#endif