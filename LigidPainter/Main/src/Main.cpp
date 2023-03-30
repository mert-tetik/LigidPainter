#include<iostream>

#include "Core/LigidPainter.h"

int main() 
{
	// ! FRC-SCOUT by Sowamon
    std::cout << "Started" << std::endl;
	LigidPainter ligidPainter;
	bool end = ligidPainter.run();
	return 0;
}