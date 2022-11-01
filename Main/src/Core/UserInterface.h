#ifndef RGDUI_H
#define RGDUI_H

struct ColorData //RigidPainter color palette
{
	glm::vec3 viewportBackColor = glm::vec3(0.23f, 0.23f, 0.23f);
	glm::vec3 panelColor = glm::vec3(0.15f, 0.15f, 0.15f);
	glm::vec3 panelHoldColor = glm::vec3(0.05f, 0.05f, 0.05f);
	glm::vec3 buttonColor = glm::vec3(0.23f, 0.23f, 0.23f);
	glm::vec3 menuBarColor = glm::vec3(0.05f, 0.05f, 0.05f);
	glm::vec3 textBoxColor = glm::vec3(0.05f, 0.05f, 0.05f);
	glm::vec3 checkBoxColor = glm::vec3(0.05f, 0.05f, 0.05f);
	glm::vec3 checkBoxCheckedColor = glm::vec3(0.17f, 0.17f, 0.17f);
	glm::vec3 rangeBarBack = glm::vec3(0.1f, 0.1f, 0.1f);
	glm::vec3 rangeBarFront = glm::vec3(0.6f, 0.6f, 0.6f);
	glm::vec3 textColor = glm::vec3(1.0f, 1.0f, 1.0f);
};
struct buttonData {

};
class UserInterface {
public:
	void panel(float panelLoc, float movePanel_x);
	void box(float width, float height, float position_x, float position_y, std::string text, glm::vec3 color, float textRatio, bool isTextBox, bool isMaskImageBox);
	bool isMouseOnButton(GLFWwindow* window, float width, float height, float position_x, float position_y, int mouseXpos, int mouseYpos, bool isPanelMoving);
	void setViewportBgColor();
	void renderText(unsigned int program, std::string text, float x, float y, float scale, glm::vec3 color);
	void uploadChars();
	void renderMenubar(GLFWwindow* window);
	void checkBox(float position_x, float position_y, std::string text, glm::vec3 color, bool mouseHover, bool checked);
	void panelChangeButton(float position_x, float position_y);
	bool isMouseOnPanelChangeButton(GLFWwindow* window, float position_x, float position_y, int mouseXpos, int mouseYpos);
	void rangeBar(float position_x, float position_y, float value);
	void colorBox(float position_x, float position_y);
	void colorRect(float position_x, float position_y);
};
#endif