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

Button button_texture_selected_objects;

void button_texture_selected_objects_render(Timer& timer, bool doMouseTracking)
{
    button_texture_selected_objects.render(timer, doMouseTracking);

    if(button_texture_selected_objects.clicked){
        bool anyObjSelected = false;
        for (size_t i = 0; i < getModel()->meshes.size(); i++)
        {
            if(getModel()->meshes[i].selectedObjectIndices.size())
                anyObjSelected = true;
        }
        
        if(!anyObjSelected)
            LGDLOG::start << "WARNING! No object was selected" << LGDLOG::end;

        dialog_objectTexturing.show(timer);
    }
}