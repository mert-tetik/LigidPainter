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

	UIElement uploadingModelPathTextBoxElement; //DELETED
    uploadingModelPathTextBoxElement.textBox.position_x = 0.0f;
    uploadingModelPathTextBoxElement.textBox.position_y = 1000.6f;
    uploadingModelPathTextBoxElement.textBox.position_z = 0.9f;
    uploadingModelPathTextBoxElement.textBox.width = 0.12;
    uploadingModelPathTextBoxElement.textBox.height = 0.03;
    uploadingModelPathTextBoxElement.textBox.text = "";
    uploadingModelPathTextBoxElement.panel = 1;
    uploadingModelPathTextBoxElement.attachedToMainPanel = true;
	uploadingModelPathTextBoxElement.type = "textBox";
    uploadingModelPathTextBoxElement.createProject = false;
    uploadingModelPathTextBoxElement.createProjectPos = glm::vec2(-0.8f,0.95);
    UIElements.push_back(uploadingModelPathTextBoxElement);//1

	UIElement uploadingModelPathTextElement; //DELETED
    uploadingModelPathTextElement.text.positionX = -0.05f;
    uploadingModelPathTextElement.text.positionY = 1000.64f;
    uploadingModelPathTextElement.text.text = "3D Model path";
    uploadingModelPathTextElement.text.scale = 0.00022f;
    uploadingModelPathTextElement.text.color = colorData.textBoxColor;
    uploadingModelPathTextElement.panel = 1;
    uploadingModelPathTextElement.attachedToMainPanel = true;
    uploadingModelPathTextElement.type = "text";
    uploadingModelPathTextElement.createProject = false;
    uploadingModelPathTextElement.createProjectPos = glm::vec2(-0.05f,0.35f);
	UIElements.push_back(uploadingModelPathTextElement);//2

	UIElement loadModelButtonElement; //DELETED
    loadModelButtonElement.button.positionX = 0.0f;
    loadModelButtonElement.button.positionY = 1000.45f;
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

	UIElement useUVCheckBoxElement;
    useUVCheckBoxElement.checkBox.positionX = -0.08f;
    useUVCheckBoxElement.checkBox.positionY = 0.3f;
    useUVCheckBoxElement.checkBox.text = "Render the UV";
    useUVCheckBoxElement.checkBox.checked = false;
    useUVCheckBoxElement.checkBox.texture = icons.OSq;
    useUVCheckBoxElement.panel = 1;
    useUVCheckBoxElement.attachedToMainPanel = true;
	useUVCheckBoxElement.type = "checkBox";
	useUVCheckBoxElement.createProject = true;
	useUVCheckBoxElement.createProjectPos = glm::vec2(+0.2,-0.43f);
    UIElements.push_back(useUVCheckBoxElement);//4
	

	UIElement backfaceCullingCheckBoxElement;
    backfaceCullingCheckBoxElement.checkBox.positionX = -0.08f;
    backfaceCullingCheckBoxElement.checkBox.positionY = 0.2f;
    backfaceCullingCheckBoxElement.checkBox.text = "Backface Culling";
    backfaceCullingCheckBoxElement.checkBox.checked = true;
    backfaceCullingCheckBoxElement.checkBox.texture = icons.OSq;
    backfaceCullingCheckBoxElement.panel = 1;
    backfaceCullingCheckBoxElement.attachedToMainPanel = true;
	backfaceCullingCheckBoxElement.type = "checkBox";
	backfaceCullingCheckBoxElement.createProject = true;
	backfaceCullingCheckBoxElement.createProjectPos = glm::vec2(-0.2,-0.43f);
    UIElements.push_back(backfaceCullingCheckBoxElement);//5


	UIElement modelPanel3DModelsTextElement; //DELETED
    modelPanel3DModelsTextElement.text.positionX = -0.05f;
    modelPanel3DModelsTextElement.text.positionY = 1000.08f-0.1f;
    modelPanel3DModelsTextElement.text.text = "3D Models";
    modelPanel3DModelsTextElement.text.scale = 0.00022f;
    modelPanel3DModelsTextElement.text.color = colorData.textBoxColor;
    modelPanel3DModelsTextElement.panel = 1;
    modelPanel3DModelsTextElement.attachedToMainPanel = true;
	modelPanel3DModelsTextElement.type = "text";
    UIElements.push_back(modelPanel3DModelsTextElement);//6

    //Removed
	UIElement loadCustomModelButtonElement;
    loadCustomModelButtonElement.button.positionX = 0.0f;
    loadCustomModelButtonElement.button.positionY = -10000.3f;
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

    UIElement addBrushMaskTextureButtonElement;//Removed
    addBrushMaskTextureButtonElement.button.positionX = 0.0f;
    addBrushMaskTextureButtonElement.button.positionY = 1000000000000.f;
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
    useNegativeCheckBoxElement.checkBox.texture = icons.OSq;
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
    brushRotationTextElement.text.positionY = -0.135f+0.12f-0.035f;
    brushRotationTextElement.text.text = "Rotation";
    brushRotationTextElement.text.scale = 0.00022f;
    brushRotationTextElement.text.color = colorData.textBoxColor;
    brushRotationTextElement.panel = 3;
    brushRotationTextElement.attachedToMainPanel = true;
    brushRotationTextElement.type = "text";
    UIElements.push_back(brushRotationTextElement);//15

	UIElement brushRotationRangeBarElement;
	brushRotationRangeBarElement.rangeBar.positionX = 0.0f;
	brushRotationRangeBarElement.rangeBar.positionY = -0.17f+0.08f+0.02f;
	brushRotationRangeBarElement.rangeBar.value = -0.11f;
	brushRotationRangeBarElement.panel = 3;
	brushRotationRangeBarElement.attachedToMainPanel = true;
	brushRotationRangeBarElement.type = "rangeBar";
    UIElements.push_back(brushRotationRangeBarElement);//16

	//Brush opacity range bar
    UIElement brushOpacityTextElement;
    brushOpacityTextElement.text.positionX = -0.11f;
    brushOpacityTextElement.text.positionY =-0.005f+0.1f-0.035f;
    brushOpacityTextElement.text.text = "Opacity";
    brushOpacityTextElement.text.scale = 0.00022f;
    brushOpacityTextElement.text.color = colorData.textBoxColor;
    brushOpacityTextElement.panel = 3;
    brushOpacityTextElement.attachedToMainPanel = true;
    brushOpacityTextElement.type = "text";
    UIElements.push_back(brushOpacityTextElement);//17

	UIElement brushOpacityRangeBarElement;
	brushOpacityRangeBarElement.rangeBar.positionX = 0.0f;
	brushOpacityRangeBarElement.rangeBar.positionY = -0.04f+0.06f+0.02f;
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
    mirrorXCheckBoxElement.checkBox.texture = icons.OSq;
    mirrorXCheckBoxElement.panel = 3;
    mirrorXCheckBoxElement.attachedToMainPanel = false;
    mirrorXCheckBoxElement.focusMode = true;
    mirrorXCheckBoxElement.type = "checkBox";
    UIElements.push_back(mirrorXCheckBoxElement);//23

	UIElement mirrorYCheckBoxElement;
    mirrorYCheckBoxElement.checkBox.positionX = -0.10f;
    mirrorYCheckBoxElement.checkBox.positionY = 0.9f;
    mirrorYCheckBoxElement.checkBox.text = "Y";
    mirrorYCheckBoxElement.checkBox.texture = icons.OSq;
    mirrorYCheckBoxElement.panel = 3;
    mirrorYCheckBoxElement.attachedToMainPanel = false;
    mirrorYCheckBoxElement.focusMode = true;
    mirrorYCheckBoxElement.type = "checkBox";
    UIElements.push_back(mirrorYCheckBoxElement);//24

	UIElement mirrorZCheckBoxElement;
    mirrorZCheckBoxElement.checkBox.positionX = -0.04f;
    mirrorZCheckBoxElement.checkBox.positionY = 0.9f;
    mirrorZCheckBoxElement.checkBox.text = "Z";
    mirrorZCheckBoxElement.checkBox.texture = icons.OSq;
    mirrorZCheckBoxElement.panel = 3;
    mirrorZCheckBoxElement.attachedToMainPanel = false;
    mirrorZCheckBoxElement.focusMode = true;
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
    jpgCheckBoxElement.checkBox.checked = true;
    jpgCheckBoxElement.checkBox.texture = icons.OSq;
    jpgCheckBoxElement.panel = 4;
    jpgCheckBoxElement.attachedToMainPanel = true;
    jpgCheckBoxElement.type = "checkBox";
    UIElements.push_back(jpgCheckBoxElement);//28

    UIElement pngCheckBoxElement;
    pngCheckBoxElement.checkBox.positionX = 0.05f;
    pngCheckBoxElement.checkBox.positionY = 0.4f;
    pngCheckBoxElement.checkBox.text = "";
    pngCheckBoxElement.checkBox.texture = icons.OSq;
    pngCheckBoxElement.panel = 4;
    pngCheckBoxElement.attachedToMainPanel = true;
    pngCheckBoxElement.type = "checkBox";
    UIElements.push_back(pngCheckBoxElement);//29

    UIElement downloadButtonElement;
    downloadButtonElement.button.positionX = 0.0f;
    downloadButtonElement.button.positionY = 0.2f;
    downloadButtonElement.button.positionZ = 0.9f;
    downloadButtonElement.button.width = 0.08f;
    downloadButtonElement.button.height = 0.04f;
    downloadButtonElement.button.color = colorData.buttonColor;
    downloadButtonElement.button.colorHover = colorData.buttonColorHover;
    downloadButtonElement.button.transitionMixVal = 0.0f;
    downloadButtonElement.button.buttonCurveReduce = 10.0f;
    downloadButtonElement.button.text = "Export";
    downloadButtonElement.button.textRatio = 0.0325f;
    downloadButtonElement.panel = 4;
    downloadButtonElement.attachedToMainPanel = true;
    downloadButtonElement.type = "button";
    UIElements.push_back(downloadButtonElement);//30
    

    //---------------------------SETTINGS PANEL---------------------------

    const float pressYSettingsPanel = 0.4f;

    UIElement skyboxCheckBoxElement;
    skyboxCheckBoxElement.checkBox.positionX = 0.03f;
    skyboxCheckBoxElement.checkBox.positionY = -0.02f-pressYSettingsPanel;
    skyboxCheckBoxElement.checkBox.text = "skybox";
    skyboxCheckBoxElement.checkBox.checked = true;
    skyboxCheckBoxElement.checkBox.mouseHover = false;
    skyboxCheckBoxElement.checkBox.texture = icons.OSq;
    skyboxCheckBoxElement.panel = 5;
    skyboxCheckBoxElement.attachedToMainPanel = true;
    skyboxCheckBoxElement.type = "checkBox";
    UIElements.push_back(skyboxCheckBoxElement);//31

    UIElement skyBoxExposureRangeBarElement;
	skyBoxExposureRangeBarElement.rangeBar.positionX = 0.0f;
	skyBoxExposureRangeBarElement.rangeBar.positionY = -0.67f-pressYSettingsPanel;
	skyBoxExposureRangeBarElement.rangeBar.value = 0.f;
	skyBoxExposureRangeBarElement.panel = 5;
	skyBoxExposureRangeBarElement.attachedToMainPanel = true;
	skyBoxExposureRangeBarElement.type = "rangeBar";
    UIElements.push_back(skyBoxExposureRangeBarElement);//32

    UIElement skyBoxRotationRangeBarElement;
	skyBoxRotationRangeBarElement.rangeBar.positionX = 0.0f;
	skyBoxRotationRangeBarElement.rangeBar.positionY = -0.75f-pressYSettingsPanel;
	skyBoxRotationRangeBarElement.rangeBar.value = 0.f;
	skyBoxRotationRangeBarElement.panel = 5;
	skyBoxRotationRangeBarElement.attachedToMainPanel = true;
	skyBoxRotationRangeBarElement.type = "rangeBar";
    UIElements.push_back(skyBoxRotationRangeBarElement);//32
    
    UIElement realtimeMaterialRenderingCheckBoxElement;
    realtimeMaterialRenderingCheckBoxElement.checkBox.positionX = -0.1f;
    realtimeMaterialRenderingCheckBoxElement.checkBox.positionY = 1.125f-pressYSettingsPanel;
    realtimeMaterialRenderingCheckBoxElement.checkBox.text = "Realtime material rendering";
    realtimeMaterialRenderingCheckBoxElement.checkBox.checked = false;
    realtimeMaterialRenderingCheckBoxElement.checkBox.mouseHover = false;
    realtimeMaterialRenderingCheckBoxElement.checkBox.texture = icons.OSq;
    realtimeMaterialRenderingCheckBoxElement.panel = 5;
    realtimeMaterialRenderingCheckBoxElement.attachedToMainPanel = true;
    realtimeMaterialRenderingCheckBoxElement.type = "checkBox";
    UIElements.push_back(realtimeMaterialRenderingCheckBoxElement);//33
    
    UIElement stabilizeFpsCheckBoxElement;
    stabilizeFpsCheckBoxElement.checkBox.positionX = -0.1f;
    stabilizeFpsCheckBoxElement.checkBox.positionY = 1.2f-pressYSettingsPanel;
    stabilizeFpsCheckBoxElement.checkBox.text = "sync";
    stabilizeFpsCheckBoxElement.checkBox.checked = false;
    stabilizeFpsCheckBoxElement.checkBox.mouseHover = false;
    stabilizeFpsCheckBoxElement.checkBox.texture = icons.OSq;
    stabilizeFpsCheckBoxElement.panel = 5;
    stabilizeFpsCheckBoxElement.attachedToMainPanel = true;
    stabilizeFpsCheckBoxElement.type = "checkBox";
    UIElements.push_back(stabilizeFpsCheckBoxElement);//33

    UIElement skyBoxExposureTextElement;
    skyBoxExposureTextElement.text.positionX = -0.11f;
    skyBoxExposureTextElement.text.positionY = -0.65-pressYSettingsPanel;
    skyBoxExposureTextElement.text.text = "Exposure";
    skyBoxExposureTextElement.text.scale = 0.00022f;
    skyBoxExposureTextElement.text.color = colorData.textBoxColor;
    skyBoxExposureTextElement.panel = 5;
    skyBoxExposureTextElement.attachedToMainPanel = true;
    skyBoxExposureTextElement.type = "text";
    UIElements.push_back(skyBoxExposureTextElement);//34

    UIElement skyBoxRotationTextElement;
    skyBoxRotationTextElement.text.positionX = -0.11f;
    skyBoxRotationTextElement.text.positionY = -0.73-pressYSettingsPanel;
    skyBoxRotationTextElement.text.text = "Rotation";
    skyBoxRotationTextElement.text.scale = 0.00022f;
    skyBoxRotationTextElement.text.color = colorData.textBoxColor;
    skyBoxRotationTextElement.panel = 5;
    skyBoxRotationTextElement.attachedToMainPanel = true;
    skyBoxRotationTextElement.type = "text";
    UIElements.push_back(skyBoxRotationTextElement);//35

    UIElement imageCheckBoxElement;
    imageCheckBoxElement.checkBox.positionX = -0.09f;
    imageCheckBoxElement.checkBox.positionY = -0.02f-pressYSettingsPanel;
    imageCheckBoxElement.checkBox.text = "Image";
    imageCheckBoxElement.checkBox.checked = true;
    imageCheckBoxElement.checkBox.mouseHover = false;
    imageCheckBoxElement.checkBox.texture = icons.OSq;
    imageCheckBoxElement.panel = 5;
    imageCheckBoxElement.attachedToMainPanel = true;
    imageCheckBoxElement.type = "checkBox";
    UIElements.push_back(imageCheckBoxElement);//36

    UIElement viewportImageTextBoxElement;
    viewportImageTextBoxElement.textBox.position_x = 0.0f;
    viewportImageTextBoxElement.textBox.position_y = 0.05f-pressYSettingsPanel;
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
    normalmapCheckBoxElement.checkBox.positionX = -0.1f;
    normalmapCheckBoxElement.checkBox.positionY = 0.16f;
    normalmapCheckBoxElement.checkBox.text = "Normal map";
    normalmapCheckBoxElement.checkBox.checked = true;
    normalmapCheckBoxElement.checkBox.mouseHover = false;
    normalmapCheckBoxElement.checkBox.texture = icons.OSq;
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
    noiseCheckBoxElement.checkBox.positionX = -0.1f;
    noiseCheckBoxElement.checkBox.positionY = -0.06f;
    noiseCheckBoxElement.checkBox.text = "Noise";
    noiseCheckBoxElement.checkBox.checked = false;
    noiseCheckBoxElement.checkBox.mouseHover = false;
    noiseCheckBoxElement.checkBox.texture = icons.OSq;
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
    generateTextureButtonElement.button.positionY = -0.56f;
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
    bakeButtonElement.button.positionY = -0.66f;
    bakeButtonElement.button.positionZ = 0.9f;
    bakeButtonElement.button.width = 0.08f;
    bakeButtonElement.button.height = 0.04f;
    bakeButtonElement.button.color = colorData.buttonColor;
    bakeButtonElement.button.colorHover = colorData.buttonColorHover;
    bakeButtonElement.button.transitionMixVal = 0.0f;
    bakeButtonElement.button.buttonCurveReduce = 10.0f;
    bakeButtonElement.button.text = "Make Marked Nodes";
    bakeButtonElement.button.textRatio = 0.08f;
    bakeButtonElement.panel = 6;
    bakeButtonElement.attachedToMainPanel = true;
    bakeButtonElement.type = "button";
    UIElements.push_back(bakeButtonElement);//41

    UIElement outSubmeshesButtonElement;
    outSubmeshesButtonElement.button.positionX = 0.0f;
    outSubmeshesButtonElement.button.positionY = -0.76f;
    outSubmeshesButtonElement.button.positionZ = 0.9f;
    outSubmeshesButtonElement.button.width = 0.08f;
    outSubmeshesButtonElement.button.height = 0.04f;
    outSubmeshesButtonElement.button.color = colorData.buttonColor;
    outSubmeshesButtonElement.button.colorHover = colorData.buttonColorHover;
    outSubmeshesButtonElement.button.transitionMixVal = 0.0f;
    outSubmeshesButtonElement.button.buttonCurveReduce = 10.0f;
    outSubmeshesButtonElement.button.text = "Out the submeshes";
    outSubmeshesButtonElement.button.textRatio = 0.08f;
    outSubmeshesButtonElement.panel = 6;
    outSubmeshesButtonElement.attachedToMainPanel = true;
    outSubmeshesButtonElement.type = "button";
    UIElements.push_back(outSubmeshesButtonElement);//41


    //---------PAINTING PANEL----------

    UIElement paintThroughCheckBoxElement;
    paintThroughCheckBoxElement.checkBox.positionX = -0.06f;
    paintThroughCheckBoxElement.checkBox.positionY = -1.05f;
    paintThroughCheckBoxElement.checkBox.text = "Paint Through";
    paintThroughCheckBoxElement.checkBox.checked = false;
    paintThroughCheckBoxElement.checkBox.mouseHover = false;
    paintThroughCheckBoxElement.checkBox.texture = icons.OSq;
    paintThroughCheckBoxElement.panel = 3;
    paintThroughCheckBoxElement.attachedToMainPanel = true;
    paintThroughCheckBoxElement.type = "checkBox";
    UIElements.push_back(paintThroughCheckBoxElement);//38
    
    UIElement paintOverCheckBoxElement;
    paintOverCheckBoxElement.checkBox.positionX = -0.09f;
    paintOverCheckBoxElement.checkBox.positionY = -0.05f;
    paintOverCheckBoxElement.checkBox.text = "Paint Over";
    paintOverCheckBoxElement.checkBox.checked = false;
    paintOverCheckBoxElement.checkBox.mouseHover = false;
    paintOverCheckBoxElement.checkBox.texture = icons.OSq;
    paintOverCheckBoxElement.panel = 3;
    paintOverCheckBoxElement.attachedToMainPanel = true;
    paintOverCheckBoxElement.type = "checkBox";
    UIElements.push_back(paintOverCheckBoxElement);//38
    
    UIElement displayPaintOverTextureCheckBoxElement;
    displayPaintOverTextureCheckBoxElement.checkBox.positionX = -0.09f;
    displayPaintOverTextureCheckBoxElement.checkBox.positionY = -0.105f;
    displayPaintOverTextureCheckBoxElement.checkBox.text = "Display the texture";
    displayPaintOverTextureCheckBoxElement.checkBox.checked = false;
    displayPaintOverTextureCheckBoxElement.checkBox.mouseHover = false;
    displayPaintOverTextureCheckBoxElement.checkBox.texture = icons.OSq;
    displayPaintOverTextureCheckBoxElement.panel = 3;
    displayPaintOverTextureCheckBoxElement.attachedToMainPanel = true;
    displayPaintOverTextureCheckBoxElement.type = "checkBox";
    UIElements.push_back(displayPaintOverTextureCheckBoxElement);//38

    UIElement selectPaintOverTextureIconElement;
    selectPaintOverTextureIconElement.icon.positionX = -0.08f;
    selectPaintOverTextureIconElement.icon.positionY = -0.17f;
    selectPaintOverTextureIconElement.icon.positionZ = 0.9f;
    selectPaintOverTextureIconElement.icon.width = 0.015f;
    selectPaintOverTextureIconElement.icon.height = 0.03f;
    selectPaintOverTextureIconElement.icon.color = colorData.iconColor;
    selectPaintOverTextureIconElement.icon.colorHover = colorData.iconColorHover;
    selectPaintOverTextureIconElement.icon.mixVal = 0.0f;
    selectPaintOverTextureIconElement.icon.clickable = true;
    selectPaintOverTextureIconElement.icon.icon = icons.AddTexture;
    selectPaintOverTextureIconElement.panel = 3;
    selectPaintOverTextureIconElement.attachedToMainPanel = true;
    selectPaintOverTextureIconElement.type = "icon";
    UIElements.push_back(selectPaintOverTextureIconElement);//32

    UIElement selectPaintOverTextureNameTextElement;
	selectPaintOverTextureNameTextElement.text.positionX = -0.06f;
	selectPaintOverTextureNameTextElement.text.positionY = -0.17f;
	selectPaintOverTextureNameTextElement.text.text = "";
    selectPaintOverTextureNameTextElement.text.scale = 0.00022f;
    selectPaintOverTextureNameTextElement.text.color = colorData.textColor;
	selectPaintOverTextureNameTextElement.panel = 3;
	selectPaintOverTextureNameTextElement.attachedToMainPanel = true;
	selectPaintOverTextureNameTextElement.type = "text";
    UIElements.push_back(selectPaintOverTextureNameTextElement);//12

    float paintOverSetPos = 0.02f;

    UIElement paintOverPosXRangeBarElement;
	paintOverPosXRangeBarElement.rangeBar.positionX = -0.03f;
	paintOverPosXRangeBarElement.rangeBar.positionY = -0.23f-paintOverSetPos;
	paintOverPosXRangeBarElement.rangeBar.isConstant = true;
	paintOverPosXRangeBarElement.rangeBar.value = 0.f;
	paintOverPosXRangeBarElement.panel = 3;
	paintOverPosXRangeBarElement.attachedToMainPanel = true;
	paintOverPosXRangeBarElement.type = "rangeBar";
    UIElements.push_back(paintOverPosXRangeBarElement);
    
    UIElement paintOverPosYRangeBarElement;
	paintOverPosYRangeBarElement.rangeBar.positionX = +0.03f;
	paintOverPosYRangeBarElement.rangeBar.positionY = -0.23f-paintOverSetPos;
	paintOverPosYRangeBarElement.rangeBar.isConstant = true;
	paintOverPosYRangeBarElement.rangeBar.value = 0.f;
	paintOverPosYRangeBarElement.panel = 3;
	paintOverPosYRangeBarElement.attachedToMainPanel = true;
	paintOverPosYRangeBarElement.type = "rangeBar";
    UIElements.push_back(paintOverPosYRangeBarElement);
    
    UIElement paintOverScaleXRangeBarElement;
	paintOverScaleXRangeBarElement.rangeBar.positionX = -0.03f;
	paintOverScaleXRangeBarElement.rangeBar.positionY = -0.28f-paintOverSetPos;
	paintOverScaleXRangeBarElement.rangeBar.isConstant = true;
	paintOverScaleXRangeBarElement.rangeBar.value = 1.f;
	paintOverScaleXRangeBarElement.panel = 3;
	paintOverScaleXRangeBarElement.attachedToMainPanel = true;
	paintOverScaleXRangeBarElement.type = "rangeBar";
    UIElements.push_back(paintOverScaleXRangeBarElement);
    
    UIElement paintOverScaleYRangeBarElement;
	paintOverScaleYRangeBarElement.rangeBar.positionX = +0.03f;
	paintOverScaleYRangeBarElement.rangeBar.positionY = -0.28f-paintOverSetPos;
	paintOverScaleYRangeBarElement.rangeBar.isConstant = true;
	paintOverScaleYRangeBarElement.rangeBar.value = 1.f;
	paintOverScaleYRangeBarElement.panel = 3;
	paintOverScaleYRangeBarElement.attachedToMainPanel = true;
	paintOverScaleYRangeBarElement.type = "rangeBar";
    UIElements.push_back(paintOverScaleYRangeBarElement);
    
    UIElement paintOverScaleTextElement;
	paintOverScaleTextElement.text.positionX = -0.11f;
	paintOverScaleTextElement.text.positionY = -0.29f-paintOverSetPos;
	paintOverScaleTextElement.text.text = "Scale";
    paintOverScaleTextElement.text.scale = 0.0002f;
    paintOverScaleTextElement.text.color = colorData.textColor;
	paintOverScaleTextElement.panel = 3;
	paintOverScaleTextElement.attachedToMainPanel = true;
	paintOverScaleTextElement.type = "text";
    UIElements.push_back(paintOverScaleTextElement);//12
    
    UIElement paintOverPosTextElement;
	paintOverPosTextElement.text.positionX = -0.1f;
	paintOverPosTextElement.text.positionY = -0.24f-paintOverSetPos;
	paintOverPosTextElement.text.text = "Pos";
    paintOverPosTextElement.text.scale = 0.0002f;
    paintOverPosTextElement.text.color = colorData.textColor;
	paintOverPosTextElement.panel = 3;
	paintOverPosTextElement.attachedToMainPanel = true;
	paintOverPosTextElement.type = "text";
    UIElements.push_back(paintOverPosTextElement);//12

    UIElement paintOverXTextElement;
	paintOverXTextElement.text.positionX = -0.038f;
	paintOverXTextElement.text.positionY = -0.21f-paintOverSetPos;
	paintOverXTextElement.text.text = "X";
    paintOverXTextElement.text.scale = 0.00015f;
    paintOverXTextElement.text.color = colorData.textColor;
	paintOverXTextElement.panel = 3;
	paintOverXTextElement.attachedToMainPanel = true;
	paintOverXTextElement.type = "text";
    UIElements.push_back(paintOverXTextElement);//12
    
    UIElement paintOverYTextElement;
	paintOverYTextElement.text.positionX = +0.022f;
	paintOverYTextElement.text.positionY = -0.21f-paintOverSetPos;
	paintOverYTextElement.text.text = "Y";
    paintOverYTextElement.text.scale = 0.00015f;
    paintOverYTextElement.text.color = colorData.textColor;
	paintOverYTextElement.panel = 3;
	paintOverYTextElement.attachedToMainPanel = true;
	paintOverYTextElement.type = "text";
    UIElements.push_back(paintOverYTextElement);//12

    UIElement dynamicPaintingCheckBoxElement;
    dynamicPaintingCheckBoxElement.checkBox.positionX = -0.44f;
    dynamicPaintingCheckBoxElement.checkBox.positionY = 0.9f;
    dynamicPaintingCheckBoxElement.checkBox.text = "";
    dynamicPaintingCheckBoxElement.checkBox.checked = false;
    dynamicPaintingCheckBoxElement.checkBox.texture = icons.OSq;
    dynamicPaintingCheckBoxElement.panel = 3;
    dynamicPaintingCheckBoxElement.focusMode = true;
    dynamicPaintingCheckBoxElement.attachedToMainPanel = false;
    dynamicPaintingCheckBoxElement.type = "checkBox";
    UIElements.push_back(dynamicPaintingCheckBoxElement);//23

    UIElement dynamicPaintingIconElement;
    dynamicPaintingIconElement.icon.positionX = -0.415f;
    dynamicPaintingIconElement.icon.positionY = 0.9f;
    dynamicPaintingIconElement.icon.positionZ = 0.95f;
    dynamicPaintingIconElement.icon.width = 0.015f;
    dynamicPaintingIconElement.icon.height = 0.03f;
    dynamicPaintingIconElement.icon.color = colorData.buttonColor;
    dynamicPaintingIconElement.icon.colorHover = colorData.buttonColorHover;
    dynamicPaintingIconElement.icon.mixVal = 0.0f;
    dynamicPaintingIconElement.icon.icon = icons.MaskGausBlur;
    dynamicPaintingIconElement.focusMode = true;
    dynamicPaintingIconElement.panel = 3;
    dynamicPaintingIconElement.attachedToMainPanel = false;
	dynamicPaintingIconElement.type = "icon";
    UIElements.push_back(dynamicPaintingIconElement);//35
    
    UIElement maskPaintingCheckBoxElement;
    maskPaintingCheckBoxElement.checkBox.positionX = -0.38f;
    maskPaintingCheckBoxElement.checkBox.positionY = 0.9f;
    maskPaintingCheckBoxElement.checkBox.text = "";
    maskPaintingCheckBoxElement.checkBox.checked = true;
    maskPaintingCheckBoxElement.checkBox.texture = icons.OSq;
    maskPaintingCheckBoxElement.panel = 3;
    maskPaintingCheckBoxElement.focusMode = true;
    maskPaintingCheckBoxElement.attachedToMainPanel = false;
    maskPaintingCheckBoxElement.type = "checkBox";
    UIElements.push_back(maskPaintingCheckBoxElement);//23

    UIElement maskPaintingIconElement;
    maskPaintingIconElement.icon.positionX = -0.355f;
    maskPaintingIconElement.icon.positionY = 0.9f;
    maskPaintingIconElement.icon.positionZ = 0.95f;
    maskPaintingIconElement.icon.width = 0.015f;
    maskPaintingIconElement.icon.height = 0.03f;
    maskPaintingIconElement.icon.color = colorData.buttonColor;
    maskPaintingIconElement.icon.colorHover = colorData.buttonColorHover;
    maskPaintingIconElement.icon.mixVal = 0.0f;
    maskPaintingIconElement.icon.icon = icons.ColorPicker;
    maskPaintingIconElement.focusMode = true;;
    maskPaintingIconElement.panel = 3;
    maskPaintingIconElement.attachedToMainPanel = false;
	maskPaintingIconElement.type = "icon";
    UIElements.push_back(maskPaintingIconElement);//35

    UIElement colorPaintingCheckBoxElement;
    colorPaintingCheckBoxElement.checkBox.positionX = -0.32f;
    colorPaintingCheckBoxElement.checkBox.positionY = 0.9f;
    colorPaintingCheckBoxElement.checkBox.text = "";
    colorPaintingCheckBoxElement.checkBox.texture = icons.OSq;
    colorPaintingCheckBoxElement.panel = 3;
    colorPaintingCheckBoxElement.focusMode = true;
    colorPaintingCheckBoxElement.attachedToMainPanel = false;
    colorPaintingCheckBoxElement.type = "checkBox";
    UIElements.push_back(colorPaintingCheckBoxElement);//24

    UIElement colorPaintingIconElement;
    colorPaintingIconElement.icon.positionX = -0.295f;
    colorPaintingIconElement.icon.positionY = 0.9f;
    colorPaintingIconElement.icon.positionZ = 0.95f;
    colorPaintingIconElement.icon.width = 0.015f;
    colorPaintingIconElement.icon.height = 0.03f;
    colorPaintingIconElement.icon.color = glm::vec4(149.f/255.f, 207.f/255.f, 138.f/255.f,1.f);
    colorPaintingIconElement.icon.colorHover = colorData.buttonColorHover;
    colorPaintingIconElement.icon.mixVal = 0.0f;
    colorPaintingIconElement.icon.icon = icons.ColorPicker;
    colorPaintingIconElement.focusMode = true;
    colorPaintingIconElement.panel = 3;
    colorPaintingIconElement.attachedToMainPanel = false;
	colorPaintingIconElement.type = "icon";
    UIElements.push_back(colorPaintingIconElement);//35

	UIElement normalmapPaintingCheckBoxElement;
    normalmapPaintingCheckBoxElement.checkBox.positionX = -0.26f;
    normalmapPaintingCheckBoxElement.checkBox.positionY = 0.9f;
    normalmapPaintingCheckBoxElement.checkBox.text = "";
    normalmapPaintingCheckBoxElement.checkBox.texture = icons.OSq;
    normalmapPaintingCheckBoxElement.panel = 3;
    normalmapPaintingCheckBoxElement.focusMode = true;
    normalmapPaintingCheckBoxElement.attachedToMainPanel = false;
    normalmapPaintingCheckBoxElement.type = "checkBox";
    UIElements.push_back(normalmapPaintingCheckBoxElement);//25

    UIElement normalPaintingIconElement;
    normalPaintingIconElement.icon.positionX = -0.235f;
    normalPaintingIconElement.icon.positionY = 0.9f;
    normalPaintingIconElement.icon.positionZ = 0.95f;
    normalPaintingIconElement.icon.width = 0.015f;
    normalPaintingIconElement.icon.height = 0.03f;
    normalPaintingIconElement.icon.color = glm::vec4(128.f/255.f,127.f/255.f,255.f/255.f,1.f);
    normalPaintingIconElement.icon.colorHover = colorData.buttonColorHover;
    normalPaintingIconElement.icon.mixVal = 0.0f;
    normalPaintingIconElement.icon.icon = icons.ColorPicker;
    normalPaintingIconElement.focusMode = true;
    normalPaintingIconElement.panel = 3;
    normalPaintingIconElement.attachedToMainPanel = false;
	normalPaintingIconElement.type = "icon";
    UIElements.push_back(normalPaintingIconElement);//35

    UIElement subSelectedImagePowerRangeBarElement;
	subSelectedImagePowerRangeBarElement.rangeBar.positionX = 0.0f;
	subSelectedImagePowerRangeBarElement.rangeBar.positionY = -0.95f;
	subSelectedImagePowerRangeBarElement.rangeBar.value = -0.11f;
	subSelectedImagePowerRangeBarElement.panel = 3;
	subSelectedImagePowerRangeBarElement.attachedToMainPanel = true;
	subSelectedImagePowerRangeBarElement.type = "rangeBar";
    UIElements.push_back(subSelectedImagePowerRangeBarElement);//12

    UIElement subSelectedImagePowerRangeBarTextElement;
	subSelectedImagePowerRangeBarTextElement.text.positionX = -0.11f;
	subSelectedImagePowerRangeBarTextElement.text.positionY = -0.92f;
	subSelectedImagePowerRangeBarTextElement.text.text = "Subselected texture power";
    subSelectedImagePowerRangeBarTextElement.text.scale = 0.00022f;
    subSelectedImagePowerRangeBarTextElement.text.color = colorData.textColor;
	subSelectedImagePowerRangeBarTextElement.panel = 3;
	subSelectedImagePowerRangeBarTextElement.attachedToMainPanel = true;
	subSelectedImagePowerRangeBarTextElement.type = "text";
    UIElements.push_back(subSelectedImagePowerRangeBarTextElement);//12

    UIElement focusModeCheckBoxElement;
    focusModeCheckBoxElement.checkBox.positionX = -0.63f;
    focusModeCheckBoxElement.checkBox.positionY = 0.9f;
    focusModeCheckBoxElement.checkBox.text = "Focus Mode";
    focusModeCheckBoxElement.checkBox.texture = icons.OSq;
    focusModeCheckBoxElement.panel = 3;
    focusModeCheckBoxElement.attachedToMainPanel = false;
    focusModeCheckBoxElement.focusMode = true;
    focusModeCheckBoxElement.type = "checkBox";
    UIElements.push_back(focusModeCheckBoxElement);//25

    UIElement mirrorXRangeBarElement;
	mirrorXRangeBarElement.rangeBar.positionX = -0.16f;
	mirrorXRangeBarElement.rangeBar.positionY = 0.85f;
	mirrorXRangeBarElement.rangeBar.widthDivider = 2.f;
	mirrorXRangeBarElement.rangeBar.value = 0.0f;
	mirrorXRangeBarElement.rangeBar.isConstant = true;
	mirrorXRangeBarElement.panel = 3;
	mirrorXRangeBarElement.attachedToMainPanel = true;
	mirrorXRangeBarElement.focusMode = true;
	mirrorXRangeBarElement.type = "rangeBar";
    UIElements.push_back(mirrorXRangeBarElement);//12
    
    UIElement mirrorYRangeBarElement;
	mirrorYRangeBarElement.rangeBar.positionX = -0.1f;
	mirrorYRangeBarElement.rangeBar.positionY = 0.85f;
	mirrorYRangeBarElement.rangeBar.widthDivider = 2.f;
	mirrorYRangeBarElement.rangeBar.value = 0.0f;
	mirrorYRangeBarElement.rangeBar.isConstant = true;
	mirrorYRangeBarElement.panel = 3;
	mirrorYRangeBarElement.attachedToMainPanel = true;
	mirrorYRangeBarElement.focusMode = true;
	mirrorYRangeBarElement.type = "rangeBar";
    UIElements.push_back(mirrorYRangeBarElement);//12
    
    UIElement mirrorZRangeBarElement;
	mirrorZRangeBarElement.rangeBar.positionX = -0.04f;
	mirrorZRangeBarElement.rangeBar.positionY = 0.85f;
	mirrorZRangeBarElement.rangeBar.widthDivider = 2.f;
	mirrorZRangeBarElement.rangeBar.value = 0.0f;
	mirrorZRangeBarElement.rangeBar.isConstant = true;
	mirrorZRangeBarElement.panel = 3;
	mirrorZRangeBarElement.attachedToMainPanel = true;
	mirrorZRangeBarElement.focusMode = true;
	mirrorZRangeBarElement.type = "rangeBar";
    UIElements.push_back(mirrorZRangeBarElement);//12

    UIElement albedoChannelCheckBoxElement;
    albedoChannelCheckBoxElement.checkBox.positionX = -0.05f;
    albedoChannelCheckBoxElement.checkBox.positionY = -1.15f;
    albedoChannelCheckBoxElement.checkBox.checked = true;
    albedoChannelCheckBoxElement.checkBox.text = "Albedo";
    albedoChannelCheckBoxElement.checkBox.texture = icons.OSq;
    albedoChannelCheckBoxElement.panel = 3;
    albedoChannelCheckBoxElement.attachedToMainPanel = true;
    albedoChannelCheckBoxElement.focusMode = false;
    albedoChannelCheckBoxElement.type = "checkBox";
    UIElements.push_back(albedoChannelCheckBoxElement);//25

    UIElement roughnessChannelCheckBoxElement;
    roughnessChannelCheckBoxElement.checkBox.positionX = -0.05f;
    roughnessChannelCheckBoxElement.checkBox.positionY = -1.2f;
    roughnessChannelCheckBoxElement.checkBox.text = "Roughness";
    roughnessChannelCheckBoxElement.checkBox.texture = icons.OSq;
    roughnessChannelCheckBoxElement.panel = 3;
    roughnessChannelCheckBoxElement.attachedToMainPanel = true;
    roughnessChannelCheckBoxElement.focusMode = false;
    roughnessChannelCheckBoxElement.type = "checkBox";
    UIElements.push_back(roughnessChannelCheckBoxElement);//25
    
    UIElement metallicChannelCheckBoxElement;
    metallicChannelCheckBoxElement.checkBox.positionX = -0.05f;
    metallicChannelCheckBoxElement.checkBox.positionY = -1.25f;
    metallicChannelCheckBoxElement.checkBox.text = "Metallic";
    metallicChannelCheckBoxElement.checkBox.texture = icons.OSq;
    metallicChannelCheckBoxElement.panel = 3;
    metallicChannelCheckBoxElement.attachedToMainPanel = true;
    metallicChannelCheckBoxElement.focusMode = false;
    metallicChannelCheckBoxElement.type = "checkBox";
    UIElements.push_back(metallicChannelCheckBoxElement);//25
    
    UIElement normalChannelCheckBoxElement;
    normalChannelCheckBoxElement.checkBox.positionX = -0.05f;
    normalChannelCheckBoxElement.checkBox.positionY = -1.3f;
    normalChannelCheckBoxElement.checkBox.text = "Normal";
    normalChannelCheckBoxElement.checkBox.texture = icons.OSq;
    normalChannelCheckBoxElement.panel = 3;
    normalChannelCheckBoxElement.attachedToMainPanel = true;
    normalChannelCheckBoxElement.focusMode = false;
    normalChannelCheckBoxElement.type = "checkBox";
    UIElements.push_back(normalChannelCheckBoxElement);//25

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

	UIElement folderIconElement; //Deleted
    folderIconElement.icon.positionX = 0.1f;
    folderIconElement.icon.positionY = 1000.6f;
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

    //Load plane button (deleted)
	UIElement loadPlaneModelButtonElement;
    loadPlaneModelButtonElement.icon.positionX = 0.0f;
    loadPlaneModelButtonElement.icon.positionY = -1000.1f;
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

	//Load sphere button (deleted)
	UIElement loadSphereModelButtonElement;
    loadSphereModelButtonElement.icon.positionX = 0.0f;
    loadSphereModelButtonElement.icon.positionY = -1000.2f;
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

	UIElement planeIconElement;//(deleted)
    planeIconElement.icon.positionX = 0.0f;
    planeIconElement.icon.positionY = -1000.1f;
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

    UIElement sphereIconElement;//(deleted)
    sphereIconElement.icon.positionX = 0.0f;
    sphereIconElement.icon.positionY = -1000.2f;
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
    
    UIElement addBrushMaskTextureIconElement;
    addBrushMaskTextureIconElement.icon.positionX = 0.1f;
    addBrushMaskTextureIconElement.icon.positionY = 0.85f;
    addBrushMaskTextureIconElement.icon.positionZ = 0.9f;
    addBrushMaskTextureIconElement.icon.width = 0.015;
    addBrushMaskTextureIconElement.icon.height = 0.03f;
    addBrushMaskTextureIconElement.icon.color = colorData.iconColor;
    addBrushMaskTextureIconElement.icon.colorHover = colorData.iconColorHover;
    addBrushMaskTextureIconElement.icon.mixVal = 0.0f;
    addBrushMaskTextureIconElement.icon.icon = icons.Plus;
    addBrushMaskTextureIconElement.icon.clickable = true;
    addBrushMaskTextureIconElement.panel = 3;
    addBrushMaskTextureIconElement.attachedToMainPanel = true;
	addBrushMaskTextureIconElement.type = "icon";
    UIElements.push_back(addBrushMaskTextureIconElement);//38
    
    UIElement selectBrushMaskTextureIconElement;
    selectBrushMaskTextureIconElement.icon.positionX = 0.05f;
    selectBrushMaskTextureIconElement.icon.positionY = 0.85f;
    selectBrushMaskTextureIconElement.icon.positionZ = 0.9f;
    selectBrushMaskTextureIconElement.icon.width = 0.015f;
    selectBrushMaskTextureIconElement.icon.height = 0.03;
    selectBrushMaskTextureIconElement.icon.color = colorData.iconColor;
    selectBrushMaskTextureIconElement.icon.colorHover = colorData.iconColorHover;
    selectBrushMaskTextureIconElement.icon.mixVal = 0.0f;
    selectBrushMaskTextureIconElement.icon.icon = icons.AddTexture;
    selectBrushMaskTextureIconElement.icon.clickable = true;
    selectBrushMaskTextureIconElement.panel = 3;
    selectBrushMaskTextureIconElement.attachedToMainPanel = true;
	selectBrushMaskTextureIconElement.type = "icon";
    UIElements.push_back(selectBrushMaskTextureIconElement);//38


    UIElement generateTextCheckBoxElement;
    generateTextCheckBoxElement.checkBox.positionX = -0.1f;
    generateTextCheckBoxElement.checkBox.positionY = -0.26f;
    generateTextCheckBoxElement.checkBox.text = "Text";
    generateTextCheckBoxElement.checkBox.checked = false;
    generateTextCheckBoxElement.checkBox.mouseHover = false;
    generateTextCheckBoxElement.checkBox.texture = icons.OSq;
    generateTextCheckBoxElement.panel = 6;
    generateTextCheckBoxElement.attachedToMainPanel = true;
    generateTextCheckBoxElement.type = "checkBox";
    UIElements.push_back(generateTextCheckBoxElement);//38

	UIElement generateTextSizeRangeBarElement;
	generateTextSizeRangeBarElement.rangeBar.positionX = 0.0f;
	generateTextSizeRangeBarElement.rangeBar.positionY = -0.3;
	generateTextSizeRangeBarElement.rangeBar.value = 0.0f;
	generateTextSizeRangeBarElement.panel = 6;
	generateTextSizeRangeBarElement.attachedToMainPanel = true;
	generateTextSizeRangeBarElement.type = "rangeBar";
    UIElements.push_back(generateTextSizeRangeBarElement);//12
	
    UIElement generateTextLeftAlignCheckBoxElement;
    generateTextLeftAlignCheckBoxElement.checkBox.positionX = -0.04f;
    generateTextLeftAlignCheckBoxElement.checkBox.positionY = -0.36f;
    generateTextLeftAlignCheckBoxElement.checkBox.text = "";
    generateTextLeftAlignCheckBoxElement.checkBox.checked = true;
    generateTextLeftAlignCheckBoxElement.checkBox.mouseHover = false;
    generateTextLeftAlignCheckBoxElement.checkBox.texture = icons.AlignLeft;
    generateTextLeftAlignCheckBoxElement.panel = 6;
    generateTextLeftAlignCheckBoxElement.attachedToMainPanel = true;
    generateTextLeftAlignCheckBoxElement.type = "checkBox";
    UIElements.push_back(generateTextLeftAlignCheckBoxElement);//38
    
    UIElement generateTextMidAlignCheckBoxElement;
    generateTextMidAlignCheckBoxElement.checkBox.positionX = 0.0f;
    generateTextMidAlignCheckBoxElement.checkBox.positionY = -0.36f;
    generateTextMidAlignCheckBoxElement.checkBox.text = "";
    generateTextMidAlignCheckBoxElement.checkBox.checked = false;
    generateTextMidAlignCheckBoxElement.checkBox.mouseHover = false;
    generateTextMidAlignCheckBoxElement.checkBox.texture = icons.AlignMid;
    generateTextMidAlignCheckBoxElement.panel = 6;
    generateTextMidAlignCheckBoxElement.attachedToMainPanel = true;
    generateTextMidAlignCheckBoxElement.type = "checkBox";
    UIElements.push_back(generateTextMidAlignCheckBoxElement);//38

    UIElement generateTextRightAlignCheckBoxElement;
    generateTextRightAlignCheckBoxElement.checkBox.positionX = 0.04f;
    generateTextRightAlignCheckBoxElement.checkBox.positionY = -0.36f;
    generateTextRightAlignCheckBoxElement.checkBox.text = "";
    generateTextRightAlignCheckBoxElement.checkBox.checked = false;
    generateTextRightAlignCheckBoxElement.checkBox.mouseHover = false;
    generateTextRightAlignCheckBoxElement.checkBox.texture = icons.AlignRight;
    generateTextRightAlignCheckBoxElement.panel = 6;
    generateTextRightAlignCheckBoxElement.attachedToMainPanel = true;
    generateTextRightAlignCheckBoxElement.type = "checkBox";
    UIElements.push_back(generateTextRightAlignCheckBoxElement);//38
    
    UIElement generateTextTextureTextTextBoxElement;
    generateTextTextureTextTextBoxElement.textBox.position_x = 0.0f;
    generateTextTextureTextTextBoxElement.textBox.position_y = -0.46f;
    generateTextTextureTextTextBoxElement.textBox.position_z = 0.9f;
    generateTextTextureTextTextBoxElement.textBox.width = 0.12;
    generateTextTextureTextTextBoxElement.textBox.height = 0.03;
    generateTextTextureTextTextBoxElement.textBox.text = "MyProject";
    generateTextTextureTextTextBoxElement.textBox.rich = true;
    generateTextTextureTextTextBoxElement.panel = 6;
    generateTextTextureTextTextBoxElement.attachedToMainPanel = true;
	generateTextTextureTextTextBoxElement.type = "textBox";
	generateTextTextureTextTextBoxElement.createProject = true;
	generateTextTextureTextTextBoxElement.createProjectPos = glm::vec2(0.f,0.55);
    UIElements.push_back(generateTextTextureTextTextBoxElement);//26


    UIElement generateBlackToAlphaCheckBoxElement;
    generateBlackToAlphaCheckBoxElement.checkBox.positionX = -0.1f;
    generateBlackToAlphaCheckBoxElement.checkBox.positionY = -0.66f;
    generateBlackToAlphaCheckBoxElement.checkBox.text = "Black To Alpha";
    generateBlackToAlphaCheckBoxElement.checkBox.checked = false;
    generateBlackToAlphaCheckBoxElement.checkBox.mouseHover = false;
    generateBlackToAlphaCheckBoxElement.checkBox.texture = icons.OSq;
    generateBlackToAlphaCheckBoxElement.panel = 6;
    generateBlackToAlphaCheckBoxElement.attachedToMainPanel = true;
    generateBlackToAlphaCheckBoxElement.type = "checkBox";
    UIElements.push_back(generateBlackToAlphaCheckBoxElement);//38

    UIElement TDModelSizeRangeBarElement;
	TDModelSizeRangeBarElement.rangeBar.positionX = 0.0f;
	TDModelSizeRangeBarElement.rangeBar.positionY = 0.f;
	TDModelSizeRangeBarElement.rangeBar.value = 1.0f;
	TDModelSizeRangeBarElement.rangeBar.isConstant = true;
	TDModelSizeRangeBarElement.rangeBar.isRich = true;
	TDModelSizeRangeBarElement.panel = 1;
	TDModelSizeRangeBarElement.attachedToMainPanel = true;
	TDModelSizeRangeBarElement.type = "rangeBar";
    UIElements.push_back(TDModelSizeRangeBarElement);//12
    
    
    UIElement TDModelPosXRangeBarElement;
	TDModelPosXRangeBarElement.rangeBar.positionX = 0.0f;
	TDModelPosXRangeBarElement.rangeBar.positionY = -0.1f;
	TDModelPosXRangeBarElement.rangeBar.value = 0.0f;
	TDModelPosXRangeBarElement.rangeBar.isConstant = true;
	TDModelPosXRangeBarElement.rangeBar.isRich = true;
	TDModelPosXRangeBarElement.panel = 1;
	TDModelPosXRangeBarElement.attachedToMainPanel = true;
	TDModelPosXRangeBarElement.type = "rangeBar";
    UIElements.push_back(TDModelPosXRangeBarElement);//12
    
    UIElement TDModelPosYRangeBarElement;
	TDModelPosYRangeBarElement.rangeBar.positionX = 0.0f;
	TDModelPosYRangeBarElement.rangeBar.positionY = -0.15f;
	TDModelPosYRangeBarElement.rangeBar.value = 0.0f;
	TDModelPosYRangeBarElement.rangeBar.isConstant = true;
	TDModelPosYRangeBarElement.rangeBar.isRich = true;
	TDModelPosYRangeBarElement.panel = 1;
	TDModelPosYRangeBarElement.attachedToMainPanel = true;
	TDModelPosYRangeBarElement.type = "rangeBar";
    UIElements.push_back(TDModelPosYRangeBarElement);//12
    
    UIElement TDModelPosZRangeBarElement;
	TDModelPosZRangeBarElement.rangeBar.positionX = 0.0f;
	TDModelPosZRangeBarElement.rangeBar.positionY = -0.2f;
	TDModelPosZRangeBarElement.rangeBar.value = 0.0f;
	TDModelPosZRangeBarElement.rangeBar.isConstant = true;
	TDModelPosZRangeBarElement.rangeBar.isRich = true;
	TDModelPosZRangeBarElement.panel = 1;
	TDModelPosZRangeBarElement.attachedToMainPanel = true;
	TDModelPosZRangeBarElement.type = "rangeBar";
    UIElements.push_back(TDModelPosZRangeBarElement);//12
    
    return UIElements;
}