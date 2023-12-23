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

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <filesystem>

void updateLibraryPanelDisplayerElements(
                                            Panel &libraryPanelDisplayer, 
                                            int frameCounter
                                        )
{
    
    //!LIBRARY PANEL DISPLAYER
    //Update the library displayer panel every time library changed
    if(Library::isChanged()){
        libraryPanelDisplayer.sections.clear(); //Remove all the elements of the library panel displayer
        
        //Create a new section
        Section libSection;
        libSection.header = Element(Button()); //Has no section button
        //Fill the elements of the section using the data in the library structure
        if(Library::getSelectedElementIndex() == 0){//Update textures
            for (size_t i = 0; i < Library::getTextureArraySize(); i++)
            {
                //Push texture elements into the section
                libSection.elements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4.f),Library::getTexture(i)->title       , *Library::getTexture(i), 0.f,false))) ;
            }
        }
        else if(Library::getSelectedElementIndex() == 1){ //Update materials
            for (size_t i = 0; i < Library::getMaterialArraySize(); i++)
            {
                //Push texture elements into the section
                libSection.elements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4.f),Library::getMaterial(i)->title       , Library::getMaterial(i)->displayingTexture, 0.f,false))) ;
            }
        }
        else if(Library::getSelectedElementIndex() == 2){ //Update materials
            for (size_t i = 0; i < Library::getBrushArraySize(); i++)
            {
                //Push texture elements into the section
                libSection.elements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4.f),Library::getBrush(i)->title       , Library::getBrush(i)->displayingTexture, 0.f,false))) ;
            }
        }
        else if(Library::getSelectedElementIndex() == 3){ //Update tdmodels
            for (size_t i = 0; i < Library::getModelArraySize(); i++)
            {
                //Push texture elements into the section
                libSection.elements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4.f),Library::getModel(i)->title       , Library::getModel(i)->displayingTxtr, 0.f,false))) ;
            }
        }
        else if(Library::getSelectedElementIndex() == 4){ //Fonts
        
        }
        else if(Library::getSelectedElementIndex() == 5){ //Scripts
        
        }
        else if(Library::getSelectedElementIndex() == 6){ //Filters
            for (size_t i = 0; i < Library::getFilterArraySize(); i++)
            {
                //Push texture elements into the section
                libSection.elements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4.f),Library::getFilter(i)->title, Library::getFilter(i)->displayingTxtr, 0.f,false))) ;
            }
        }
        else if(Library::getSelectedElementIndex() == 8){ //Texture packs
            for (size_t i = 0; i < Library::getTexturePackArraySize(); i++)
            {
                //Push texture elements into the section
                libSection.elements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4.f),Library::getTexturePack(i)->title, appTextures.packageIcon, 0.f,false))) ;
            }
        }

        //Give the section
        libraryPanelDisplayer.sections.push_back(Section(Element(Button()),libSection.elements));
    }
    
    //Set library changed to false after updating some stuff after library change
    Library::setChanged(false);
}