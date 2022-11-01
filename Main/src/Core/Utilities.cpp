#include<iostream>
#include<glad/glad.h>
#include <vector>
#include<GLFW/glfw3.h>
#include<windows.h>
#include <string>
#include <fstream>
#include <sstream>
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//#include <glm/gtx/string_cast.hpp>
#include "Utilities.h"
#include "stb_image_write.h"

using namespace std;

std::string Utilities::readFile(const char* path) {
	string fullText;
	fstream my_file;
	my_file.open(path, ios::in);
	if (my_file.is_open()) {
		char mychar;
		while (my_file) {
			mychar = my_file.get();
			fullText += mychar;
		}
	}
	my_file.close();
	return fullText;
}
string Utilities::getLastWordBySeparatingWithChar(string s, char del)
{
	vector<string> words;
	stringstream ss(s);
	string word;
	while (!ss.eof()) {
		getline(ss, word, del);
		words.push_back(word);
	}
	return words[words.size() - 1];
}
string Utilities::openFileDialog() { //!!Using Win header!!
	OPENFILENAME ofn;       // common dialog box structure
	char szFile[260];       // buffer for file name
	HANDLE hf;              // file handle

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = szFile;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 

	if (GetOpenFileName(&ofn) == TRUE)
		return (string)ofn.lpstrFile;
	return "";
}