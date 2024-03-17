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

void current_mode_hint_displayer_render(Timer& timer, Painter& painter)
{
    Button currentModeDisplayer(ELEMENT_STYLE_SOLID, glm::vec2(6.f, 1.f), ""  , Texture(), 0.f, true);
    currentModeDisplayer.color = glm::vec4(0.f);
    currentModeDisplayer.outline = false;
    
    Button currentModeHintDisplayer(ELEMENT_STYLE_SOLID, glm::vec2(6.f, 1.f), ""  , Texture(), 0.f, true);
    currentModeHintDisplayer.color = glm::vec4(0.f);
    currentModeHintDisplayer.outline = false;

    if(painter.selectedDisplayingModeIndex == 0){
        currentModeDisplayer.text = "Object Selection Mode";
        int selectedObjCount = 0;
        int objCount = 0;
        for (size_t i = 0; i < getModel()->meshes.size(); i++)
        {
            selectedObjCount += getModel()->meshes[i].selectedObjectIndices.size();
            objCount += getModel()->meshes[i].objects.size();
        }
        
        currentModeHintDisplayer.text = "Objects " + std::to_string(selectedObjCount) + "/" + std::to_string(objCount);
    }
    else if(painter.selectedDisplayingModeIndex == 1){
        currentModeDisplayer.text = "Material Painting mode";
        currentModeHintDisplayer.text = "Regular Painting";
    }
    else if(painter.selectedDisplayingModeIndex == 2){
        currentModeDisplayer.text = "Single Texture Painting mode";
        currentModeHintDisplayer.text = "Regular Painting";
    }
    else{
        currentModeDisplayer.text = "Unknown mode";
        currentModeHintDisplayer.text = "Unknown mode";
    }

    if(painter.oXSide.active || painter.oYSide.active || painter.oZSide.active){
        
        if(painter.oXSide.active && painter.oYSide.active && painter.oZSide.active)
            currentModeHintDisplayer.text = "Mirror XYZ";
        
        else if(painter.oXSide.active && painter.oYSide.active)
            currentModeHintDisplayer.text = "Mirror XY";
        
        else if(painter.oYSide.active && painter.oZSide.active)
            currentModeHintDisplayer.text = "Mirror YZ";
        
        else if(painter.oXSide.active && painter.oZSide.active)
            currentModeHintDisplayer.text = "Mirror XZ";
        
        else if(painter.oXSide.active)
            currentModeHintDisplayer.text = "Mirror X";
        
        else if(painter.oYSide.active)
            currentModeHintDisplayer.text = "Mirror Y";
        
        else if(painter.oZSide.active)
            currentModeHintDisplayer.text = "Mirror Z";

    }


    if(painter.faceSelection.activated){
        currentModeHintDisplayer.text = "Masking To Selected Mesh";
    }
    if(painter.faceSelection.editMode){
        currentModeDisplayer.text = "Face Selection Mode (Edit)";
        if(painter.selectedMeshIndex < getModel()->meshes.size())
            currentModeHintDisplayer.text = "Faces Total : " + std::to_string(getModel()->meshes[painter.selectedMeshIndex].indices.size() / 3);
    }
    
    if(painter.usePaintingOver){
        currentModeHintDisplayer.text = "Painting Over Mode";
    }
    if(painter.paintingoverTextureEditorMode){
        currentModeDisplayer.text = "Painting Over Edit Mode";
        
        currentModeHintDisplayer.text = std::to_string(paintingOverTextureFields.size()) + " Textures";
    }

    currentModeDisplayer.pos = panel_displaying_modes.pos;
    currentModeDisplayer.pos.x = panel_layers.pos.x - panel_layers.scale.x - currentModeDisplayer.scale.x;
    currentModeDisplayer.pos.y += panel_displaying_modes.scale.y + currentModeDisplayer.scale.y + 1.f;
    
    currentModeHintDisplayer.pos = currentModeDisplayer.pos;
    currentModeHintDisplayer.pos.x = panel_layers.pos.x - panel_layers.scale.x - currentModeHintDisplayer.scale.x;
    currentModeHintDisplayer.pos.y += currentModeDisplayer.scale.y + currentModeHintDisplayer.scale.y;

    currentModeDisplayer.render(timer, false);
    currentModeHintDisplayer.render(timer, false);
}