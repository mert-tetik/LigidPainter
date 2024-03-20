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

#include "Renderer.h"
#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "Toolkit/Layers/Layers.hpp"
#include "GUI/Panels.hpp"

#include <string>
#include <vector>

Panel panel_objects;

void panel_objects_render(Timer& timer, bool doMouseTracking){
   
   // Coloring the selected object
    for (size_t secI = 0; secI < panel_objects.sections.size(); secI++)
    {
        for (size_t elI = 0; elI < panel_objects.sections[secI].elements.size(); elI++){
            bool match = false;
            if(secI < getScene()->model->meshes.size()){
                for (size_t i = 0; i < getScene()->model->meshes[secI].selectedObjectIndices.size(); i++)
                {
                    if(getScene()->model->meshes[secI].selectedObjectIndices[i] == elI)
                        match = true;
                }
            }

            if(match)
                panel_objects.sections[secI].elements[elI].button.color = ColorPalette::themeColor;
            else
                panel_objects.sections[secI].elements[elI].button.color = ColorPalette::secondColor;
        }
    }
    
    panel_objects.render(timer, doMouseTracking);
    if(panel_objects.resizingDone){
        panels_transform();
    }

    // Update the elements according to new mesh
    if(getScene()->model->newModelAdded){
        panel_objects.sections.clear();
        
        for (size_t meshI = 0; meshI < getScene()->model->meshes.size(); meshI++)
        {
            Section section;
            section.header = SectionHolder(ColorPalette::secondColor,0.f,getScene()->model->meshes[meshI].materialName);
            section.header.sectionHolder.active = true;

            for (size_t objI = 0; objI < getScene()->model->meshes[meshI].objects.size(); objI++){
                Element btn = Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1.f), getScene()->model->meshes[meshI].objects[objI].title, Texture(), 0.f, false));
                btn.button.color = ColorPalette::mainColor;
                section.elements.push_back(btn);
            }

            panel_objects.sections.push_back(section);
        }
    }
}