#ifndef LGDUI_H
#define LGDUI_H

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

	glm::vec3 textColor = glm::vec3(1.0f, 1.0f, 1.0f);

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
class UserInterface {
public:

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