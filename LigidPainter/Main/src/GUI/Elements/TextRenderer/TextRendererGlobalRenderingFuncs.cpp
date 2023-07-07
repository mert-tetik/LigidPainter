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

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

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

/// @brief renders the text
/// @param shader 
void TextRenderer::renderText(Shader shader){
	rndrTxt(shader,0);
}

/// @brief text rendering for the textbox
/// @param shader 
/// @param textPosCharIndex 
void TextRenderer::renderText(Shader shader,int &textPosCharIndex, glm::vec4 textColor){
	//If the active char is not equal to the active char 2 the chars between them will be selected

	//Get the position of the chars from the text (w axis will contain the position of the char at the index of activeChar2)
	rndrTxt(shader,textPosCharIndex);
	
	//If textbox is active update the timer
	if(textDataActive){
		if(LigidGL::getTime() - this->timer.lastTimeT > 5.)
			this->timer.lastTimeT = LigidGL::getTime();

		this->timer.runTimer(0.5f);
	}	

	//Change the alpha value of the insertion pointer cursor 
	textColor.a = 0.5;
    shader.setVec4("properties.color2"  ,     textColor    );

	//Render the insertion pointer cursor
	renderInsertionPointCursor(shader,textPosCharIndex);
}