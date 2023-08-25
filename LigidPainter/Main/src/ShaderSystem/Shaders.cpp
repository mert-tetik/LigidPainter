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
Shader __txtrEditorResizeShader;
Shader __txtrEditorBlurShader;
Shader __txtrEditorDistortionShader;
Shader __txtrEditorNormalMapShader;
Shader __txtrEditorColoringShader;
Shader __splitTexturesShader;
Shader __solidShadingShader;

void ShaderSystem::initShaderSystem(){
    __tdModelShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/3D_model.vert", "LigidPainter/Resources/Shaders/aFrag/PBR.frag");
    __depth3D.loadShaderPP("LigidPainter/Resources/Shaders/aVert/3D_model.vert", "LigidPainter/Resources/Shaders/aFrag/Depth3D.frag");
    __skyboxBall.loadShaderPP("LigidPainter/Resources/Shaders/aVert/3D_model.vert", "LigidPainter/Resources/Shaders/aFrag/SkyboxBall.frag");
    __textureUpdatingShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert", "LigidPainter/Resources/Shaders/aFrag/UpdatingTexture.frag");
    __skyboxShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/3D_skybox.vert", "LigidPainter/Resources/Shaders/aFrag/Skybox.frag");
    __prefilteringShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/3D_skybox.vert", "LigidPainter/Resources/Shaders/aFrag/PrefilterSkybox.frag");
    __buttonShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/Button.frag");
    __connectionCurve.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/ConnectionCurve.frag");
    __colorPicker.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/ColorPicker.frag");
    __twoDPainting.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/2DPainting.frag");
    __twoDPaintingModeAreaShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/UpdatingTexture.frag");
    __circleShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/Circle.frag");
    __dotsShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/Dots.frag");
    __heightToNormalMap.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/HeightToNormal.frag");
    __boundaryExpandingShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/BoundaryExpanding.frag");
    __uvMaskShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert", "LigidPainter/Resources/Shaders/aFrag/Color.frag");
    __bluringShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/SinglePassBlur.frag");
    __proceduralDisplayerShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/TextureSelectionProceduralDisplayer.frag");
    __to2DProcedural.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert", "LigidPainter/Resources/Shaders/aFrag/To2DProcedural.frag");
    __colorIDMaskingShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/MaterialID.frag");
    __grayScaleIDMaskingShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/GrayScaleMasking.frag");
    __sceneTilesShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/3D_model.vert", "LigidPainter/Resources/Shaders/aFrag/SceneTiles.frag");
    __sceneAxisDisplayerShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/3D_model.vert", "LigidPainter/Resources/Shaders/aFrag/SceneAxisDisplayer.frag");
    __paintOverTextureGen.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/PaintOverTextureGen.frag");
    __txtrEditorResizeShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/TextureEditor/Resize.frag");
    __txtrEditorBlurShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/TextureEditor/Blur.frag");
    __txtrEditorDistortionShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/TextureEditor/Distortion.frag");
    __txtrEditorNormalMapShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert" , "LigidPainter/Resources/Shaders/TextureEditor/NormalMap.frag");
    __txtrEditorColoringShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert" , "LigidPainter/Resources/Shaders/TextureEditor/Coloring.frag");
    __splitTexturesShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert" , "LigidPainter/Resources/Shaders/aFrag/SplitTextures.frag");
    __solidShadingShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/3D_model.vert" , "LigidPainter/Resources/Shaders/aFrag/SolidShading.frag");
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
Shader ShaderSystem::txtrEditorResizeShader(){
    return __txtrEditorResizeShader;
}
Shader ShaderSystem::txtrEditorBlurShader(){
    return __txtrEditorBlurShader;
}
Shader ShaderSystem::txtrEditorDistortionShader(){
    return __txtrEditorDistortionShader;
}
Shader ShaderSystem::txtrEditorNormalMapShader(){
    return __txtrEditorNormalMapShader;
}
Shader ShaderSystem::txtrEditorColoringShader(){
    return __txtrEditorColoringShader;
}
Shader ShaderSystem::splitTexturesShader(){
    return __splitTexturesShader;
}
Shader ShaderSystem::solidShadingShader(){
    return __solidShadingShader;
}