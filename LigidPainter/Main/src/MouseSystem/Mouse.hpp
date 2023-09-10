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

#ifndef MOUSE_HPP
#define MOUSE_HPP

//OpenGL & window 
#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

//OpenGL Math Library GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector> //std::vector
#include <string> //std::string
#include <map> //std::map

class LigidWindow;
class LigidCursor;

namespace Mouse
{
	void loadCursors();
    bool isCursorPointer();
	void setCursor(LigidCursor cursor);
	void updateCursor();
    bool isMouseHover(glm::vec2 scale, glm::vec2 position);

    glm::vec2* cursorPos();
    glm::vec2* mouseOffset();
    int* mods();
    int* action();
    bool* LClick();
    bool* RClick();
    bool* MClick();
    bool* LDoubleClick();
    bool* LPressed();
    bool* RPressed();
    bool* MPressed();
    double* mouseScroll();
    LigidCursor* defaultCursor();
    LigidCursor* pointerCursor();
    LigidCursor* hSlideCursor();
    LigidCursor* vSlideCursor();
    LigidCursor* inkPenCursor();
    LigidCursor* activeCursor();
    int* cursorTypeGotSet();
};

#endif