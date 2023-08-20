    /*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    From LearnOpenGL.com 

*/

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "UTIL/Util.hpp"
#include "ShaderSystem/Shader.hpp"
    
Shader __tdModelShader;
Shader __skyboxShader;
Shader __skyboxBall;
Shader __buttonShader;
Shader __prefilteringShader;
Shader __connectionCurve;
Shader __colorPicker;
Shader __twoDPainting;
Shader __depth3D;
Shader __textureUpdatingShader;
Shader __twoDPaintingModeAreaShader;
Shader __circleShader;
Shader __dotsShader;
Shader __heightToNormalMap;
Shader __boundaryExpandingShader;
Shader __uvMaskShader;
Shader __bluringShader;
Shader __proceduralDisplayerShader;
Shader __colorIDMaskingShader;
Shader __grayScaleIDMaskingShader;
Shader __to2DProcedural;
Shader __sceneTilesShader;
Shader __sceneAxisDisplayerShader;
Shader __paintOverTextureGen;

void ShaderSystem::initShaderSystem(){
    __tdModelShader =                 Shader("LigidPainter/Resources/Shaders/aVert/3D_model.vert"           ,   "LigidPainter/Resources/Shaders/aFrag/PBR.frag"                                   ,nullptr    ,nullptr,   nullptr      );
    __depth3D =                       Shader("LigidPainter/Resources/Shaders/aVert/3D_model.vert"           ,   "LigidPainter/Resources/Shaders/aFrag/Depth3D.frag"                               ,nullptr    ,nullptr,   nullptr      );
    __skyboxBall =                    Shader("LigidPainter/Resources/Shaders/aVert/3D_model.vert"           ,   "LigidPainter/Resources/Shaders/aFrag/SkyboxBall.frag"                            ,nullptr    ,nullptr,   nullptr      );
    __textureUpdatingShader =         Shader("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert"        ,   "LigidPainter/Resources/Shaders/aFrag/UpdatingTexture.frag"                       ,nullptr    ,nullptr,   nullptr      );
    __skyboxShader =                  Shader("LigidPainter/Resources/Shaders/aVert/3D_skybox.vert"          ,   "LigidPainter/Resources/Shaders/aFrag/Skybox.frag"                                ,nullptr    ,nullptr,   nullptr      );
    __prefilteringShader =            Shader("LigidPainter/Resources/Shaders/aVert/3D_skybox.vert"          ,   "LigidPainter/Resources/Shaders/aFrag/PrefilterSkybox.frag"                       ,nullptr    ,nullptr,   nullptr      );
    __buttonShader =                  Shader("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert"        ,   "LigidPainter/Resources/Shaders/aFrag/Button.frag"                                ,nullptr    ,nullptr,   nullptr      );
    __connectionCurve =               Shader("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert"        ,   "LigidPainter/Resources/Shaders/aFrag/ConnectionCurve.frag"                       ,nullptr    ,nullptr,   nullptr      );
    __colorPicker =                   Shader("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert"        ,   "LigidPainter/Resources/Shaders/aFrag/ColorPicker.frag"                           ,nullptr    ,nullptr,   nullptr      );
    __twoDPainting =                  Shader("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert"        ,   "LigidPainter/Resources/Shaders/aFrag/2DPainting.frag"                            ,nullptr    ,nullptr,   nullptr      );
    __twoDPaintingModeAreaShader =    Shader("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert"        ,   "LigidPainter/Resources/Shaders/aFrag/UpdatingTexture.frag"                       ,nullptr    ,nullptr,   nullptr      );
    __circleShader =                  Shader("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert"        ,   "LigidPainter/Resources/Shaders/aFrag/Circle.frag"                                ,nullptr    ,nullptr,   nullptr      );
    __dotsShader =                    Shader("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert"        ,   "LigidPainter/Resources/Shaders/aFrag/Dots.frag"                                  ,nullptr    ,nullptr,   nullptr      );
    __heightToNormalMap =             Shader("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert"        ,   "LigidPainter/Resources/Shaders/aFrag/HeightToNormal.frag"                        ,nullptr    ,nullptr,   nullptr      );
    __boundaryExpandingShader =       Shader("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert"        ,   "LigidPainter/Resources/Shaders/aFrag/BoundaryExpanding.frag"                     ,nullptr    ,nullptr,   nullptr      );
    __uvMaskShader =                  Shader("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert"        ,   "LigidPainter/Resources/Shaders/aFrag/Color.frag"                                 ,nullptr    ,nullptr,   nullptr      );
    __bluringShader =                 Shader("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert"        ,   "LigidPainter/Resources/Shaders/aFrag/SinglePassBlur.frag"                        ,nullptr    ,nullptr,   nullptr      );
    __proceduralDisplayerShader =     Shader("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert"        ,   "LigidPainter/Resources/Shaders/aFrag/TextureSelectionProceduralDisplayer.frag"   ,nullptr    ,nullptr,   nullptr      );
    __to2DProcedural =                Shader("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert"        ,   "LigidPainter/Resources/Shaders/aFrag/To2DProcedural.frag"                        ,nullptr    ,nullptr,   nullptr      );
    __colorIDMaskingShader =          Shader("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert"        ,   "LigidPainter/Resources/Shaders/aFrag/MaterialID.frag"                            ,nullptr    ,nullptr,   nullptr      );
    __grayScaleIDMaskingShader =      Shader("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert"        ,   "LigidPainter/Resources/Shaders/aFrag/GrayScaleMasking.frag"                      ,nullptr    ,nullptr,   nullptr      );
    __sceneTilesShader =              Shader("LigidPainter/Resources/Shaders/aVert/3D_model.vert"           ,   "LigidPainter/Resources/Shaders/aFrag/SceneTiles.frag"                            ,nullptr    ,nullptr,   nullptr      );
    __sceneAxisDisplayerShader =      Shader("LigidPainter/Resources/Shaders/aVert/3D_model.vert"           ,   "LigidPainter/Resources/Shaders/aFrag/SceneAxisDisplayer.frag"                    ,nullptr    ,nullptr,   nullptr      );
    __paintOverTextureGen =           Shader("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert"        ,   "LigidPainter/Resources/Shaders/aFrag/PaintOverTextureGen.frag"                   ,nullptr    ,nullptr,   nullptr      );
}

