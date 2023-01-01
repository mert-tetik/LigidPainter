#ifndef LGDUI_H
#define LGDUI_H

#include <map>

#define UIUploadingModelPathTextBox 0
#define UIuploadingModelPathText 1
#define UIloadModelButton 2
#define UIautoTriangulateCheckBox 3
#define UIbackfaceCullingCheckBox 4
#define UImodelPanel3DModelsText 5
#define UIloadPlaneModelButton 6
#define UIloadSphereModelButton 7
#define UIloadCustomModelButton 8
#define UIaddBrushMaskTextureButton 9
#define UIuseNegativeCheckBox 10
#define UImaskTextureFileNameText 11
#define UIbrushSizeText 12
#define UIbrushSizeRangeBar 13
#define UIbrushBlurText 14
#define UIbrushBlurRangeBar 15
#define UIbrushRotationText 16
#define UIbrushRotationRangeBar 17
#define UIbrushOpacityText 18
#define UIbrushOpacityRangeBar 19
#define UIbrushSpacingText 20
#define UIbrushSpacingRangeBar 21
#define UIbrushBordersText 22
#define UIbrushBordersRangeBar 23
#define UImirrorXCheckBox 24
#define UImirrorYCheckBox 25
#define UImirrorZCheckBox 26
#define UIexportingPathTextBox 27 
#define UIexportingFolderNameTextBox 28
#define UIjpgCheckBox 29
#define UIpngCheckBox 30
#define UIdownloadButton 31
#define UIjpgIcon 32
#define UIpngIcon 33
#define UIfolderIcon 34
#define UIplaneIcon 35
#define UIsphereIcon 36

struct ColorData //LigidPainter color palette
{
	glm::vec3 LigidPainterThemeColor = glm::vec3(0.043f,0.635f,0.823f);

	glm::vec4 viewportBackColor = glm::vec4(0.23f, 0.23f, 0.23f,1.0f);

	glm::vec4 panelColor = glm::vec4(0.15f, 0.15f, 0.15f,0.5f);
	glm::vec4 panelColorSnd = glm::vec4(0.05f, 0.05f, 0.05f,0.5f);
	glm::vec4 panelHoldColor = glm::vec4(0.05f, 0.05f, 0.05f,0.5f);

	glm::vec4 buttonColor = glm::vec4(0.13f, 0.13f, 0.13f,0.5f);
	glm::vec4 buttonColorHover = glm::vec4(0.0f, 0.0f, 0.0f,0.5f);

	glm::vec4 menuBarColor = glm::vec4(0.05f, 0.05f, 0.05f,0.5f);
	glm::vec4 textBoxColor = glm::vec4(0.05f, 0.05f, 0.05f,0.5f);
	glm::vec4 textBoxColorClicked = glm::vec4(LigidPainterThemeColor,0.5f);

	glm::vec4 checkBoxColor = glm::vec4(0.05f, 0.05f, 0.05f,0.5f);
	glm::vec4 checkBoxHoverColor = glm::vec4(0.15f, 0.15f, 0.15f,0.5f);
	glm::vec4 checkBoxCheckedColor = glm::vec4(1.0f, 1.0f, 1.0f,0.5f);

	glm::vec4 rangeBarBack = glm::vec4(0.65f, 0.65f, 0.65f,0.5f);
	glm::vec4 rangeBarFront = glm::vec4(0.95f, 0.95f, 0.95f,0.5f);
	glm::vec4 rangeBarSlide = glm::vec4(0.95f, 0.95f, 0.95f,0.5f);

	glm::vec4 textColor = glm::vec4(1.0f, 1.0f, 1.0f,1.0f);

	glm::vec4 colorBoxIndicatorColor = glm::vec4(0.05f, 0.05f, 0.05f,0.5f);

	glm::vec4 iconColor = glm::vec4(1.0f,1.0f,1.0f,0.5f);
	glm::vec4 iconColorHover = glm::vec4(0.5f,0.5f,0.5f,0.5f);
	
	glm::vec4 numericModifierArrowColor = glm::vec4(0.6f, 0.6f, 0.6f,0.5f);
	glm::vec4 numericModifierArrowHoverColor = glm::vec4(0.95f, 0.95f, 0.95f,0.5f);

	glm::vec4 textureDemonstratorButtonColor = glm::vec4(LigidPainterThemeColor,0.5f);

	
	glm::vec4 buttonMaskTxtrPanelColor = glm::vec4(0.13f, 0.13f, 0.13f,0.5f);
	glm::vec3 chosenBrushMaskTextureColor = glm::vec3(LigidPainterThemeColor);
	glm::vec3 brushMaskIconColor = glm::vec3(1.0f,1.0f,1.0f);
	glm::vec4 maskPanelSliderColor = glm::vec4(0.0f,0.0f,0.0f,0.5f);
	glm::vec4 maskPanelSliderBackgroundColor = glm::vec4(1.0f,1.0f,1.0f,0.5f);
	
