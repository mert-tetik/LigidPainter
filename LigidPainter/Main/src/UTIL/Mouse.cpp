/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

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
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "UTIL/Util.hpp"

#include <string>
#include <iostream>
#include <vector>

/// @brief Default constructor (just in case)
Mouse::Mouse(){}

/// @brief Main constructor 
Mouse::Mouse(LigidWindow window){
	this->window = window;
}

/// @brief Load all the cursors (that function is called in renderer constructor)
void Mouse::loadCursors(){
	//Help importing texture
	Texture texture;

	int cursorScaleX = 0;
	int cursorScaleY = 0;
	int channel = 0;

	unsigned char* pixels;

	pixels = texture.getTextureDataViaPath("LigidPainter/Resources/Ico/PointerIcon.png", cursorScaleX, cursorScaleY, channel , 4, false); //rgba 0 
	pointerCursor.createCursor(cursorScaleX, cursorScaleY, 15, 0, pixels);
	activeCursor = pointerCursor;
	//stbi_image_free(pixels);

	pixels = texture.getTextureDataViaPath("LigidPainter/Resources/Ico/DefaultIcon.png", cursorScaleX, cursorScaleY, channel , 4,false);
	defaultCursor.createCursor(cursorScaleX, cursorScaleY, 7,0 , pixels);
	//stbi_image_free(pixels);

	pixels = texture.getTextureDataViaPath("LigidPainter/Resources/Ico/hSlideCursor.png", cursorScaleX, cursorScaleY, channel , 4,false); //rgba 0 
	hSlideCursor.createCursor(cursorScaleX, cursorScaleY, 15, 0, pixels);
	//stbi_image_free(pixels);
	
	pixels = texture.getTextureDataViaPath("LigidPainter/Resources/Ico/vSlideCursor.png", cursorScaleX, cursorScaleY, channel , 4,false); //rgba 0 
	vSlideCursor.createCursor(cursorScaleX, cursorScaleY, 15, 0, pixels);
	//stbi_image_free(pixels);
}

/// @brief Set a cursor
/// @param cursor a LigidCursor variable from the Mouse class public member varibles 
void Mouse::setCursor(LigidCursor cursor){
	activeCursor = cursor;
}

/// @brief Was called at the end of the renderer.render (every frame)
void Mouse::updateCursor(){//Call that every frame after rendering the UI elements
	this->window.setCursor(activeCursor);
	activeCursor = defaultCursor;
}

struct aRectangle {
    int x;
    int y;
    int width;
    int height;
};

/// @brief 
/// @return if the cursor position is on top of a rectangle with the transform values of given parameters 
bool Mouse::isMouseHover(glm::vec2 scale, glm::vec2 pos) {
    // Calculate the rectangle's coordinates
    aRectangle rectangle;
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
