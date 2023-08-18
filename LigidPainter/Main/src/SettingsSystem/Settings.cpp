/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"
#include "GUI/GUI.hpp"
#include "LibrarySystem/Library.hpp"
#include "SettingsSystem/Settings.hpp"

#include <vector>
#include <string>

Settings::AppTextures __appTextures;

void Settings::loadAppTextures(){
    __appTextures.TDModelIcon.load("./LigidPainter/Resources/Icons/TDModel.png");
    __appTextures.softenIcon.load("./LigidPainter/Resources/Icons/Soften.png");
    __appTextures.smearIcon.load("./LigidPainter/Resources/Icons/Smear.png");
    __appTextures.modifierIcon.load("./LigidPainter/Resources/Icons/Modifier.png");
    __appTextures.environmentIcon.load("./LigidPainter/Resources/Icons/Environment.png");
    __appTextures.dropperIcon.load("./LigidPainter/Resources/Icons/Dropper.png");
    __appTextures.colorIcon.load("./LigidPainter/Resources/Icons/Color.png");
    __appTextures.brushIcon.load("./LigidPainter/Resources/Icons/Brush.png");
    __appTextures.arrowR.load("./LigidPainter/Resources/Icons/ArrowR.png");
    __appTextures.arrowL.load("./LigidPainter/Resources/Icons/ArrowL.png");
    __appTextures.threeDIcon.load("./LigidPainter/Resources/Icons/3D.png");
    __appTextures.twoDIcon.load("./LigidPainter/Resources/Icons/2D.png");
    __appTextures.exportIcon.load("./LigidPainter/Resources/Icons/Export.png");
    __appTextures.areaPaintingIcon.load("./LigidPainter/Resources/Icons/Area.png");
    __appTextures.gearIcon.load("./LigidPainter/Resources/Icons/Gear.png");
    __appTextures.nodeDisplayingMode.load("./LigidPainter/Resources/Icons/NodeDisplayingMode.png");
    __appTextures.paintingDisplayingMode.load("./LigidPainter/Resources/Icons/PaintingDisplayingMode.png");
    __appTextures.solidPaintingDisplayingMode.load("./LigidPainter/Resources/Icons/SolidPaintingDisplayingMode.png");
    __appTextures.ligidPainterIcon.load("./LigidPainter/Resources/Icons/LigidPainter.png");
    __appTextures.logButtonL.load("./LigidPainter/Resources/Icons/LogButtonL.png");
    __appTextures.logButtonR.load("./LigidPainter/Resources/Icons/LogButtonR.png");
    __appTextures.logButtonM.load("./LigidPainter/Resources/Icons/LogButtonM.png");
    __appTextures.greetingDialogImage.load("./LigidPainter/Resources/Images/greetingDialogImage.jpg");
    __appTextures.noMaterialConnectedToTheMeshWarningImage.load("./LigidPainter/Resources/Images/NoMaterialWarning.jpg");
}

namespace Settings{
    AppTextures appTextures(){
        return __appTextures;
    }
};