	glm::vec4 messageBoxPanelColor = glm::vec4(0.13f, 0.13f, 0.13f,0.5f);
	glm::vec4 messageBoxButtonColor = glm::vec4(0.13f, 0.13f, 0.13f,0.5f);
};
struct buttonData {

};

//Width will be added to the midPanelPos if attachedToMainPanel is true
struct Container{
	float width;
	float height;
	float positionX;
	float positionY;
	float positionZ;
	glm::vec4 color;
	
	
};

struct Button{
	float width;
	float height;
	float positionX;
	float positionY;
	float positionZ;
	std::string text;
	float textRatio;
	glm::vec4 color;
	glm::vec4 colorHover;
	float buttonCurveReduce;
	float transitionMixVal;
	bool hover = false;
};

struct RangeBar{
	float positionX;
	float positionY;
	float value;
	bool hover = false;
};

struct Icon{
	float width;
	float height;
	float positionX;
	float positionY;
	float positionZ;
	unsigned int icon;
	glm::vec3 color;
	glm::vec3 colorHover;
	float mixVal;
	bool hover = false;
};

struct Text{
	std::string text;
	glm::vec4 color;
	float positionX;
	float positionY;
	float scale;
};

struct CheckBox{
	float positionX;
	float positionY;
	std::string text;
	bool mouseHover;
	bool checked = false;
};
struct TextBox{
	float width;
	float height;
	float position_x;
	float position_y;
	float position_z;
	bool hover = false;
	float transitionMixVal = 0.0f;
	bool clicked = false;
	std::string text;
};

struct Box{
	float width;
	float height;
	float position_x;
	float position_y;
	float position_z;
	float buttonCurveReduce;
	glm::vec4 color;
	glm::vec4 colorTransitionColor; 
	float mixVal;
};

struct UIElement{
	Container container;
	Button button;
	RangeBar rangeBar;
	Icon icon;
	Text text;
	CheckBox checkBox;
	TextBox textBox;
	Box box;
	const char* type;
	bool attachedToMainPanel;
	int panel; //0 = Independent from panels , 1 = 3D model panel , 2 = Materials panel , 3 = Painting Panel , 4 = Export Panel
};

struct UI{
	std::map<std::string, UIElement> uiElements;
	std::vector<std::string> uiIndex;
};


class UserInterface {
public:
	std::vector<UIElement> UserInterface::getUiElements(Icons icons);

	//Visuality
	void panel(float panelLoc, float movePanel_x);
	void textureDemonstrator(float width,float height, float position_x,float position_y,float z);
	void box(float width, float height, float position_x, float position_y, std::string text, glm::vec4 color, float textRatio, bool isTextBox, bool isMaskImageBox, float buttonCurveReduce,float z,glm::vec4 colorTransitionColor , float mixVal);
	void iconBox(float width, float height, float position_x, float position_y,float z, unsigned int icon,float mixVal,glm::vec3 color,glm::vec3 colorHover);
	void setViewportBgColor();
	void renderText(unsigned int program, std::string text, float x, float y, float scale);
	void renderMenubar(GLFWwindow* window);
	void checkBox(float position_x, float position_y, std::string text, glm::vec4 color, bool mouseHover, bool checked);
	void panelChangeButton(float position_x, float position_y);
	void rangeBar(float position_x, float position_y, float value);
	void colorBox(float position_x, float position_y, float valueX, float valueY);
	glm::vec3 colorRect(float position_x, float position_y,float value, unsigned int FBO,GLFWwindow* window,glm::mat4 projection,bool updateHueVal);
	void decorationSquare(float position_x, float position_y);
	void numericModifier(float position_x,float position_y,unsigned int leftArrow,unsigned int rightArrow,float z,int value,float mixValP,float mixValN);
	
	//Calculations and More
	bool isMouseOnPanelChangeButton(GLFWwindow* window, float position_x, float position_y, double mouseXpos, double mouseYpos);
	bool isMouseOnButton(GLFWwindow* window, float width, float height, float position_x, float position_y, double mouseXpos, double mouseYpos, bool isPanelMoving);
	bool isMouseOnCoords(GLFWwindow*window,double mouseXpos, double mouseYpos,std::vector<float> buttonCoor,bool isPanelMoving);
	
	void sendProgramsToUserInterface(Programs appPrograms);
    void sendMaxWindowSize(int maxScreenWidth,int maxScreenHeight);
	void sendMaxWindowSizeToCalculationsAndMore(int maxScreenWidth,int maxScreenHeight);
};
#endif 