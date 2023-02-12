#include<iostream>

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <filesystem>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Core/Model/model.h"

#include "Core/Render/Render.h"
#include "Core/LigidPainter.h"
#include "Core/UI/UserInterface.h"
#include "Core/Utilities.h"
#include "Core/gl.h"
#include "Core/Texture/Texture.h"

#include "tinyfiledialogs.h"




std::vector<UIElement> UserInterface::getUiElements(Icons icons) {

	std::vector<UIElement> UIElements;

	ColorData colorData;


	//---------------------------MODEL PANEL---------------------------

	UIElement uploadingModelPathTextBoxElement;
    uploadingModelPathTextBoxElement.textBox.position_x = 0.0f;
    uploadingModelPathTextBoxElement.textBox.position_y = 0.6f;
    uploadingModelPathTextBoxElement.textBox.position_z = 0.9f;
    uploadingModelPathTextBoxElement.textBox.width = 0.12;
    uploadingModelPathTextBoxElement.textBox.height = 0.03;
    uploadingModelPathTextBoxElement.textBox.text = "";
    uploadingModelPathTextBoxElement.panel = 1;
    uploadingModelPathTextBoxElement.attachedToMainPanel = true;
	uploadingModelPathTextBoxElement.type = "textBox";
    UIElements.push_back(uploadingModelPathTextBoxElement);//1

	UIElement uploadingModelPathTextElement;
    uploadingModelPathTextElement.text.positionX = -0.05f;
    uploadingModelPathTextElement.text.positionY = 0.64f;
    uploadingModelPathTextElement.text.text = "File Path";
    uploadingModelPathTextElement.text.scale = 0.00022f;
    uploadingModelPathTextElement.text.color = colorData.textBoxColor;
    uploadingModelPathTextElement.panel = 1;
    uploadingModelPathTextElement.attachedToMainPanel = true;
    uploadingModelPathTextElement.type = "text";
	UIElements.push_back(uploadingModelPathTextElement);//2

	UIElement loadModelButtonElement;
    loadModelButtonElement.button.positionX = 0.0f;
    loadModelButtonElement.button.positionY = 0.45f;
    loadModelButtonElement.button.positionZ = 0.9f;
    loadModelButtonElement.button.width = 0.08f;
    loadModelButtonElement.button.height = 0.04f;
    loadModelButtonElement.button.color = colorData.buttonColor;
    loadModelButtonElement.button.colorHover = colorData.buttonColorHover;
    loadModelButtonElement.button.transitionMixVal = 0.0f;
    loadModelButtonElement.button.buttonCurveReduce = 10.0f;
    loadModelButtonElement.button.text = "Load";
    loadModelButtonElement.button.textRatio = 0.022f;
    loadModelButtonElement.panel = 1;
    loadModelButtonElement.attachedToMainPanel = true;
	loadModelButtonElement.type = "button";
    UIElements.push_back(loadModelButtonElement);//3

	UIElement autoTriangulateCheckBoxElement;
    autoTriangulateCheckBoxElement.checkBox.positionX = -0.08f;
    autoTriangulateCheckBoxElement.checkBox.positionY = 0.3f;
    autoTriangulateCheckBoxElement.checkBox.text = "Auto triangulate";
    autoTriangulateCheckBoxElement.checkBox.checked = true;
    autoTriangulateCheckBoxElement.panel = 1;
    autoTriangulateCheckBoxElement.attachedToMainPanel = true;
	autoTriangulateCheckBoxElement.type = "checkBox";
    UIElements.push_back(autoTriangulateCheckBoxElement);//4
	

	UIElement backfaceCullingCheckBoxElement;
    backfaceCullingCheckBoxElement.checkBox.positionX = -0.08f;
    backfaceCullingCheckBoxElement.checkBox.positionY = 0.2f;
    backfaceCullingCheckBoxElement.checkBox.text = "Backface Culling";
    backfaceCullingCheckBoxElement.panel = 1;
    backfaceCullingCheckBoxElement.attachedToMainPanel = true;
	backfaceCullingCheckBoxElement.type = "checkBox";
    UIElements.push_back(backfaceCullingCheckBoxElement);//5


	UIElement modelPanel3DModelsTextElement;
    modelPanel3DModelsTextElement.text.positionX = -0.05f;
    modelPanel3DModelsTextElement.text.positionY = 0.08f-0.1f;
    modelPanel3DModelsTextElement.text.text = "3D Models";
    modelPanel3DModelsTextElement.text.scale = 0.00022f;
    modelPanel3DModelsTextElement.text.color = colorData.textBoxColor;
    modelPanel3DModelsTextElement.panel = 1;
    modelPanel3DModelsTextElement.attachedToMainPanel = true;
	modelPanel3DModelsTextElement.type = "text";
    UIElements.push_back(modelPanel3DModelsTextElement);//6


	UIElement loadCustomModelButtonElement;
    loadCustomModelButtonElement.button.positionX = 0.0f;
    loadCustomModelButtonElement.button.positionY = -0.3f;
    loadCustomModelButtonElement.button.positionZ = 0.9f;
    loadCustomModelButtonElement.button.width = 0.06f;
    loadCustomModelButtonElement.button.height = 0.04f;
    loadCustomModelButtonElement.button.color = colorData.buttonColor;
    loadCustomModelButtonElement.button.colorHover = colorData.buttonColorHover;
    loadCustomModelButtonElement.button.transitionMixVal = 0.0f;
    loadCustomModelButtonElement.button.buttonCurveReduce = 10.0f;
    loadCustomModelButtonElement.button.text = "Custom";
    loadCustomModelButtonElement.button.textRatio = 0.032f;
    loadCustomModelButtonElement.panel = 1;
    loadCustomModelButtonElement.attachedToMainPanel = true;
    loadCustomModelButtonElement.type = "button";
    UIElements.push_back(loadCustomModelButtonElement);//7


     //---------------------------PAINTING PANEL---------------------------

    UIElement addBrushMaskTextureButtonElement;
    addBrushMaskTextureButtonElement.button.positionX = 0.0f;
    addBrushMaskTextureButtonElement.button.positionY = 0.85f;
    addBrushMaskTextureButtonElement.button.positionZ = 0.9f;
    addBrushMaskTextureButtonElement.button.width = 0.1f;
    addBrushMaskTextureButtonElement.button.height = 0.04f;
    addBrushMaskTextureButtonElement.button.color = colorData.buttonColor;
    addBrushMaskTextureButtonElement.button.colorHover = colorData.buttonColorHover;
    addBrushMaskTextureButtonElement.button.transitionMixVal = 0.0f;
    addBrushMaskTextureButtonElement.button.buttonCurveReduce = 10.0f;
    addBrushMaskTextureButtonElement.button.text = "Add Mask Texture";
    addBrushMaskTextureButtonElement.button.textRatio = 0.075f;
    addBrushMaskTextureButtonElement.panel = 3;
    addBrushMaskTextureButtonElement.attachedToMainPanel = true;
    addBrushMaskTextureButtonElement.type = "button";
    UIElements.push_back(addBrushMaskTextureButtonElement);//8
		
    UIElement useNegativeCheckBoxElement;
    useNegativeCheckBoxElement.checkBox.positionX = -0.03f;
    useNegativeCheckBoxElement.checkBox.positionY = 0.45f;
    useNegativeCheckBoxElement.checkBox.text = "Invert";
    useNegativeCheckBoxElement.panel = 3;
    useNegativeCheckBoxElement.attachedToMainPanel = true;
    useNegativeCheckBoxElement.type = "checkBox";
	UIElements.push_back(useNegativeCheckBoxElement);//9

    UIElement maskTextureFileNameTextElement;
    maskTextureFileNameTextElement.text.positionX = -0.03f;
    maskTextureFileNameTextElement.text.positionY = 0.37f;
    maskTextureFileNameTextElement.text.text = "PlainCircle.png";
    maskTextureFileNameTextElement.text.scale = 0.00022f;
    maskTextureFileNameTextElement.text.color = colorData.textColor;
    maskTextureFileNameTextElement.panel = 3;
    maskTextureFileNameTextElement.attachedToMainPanel = true;
    maskTextureFileNameTextElement.type = "text";
    UIElements.push_back(maskTextureFileNameTextElement);//10
	 
	//Brush size range bar
    UIElement brushSizeTextElement;
    brushSizeTextElement.text.positionX = -0.11f;
    brushSizeTextElement.text.positionY = 0.25f+0.06f-0.035f;
    brushSizeTextElement.text.text = "Size";
    brushSizeTextElement.text.scale = 0.00022f;
    brushSizeTextElement.text.color = colorData.textBoxColor;
    brushSizeTextElement.panel = 3;
    brushSizeTextElement.attachedToMainPanel = true;
    brushSizeTextElement.type = "text";
    UIElements.push_back(brushSizeTextElement);//11

	UIElement brushSizeRangeBarElement;
	brushSizeRangeBarElement.rangeBar.positionX = 0.0f;
	brushSizeRangeBarElement.rangeBar.positionY = 0.22f+0.02f+0.02f;
	brushSizeRangeBarElement.rangeBar.value = 0.0f;
	brushSizeRangeBarElement.panel = 3;
	brushSizeRangeBarElement.attachedToMainPanel = true;
	brushSizeRangeBarElement.type = "rangeBar";
    UIElements.push_back(brushSizeRangeBarElement);//12

	//Brush blur range bar
    UIElement brushBlurTextElement;
    brushBlurTextElement.text.positionX = -0.11f;
    brushBlurTextElement.text.positionY = +0.125f+0.08f-0.035f;
    brushBlurTextElement.text.text = "Blur";
    brushBlurTextElement.text.scale = 0.00022f;
    brushBlurTextElement.text.color = colorData.textBoxColor;
    brushBlurTextElement.panel = 3;
    brushBlurTextElement.attachedToMainPanel = true;
    brushBlurTextElement.type = "text";
    UIElements.push_back(brushBlurTextElement);//13

	UIElement brushBlurRangeBarElement;
	brushBlurRangeBarElement.rangeBar.positionX = 0.0f;
	brushBlurRangeBarElement.rangeBar.positionY = 0.09f+0.04f+0.02f;
	brushBlurRangeBarElement.rangeBar.value = -0.11f;
	brushBlurRangeBarElement.panel = 3;
	brushBlurRangeBarElement.attachedToMainPanel = true;
	brushBlurRangeBarElement.type = "rangeBar";
    UIElements.push_back(brushBlurRangeBarElement);//14

	//Brush rotation range bar
    UIElement brushRotationTextElement;
    brushRotationTextElement.text.positionX = -0.11f;
    brushRotationTextElement.text.positionY = -0.005f+0.1f-0.035f;
    brushRotationTextElement.text.text = "Rotation";
    brushRotationTextElement.text.scale = 0.00022f;
    brushRotationTextElement.text.color = colorData.textBoxColor;
    brushRotationTextElement.panel = 3;
    brushRotationTextElement.attachedToMainPanel = true;
    brushRotationTextElement.type = "text";
    UIElements.push_back(brushRotationTextElement);//15

	UIElement brushRotationRangeBarElement;
	brushRotationRangeBarElement.rangeBar.positionX = 0.0f;
	brushRotationRangeBarElement.rangeBar.positionY = -0.04f+0.06f+0.02f;
	brushRotationRangeBarElement.rangeBar.value = -0.11f;
	brushRotationRangeBarElement.panel = 3;
	brushRotationRangeBarElement.attachedToMainPanel = true;
	brushRotationRangeBarElement.type = "rangeBar";
    UIElements.push_back(brushRotationRangeBarElement);//16

	//Brush opacity range bar
    UIElement brushOpacityTextElement;
    brushOpacityTextElement.text.positionX = -0.11f;
    brushOpacityTextElement.text.positionY = -0.135f+0.12f-0.035f;
    brushOpacityTextElement.text.text = "Opacity";
    brushOpacityTextElement.text.scale = 0.00022f;
    brushOpacityTextElement.text.color = colorData.textBoxColor;
    brushOpacityTextElement.panel = 3;
    brushOpacityTextElement.attachedToMainPanel = true;
    brushOpacityTextElement.type = "text";
    UIElements.push_back(brushOpacityTextElement);//17

	UIElement brushOpacityRangeBarElement;
	brushOpacityRangeBarElement.rangeBar.positionX = 0.0f;
	brushOpacityRangeBarElement.rangeBar.positionY = -0.17f+0.08f+0.02f;
	brushOpacityRangeBarElement.rangeBar.value = 0.11f;
	brushOpacityRangeBarElement.panel = 3;
	brushOpacityRangeBarElement.attachedToMainPanel = true;
	brushOpacityRangeBarElement.type = "rangeBar";
    UIElements.push_back(brushOpacityRangeBarElement);//18
    
	//Brush spacing range bar
    UIElement brushSpacingTextElement;
    brushSpacingTextElement.text.positionX = -0.11f;
    brushSpacingTextElement.text.positionY = -0.265f+0.14f-0.035f;
    brushSpacingTextElement.text.text = "Spacing";
    brushSpacingTextElement.text.scale = 0.00022f;
    brushSpacingTextElement.text.color = colorData.textBoxColor;
    brushSpacingTextElement.panel = 3;
    brushSpacingTextElement.attachedToMainPanel = true;
    brushSpacingTextElement.type = "text";
    UIElements.push_back(brushSpacingTextElement);//19

	UIElement brushSpacingRangeBarElement;
	brushSpacingRangeBarElement.rangeBar.positionX = 0.0f;
	brushSpacingRangeBarElement.rangeBar.positionY = -0.30f+0.1f+0.02f;
	brushSpacingRangeBarElement.rangeBar.value = -0.11f;
	brushSpacingRangeBarElement.panel = 3;
	brushSpacingRangeBarElement.attachedToMainPanel = true;
	brushSpacingRangeBarElement.type = "rangeBar";
    UIElements.push_back(brushSpacingRangeBarElement);//20

	//Brush spacing range bar
    UIElement brushBordersTextElement;
    brushBordersTextElement.text.positionX = -0.11f;
    brushBordersTextElement.text.positionY = -0.285f + 0.02f;
    brushBordersTextElement.text.text = "Borders";
    brushBordersTextElement.text.scale = 0.00022f;
    brushBordersTextElement.text.color = colorData.textBoxColor;
    brushBordersTextElement.panel = 3;
    brushBordersTextElement.attachedToMainPanel = true;
    brushBordersTextElement.type = "text";
    UIElements.push_back(brushBordersTextElement);//21

	UIElement brushBordersRangeBarElement;
	brushBordersRangeBarElement.rangeBar.positionX = 0.0f;
	brushBordersRangeBarElement.rangeBar.positionY = -0.31f+0.02f;
	brushBordersRangeBarElement.rangeBar.value = 0.06f;
	brushBordersRangeBarElement.panel = 3;
	brushBordersRangeBarElement.attachedToMainPanel = true;
	brushBordersRangeBarElement.type = "rangeBar";
    UIElements.push_back(brushBordersRangeBarElement);//22

	UIElement mirrorXCheckBoxElement;
    mirrorXCheckBoxElement.checkBox.positionX = -0.16f;
    mirrorXCheckBoxElement.checkBox.positionY = 0.9f;
    mirrorXCheckBoxElement.checkBox.text = "X";
    mirrorXCheckBoxElement.panel = 3;
    mirrorXCheckBoxElement.attachedToMainPanel = false;
    mirrorXCheckBoxElement.type = "checkBox";
    UIElements.push_back(mirrorXCheckBoxElement);//23

	UIElement mirrorYCheckBoxElement;
    mirrorYCheckBoxElement.checkBox.positionX = -0.10f;
    mirrorYCheckBoxElement.checkBox.positionY = 0.9f;
    mirrorYCheckBoxElement.checkBox.text = "Y";
    mirrorYCheckBoxElement.panel = 3;
    mirrorYCheckBoxElement.attachedToMainPanel = false;
    mirrorYCheckBoxElement.type = "checkBox";
    UIElements.push_back(mirrorYCheckBoxElement);//24

	UIElement mirrorZCheckBoxElement;
    mirrorZCheckBoxElement.checkBox.positionX = -0.04f;
    mirrorZCheckBoxElement.checkBox.positionY = 0.9f;
    mirrorZCheckBoxElement.checkBox.text = "Z";
    mirrorZCheckBoxElement.panel = 3;
    mirrorZCheckBoxElement.attachedToMainPanel = false;
    mirrorZCheckBoxElement.type = "checkBox";
    UIElements.push_back(mirrorZCheckBoxElement);//25
	
    //---------------------------EXPORT PANEL---------------------------

	
    UIElement exportingPathTextBoxElement;
    exportingPathTextBoxElement.textBox.position_x = 0.0f;
    exportingPathTextBoxElement.textBox.position_y = 0.6f;
    exportingPathTextBoxElement.textBox.position_z = 0.9f;
    exportingPathTextBoxElement.textBox.width = 0.12;
    exportingPathTextBoxElement.textBox.height = 0.03;
    exportingPathTextBoxElement.textBox.text = "Choose Destination Path";
    exportingPathTextBoxElement.panel = 4;
    exportingPathTextBoxElement.attachedToMainPanel = true;
	exportingPathTextBoxElement.type = "textBox";
    UIElements.push_back(exportingPathTextBoxElement);//26

    UIElement exportingFolderNameTextBoxElement;
    exportingFolderNameTextBoxElement.textBox.position_x = 0.0f;
    exportingFolderNameTextBoxElement.textBox.position_y = 0.5f;
    exportingFolderNameTextBoxElement.textBox.position_z = 0.9f;
    exportingFolderNameTextBoxElement.textBox.width = 0.12;
    exportingFolderNameTextBoxElement.textBox.height = 0.03;
    exportingFolderNameTextBoxElement.textBox.text = "LP_Export";
    exportingFolderNameTextBoxElement.panel = 4;
    exportingFolderNameTextBoxElement.attachedToMainPanel = true;
    exportingFolderNameTextBoxElement.type = "textBox";
    UIElements.push_back(exportingFolderNameTextBoxElement);//27

    UIElement jpgCheckBoxElement;
    jpgCheckBoxElement.checkBox.positionX = -0.11f;
    jpgCheckBoxElement.checkBox.positionY = 0.4f;
    jpgCheckBoxElement.checkBox.text = "";
    jpgCheckBoxElement.panel = 4;
    jpgCheckBoxElement.attachedToMainPanel = true;
    jpgCheckBoxElement.type = "checkBox";
    UIElements.push_back(jpgCheckBoxElement);//28

    UIElement pngCheckBoxElement;
    pngCheckBoxElement.checkBox.positionX = 0.05f;
    pngCheckBoxElement.checkBox.positionY = 0.4f;
    pngCheckBoxElement.checkBox.text = "";
    pngCheckBoxElement.panel = 4;
    pngCheckBoxElement.attachedToMainPanel = true;
    pngCheckBoxElement.type = "checkBox";
    UIElements.push_back(pngCheckBoxElement);//29

    UIElement downloadButtonElement;
    downloadButtonElement.button.positionX = 0.0f;
    downloadButtonElement.button.positionY = 0.2f;
    downloadButtonElement.button.positionZ = 0.9f;
    downloadButtonElement.button.width = 0.1f;
    downloadButtonElement.button.height = 0.04f;
    downloadButtonElement.button.color = colorData.buttonColor;
    downloadButtonElement.button.colorHover = colorData.buttonColorHover;
    downloadButtonElement.button.transitionMixVal = 0.0f;
    downloadButtonElement.button.buttonCurveReduce = 10.0f;
    downloadButtonElement.button.text = "Export";
    downloadButtonElement.button.textRatio = 0.045f;
    downloadButtonElement.panel = 4;
    downloadButtonElement.attachedToMainPanel = true;
    downloadButtonElement.type = "button";
    UIElements.push_back(downloadButtonElement);//30
    

    //---------------------------SETTINGS PANEL---------------------------



    UIElement skyboxCheckBoxElement;
    skyboxCheckBoxElement.checkBox.positionX = -0.03f;
    skyboxCheckBoxElement.checkBox.positionY = 0.0f;
    skyboxCheckBoxElement.checkBox.text = "skybox";
    skyboxCheckBoxElement.checkBox.checked = true;
    skyboxCheckBoxElement.checkBox.mouseHover = false;
    skyboxCheckBoxElement.panel = 5;
    skyboxCheckBoxElement.attachedToMainPanel = true;
    skyboxCheckBoxElement.type = "checkBox";
    UIElements.push_back(skyboxCheckBoxElement);//31

    UIElement skyBoxExposureRangeBarElement;
	skyBoxExposureRangeBarElement.rangeBar.positionX = 0.0f;
	skyBoxExposureRangeBarElement.rangeBar.positionY = -0.67f;
	skyBoxExposureRangeBarElement.rangeBar.value = 0.f;
	skyBoxExposureRangeBarElement.panel = 5;
	skyBoxExposureRangeBarElement.attachedToMainPanel = true;
	skyBoxExposureRangeBarElement.type = "rangeBar";
    UIElements.push_back(skyBoxExposureRangeBarElement);//32

    UIElement skyBoxRotationRangeBarElement;
	skyBoxRotationRangeBarElement.rangeBar.positionX = 0.0f;
	skyBoxRotationRangeBarElement.rangeBar.positionY = -0.75f;
	skyBoxRotationRangeBarElement.rangeBar.value = 0.f;
	skyBoxRotationRangeBarElement.panel = 5;
	skyBoxRotationRangeBarElement.attachedToMainPanel = true;
	skyBoxRotationRangeBarElement.type = "rangeBar";
    UIElements.push_back(skyBoxRotationRangeBarElement);//32
    
    UIElement stabilizeFpsCheckBoxElement;
    stabilizeFpsCheckBoxElement.checkBox.positionX = -0.03f;
    stabilizeFpsCheckBoxElement.checkBox.positionY = 0.27f;
    stabilizeFpsCheckBoxElement.checkBox.text = "sync";
    stabilizeFpsCheckBoxElement.checkBox.checked = true;
    stabilizeFpsCheckBoxElement.checkBox.mouseHover = false;
    stabilizeFpsCheckBoxElement.panel = 5;
    stabilizeFpsCheckBoxElement.attachedToMainPanel = true;
    stabilizeFpsCheckBoxElement.type = "checkBox";
    UIElements.push_back(stabilizeFpsCheckBoxElement);//33

    UIElement skyBoxExposureTextElement;
    skyBoxExposureTextElement.text.positionX = -0.11f;
    skyBoxExposureTextElement.text.positionY = -0.65;
    skyBoxExposureTextElement.text.text = "Exposure";
    skyBoxExposureTextElement.text.scale = 0.00022f;
    skyBoxExposureTextElement.text.color = colorData.textBoxColor;
    skyBoxExposureTextElement.panel = 5;
    skyBoxExposureTextElement.attachedToMainPanel = true;
    skyBoxExposureTextElement.type = "text";
    UIElements.push_back(skyBoxExposureTextElement);//34

    UIElement skyBoxRotationTextElement;
    skyBoxRotationTextElement.text.positionX = -0.11f;
    skyBoxRotationTextElement.text.positionY = -0.73;
    skyBoxRotationTextElement.text.text = "Rotation";
    skyBoxRotationTextElement.text.scale = 0.00022f;
    skyBoxRotationTextElement.text.color = colorData.textBoxColor;
    skyBoxRotationTextElement.panel = 5;
    skyBoxRotationTextElement.attachedToMainPanel = true;
    skyBoxRotationTextElement.type = "text";
    UIElements.push_back(skyBoxRotationTextElement);//35

    UIElement imageCheckBoxElement;
    imageCheckBoxElement.checkBox.positionX = -0.03f;
    imageCheckBoxElement.checkBox.positionY = 0.16f;
    imageCheckBoxElement.checkBox.text = "Image";
    imageCheckBoxElement.checkBox.checked = true;
    imageCheckBoxElement.checkBox.mouseHover = false;
    imageCheckBoxElement.panel = 5;
    imageCheckBoxElement.attachedToMainPanel = true;
    imageCheckBoxElement.type = "checkBox";
    UIElements.push_back(imageCheckBoxElement);//36

    UIElement viewportImageTextBoxElement;
    viewportImageTextBoxElement.textBox.position_x = 0.0f;
    viewportImageTextBoxElement.textBox.position_y = 0.1f;
    viewportImageTextBoxElement.textBox.position_z = 0.9f;
    viewportImageTextBoxElement.textBox.width = 0.12;
    viewportImageTextBoxElement.textBox.height = 0.03;
    viewportImageTextBoxElement.textBox.text = "Add Image";
    viewportImageTextBoxElement.panel = 5;
    viewportImageTextBoxElement.attachedToMainPanel = true;
    viewportImageTextBoxElement.type = "textBox";
    UIElements.push_back(viewportImageTextBoxElement);//37
    
    //---------------------------GENERATOR PANEL---------------------------
    

    UIElement normalmapCheckBoxElement;
    normalmapCheckBoxElement.checkBox.positionX = -0.03f;
    normalmapCheckBoxElement.checkBox.positionY = 0.16f;
    normalmapCheckBoxElement.checkBox.text = "Normal map";
    normalmapCheckBoxElement.checkBox.checked = true;
    normalmapCheckBoxElement.checkBox.mouseHover = false;
    normalmapCheckBoxElement.panel = 6;
    normalmapCheckBoxElement.attachedToMainPanel = true;
    normalmapCheckBoxElement.type = "checkBox";
    UIElements.push_back(normalmapCheckBoxElement);//38

    UIElement normalStrengthTextElement;
    normalStrengthTextElement.text.positionX = -0.11f;
    normalStrengthTextElement.text.positionY = 0.1;
    normalStrengthTextElement.text.text = "Strength";
    normalStrengthTextElement.text.scale = 0.00022f;
    normalStrengthTextElement.text.color = colorData.textBoxColor;
    normalStrengthTextElement.panel = 6;
    normalStrengthTextElement.attachedToMainPanel = true;
    normalStrengthTextElement.type = "text";
    UIElements.push_back(normalStrengthTextElement);//39

    UIElement normalStrengthRangeBarElement;
	normalStrengthRangeBarElement.rangeBar.positionX = 0.0f;
	normalStrengthRangeBarElement.rangeBar.positionY = 0.06f;
	normalStrengthRangeBarElement.rangeBar.value = 0.f;
	normalStrengthRangeBarElement.panel = 6;
	normalStrengthRangeBarElement.attachedToMainPanel = true;
	normalStrengthRangeBarElement.type = "rangeBar";
    UIElements.push_back(normalStrengthRangeBarElement);//40

    UIElement noiseCheckBoxElement;
    noiseCheckBoxElement.checkBox.positionX = -0.03f;
    noiseCheckBoxElement.checkBox.positionY = -0.06f;
    noiseCheckBoxElement.checkBox.text = "Noise";
    noiseCheckBoxElement.checkBox.checked = false;
    noiseCheckBoxElement.checkBox.mouseHover = false;
    noiseCheckBoxElement.panel = 6;
    noiseCheckBoxElement.attachedToMainPanel = true;
    noiseCheckBoxElement.type = "checkBox";
    UIElements.push_back(noiseCheckBoxElement);//38

    UIElement noiseStrengthTextElement;
    noiseStrengthTextElement.text.positionX = -0.11f;
    noiseStrengthTextElement.text.positionY = -0.12;
    noiseStrengthTextElement.text.text = "Strength";
    noiseStrengthTextElement.text.scale = 0.00022f;
    noiseStrengthTextElement.text.color = colorData.textBoxColor;
    noiseStrengthTextElement.panel = 6;
    noiseStrengthTextElement.attachedToMainPanel = true;
    noiseStrengthTextElement.type = "text";
    UIElements.push_back(noiseStrengthTextElement);//39

    UIElement noiseStrengthRangeBarElement;
	noiseStrengthRangeBarElement.rangeBar.positionX = 0.0f;
	noiseStrengthRangeBarElement.rangeBar.positionY = -0.16f;
	noiseStrengthRangeBarElement.rangeBar.value = 0.f;
	noiseStrengthRangeBarElement.panel = 6;
	noiseStrengthRangeBarElement.attachedToMainPanel = true;
	noiseStrengthRangeBarElement.type = "rangeBar";
    UIElements.push_back(noiseStrengthRangeBarElement);//40
    
    UIElement generateTextureButtonElement;
    generateTextureButtonElement.button.positionX = 0.0f;
    generateTextureButtonElement.button.positionY = -0.26f;
    generateTextureButtonElement.button.positionZ = 0.9f;
    generateTextureButtonElement.button.width = 0.05f;
    generateTextureButtonElement.button.height = 0.04f;
    generateTextureButtonElement.button.color = colorData.buttonColor;
    generateTextureButtonElement.button.colorHover = colorData.buttonColorHover;
    generateTextureButtonElement.button.transitionMixVal = 0.0f;
    generateTextureButtonElement.button.buttonCurveReduce = 10.0f;
    generateTextureButtonElement.button.text = "Generate";
    generateTextureButtonElement.button.textRatio = 0.038f;
    generateTextureButtonElement.panel = 6;
    generateTextureButtonElement.attachedToMainPanel = true;
    generateTextureButtonElement.type = "button";
    UIElements.push_back(generateTextureButtonElement);//41
    
    UIElement bakeButtonElement;
    bakeButtonElement.button.positionX = 0.0f;
    bakeButtonElement.button.positionY = -0.46f;
    bakeButtonElement.button.positionZ = 0.9f;
    bakeButtonElement.button.width = 0.05f;
    bakeButtonElement.button.height = 0.04f;
    bakeButtonElement.button.color = colorData.buttonColor;
    bakeButtonElement.button.colorHover = colorData.buttonColorHover;
    bakeButtonElement.button.transitionMixVal = 0.0f;
    bakeButtonElement.button.buttonCurveReduce = 10.0f;
    bakeButtonElement.button.text = "Bake";
    bakeButtonElement.button.textRatio = 0.038f;
    bakeButtonElement.panel = 6;
    bakeButtonElement.attachedToMainPanel = true;
    bakeButtonElement.type = "button";
    UIElements.push_back(bakeButtonElement);//41

	//-------------ICONS-------------


    UIElement jpgIconElement;
    jpgIconElement.icon.positionX = -0.06f;
    jpgIconElement.icon.positionY = 0.4f;
    jpgIconElement.icon.positionZ = 0.9f;
    jpgIconElement.icon.width = 0.05f;
    jpgIconElement.icon.height = 0.065f;
    jpgIconElement.icon.color = colorData.iconColor;
    jpgIconElement.icon.colorHover = colorData.iconColorHover;
    jpgIconElement.icon.mixVal = 0.0f;
    jpgIconElement.icon.icon = icons.JpgFile;
    jpgIconElement.panel = 4;
    jpgIconElement.attachedToMainPanel = true;
    jpgIconElement.type = "icon";
    UIElements.push_back(jpgIconElement);//32

    UIElement pngIconElement;
    pngIconElement.icon.positionX = +0.1f;
    pngIconElement.icon.positionY = 0.4f;
    pngIconElement.icon.positionZ = 0.9f;
    pngIconElement.icon.width = 0.05f;
    pngIconElement.icon.height = 0.065f;
    pngIconElement.icon.color = colorData.iconColor;
    pngIconElement.icon.colorHover = colorData.iconColorHover;
    pngIconElement.icon.mixVal = 0.0f;
    pngIconElement.icon.icon = icons.PngFile;
    pngIconElement.panel = 4;
    pngIconElement.attachedToMainPanel = true;
    pngIconElement.type = "icon";
    UIElements.push_back(pngIconElement);//33

	UIElement folderIconElement;
    folderIconElement.icon.positionX = 0.1f;
    folderIconElement.icon.positionY = 0.6f;
    folderIconElement.icon.positionZ = 0.99f;
    folderIconElement.icon.width = 0.02f;
    folderIconElement.icon.height = 0.04f;
    folderIconElement.icon.color = colorData.iconColor;
    folderIconElement.icon.colorHover = colorData.iconColorHover;
    folderIconElement.icon.mixVal = 0.0f;
    folderIconElement.icon.icon = icons.Folder;
    folderIconElement.panel = 1;
    folderIconElement.attachedToMainPanel = true;
    folderIconElement.type = "icon";
    UIElements.push_back(folderIconElement);//34

    //Load plane button
	UIElement loadPlaneModelButtonElement;
    loadPlaneModelButtonElement.icon.positionX = 0.0f;
    loadPlaneModelButtonElement.icon.positionY = -0.1f;
    loadPlaneModelButtonElement.icon.positionZ = 0.9f;
    loadPlaneModelButtonElement.icon.width = 0.03f;
    loadPlaneModelButtonElement.icon.height = 0.055f;
    loadPlaneModelButtonElement.icon.color = colorData.buttonColor;
    loadPlaneModelButtonElement.icon.colorHover = colorData.buttonColorHover;
    loadPlaneModelButtonElement.icon.mixVal = 0.0f;
    loadPlaneModelButtonElement.icon.icon = icons.Circle;
    loadPlaneModelButtonElement.icon.clickable = true;
    loadPlaneModelButtonElement.panel = 1;
    loadPlaneModelButtonElement.attachedToMainPanel = true;
	loadPlaneModelButtonElement.type = "icon";
    UIElements.push_back(loadPlaneModelButtonElement);//35

	//Load sphere button 
	UIElement loadSphereModelButtonElement;
    loadSphereModelButtonElement.icon.positionX = 0.0f;
    loadSphereModelButtonElement.icon.positionY = -0.2f;
    loadSphereModelButtonElement.icon.positionZ = 0.91f;
    loadSphereModelButtonElement.icon.width = 0.03f;
    loadSphereModelButtonElement.icon.height = 0.055f;
    loadSphereModelButtonElement.icon.color = colorData.buttonColor;
    loadSphereModelButtonElement.icon.colorHover = colorData.buttonColorHover;
    loadSphereModelButtonElement.icon.mixVal = 0.0f;
    loadSphereModelButtonElement.icon.icon = icons.Circle;
    loadSphereModelButtonElement.icon.clickable = true;
    loadSphereModelButtonElement.panel = 1;
    loadSphereModelButtonElement.attachedToMainPanel = true;
	loadSphereModelButtonElement.type = "icon";
    UIElements.push_back(loadSphereModelButtonElement);//36

	UIElement planeIconElement;
    planeIconElement.icon.positionX = 0.0f;
    planeIconElement.icon.positionY = -0.1f;
    planeIconElement.icon.positionZ = 0.99f;
    planeIconElement.icon.width = 0.0175f;
    planeIconElement.icon.height = 0.035f;
    planeIconElement.icon.color = colorData.iconColor;
    planeIconElement.icon.colorHover = colorData.iconColorHover;
    planeIconElement.icon.mixVal = 0.0f;
    planeIconElement.icon.icon = icons.Panel;
    planeIconElement.panel = 1;
    planeIconElement.attachedToMainPanel = true;
	planeIconElement.type = "icon";
    UIElements.push_back(planeIconElement);//37

    UIElement sphereIconElement;
    sphereIconElement.icon.positionX = 0.0f;
    sphereIconElement.icon.positionY = -0.2f;
    sphereIconElement.icon.positionZ = 0.99f;
    sphereIconElement.icon.width = 0.03f/1.6f;
    sphereIconElement.icon.height = 0.055f/1.6f;
    sphereIconElement.icon.color = colorData.iconColor;
    sphereIconElement.icon.colorHover = colorData.iconColorHover;
    sphereIconElement.icon.mixVal = 0.0f;
    sphereIconElement.icon.icon = icons.Sphere;
    sphereIconElement.panel = 1;
    sphereIconElement.attachedToMainPanel = true;
	sphereIconElement.type = "icon";
    UIElements.push_back(sphereIconElement);//38
	return UIElements;
}