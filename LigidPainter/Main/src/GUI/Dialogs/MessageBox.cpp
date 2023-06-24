
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 Copyright (c) 2022-2023, LigidTools 

 All rights reserved.

 Official GitHub Link : https:github.com/mert-tetik/LigidPainter
 Official Web Page : https:ligidtools.com/ligidpainter

 ---------------------------------------------------------------------------
 */

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"

#include <string>
#include <iostream>
#include <vector>

#include "tinyfiledialogs.h"

/// @brief 
/// @param title The brief text displayed on the app bar
/// @param message The large text displayed on the panel
/// @param type MESSAGEBOX_TYPE_ERROR, MESSAGEBOX_TYPE_INFO, MESSAGEBOX_TYPE_WARNING, MESSAGEBOX_TYPE_QUESTION
/// @param button MESSAGEBOX_BUTTON_OK, MESSAGEBOX_BUTTON_OKCANCEL, MESSAGEBOX_BUTTON_YESNO, MESSAGEBOX_BUTTON_YESNOCANCEL
/// @return 2 : YES | 1 : NO | 0 : CANCEL       &       1 : YES | 0 : NO 
int showMessageBox(
                    const std::string title, 
                    const std::string message,
                    const int type, 
                    const int button
                )
{
    
    //Get the type
    const char* typeStr = "warning";
    
    if(type  == MESSAGEBOX_TYPE_ERROR)
        typeStr = "error";

    else if(type  == MESSAGEBOX_TYPE_INFO)
        typeStr = "info";
    
    else if(type  == MESSAGEBOX_TYPE_WARNING)
        typeStr = "warning";
    
    else if(type  == MESSAGEBOX_TYPE_QUESTION)
        typeStr = "question";

    

    //Get the button
    const char* buttonStr = "Ok";
    
    if(button == MESSAGEBOX_BUTTON_OK)
        buttonStr = "ok";
    
    else if(button == MESSAGEBOX_BUTTON_OKCANCEL)
        buttonStr = "okcancel";
    
    else if(button == MESSAGEBOX_BUTTON_YESNO)
        buttonStr = "yesno";
    
    else if(button == MESSAGEBOX_BUTTON_YESNOCANCEL)
        buttonStr = "yesnocancel";


    //Show the message box
    return tinyfd_messageBox(
                        title.c_str(), 
                        message.c_str(), 
                        buttonStr, 
                        typeStr, 
                        1
                    );
}