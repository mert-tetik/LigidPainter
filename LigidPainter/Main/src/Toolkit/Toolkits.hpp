
/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef TOOLKITS_HPP
#define TOOLKITS_HPP
#include "Toolkit/Layers/Layers.hpp"
#include "Toolkit/TextureFieldScene/TextureFieldScene.hpp"
#include "Toolkit/VectorScene/VectorScene.hpp"
#include "Toolkit/Dots/Dots.hpp"
#include "Toolkit/PaintingCursor/PaintingCursor.hpp"
#include "Toolkit/LinePaintingTool/LinePaintingTool.hpp"
#include "Toolkit/3DCursor/3DCursor.hpp"

void render_toolkits_before_panels(Timer& timer);
void render_toolkits_after_panels(Timer& timer);

#endif
