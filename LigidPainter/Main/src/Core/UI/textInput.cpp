#include<iostream>

#include <string>

#include "Core/LigidPainter.h"
#include "Core/UI/UserInterface.h"

bool UserInterface::textInput(int key, int action,bool caps,std::string &text,int threshold){
    bool valueChanged = false;
    if(key >= 320 && key <=329){
		//Numpad Optimization
		key -= 272;
	}
	if(action == 0 || action == 2){ //Take input

		if(text.size() < threshold){
			if(key == 32){
				//Space
				text += ' ';
                valueChanged = true;
			}
			else if(isalpha((char)key)){
				if(!caps){
					text+=(char)key+32;//lowercase
                    valueChanged = true;
				}
				else{
					text+=(char)key;//UPPERCASE
                    valueChanged = true;
				}
			}
			else if(isdigit((char)key)){
				text+=(char)(key);
                valueChanged = true;
			}
		}

		if(key == 259){
			//Backspace
			if(text != ""){
				text.pop_back();
                valueChanged = true;
			}
		}
	}
    return valueChanged;
}

bool UserInterface::textInputHex(int key, int action, std::string &text, int &activeChar){
    bool valueChanged = false;
    if(action == 0 || action == 2){ 
        if(isdigit((char)key)){
	    	//Add decimal numbers
	    	text[activeChar] = (char)(key);
            valueChanged = true;
	    	activeChar++;
	    }
	    else{
	    	//Add hexadecimal numbers
	    	const char* chars = "ABCDEF";
	    	for (int i = 0; i < 6; i++)
	    	{
	    		if(key == chars[i]){
	    			text[activeChar] = (char)(chars[i]+32);
                    valueChanged = true;
	    			activeChar++;
	    		}
	    	}
	    }
        if(key == 259 && activeChar != 0){
	    	//Backspace
	    	text[activeChar] = '0';
            valueChanged = true;
            activeChar--;
	    }
    }
    return valueChanged;
}