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
#include "ShaderSystem/Shader.hpp"
#include "LibrarySystem/Library.hpp"
#include "MouseSystem/Mouse.hpp"
#include "SettingsSystem/Settings.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"
#include "Layers/Layers.hpp"
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
            if(secI < getModel()->meshes.size()){
                for (size_t i = 0; i < getModel()->meshes[secI].selectedObjectIndices.size(); i++)
                {
                    if(getModel()->meshes[secI].selectedObjectIndices[i] == elI)
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
    if(getModel()->newModelAdded){
        panel_objects.sections.clear();
        
        for (size_t meshI = 0; meshI < getModel()->meshes.size(); meshI++)
        {
            Section section;
            section.header = SectionHolder(ColorPalette::secondColor,0.f,getModel()->meshes[meshI].materialName);
            section.header.sectionHolder.active = true;

            for (size_t objI = 0; objI < getModel()->meshes[meshI].objects.size(); objI++){
                Element btn = Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1.f), getModel()->meshes[meshI].objects[objI].title, Texture(), 0.f, false));
                btn.button.color = ColorPalette::mainColor;
                section.elements.push_back(btn);
            }

            panel_objects.sections.push_back(section);
        }
    }
}