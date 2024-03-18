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

#include "Renderer.h"
#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "Toolkit/Layers/Layers.hpp"
#include "GUI/Panels.hpp"

#include <string>
#include <vector>

CheckComboList checkComboList_painting_over;

ProceduralProperties lastPaintingOverTextureFieldAddViaTextureSelectionDialogProceduralProperties;
void checkComboList_painting_over_render(Timer& timer, Painter& painter, bool doMouseTracking){    

    checkComboList_painting_over.checkButton.clickState1 = true;
    checkComboList_painting_over.checkButton.color2 = checkComboList_painting_over.checkButton.color;
    checkComboList_painting_over.render(timer, doMouseTracking);

    if(shortcuts_F1())
        checkComboList_painting_over.panel.sections[0].elements[0].checkBox.clickState1 = !checkComboList_painting_over.panel.sections[0].elements[0].checkBox.clickState1;
    if(shortcuts_F2())
        checkComboList_painting_over.panel.sections[0].elements[1].checkBox.clickState1 = !checkComboList_painting_over.panel.sections[0].elements[1].checkBox.clickState1;

    /*
    painter.usePaintingOver = checkComboList_painting_over.panel.sections[0].elements[0].checkBox.clickState1;
    painter.paintingoverTextureEditorMode = checkComboList_painting_over.panel.sections[0].elements[1].checkBox.clickState1;
    painter.paintingOverGrayScale = checkComboList_painting_over.panel.sections[0].elements[4].checkBox.clickState1;
    */

    if(checkComboList_painting_over.panel.sections[0].elements[2].button.clicked){
        Texture texture;
        texture.proceduralProps = lastPaintingOverTextureFieldAddViaTextureSelectionDialogProceduralProperties;
        dialog_textureSelection.show(timer, texture, 512, true);
        
        if(texture.ID){
            lastPaintingOverTextureFieldAddViaTextureSelectionDialogProceduralProperties = texture.proceduralProps;
            registerTextureFieldAction("New texture field via texture selection dialog", getTextureFieldScene());
            getTextureFieldScene()->add_new(TextureField(texture));
        }
    }
    else if(checkComboList_painting_over.panel.sections[0].elements[3].button.clicked){
        std::string test = showFileSystemObjectSelectionDialog("Select a texture file.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_TEXTURE, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);

        if(test.size()){
            Texture texture;
            texture.load(test.c_str());
            registerTextureFieldAction("New texture field via path", getTextureFieldScene());
            getTextureFieldScene()->add_new(TextureField(texture));
        }
    }
}