/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

⠀⠀⠀⠀⠀⠀⠀⣷⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⣿⣿⣷⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⣿⣿⣿⣿⣷⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⣿⣿⣿⣿⣿⣿⣷⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⣿⣿⣿⣿⣿⣿⣿⣿⣷⣤⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣦⡀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⠿⠿⠿⠿⠿⠦⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⣿⣿⣿⣿⡿⢿⣿⣿⣿⣧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⣿⣿⠟⠁⠀⠘⣿⣿⣿⣿⣧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠟⠁⠀⠀⠀⠀⠸⣿⣿⣿⣿⡆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠹⣿⣿⣿⣿⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢹⣿⠿⠛⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
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
	//Cursor pos
    glm::vec2 cursorPos = glm::vec2(0);
    glm::vec2 mouseOffset;

	
	//Cursor actions
	int mods = 0;

	bool LClick = false; //Left click - button 0
	bool RClick = false; //Right click - button 1
	bool MClick = false; //Middle click - button 2

	bool LDoubleClick = false; //Left click - button 0

	bool LPressed = false;
	bool RPressed = false;
	bool MPressed = false;

	double mouseScroll = 0.;

	//Cursors
	GLFWcursor* defaultCursor;
	GLFWcursor* pointerCursor;
	GLFWcursor* hSlideCursor;
	GLFWcursor* vSlideCursor;
	
	GLFWcursor* activeCursor; //updateCursor sets that cursor as the cursor
	

    GLFWwindow* window;

    Mouse(){}

    Mouse(GLFWwindow* window){
        this->window = window;
    }

	void loadCursors(){
		//Help importing texture
		Texture texture;

		GLFWimage images[1];
		int channels = 0;
		
		images[0].pixels = texture.getTextureDataViaPath("LigidPainter/Resources/Ico/PointerIcon.png", images[0].width, images[0].height, channels, 4,false); //rgba channels 
		pointerCursor = glfwCreateCursor(images,15,0);
		activeCursor = pointerCursor;
		stbi_image_free(images[0].pixels);
	
		images[0].pixels = texture.getTextureDataViaPath("LigidPainter/Resources/Ico/DefaultIcon.png", images[0].width, images[0].height, channels, 4,false);
		defaultCursor = glfwCreateCursor(images,7,0);
		stbi_image_free(images[0].pixels);
	
		images[0].pixels = texture.getTextureDataViaPath("LigidPainter/Resources/Ico/hSlideCursor.png", images[0].width, images[0].height, channels, 4,false); //rgba channels 
		hSlideCursor = glfwCreateCursor(images,15,0);
		stbi_image_free(images[0].pixels);
		
		images[0].pixels = texture.getTextureDataViaPath("LigidPainter/Resources/Ico/vSlideCursor.png", images[0].width, images[0].height, channels, 4,false); //rgba channels 
		vSlideCursor = glfwCreateCursor(images,15,0);
		stbi_image_free(images[0].pixels);
	}

	void setCursor(GLFWcursor* cursor){
		activeCursor = cursor;
	}
	void updateCursor(){//Call that every frame after rendering the UI elements
		glfwSetCursor(window,activeCursor);
		activeCursor = defaultCursor;
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