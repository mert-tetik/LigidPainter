/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"
#include "GUI/Panels.hpp"

#include "3D/ThreeD.hpp"

#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "UTIL/Painting/Painter.hpp"

#include "Toolkit/Toolkits.hpp"

#include <string>
#include <iostream>
#include <vector>


static glm::vec2 straightLinePaintingStartPos = glm::vec2(-1.f);
static glm::vec2 straightLinePaintingDirectionPos = glm::vec2(-1.f);
static bool leaved_threshold = false;
static bool leaved_and_got_in_threshold = false;

void line_painting(Timer& timer, bool applyStroke, bool firstFrame, bool curvy){
    float threshold = 2.f;
    bool inside_threshold = glm::distance(straightLinePaintingStartPos, *Mouse::cursorPos() / *Settings::videoScale() * 100.f) < threshold;

    if(!applyStroke){
        if(firstFrame){
            straightLinePaintingDirectionPos = glm::vec2(-1.f);

            straightLinePaintingStartPos = *Mouse::cursorPos() / *Settings::videoScale() * 100.f;
            glm::vec2 cursorOffset = *Mouse::mouseOffset();
            
            if(cursorOffset.x || cursorOffset.y)
                cursorOffset = glm::normalize(cursorOffset);
        }

        float offset_threshold = 7.f;
        bool inside_offset_threshold = glm::distance(straightLinePaintingStartPos, *Mouse::cursorPos() / *Settings::videoScale() * 100.f) < offset_threshold;
        
        if(!curvy){
            straightLinePaintingDirectionPos = straightLinePaintingStartPos; 
        }
        else{
            if(!inside_offset_threshold && straightLinePaintingDirectionPos == glm::vec2(-1.f)){
                straightLinePaintingDirectionPos = *Mouse::cursorPos() / *Settings::videoScale() * 100.f;
            }
        }

        if(!inside_threshold){
            leaved_threshold = true;
            leaved_and_got_in_threshold = false;
        }


        if(inside_threshold && leaved_threshold){
            leaved_and_got_in_threshold = true;
        }
        
        VectorStroke stroke(straightLinePaintingStartPos, *Mouse::cursorPos() / *Settings::videoScale() * 100.f, (straightLinePaintingDirectionPos != glm::vec2(-1.f)) ? straightLinePaintingDirectionPos : straightLinePaintingStartPos);

        if(!leaved_and_got_in_threshold)
            stroke.draw_single_stroke(timer, 0.0005f);
    }
    else{
        std::vector<VectorStroke> strokeArray;
        strokeArray.push_back(VectorStroke(straightLinePaintingStartPos, *Mouse::cursorPos() / *Settings::videoScale() * 100.f, (straightLinePaintingDirectionPos != glm::vec2(-1.f)) ? straightLinePaintingDirectionPos : straightLinePaintingStartPos));
        
        VectorScene scene = VectorScene(strokeArray, {});
        bool success = false;
        PaintSettings settings = get_paint_settings_using_GUI_data(&success);

        if(success && !leaved_and_got_in_threshold){
            scene.apply_strokes(false, checkBox_wrap_mode.clickState1, settings);
        }

        leaved_threshold = false;
        leaved_and_got_in_threshold = false;
    }
}