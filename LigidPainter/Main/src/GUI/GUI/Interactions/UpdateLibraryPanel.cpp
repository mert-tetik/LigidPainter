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

void updateLibraryPanelDisplayerElements(
                                            Panel &libraryPanelDisplayer, 
                                            Library &library, 
                                            ColorPalette& colorPalette, 
                                            Shaders &shaders,
                                            int frameCounter
                                        )
{
    
    //!LIBRARY PANEL DISPLAYER
    //Update the library displayer panel every time library changed
    if(library.changed){
        libraryPanelDisplayer.sections.clear(); //Remove all the elements of the library panel displayer
        
        //Create a new section
        Section libSection;
        libSection.header = Element(Button()); //Has no section button
        //Fill the elements of the section using the data in the library structure
        if(library.selectedElementIndex == 0){//Update textures
            for (size_t i = 0; i < library.textures.size(); i++)
            {
                //Push texture elements into the section
                libSection.elements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4.f),colorPalette,shaders.buttonShader,library.textures[i].title       , library.textures[i], 0.f,false))) ;
            }
        }
        else if(library.selectedElementIndex == 1){ //Update materials
            for (size_t i = 0; i < library.materials.size(); i++)
            {
                //Push texture elements into the section
                libSection.elements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4.f),colorPalette,shaders.buttonShader,library.materials[i].title       , Texture(library.materials[i].displayingTexture), 0.f,false))) ;
            }
        }
        else if(library.selectedElementIndex == 2){ //Update materials
            for (size_t i = 0; i < library.brushes.size(); i++)
            {
                //Push texture elements into the section
                libSection.elements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4.f),colorPalette,shaders.buttonShader,library.brushes[i].title       , Texture(library.brushes[i].displayingTexture), 0.f,false))) ;
            }
        }
        else if(library.selectedElementIndex == 3){ //Update tdmodels
            for (size_t i = 0; i < library.TDModels.size(); i++)
            {
                //Push texture elements into the section
                libSection.elements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4.f),colorPalette,shaders.buttonShader,library.TDModels[i].title       , Texture(), 0.f,false))) ;
            }
        }
        //Give the section
        libraryPanelDisplayer.sections.push_back(Section(Element(Button()),libSection.elements));
    }
    
    //Set library changed to false after updating some stuff after library change
    library.changed = false;
}