    /*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    From LearnOpenGL.com 

*/

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <atomic>

#include "UTIL/Util.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Settings/Settings.hpp"
    
Shader __tdModelShader;
Shader __PBRDisplayOnly;
Shader __skyboxShader;
Shader __skyboxModelShader;
Shader __skyboxBall;
Shader __buttonShader;
Shader __prefilteringShader;
Shader __vectoralCurve;
Shader __twoDPainting;
Shader __depth3D;
Shader __renderModelData;
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
Shader __textureRenderingShader;
Shader __normalVectorShader;
Shader __edgeWearShader;
Shader __bakingShader;
Shader __defaultFramebufferShader;
Shader __nodeConnectionCurve;
Shader __projectingPaintedTextureShader;
Shader __projectedPaintingTextureMixerShader;
Shader __primitiveIDShader;
Shader __modelObjectID;
Shader __objectTexturingAssign;
Shader __AOGen;
Shader __twoPassBlur;
Shader __gizmo;
Shader __color3d;
Shader __color2d;
Shader __threeDTextureRenderingShader;
Shader __alphaZero3D;
Shader __color3DInstancing;
Shader __layersUpdate;
Shader __solidPaintingShader;
Shader __removeUnselectedFacesShader;

void ShaderSystem::initShaderSystem(){
    __tdModelShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/3D_model.vert", "LigidPainter/Resources/Shaders/aFrag/PBR.frag");
    __PBRDisplayOnly.loadShaderPP("LigidPainter/Resources/Shaders/aVert/3D_model.vert", "LigidPainter/Resources/Shaders/aFrag/PBR_Display_Only.frag");
    __depth3D.loadShaderPP("LigidPainter/Resources/Shaders/aVert/3D_model.vert", "LigidPainter/Resources/Shaders/aFrag/Depth3D.frag");
    __alphaZero3D.loadShaderPP("LigidPainter/Resources/Shaders/aVert/3D_model.vert", "LigidPainter/Resources/Shaders/aFrag/AlphaZero3D.frag");
    __renderModelData.loadShaderPP("LigidPainter/Resources/Shaders/aVert/3D_model.vert", "LigidPainter/Resources/Shaders/aFrag/RenderModelData.frag");
    __skyboxBall.loadShaderPP("LigidPainter/Resources/Shaders/aVert/3D_model.vert", "LigidPainter/Resources/Shaders/aFrag/SkyboxBall.frag");
    __primitiveIDShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/3D_model.vert", "LigidPainter/Resources/Shaders/aFrag/PrimitiveID.frag");
    __AOGen.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert", "LigidPainter/Resources/Shaders/aFrag/AOGen.frag");
    __bakingShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert", "LigidPainter/Resources/Shaders/aFrag/Baking.frag");
    __textureUpdatingShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert", "LigidPainter/Resources/Shaders/aFrag/UpdatingTexture.frag");
    __skyboxShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/3D_skybox.vert", "LigidPainter/Resources/Shaders/aFrag/Skybox.frag");
    __skyboxModelShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/3D_model.vert", "LigidPainter/Resources/Shaders/aFrag/Skybox.frag");
    __prefilteringShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/3D_skybox.vert", "LigidPainter/Resources/Shaders/aFrag/PrefilterSkybox.frag");
    __buttonShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/Button.frag");
    __layersUpdate.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/LayersUpdate.frag");
    __vectoralCurve.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/VectoralCurve.frag");
    __gizmo.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/Gizmo.frag");
    __twoDPainting.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/2DPainting.frag");
    __twoDPaintingModeAreaShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/3D_model.vert", "LigidPainter/Resources/Shaders/aFrag/UpdatingTexture.frag");
    __circleShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/Circle.frag");
    __dotsShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/Dots.frag");
    __heightToNormalMap.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/HeightToNormal.frag");
    __boundaryExpandingShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/BoundaryExpanding.frag");
    __uvMaskShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert", "LigidPainter/Resources/Shaders/aFrag/Color.frag");
    __color3d.loadShaderPP("LigidPainter/Resources/Shaders/aVert/3D_model.vert", "LigidPainter/Resources/Shaders/aFrag/Color.frag");
    __color2d.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/Color.frag");
    __bluringShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/SinglePassBlur.frag");
    __twoPassBlur.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/TwoPassBlur.frag");
    __proceduralDisplayerShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/TextureSelectionProceduralDisplayer.frag");
    __to2DProcedural.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert", "LigidPainter/Resources/Shaders/aFrag/To2DProcedural.frag");
    __normalVectorShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert", "LigidPainter/Resources/Shaders/aFrag/NormalVector.frag");
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
    __solidPaintingShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/3D_model.vert" , "LigidPainter/Resources/Shaders/aFrag/SolidPainting.frag");
    __textureRenderingShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert" , "LigidPainter/Resources/Shaders/aFrag/TextureRendering.frag");
    __threeDTextureRenderingShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/3D_model.vert" , "LigidPainter/Resources/Shaders/aFrag/TextureRendering.frag");
    __edgeWearShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert" , "LigidPainter/Resources/Shaders/aFrag/EdgeWear.frag");
    __defaultFramebufferShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert" , "LigidPainter/Resources/Shaders/aFrag/DefaultFramebuffer.frag");
    __nodeConnectionCurve.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert" , "LigidPainter/Resources/Shaders/aFrag/NodeConnectionCurve.frag");
    __projectingPaintedTextureShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert", "LigidPainter/Resources/Shaders/aFrag/ProjectingPaintedTexture.frag");
    __projectedPaintingTextureMixerShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert", "LigidPainter/Resources/Shaders/aFrag/ProjectedPaintingTextureMixer.frag");
    __modelObjectID.loadShaderPP("LigidPainter/Resources/Shaders/aVert/3D_model.vert" , "LigidPainter/Resources/Shaders/aFrag/ModelObjectID.frag");
    __objectTexturingAssign.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert" , "LigidPainter/Resources/Shaders/aFrag/ObjectTexturingAssign.frag");
    __color3DInstancing.loadShaderPP("LigidPainter/Resources/Shaders/aVert/3D_model_instancing.vert","LigidPainter/Resources/Shaders/aFrag/Color.frag");
    __removeUnselectedFacesShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert","LigidPainter/Resources/Shaders/aFrag/RemoveUnselectedFaces.frag");

    MaterialModifierShaders::init_shaders();
}

