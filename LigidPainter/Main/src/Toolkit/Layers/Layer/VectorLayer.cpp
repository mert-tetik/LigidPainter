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

#include "GUI/Elements.hpp"
#include "GUI/Dialogs.hpp"
#include "Toolkit/Layers/Layers.hpp"

VectorLayer::VectorLayer(const unsigned int resolution){
    this->title = "Vector Layer";
    this->layerType = "vector";
    this->layerIcon = appTextures.inkPenIcon;
    this->updateLayerButton();
    this->genResultChannels(resolution);
}

std::vector<VectorStroke3D> lastPainterStrokes;

static Button cancel_editing_vectors_btn = Button(ELEMENT_STYLE_BASIC, glm::vec2(7.f,2.f), "Quit Vector Editing", Texture(), 0.f, false);
static bool firstFrameActivated = true;
void VectorLayer::render_element_selection_panel(Timer& timer, bool doMouseTracking, const unsigned int resolution, Mesh& mesh){
    
    if(firstFrameActivated){
        // Not needed rn
        painter.vectorStrokes3D = this->strokes;
    }
    
    lastPainterStrokes = painter.vectorStrokes3D;

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
        this->render(resolution, mesh);
    }

    if(!this->elementSelectionMode)
        firstFrameActivated = true;

    if(!elementSelectionMode)
        painter.vectorStrokes3D = lastPainterStrokes;
}

void VectorLayer::render(const unsigned int resolution, Mesh& mesh){
    this->updateResultTextureResolutions(resolution, mesh);
    
    lastPainterStrokes = painter.vectorStrokes3D;
    painter.vectorStrokes3D = this->strokes;

    painter.applyVectorStrokes(
                                {}, 
                                Panel(), 
                                0, 
                                Filter(), 
                                Box(), 
                                Material(),
                                {},
                                false
                            );
        
    getScene()->get_selected_mesh()->layerScene.update_result(resolution, glm::vec3(0.f), mesh);
    
    painter.vectorStrokes3D = lastPainterStrokes;
}