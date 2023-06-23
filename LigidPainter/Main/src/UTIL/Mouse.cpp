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

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "UTIL/Util.hpp"

#include <string>
#include <iostream>
#include <vector>

Mouse::Mouse(){}

Mouse::Mouse(GLFWwindow* window){
	this->window = window;
}

void Mouse::loadCursors(){
	//Help importing texture
	Texture texture;

	GLFWimage images[1];
	int channels = 0;
	
	images[0].pixels = texture.getTextureDataViaPath("LigidPainter/Resources/Ico/PointerIcon.png", images[0].width, images[0].height, channels, 4,false); //rgba channels 
	pointerCursor = glfwCreateCursor(images,15,0);
	activeCursor = pointerCursor;
	//stbi_image_free(images[0].pixels);

	images[0].pixels = texture.getTextureDataViaPath("LigidPainter/Resources/Ico/DefaultIcon.png", images[0].width, images[0].height, channels, 4,false);
	defaultCursor = glfwCreateCursor(images,7,0);
	//stbi_image_free(images[0].pixels);

	images[0].pixels = texture.getTextureDataViaPath("LigidPainter/Resources/Ico/hSlideCursor.png", images[0].width, images[0].height, channels, 4,false); //rgba channels 
	hSlideCursor = glfwCreateCursor(images,15,0);
	//stbi_image_free(images[0].pixels);
	
	images[0].pixels = texture.getTextureDataViaPath("LigidPainter/Resources/Ico/vSlideCursor.png", images[0].width, images[0].height, channels, 4,false); //rgba channels 
	vSlideCursor = glfwCreateCursor(images,15,0);
	//stbi_image_free(images[0].pixels);
}

void Mouse::setCursor(GLFWcursor* cursor){
	activeCursor = cursor;
}

void Mouse::updateCursor(){//Call that every frame after rendering the UI elements
	glfwSetCursor(window,activeCursor);
	activeCursor = defaultCursor;
}

struct Rectangle {
    int x;
    int y;
    int width;
    int height;
};

bool Mouse::isMouseHover(glm::vec2 scale, glm::vec2 pos) {
    // Calculate the rectangle's coordinates
    Rectangle rectangle;
    rectangle.width = scale.x*2;
    rectangle.height = scale.y*2;
    rectangle.x = pos.x - scale.x;
    rectangle.y = pos.y - scale.y;

    // Check if the cursor position is within the rectangle's bounds
    if (this->cursorPos.x >= rectangle.x && this->cursorPos.x <= (rectangle.x + rectangle.width) &&
        
		this->cursorPos.y >= rectangle.y && this->cursorPos.y <= (rectangle.y + rectangle.height)) {
        
		return true;
    }

    return false;
}
