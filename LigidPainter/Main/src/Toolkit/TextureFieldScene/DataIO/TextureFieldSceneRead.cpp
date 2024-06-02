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

#define READBITS(var, type, loc) if(!rf.read(reinterpret_cast<char*>(   &var     ), sizeof(type))){ \
                                    LGDLOG::start<< "ERROR : Reading lgdmodel file. Failed to read at : " << loc << LGDLOG::end;\
                                    return false; \
                                }

#define READ_THREED_POINT(point)    READBITS(point.active, bool, "point.active");\
                                    READBITS(point.clickState1, bool, "point.clickState1");\
                                    READBITS(point.detailI, int, "point.detailI");\
                                    READBITS(point.pos.x, float, "point.pos.x");\
                                    READBITS(point.pos.y, float, "point.pos.y");\
                                    READBITS(point.pos.z, float, "point.pos.z");\
                                    READBITS(point.normal.x, float, "point.normal.x");\
                                    READBITS(point.normal.y, float, "point.normal.y");\
                                    READBITS(point.normal.z, float, "point.normal.z");

bool TextureFieldScene::read_data(std::ifstream& rf){
    int texture_field_size;
    READBITS(texture_field_size, int, "texture_field_size");
    for (size_t i = 0; i < texture_field_size; i++)
    {
        TextureField texture_field;

        READBITS(texture_field.active, bool, "texture_field.active");
        READBITS(texture_field.flippedH, bool, "texture_field.flippedH");
        READBITS(texture_field.flippedV, bool, "texture_field.flippedV");
        READBITS(texture_field.pos.x, float, "texture_field.pos.x");
        READBITS(texture_field.pos.y, float, "texture_field.pos.y");
        READBITS(texture_field.pos.z, float, "texture_field.pos.z");
        READBITS(texture_field.rotation, float, "texture_field.rotation");
        READBITS(texture_field.wrapMode, bool, "texture_field.wrapMode");
        READBITS(texture_field.scale.x, float, "texture_field.scale.x");
        READBITS(texture_field.scale.y, float, "texture_field.scale.y");
        texture_field.texture.readPixelData(rf);
        READ_THREED_POINT(texture_field.threeDPointTopLeft);
        READ_THREED_POINT(texture_field.threeDPointTopRight);
        READ_THREED_POINT(texture_field.threeDPointBottomLeft);
        READ_THREED_POINT(texture_field.threeDPointBottomRight);
        READ_THREED_POINT(texture_field.detailed_threeDPoint_r1_c2);
        READ_THREED_POINT(texture_field.detailed_threeDPoint_r1_c3);
        READ_THREED_POINT(texture_field.detailed_threeDPoint_r1_c4);
        READ_THREED_POINT(texture_field.detailed_threeDPoint_r2_c1);
        READ_THREED_POINT(texture_field.detailed_threeDPoint_r2_c2);
        READ_THREED_POINT(texture_field.detailed_threeDPoint_r2_c3);
        READ_THREED_POINT(texture_field.detailed_threeDPoint_r2_c4);
        READ_THREED_POINT(texture_field.detailed_threeDPoint_r2_c5);
        READ_THREED_POINT(texture_field.detailed_threeDPoint_r3_c1);
        READ_THREED_POINT(texture_field.detailed_threeDPoint_r3_c2);
        READ_THREED_POINT(texture_field.detailed_threeDPoint_r3_c3);
        READ_THREED_POINT(texture_field.detailed_threeDPoint_r3_c4);
        READ_THREED_POINT(texture_field.detailed_threeDPoint_r3_c5);
        READ_THREED_POINT(texture_field.detailed_threeDPoint_r4_c1);
        READ_THREED_POINT(texture_field.detailed_threeDPoint_r4_c2);
        READ_THREED_POINT(texture_field.detailed_threeDPoint_r4_c3);
        READ_THREED_POINT(texture_field.detailed_threeDPoint_r4_c4);
        READ_THREED_POINT(texture_field.detailed_threeDPoint_r4_c5);
        READ_THREED_POINT(texture_field.detailed_threeDPoint_r5_c2);
        READ_THREED_POINT(texture_field.detailed_threeDPoint_r5_c3);
        READ_THREED_POINT(texture_field.detailed_threeDPoint_r5_c4);

        this->texture_fields.push_back(texture_field);
    }
}