Shader ShaderSystem::tdModelShader(){
    return __tdModelShader;
}
Shader ShaderSystem::PBRDisplayOnly(){
    return __PBRDisplayOnly;
}
Shader ShaderSystem::skyboxShader(){
    return __skyboxShader;
}
Shader ShaderSystem::skyboxModelShader(){
    return __skyboxModelShader;
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
Shader ShaderSystem::vectoralCurve(){
    return __vectoralCurve;
}
Shader ShaderSystem::twoDPainting(){
    return __twoDPainting;
}
Shader ShaderSystem::depth3D(){
    return __depth3D;
}
Shader ShaderSystem::renderModelData(){
    return __renderModelData;
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
Shader ShaderSystem::textureRenderingShader(){
    return __textureRenderingShader;
}
Shader ShaderSystem::threeDTextureRenderingShader(){
    return __threeDTextureRenderingShader;
}
Shader ShaderSystem::normalVectorShader(){
    return __normalVectorShader;
}
Shader ShaderSystem::edgeWearShader(){
    return __edgeWearShader;
}
Shader ShaderSystem::bakingShader(){
    return __bakingShader;
}
Shader ShaderSystem::AOGen(){
    return __AOGen;
}
Shader ShaderSystem::defaultFramebufferShader(){
    return __defaultFramebufferShader;
}
Shader ShaderSystem::nodeConnectionCurve(){
    return __nodeConnectionCurve;
}
Shader ShaderSystem::projectingPaintedTextureShader(){
    return __projectingPaintedTextureShader;
}
Shader ShaderSystem::projectedPaintingTextureMixerShader(){
    return __projectedPaintingTextureMixerShader;
}
Shader ShaderSystem::primitiveIDShader(){
    return __primitiveIDShader;
}
Shader ShaderSystem::modelObjectID(){
    return __modelObjectID;
}
Shader ShaderSystem::objectTexturingAssign(){
    return __objectTexturingAssign;
}

Shader ShaderSystem::twoPassBlur(){
    return __twoPassBlur;
}
Shader ShaderSystem::gizmo(){
    return __gizmo;
}
Shader ShaderSystem::color3d(){
    return __color3d;
}
Shader ShaderSystem::color2d(){
    return __color2d;
}
Shader ShaderSystem::alphaZero3D(){
    return __alphaZero3D;
}
Shader ShaderSystem::color3DInstancing(){
    return __color3DInstancing;
}
Shader ShaderSystem::layersUpdate(){
    return __layersUpdate;
}
Shader ShaderSystem::solidPaintingShader(){
    return __solidPaintingShader;
}
Shader ShaderSystem::removeUnselectedFacesShader(){
    return __removeUnselectedFacesShader;
}




Shader __textureModifierShader;
Shader __dustModifierShader;
Shader __asphaltModifierShader;
Shader __liquidModifierShader;
Shader __mossModifierShader;
Shader __rustModifierShader;
Shader __skinModifierShader;
Shader __solidModifierShader;
Shader __woodenModifierShader;
Shader __mathModifierShader;

Shader MaterialModifierShaders::textureModifierShader(){
    return __textureModifierShader;
}
Shader MaterialModifierShaders::dustModifierShader(){
    return __dustModifierShader;
}
Shader MaterialModifierShaders::asphaltModifierShader(){
    return __asphaltModifierShader;
}
Shader MaterialModifierShaders::liquidModifierShader(){
    return __liquidModifierShader;
}
Shader MaterialModifierShaders::mossModifierShader(){
    return __mossModifierShader;
}
Shader MaterialModifierShaders::rustModifierShader(){
    return __rustModifierShader;
}
Shader MaterialModifierShaders::skinModifierShader(){
    return __skinModifierShader;
}
Shader MaterialModifierShaders::solidModifierShader(){
    return __solidModifierShader;
}
Shader MaterialModifierShaders::woodenModifierShader(){
    return __woodenModifierShader;
}
Shader MaterialModifierShaders::mathModifierShader(){
    return __mathModifierShader;
}

void MaterialModifierShaders::init_shaders(){
    __textureModifierShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert","LigidPainter/Resources/Shaders/MaterialModifiers/TextureModifier.frag");
    __dustModifierShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert","LigidPainter/Resources/Shaders/MaterialModifiers/DustModifier.frag");
    __asphaltModifierShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert","LigidPainter/Resources/Shaders/MaterialModifiers/AsphaltModifier.frag");
    __liquidModifierShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert","LigidPainter/Resources/Shaders/MaterialModifiers/LiquidModifier.frag");
    __mossModifierShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert","LigidPainter/Resources/Shaders/MaterialModifiers/MossModifier.frag");
    __rustModifierShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert","LigidPainter/Resources/Shaders/MaterialModifiers/RustModifier.frag");
    __skinModifierShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert","LigidPainter/Resources/Shaders/MaterialModifiers/SkinModifier.frag");
    __solidModifierShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert","LigidPainter/Resources/Shaders/MaterialModifiers/SolidModifier.frag");
    __woodenModifierShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_model_UV.vert","LigidPainter/Resources/Shaders/MaterialModifiers/WoodenModifier.frag");
    __mathModifierShader.loadShaderPP("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert","LigidPainter/Resources/Shaders/MaterialModifiers/MathModifier.frag");
}