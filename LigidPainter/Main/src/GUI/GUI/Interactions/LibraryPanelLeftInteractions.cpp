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


#include<glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "LibrarySystem/Library.hpp"
#include "MouseSystem/Mouse.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <filesystem>


void UI::libraryPanelLeftInteraction(Panel &libraryPanelLeft){
    
    //Check all the library element button if they are pressed
    for (size_t i = 0; i < libraryPanelLeft.sections[0].elements.size(); i++) 
    {
        if(libraryPanelLeft.sections[0].elements[i].button.hover && *Mouse::LClick()){//If any button element is pressed
            if(Library::getSelectedElementIndex() != i){
                libraryPanelLeft.sections[0].elements[Library::getSelectedElementIndex()].button.clickState1 = false;
                Library::changeSelectedElementIndex(i);
                renamingTextBox.active = false;
                break;
            }
        } 
    }
    
    //Keep the selected element as selected
    for (size_t i = 0; i < libraryPanelLeft.sections[0].elements.size(); i++) 
    {
        if(Library::getSelectedElementIndex() == i)
            libraryPanelLeft.sections[0].elements[i].button.clickState1 = true;
        else
            libraryPanelLeft.sections[0].elements[i].button.clickState1 = false;
    }
}