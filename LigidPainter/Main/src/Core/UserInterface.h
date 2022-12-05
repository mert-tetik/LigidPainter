#ifndef LGDUI_H
#define LGDUI_H

struct ColorData //LigidPainter color palette
{
	glm::vec3 LigidPainterThemeColor = glm::vec3(0.043f,0.635f,0.823);

	glm::vec3 viewportBackColor = glm::vec3(0.23f, 0.23f, 0.23f);
	glm::vec3 panelColor = glm::vec3(0.15f, 0.15f, 0.15f);
	glm::vec3 panelColorSnd = glm::vec3(0.05f, 0.05f, 0.05f);
	glm::vec3 panelHoldColor = glm::vec3(0.05f, 0.05f, 0.05f);
	glm::vec3 buttonColor = glm::vec3(0.13f, 0.13f, 0.13f);
	glm::vec3 buttonColorHover = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 menuBarColor = glm::vec3(0.05f, 0.05f, 0.05f);
	glm::vec3 textBoxColor = glm::vec3(0.05f, 0.05f, 0.05f);
	glm::vec3 textBoxColorClicked = LigidPainterThemeColor;
	glm::vec3 checkBoxColor = glm::vec3(0.05f, 0.05f, 0.05f);
	glm::vec3 checkBoxHoverColor = glm::vec3(0.15f, 0.15f, 0.15f);
	glm::vec3 checkBoxCheckedColor = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 rangeBarBack = glm::vec3(0.1f, 0.1f, 0.1f);
	glm::vec3 rangeBarFront = glm::vec3(0.6f, 0.6f, 0.6f);
	glm::vec3 rangeBarSlide = glm::vec3(0.3f, 0.3f, 0.3f);
	glm::vec3 textColor = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 colorBoxIndicatorColor = glm::vec3(0.05f, 0.05f, 0.05f);
	glm::vec3 iconColor = glm::vec3(1.0f,1.0f,1.0f);
	glm::vec3 iconColorHover = glm::vec3(0.5f,0.5f,0.5f);
	glm::vec3 buttonMaskTxtrPanelColor = glm::vec3(0.13f, 0.13f, 0.13f);
	glm::vec3 numericModifierArrowColor = glm::vec3(0.6f, 0.6f, 0.6f);
	glm::vec3 numericModifierArrowHoverColor = glm::vec3(0.1f, 0.1f, 0.1f);
	glm::vec3 textureDemonstratorButtonColor = LigidPainterThemeColor;
	glm::vec3 chosenBrushMaskTextureColor = LigidPainterThemeColor;
	glm::vec3 brushMaskIconColor = glm::vec3(1.0f);
};
struct buttonData {

};
class UserInterface {
public:
	void panel(float panelLoc, float movePanel_x);
	void textureDemonstrator(float width,float height, float position_x,float position_y,float z);

	void box(float width, float height, float position_x, float position_y, std::string text, glm::vec3 color, float textRatio, bool isTextBox, bool isMaskImageBox, float buttonCurveReduce,float z,glm::vec3 colorTransitionColor , float mixVal);
	void iconBox(float width, float height, float position_x, float position_y,float z, unsigned int icon,float mixVal,glm::vec3 color,glm::vec3 colorHover);
	bool isMouseOnButton(GLFWwindow* window, float width, float height, float position_x, float position_y, int mouseXpos, int mouseYpos, bool isPanelMoving);
	bool isMouseOnCoords(GLFWwindow*window,int mouseXpos, int mouseYpos,std::vector<float> buttonCoor,bool isPanelMoving);
	void setViewportBgColor();
	void renderText(unsigned int program, std::string text, float x, float y, float scale);
	void uploadChars();
	void renderMenubar(GLFWwindow* window);
	void checkBox(float position_x, float position_y, std::string text, glm::vec3 color, bool mouseHover, bool checked);
	void panelChangeButton(float position_x, float position_y);
	bool isMouseOnPanelChangeButton(GLFWwindow* window, float position_x, float position_y, int mouseXpos, int mouseYpos);
	void rangeBar(float position_x, float position_y, float value);
	void colorBox(float position_x, float position_y, float valueX, float valueY);
	void colorRect(float position_x, float position_y,float value, unsigned int FBO,GLFWwindow* window);
	void decorationSquare(float position_x, float position_y);
	Icons loadIcons();
	void numericModifier(float position_x,float position_y,unsigned int leftArrow,unsigned int rightArrow,float z,int value,float mixValP,float mixValN);
	void sendProgramsToUserInterface(Programs appPrograms);
    void sendMaxWindowSize(int maxScreenWidth,int maxScreenHeight);
};
#endif 