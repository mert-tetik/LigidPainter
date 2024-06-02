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
#include <glm/gtx/string_cast.hpp>

#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "UTIL/Mouse/Mouse.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <fstream>

#define WRITEBITS(var, type, loc) if(!wf.write(reinterpret_cast<char*>(   &var     ), sizeof(type))){ \
                                    LGDLOG::start<< "ERROR : Texture field scene data. Failed to write at : " << loc << LGDLOG::end;\
                                    return false; \
                                }

#define WRITE_THREED_POINT(point)   WRITEBITS(point.active, bool, "point.active");\
                                    WRITEBITS(point.clickState1, bool, "point.clickState1");\
                                    WRITEBITS(point.detailI, int, "point.detailI");\
                                    WRITEBITS(point.pos.x, float, "point.pos.x");\
                                    WRITEBITS(point.pos.y, float, "point.pos.y");\
                                    WRITEBITS(point.pos.z, float, "point.pos.z");\
                                    WRITEBITS(point.normal.x, float, "point.normal.x");\
                                    WRITEBITS(point.normal.y, float, "point.normal.y");\
                                    WRITEBITS(point.normal.z, float, "point.normal.z");

bool TextureFieldScene::write_data(std::ofstream& wf){
    int texture_field_size = this->texture_fields.size();
    WRITEBITS(texture_field_size, int, "texture_field_size");
    for (size_t i = 0; i < texture_field_size; i++)
    {
        WRITEBITS(this->texture_fields[i].active, bool, "this->texture_fields[i].active");
        WRITEBITS(this->texture_fields[i].flippedH, bool, "this->texture_fields[i].flippedH");
        WRITEBITS(this->texture_fields[i].flippedV, bool, "this->texture_fields[i].flippedV");
        WRITEBITS(this->texture_fields[i].pos.x, float, "this->texture_fields[i].pos.x");
        WRITEBITS(this->texture_fields[i].pos.y, float, "this->texture_fields[i].pos.y");
        WRITEBITS(this->texture_fields[i].pos.z, float, "this->texture_fields[i].pos.z");
        WRITEBITS(this->texture_fields[i].rotation, float, "this->texture_fields[i].rotation");
        WRITEBITS(this->texture_fields[i].wrapMode, bool, "this->texture_fields[i].wrapMode");
        WRITEBITS(this->texture_fields[i].scale.x, float, "this->texture_fields[i].scale.x");
        WRITEBITS(this->texture_fields[i].scale.y, float, "this->texture_fields[i].scale.y");
        this->texture_fields[i].texture.writePixelData(wf);
        WRITE_THREED_POINT(this->texture_fields[i].threeDPointTopLeft);
        WRITE_THREED_POINT(this->texture_fields[i].threeDPointTopRight);
        WRITE_THREED_POINT(this->texture_fields[i].threeDPointBottomLeft);
        WRITE_THREED_POINT(this->texture_fields[i].threeDPointBottomRight);
        WRITE_THREED_POINT(this->texture_fields[i].detailed_threeDPoint_r1_c2);
        WRITE_THREED_POINT(this->texture_fields[i].detailed_threeDPoint_r1_c3);
        WRITE_THREED_POINT(this->texture_fields[i].detailed_threeDPoint_r1_c4);
        WRITE_THREED_POINT(this->texture_fields[i].detailed_threeDPoint_r2_c1);
        WRITE_THREED_POINT(this->texture_fields[i].detailed_threeDPoint_r2_c2);
        WRITE_THREED_POINT(this->texture_fields[i].detailed_threeDPoint_r2_c3);
        WRITE_THREED_POINT(this->texture_fields[i].detailed_threeDPoint_r2_c4);
        WRITE_THREED_POINT(this->texture_fields[i].detailed_threeDPoint_r2_c5);
        WRITE_THREED_POINT(this->texture_fields[i].detailed_threeDPoint_r3_c1);
        WRITE_THREED_POINT(this->texture_fields[i].detailed_threeDPoint_r3_c2);
        WRITE_THREED_POINT(this->texture_fields[i].detailed_threeDPoint_r3_c3);
        WRITE_THREED_POINT(this->texture_fields[i].detailed_threeDPoint_r3_c4);
        WRITE_THREED_POINT(this->texture_fields[i].detailed_threeDPoint_r3_c5);
        WRITE_THREED_POINT(this->texture_fields[i].detailed_threeDPoint_r4_c1);
        WRITE_THREED_POINT(this->texture_fields[i].detailed_threeDPoint_r4_c2);
        WRITE_THREED_POINT(this->texture_fields[i].detailed_threeDPoint_r4_c3);
        WRITE_THREED_POINT(this->texture_fields[i].detailed_threeDPoint_r4_c4);
        WRITE_THREED_POINT(this->texture_fields[i].detailed_threeDPoint_r4_c5);
        WRITE_THREED_POINT(this->texture_fields[i].detailed_threeDPoint_r5_c2);
        WRITE_THREED_POINT(this->texture_fields[i].detailed_threeDPoint_r5_c3);
        WRITE_THREED_POINT(this->texture_fields[i].detailed_threeDPoint_r5_c4);
    }
}