Shader ShaderSystem::tdModelShader(){
    return __tdModelShader;
}
Shader ShaderSystem::skyboxShader(){
    return __skyboxShader;
}
Shader ShaderSystem::skyboxBall(){
    return __skyboxBall;
}
Shader ShaderSystem::buttonShader(){
    return __buttonShader;
}
Shader ShaderSystem::prefilteringShader(){
    return __prefilteringShader;
}
Shader ShaderSystem::connectionCurve(){
    return __connectionCurve;
}
Shader ShaderSystem::colorPicker(){
    return __colorPicker;
}
Shader ShaderSystem::twoDPainting(){
    return __twoDPainting;
}
Shader ShaderSystem::depth3D(){
    return __depth3D;
}
Shader ShaderSystem::textureUpdatingShader(){
    return __textureUpdatingShader;
}
Shader ShaderSystem::twoDPaintingModeAreaShader(){
    return __twoDPaintingModeAreaShader;
}
Shader ShaderSystem::circleShader(){
    return __circleShader;
}
Shader ShaderSystem::dotsShader(){
    return __dotsShader;
}
Shader ShaderSystem::heightToNormalMap(){
    return __heightToNormalMap;
}
Shader ShaderSystem::boundaryExpandingShader(){
    return __boundaryExpandingShader;
}
Shader ShaderSystem::uvMaskShader(){
    return __uvMaskShader;
}
Shader ShaderSystem::bluringShader(){
    return __bluringShader;
}
Shader ShaderSystem::proceduralDisplayerShader(){
    return __proceduralDisplayerShader;
}
Shader ShaderSystem::colorIDMaskingShader(){
    return __colorIDMaskingShader;
}
Shader ShaderSystem::grayScaleIDMaskingShader(){
    return __grayScaleIDMaskingShader;
}
Shader ShaderSystem::to2DProcedural(){
    return __to2DProcedural;
}
Shader ShaderSystem::sceneTilesShader(){
    return __sceneTilesShader;
}
Shader ShaderSystem::sceneAxisDisplayerShader(){
    return __sceneAxisDisplayerShader;
}
Shader ShaderSystem::paintOverTextureGen(){
    return __paintOverTextureGen;
}