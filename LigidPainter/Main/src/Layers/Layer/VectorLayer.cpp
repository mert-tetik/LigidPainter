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

VectorLayer::VectorLayer(const unsigned int resolution){
    this->title = "Vector Layer";
    this->layerType = "vector";
    this->layerIcon = appTextures.inkPenIcon;
    this->updateLayerButton();
    this->genResultChannels(resolution);
}

static Button cancel_editing_vectors_btn = Button(ELEMENT_STYLE_BASIC, glm::vec2(7.f,2.f), "Quit Vector Editing", Texture(), 0.f, false);
static bool firstFrameActivated = true;
void VectorLayer::render_element_selection_panel(Timer& timer, bool doMouseTracking, MaterialSelectionDialog &materialSelectionDialog, Painter& painter, const unsigned int resolution){
    
    if(firstFrameActivated){
        painter.vectorStrokes3D = this->strokes;
        painter.wrapMode = true;
        painter.selectedDisplayingModeIndex = 1;
        painter.selectedPaintingModeIndex = 5;
    }

    cancel_editing_vectors_btn.pos = glm::vec3(50.f, 20.f, 0.8f);
    cancel_editing_vectors_btn.render(timer, true);
    if(cancel_editing_vectors_btn.clicked || getContext()->window.isKeyClicked(LIGIDGL_KEY_ESCAPE) || getContext()->window.isKeyClicked(LIGIDGL_KEY_ENTER)){
        this->elementSelectionMode = false;
    }

    firstFrameActivated = false;

    bool anyStrokeInteracted = false;
    for (size_t i = 0; i < painter.vectorStrokes3D.size(); i++)
    {
        if(painter.vectorStrokes3D[i].startPoint.moving || painter.vectorStrokes3D[i].endPoint.moving)
            anyStrokeInteracted = true;
    }
    
    if(anyStrokeInteracted){
        this->render(painter, resolution);
    }

    if(!this->elementSelectionMode)
        firstFrameActivated = true;
}

void VectorLayer::render(Painter& painter, const unsigned int resolution){
    painter.applyVectorStrokes(
                                {}, 
                                Panel(), 
                                glm::mat4(0.f), 
                                0, 
                                Filter(), 
                                Box(), 
                                Material(),
                                {},
                                false
                            );
        
    painter.getSelectedMesh()->layerScene.update_result(resolution, glm::vec3(0.f));
}