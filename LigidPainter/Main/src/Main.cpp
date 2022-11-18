#include<iostream>
#include "../thirdparty/include/glad/glad.h"
#include "../thirdparty/include/GLFW/glfw3.h"
#include "../thirdparty/include/glm/glm.hpp"
#include "../thirdparty/include/glm/gtc/matrix_transform.hpp"
#include "../thirdparty/include/glm/gtc/type_ptr.hpp"
#include "Core/LigidPainter.h"

int main() 
{
	bool end;
	LigidPainter ligidPainter;
	end = ligidPainter.run();
	return 0;
}