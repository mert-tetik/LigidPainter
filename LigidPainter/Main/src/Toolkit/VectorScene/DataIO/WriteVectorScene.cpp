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

#define WRITEBITS(var, type, loc) if(!wf.write(reinterpret_cast<char*>(   &var     ), sizeof(type))){ \
                                    LGDLOG::start<< "ERROR : Writing vector scene data. Failed to write at : " << loc << LGDLOG::end;\
                                    return false; \
                                }

bool VectorScene::write_data(std::ofstream& wf){
    // 3D strokes
    uint64_t strokes3DSize = this->strokes_3D.size(); 
    WRITEBITS(strokes3DSize, uint64_t, "Mesh strokes size");        
    for (size_t strokeI = 0; strokeI < strokes3DSize; strokeI++)
    {
        WRITEBITS((this->strokes_3D)[strokeI].startPoint.pos.x, float, "3D Vector stroke start position - X");
        WRITEBITS((this->strokes_3D)[strokeI].startPoint.pos.y, float, "3D Vector stroke start position - Y");
        WRITEBITS((this->strokes_3D)[strokeI].startPoint.pos.z, float, "3D Vector stroke start position - Z");
        
        WRITEBITS((this->strokes_3D)[strokeI].startPoint.normal.x, float, "3D Vector stroke start normal - X");
        WRITEBITS((this->strokes_3D)[strokeI].startPoint.normal.y, float, "3D Vector stroke start normal - Y");
        WRITEBITS((this->strokes_3D)[strokeI].startPoint.normal.z, float, "3D Vector stroke start normal - Z");
        
        WRITEBITS((this->strokes_3D)[strokeI].endPoint.pos.x, float, "3D Vector stroke end position - X");
        WRITEBITS((this->strokes_3D)[strokeI].endPoint.pos.y, float, "3D Vector stroke end position - Y");
        WRITEBITS((this->strokes_3D)[strokeI].endPoint.pos.z, float, "3D Vector stroke end position - Z");
        
        WRITEBITS((this->strokes_3D)[strokeI].endPoint.normal.x, float, "3D Vector stroke end normal - X");
        WRITEBITS((this->strokes_3D)[strokeI].endPoint.normal.y, float, "3D Vector stroke end normal - Y");
        WRITEBITS((this->strokes_3D)[strokeI].endPoint.normal.z, float, "3D Vector stroke end normal - Z");
    }
    
    // 2D strokes
    uint64_t strokes2DSize = this->strokes_2D.size(); 
    WRITEBITS(strokes2DSize, uint64_t, "Mesh strokes size");        
    for (size_t strokeI = 0; strokeI < strokes2DSize; strokeI++)
    {
        WRITEBITS((this->strokes_2D)[strokeI].startPoint.pos.x, float, "2D Vector stroke start position - X");
        WRITEBITS((this->strokes_2D)[strokeI].startPoint.pos.y, float, "2D Vector stroke start position - Y");
        
        WRITEBITS((this->strokes_2D)[strokeI].endPoint.pos.x, float, "2D Vector stroke end position - X");
        WRITEBITS((this->strokes_2D)[strokeI].endPoint.pos.y, float, "2D Vector stroke end position - Y");
        
        WRITEBITS((this->strokes_2D)[strokeI].offsetPoint.pos.x, float, "2D Vector stroke end position - X");
        WRITEBITS((this->strokes_2D)[strokeI].offsetPoint.pos.y, float, "2D Vector stroke end position - Y");
    }

    // Stroke dialog 
    WRITEBITS(this->stroke_dialog.paint_mode, int, "this->stroke_dialog.paint_mode");
    WRITEBITS(this->stroke_dialog.twoD_wrap_mode, bool, "this->stroke_dialog.twoD_wrap_mode");
}