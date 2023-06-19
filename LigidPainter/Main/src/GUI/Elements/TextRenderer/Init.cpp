/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Text renderer constructors and text data loading

*/

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "freetype/freetype.h"

#include <string>
#include <iostream>
#include <vector>
#include <map>

#include "GUI/Elements.hpp"
#include "UTIL/Util.hpp"


/// @brief default constructor (ignore)
TextRenderer::TextRenderer(/* args */){}

/// @brief give the font to the class member variable and creates a timer
/// @param font which font will be used for the text rendering
TextRenderer::TextRenderer(Font font){
	this->font = font;
	this->timer = Timer();
}


/// @brief Load data regarding text to render or get info about the text
/// @param shader buttonShader
/// @param text the text
/// @param pos position of the text
/// @param multipleLines allow multiple lines if true
/// @param scale in 0 - 2 range / 1 is pretty big tho
/// @param minX Text boundaries min position in y axis
/// @param maxX Text boundaries max position in y axis
/// @param alignment TEXTRENDERER_ALIGNMENT_LEFT , TEXTRENDERER_ALIGNMENT_MID or TEXTRENDERER_ALIGNMENT_RIGHT
void TextRenderer::loadTextData(
                                    Shader shader, 
                                    std::string text, 
                                    glm::vec3 pos, 
                                    bool multipleLines, 
                                    float scale, 
                                    float minX,
                                    float maxX, 
                                    int alignment
                                )
{
	this->textDataShader = shader;
	this->textDataText = text;
	this->textDataPos = pos;
	this->textDataMultipleLines = multipleLines; 
	this->textDataScale = scale;
	this->textDataMinX = minX;
	this->textDataMaxX = maxX;
	this->textDataAlignment = alignment;
	this->textDataActive = false;
    this->textDataActiveChar = 0;
	this->textDataActiveChar2 = 0;
	this->textDataTextPosCharIndex = 0;  
}

/// @brief Load data regarding text to render or get info about the text
/// @param shader buttonShader
/// @param text the text
/// @param pos position of the text
/// @param multipleLines allow multiple lines if true
/// @param scale in 0 - 2 range / 1 is pretty big tho
/// @param minX Text boundaries min position in y axis
/// @param maxX Text boundaries max position in y axis
/// @param alignment TEXTRENDERER_ALIGNMENT_LEFT , TEXTRENDERER_ALIGNMENT_MID or TEXTRENDERER_ALIGNMENT_RIGHT
/// @param active render the insertion point cursor if active
/// @param activeChar location of the insertion point cursor
/// @param activeChar2 if not the same with the activeChar than the chars between the activeChar and activeChar2 will be multiselected
/// @param textPosCharIndex variable that moves the text when overflowed
void TextRenderer::loadTextData(
                                    Shader shader, 
                                    std::string text, 
                                    glm::vec3 pos, 
                                    bool multipleLines, 
                					float scale, 
                                    float minX,
                                    float maxX, 
                                    int alignment,
                                    bool active,
					                int activeChar, 
                                    int activeChar2, 
                                    int textPosCharIndex
                                )
{
	this->textDataShader = shader;
	this->textDataText = text;
	this->textDataPos = pos;
	this->textDataMultipleLines = multipleLines; 
	this->textDataScale = scale;
	this->textDataMinX = minX;
	this->textDataMaxX = maxX;
	this->textDataAlignment = alignment;
	this->textDataActive = active;
	this->textDataActiveChar = activeChar;
	this->textDataActiveChar2 = activeChar2;
	this->textDataTextPosCharIndex = textPosCharIndex;  
}