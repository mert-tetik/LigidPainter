/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef LINE_PAINTING_TOOL_HPP
#define LINE_PAINTING_TOOL_HPP

class Timer;
class Painter;

void line_painting(Timer& timer, bool applyStroke, bool firstFrame, bool curvy);

#endif
