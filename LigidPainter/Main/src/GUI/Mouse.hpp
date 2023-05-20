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

#ifndef LGDMOUSE_HPP
#define LGDMOUSE_HPP

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Shader.hpp"
#include "Box.hpp"
#include "Renderer.h"
#include "Util.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

class Mouse
{
private:
    /* data */
public:
    glm::vec2 cursorPos;
    GLFWwindow* window;

    Mouse(){}

    Mouse(GLFWwindow* window){
        this->window = window;
    }

    bool isMouseHover(glm::vec2 scale, glm::vec2 position){ //In screen coordinates
        std::vector<float> buttonCoor{
	    	// first triangle
	    	 scale.x + position.x,  scale.y + position.y, 1,1,0,0,0,0,  // top right
	    	 scale.x + position.x, -scale.y + position.y, 1,0,0,0,0,0,  // bottom right
	    	-scale.x + position.x,  scale.y + position.y, 0,1,0,0,0,0,  // top left 
	    	// second triangle
	    	 scale.x + position.x, -scale.y + position.y, 1,0,0,0,0,0,  // bottom right
	    	-scale.x + position.x, -scale.y + position.y, 0,0,0,0,0,0,  // bottom left
	    	-scale.x + position.x,  scale.y + position.y, 0,1,0,0,0,0  // top left
	    };

	    float mouseFX = (float)cursorPos.x;
	    float mouseFY = (float)cursorPos.y;

	    //Barycentric calculations
	    for (size_t i = 0; i < 2; i++)
	    {
	    	float ax = buttonCoor[0 + (24*i)];
	    	float ay = buttonCoor[1 + (24 * i)];
	    	float bx = buttonCoor[8 + (24 * i)];
	    	float by = buttonCoor[9 + (24 * i)];
	    	float cx = buttonCoor[16 + (24 * i)];
	    	float cy = buttonCoor[17 + (24 * i)];

	    	if (cy - ay == 0) {
	    		cy += 0.0001f;
	    	}

	    	float w1 = (ax * (cy - ay) + (mouseFY - ay) * (cx - ax) - mouseFX * (cy - ay)) / ((by - ay) * (cx - ax) - (bx - ax) * (cy - ay));
	    	float w2 = (mouseFY - ay - w1 * (by - ay)) / (cy - ay);
	    	if (w1 >= 0 && w2 >= 0 && (w1 + w2) <= 1) {
	    		return true;
	    	}
	    	else if (i == 1) {
	    		return false;
	    	}
	    }
    }
};

#endif