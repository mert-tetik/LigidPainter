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

#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <filesystem>


void UI::libraryPanelLeftInteraction(Panel &libraryPanelLeft, Library &library,Mouse &mouse){
    
    //Check all the library element button if they are pressed
    for (size_t i = 0; i < libraryPanelLeft.sections[0].elements.size(); i++) 
    {
        if(libraryPanelLeft.sections[0].elements[i].button.hover && mouse.LClick){//If any button element is pressed
            if(library.selectedElementIndex != i){
                libraryPanelLeft.sections[0].elements[library.selectedElementIndex].button.clickState1 = false;
                library.changeSelectedElementIndex(i);
                renamingTextBox.active = false;
                break;
            }
        } 
    }
    
    //Keep the selected element as selected
    for (size_t i = 0; i < libraryPanelLeft.sections[0].elements.size(); i++) 
    {
        if(library.selectedElementIndex == i){
            libraryPanelLeft.sections[0].elements[library.selectedElementIndex].button.clickState1 = true;
        }
    }
}