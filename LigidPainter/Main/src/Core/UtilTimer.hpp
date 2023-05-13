#include<glad/glad.h>
#include<GLFW/glfw3.h>

double programFPS = 1;

double lastTimeT = glfwGetTime();
int nbFramesT = 0;
void calculateTheSpeed(bool debugMode,GLFWwindow* window) {
	double currentTime = glfwGetTime();
	nbFramesT++;
	if (currentTime - lastTimeT >= 1.0) {
		if(nbFramesT > 5 && nbFramesT < 800)
            programFPS = (double)nbFramesT;
		
        nbFramesT = 0;
		lastTimeT += 1.0;
	}
}