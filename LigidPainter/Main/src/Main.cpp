/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<iostream>

#include "LigidPainter.h"

bool ligidPainter_DEBUG_MODE = false;
bool ligidPainter_ONLY_INIT = false;

int main(int argc, char *argv[]) 
{
	LigidPainter ligidPainter;

	if (argc >= 2) {
        // The first argument (argv[0]) is the program name
        // The second argument (argv[1]) is the variable name
        // The third argument (argv[2]) is the value

        std::string variableName = argv[1];

		if(variableName == "-DEBUG_MODE"){
			ligidPainter_DEBUG_MODE = true;
			std::cout << "STARTING LIGIDPAINTER IN THE DEBUG MODE" << std::endl;
		}
		else if(variableName == "-DEBUG_MODE_ONLY_INIT"){
			ligidPainter_DEBUG_MODE = true;
			ligidPainter_ONLY_INIT = true;
			std::cout << "STARTING LIGIDPAINTER IN THE DEBUG MODE & PROGRAM WILL BE AUTOMATICALLY CLOSED AFTER LOAD" << std::endl;
		}
		else{
			std::cout << "WARNING! INVALID ARGUMENT SET : " << variableName << " : DEBUG MODE IS 'DEBUG_MODE' " << std::endl;
		}
	} 
	bool end = ligidPainter.run();
	
	return 0;
}