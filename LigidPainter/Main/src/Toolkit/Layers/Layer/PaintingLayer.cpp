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
    this->generate_layer_buffers(resolution);

    this->painting_capture_txtr = Texture((char*)nullptr, 1024, 1024);
}

void PaintingLayer::type_specific_generate_result(const unsigned int resolution, Mesh& mesh){}

void PaintingLayer::type_specific_modification(Timer& timer, bool doMouseTracking, const unsigned int resolution, Mesh& mesh){
    dialog_objectTexturing.show(timer, mesh, this, resolution);
    
    this->type_specific_modification_enabled = false;
    return;
}