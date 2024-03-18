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

PaintingLayer::PaintingLayer(const unsigned int resolution){
    this->title = "Painting Layer";
    this->layerType = "painting";
    this->layerIcon = appTextures.brushIcon;
    this->updateLayerButton();
    this->genResultChannels(resolution);
    this->contextMenu.sections[0].elements.pop_back();
}

void PaintingLayer::render(Painter& painter, const unsigned int resolution, Mesh& mesh){
    this->updateResultTextureResolutions(resolution, mesh);
}

void PaintingLayer::render_element_selection_panel(Timer& timer, bool doMouseTracking, Painter& painter, const unsigned int resolution, Mesh& mesh){
    this->elementSelectionMode = false;
    return;
}