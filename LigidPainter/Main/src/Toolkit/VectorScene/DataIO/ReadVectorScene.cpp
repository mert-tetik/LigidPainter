/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include "GUI/Dialogs.hpp"

#include "UTIL/Settings/Settings.hpp"
#include "UTIL/GL/GL.hpp"
#include "UTIL/Texture/Texture.hpp"
#include "UTIL/Library/Library.hpp"

#define READBITS(var, type, loc) if(!rf.read(reinterpret_cast<char*>(   &var     ), sizeof(type))){ \
                                    LGDLOG::start<< "ERROR : Reading lgdmodel file. Failed to read at : " << loc << LGDLOG::end;\
                                    return false; \
                                }

bool VectorScene::read_data(std::ifstream& rf){
    // 3D strokes
    uint64_t strokes3DSize; 
    READBITS(strokes3DSize, uint64_t, "Mesh strokes size");        
    for (size_t strokeI = 0; strokeI < strokes3DSize; strokeI++)
    {
        VectorStroke3D stroke;
 
        READBITS(stroke.startPoint.pos.x, float, "3D Vector stroke start position - X");
        READBITS(stroke.startPoint.pos.y, float, "3D Vector stroke start position - Y");
        READBITS(stroke.startPoint.pos.z, float, "3D Vector stroke start position - Z");
        
        READBITS(stroke.startPoint.normal.x, float, "3D Vector stroke start normal - X");
        READBITS(stroke.startPoint.normal.y, float, "3D Vector stroke start normal - Y");
        READBITS(stroke.startPoint.normal.z, float, "3D Vector stroke start normal - Z");
        
        READBITS(stroke.endPoint.pos.x, float, "3D Vector stroke end position - X");
        READBITS(stroke.endPoint.pos.y, float, "3D Vector stroke end position - Y");
        READBITS(stroke.endPoint.pos.z, float, "3D Vector stroke end position - Z");
        
        READBITS(stroke.endPoint.normal.x, float, "3D Vector stroke end normal - X");
        READBITS(stroke.endPoint.normal.y, float, "3D Vector stroke end normal - Y");
        READBITS(stroke.endPoint.normal.z, float, "3D Vector stroke end normal - Z");

        this->strokes_3D.push_back(stroke);
    }
    
    // 2D strokes
    uint64_t strokes2DSize; 
    READBITS(strokes2DSize, uint64_t, "Mesh strokes size");        
    for (size_t strokeI = 0; strokeI < strokes2DSize; strokeI++)
    {
        VectorStroke stroke;

        READBITS(stroke.startPoint.pos.x, float, "2D Vector stroke start position - X");
        READBITS(stroke.startPoint.pos.y, float, "2D Vector stroke start position - Y");
        
        READBITS(stroke.endPoint.pos.x, float, "2D Vector stroke end position - X");
        READBITS(stroke.endPoint.pos.y, float, "2D Vector stroke end position - Y");
        
        READBITS(stroke.offsetPoint.pos.x, float, "2D Vector stroke end position - X");
        READBITS(stroke.offsetPoint.pos.y, float, "2D Vector stroke end position - Y");
        
        this->strokes_2D.push_back(stroke);
    }

    // Stroke dialog 
    int stroke_dialog_paint_mode;
    READBITS(stroke_dialog_paint_mode, int, "this->stroke_dialog.paint_mode");
    bool stroke_dialog_wrap_mode;
    READBITS(stroke_dialog_wrap_mode, bool, "this->stroke_dialog.twoD_wrap_mode");

    this->stroke_dialog.set_properties(stroke_dialog_paint_mode, stroke_dialog_wrap_mode);
}