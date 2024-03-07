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

#include <string>
#include <iostream>
#include <vector>

#include "GUI/Elements/Elements.hpp"
#include "GUI/Dialogs/Dialogs.hpp"
#include "ContextMenuSystem/ContextMenus.hpp"
#include "Layers/Layers.hpp"


void VectorLayer::render_element_selection_panel(Timer& timer, bool doMouseTracking, MaterialSelectionDialog &materialSelectionDialog, Painter& painter){
    /*painter.render3DVectors(timer, true, this->strokes);
    painter.applyVectorStrokes(this->strokes, Panel(), glm::mat4(0.f), 0, )*/
}

void VectorLayer::render(){
}