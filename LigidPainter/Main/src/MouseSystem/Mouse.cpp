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
#include "MouseSystem/Mouse.hpp"
#include "SettingsSystem/Settings.hpp"

#include <string>
#include <iostream>
#include <vector>

glm::vec2 __cursorPos = glm::vec2(0);
glm::vec2 __mouseOffset;
int __mods = 0;
int __action = 0;
bool __LClick = false; //Left click - button 0
bool __RClick = false; //Right click - button 1
bool __MClick = false; //Middle click - button 2
bool __LDoubleClick = false; //Left click - button 0
bool __LPressed = false;
bool __RPressed = false;
bool __MPressed = false;
double __mouseScroll = 0.;
LigidCursor __defaultCursor;
LigidCursor __pointerCursor;
LigidCursor __hSlideCursor;
LigidCursor __vSlideCursor;
LigidCursor __inkPenCursor;
LigidCursor __activeCursor; //updateCursor sets that cursor as the cursor
int __cursorTypeGotSet = 0;

/// @brief Load all the cursors (that function is called in renderer constructor)
void Mouse::loadCursors(){
	//Help importing texture
	Texture texture;

	int cursorScaleX = 0;
	int cursorScaleY = 0;
	int channel = 0;

	unsigned char* pixels;

	pixels = texture.getTextureDataViaPath("LigidPainter/Resources/Ico/PointerIcon.png", cursorScaleX, cursorScaleY, channel , 4, false); //rgba 0 
	__pointerCursor.createCursor(cursorScaleX, cursorScaleY, 15, 0, pixels, 1);
	__activeCursor = __pointerCursor;
	//stbi_image_free(pixels);

	pixels = texture.getTextureDataViaPath("LigidPainter/Resources/Ico/DefaultIcon.png", cursorScaleX, cursorScaleY, channel , 4,false);
	__defaultCursor.createCursor(cursorScaleX, cursorScaleY, 7,0 , pixels, 0);
	//stbi_image_free(pixels);

	pixels = texture.getTextureDataViaPath("LigidPainter/Resources/Ico/hSlideCursor.png", cursorScaleX, cursorScaleY, channel , 4,false); //rgba 0 
	__hSlideCursor.createCursor(cursorScaleX, cursorScaleY, 15, 0, pixels, 3);
	//stbi_image_free(pixels);
	
	pixels = texture.getTextureDataViaPath("LigidPainter/Resources/Ico/vSlideCursor.png", cursorScaleX, cursorScaleY, channel , 4,false); //rgba 0 
	__vSlideCursor.createCursor(cursorScaleX, cursorScaleY, 15, 0, pixels, 2);
	//stbi_image_free(pixels);

	pixels = texture.getTextureDataViaPath("LigidPainter/Resources/Ico/InkPen.png", cursorScaleX, cursorScaleY, channel , 4,false); //rgba 0 
	__inkPenCursor.createCursor(cursorScaleX, cursorScaleY, 0, 25, pixels, -1);
	//stbi_image_free(pixels);
}

/// @brief Set a cursor
/// @param cursor a LigidCursor variable from the Mouse class public member varibles 
void Mouse::setCursor(LigidCursor cursor){
	__activeCursor = cursor;
}

/// @brief Was called at the end of the renderer.render (every frame)
void Mouse::updateCursor(){//Call that every frame after rendering the UI elements
	getContext()->window.setCursor(__activeCursor);
	__cursorTypeGotSet = __activeCursor.cursorType;
	__activeCursor = __defaultCursor;
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
    if (__cursorPos.x >= rectangle.x && __cursorPos.x <= (rectangle.x + rectangle.width) &&
        
		__cursorPos.y >= rectangle.y && __cursorPos.y <= (rectangle.y + rectangle.height)) {
        
		return true;
    }

    return false;
}

bool Mouse::isCursorPointer(){
	return __cursorTypeGotSet == 1; 
}

glm::vec2* Mouse::cursorPos(){
	return &__cursorPos;
}
glm::vec2* Mouse::mouseOffset(){
	return &__mouseOffset;
}
int* Mouse::mods(){
	return &__mods;
}
int* Mouse::action(){
	return &__action;
}
bool* Mouse::LClick(){
	return &__LClick;
} 
bool* Mouse::RClick(){
	return &__RClick;
} 
bool* Mouse::MClick(){
	return &__MClick;
} 
bool* Mouse::LDoubleClick(){
	return &__LDoubleClick;
}
bool* Mouse::LPressed(){
	return &__LPressed;
} 
bool* Mouse::RPressed(){
	return &__RPressed;
} 
bool* Mouse::MPressed(){
	return &__MPressed;
} 
double* Mouse::mouseScroll(){
	return &__mouseScroll;
} 
LigidCursor* Mouse::defaultCursor(){
	return &__defaultCursor;
}
LigidCursor* Mouse::pointerCursor(){
	return &__pointerCursor;
}
LigidCursor* Mouse::hSlideCursor(){
	return &__hSlideCursor;
}
LigidCursor* Mouse::vSlideCursor(){
	return &__vSlideCursor;
}
LigidCursor* Mouse::inkPenCursor(){
	return &__inkPenCursor;
}
LigidCursor* Mouse::activeCursor(){
	return &__activeCursor;
}
int* Mouse::cursorTypeGotSet(){
	return &__cursorTypeGotSet;
}