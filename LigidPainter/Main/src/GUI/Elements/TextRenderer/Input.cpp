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


//Forward declerations for the util functions
void updateInsertionPointCursor(Timer &timer);
void deletion(int& activeChar, int& activeChar2, std::string &text);
void leftArrow(int mods, int &activeChar, int &activeChar2);
void rightArrow(int mods, int &activeChar, int &activeChar2, std::string &text);
void charInput(char &key, bool &caps, std::string& text, int& activeChar, int& activeChar2);




void TextRenderer::processTextInput(std::string &text,int &activeChar,int &activeChar2, int &textPosCharIndex){
	//If pressed to a key
    if(keyInput){

        //Show the insertion point cursor in case if invisible
        updateInsertionPointCursor(timer);
		
		//Delete
		if(key == GLFW_KEY_BACKSPACE - 256){
            deletion(activeChar,activeChar2,text);
		}

        //Left arrow
		else if(key == GLFW_KEY_LEFT-256){
            leftArrow(mods,activeChar,activeChar2);
		}

        //Right arrow
		else if(key == GLFW_KEY_RIGHT-256){
            rightArrow(mods,activeChar,activeChar2,text);
		}

        //Caps lock
		else if(key == GLFW_KEY_CAPS_LOCK-256){
			this->caps = !this->caps;
		}

        //Basic char input
		else if(mods == 0 && key < 127){ //If shift, control & alt keys are released
            charInput(key,caps,text,activeChar,activeChar2);
		}
	}
}






//---------------- UTIL FUNCTIONS -----------------

void updateInsertionPointCursor(Timer &timer){
	timer.seconds = 2;
	timer.lastTimeT = glfwGetTime();
}

void deletion(int& activeChar, int& activeChar2, std::string &text){
    
    //If selected multiple characters
    if(activeChar2 != activeChar){
        
        //If selected thru right delete the selected chars between activeChar & activeChar2
        if(activeChar < activeChar2)
            text.erase(text.begin() + activeChar + 1,text.begin()+activeChar2 + 1);
        
        //If selected thru left delete the selected chars between activeChar & activeChar2
        else
            text.erase(text.begin() + activeChar2 + 1,text.begin()+activeChar + 1);

        if(activeChar < 0)
            activeChar = 0;
        
        if(activeChar > text.size()-1)
            activeChar = text.size()-1;
        
        activeChar2 = activeChar;
    }

    //If selected a single character
    else if (activeChar != 0){ 
        text.erase(text.begin() + activeChar);
        activeChar--;
        activeChar2 = activeChar;
    }
}

void leftArrow(int mods, int &activeChar, int &activeChar2){
	//Shift pressed (multiselected)
    if(mods == 1){
		if(activeChar2 > 0)
			activeChar2--;
	}
	
    //Shift is not pressed
    else{
		if(activeChar > 0)
			activeChar--;
		activeChar2 = activeChar;
	}
}

void rightArrow(int mods, int &activeChar, int &activeChar2, std::string &text){
	//Shift pressed (multiselected)
    if(mods == 1){
		if(activeChar2 < text.size()-1)
			activeChar2++;
	}
    
    //Shift is not pressed
	else{
		if(activeChar < text.size()-1)
			activeChar++;
		activeChar2 = activeChar;
	}
}

void charInput(char &key, bool &caps, std::string& text, int& activeChar, int& activeChar2){
    
    //If char is a letter & capslock is false lowercase the char
    if(!caps && isalpha(key))
        key+=32;

    text.insert(text.begin() + (activeChar + 1),key);

    activeChar++;
    activeChar2 = activeChar;
}