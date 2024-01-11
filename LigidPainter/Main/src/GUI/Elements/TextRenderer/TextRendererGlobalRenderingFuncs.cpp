/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

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

#include "GUI/Elements/Elements.hpp"
#include "UTIL/Util.hpp"
#include "ShaderSystem/Shader.hpp"

/// @brief renders the text
void TextRenderer::renderText(){
	rndrTxt(0);
}

/// @brief text rendering for the textbox
/// @param textPosCharIndex 
void TextRenderer::renderText(int &textPosCharIndex, glm::vec4 textColor){
	//If the active char is not equal to the active char 2 the chars between them will be selected

	//Get the position of the chars from the text (w axis will contain the position of the char at the index of activeChar2)
	rndrTxt(textPosCharIndex);
	
	//If textbox is active update the timer
	if(textDataActive){
		if(LigidGL::getTime() - this->timer.lastTimeT > 5.)
			this->timer.lastTimeT = LigidGL::getTime();

		this->timer.runTimer(0.5f);
	}	

	//Change the alpha value of the insertion pointer cursor 
	textColor.a = 0.5;
    ShaderSystem::buttonShader().setVec4("properties.color2"  ,     textColor    );

	//Render the insertion pointer cursor
	renderInsertionPointCursor(textPosCharIndex);
}