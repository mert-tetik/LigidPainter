#include<iostream>
#include<GLFW/glfw3.h>

#include <fstream>
#include <sstream>
#include <glm/glm.hpp>

#include "MSHPApp.h"
#include "MSHPModelLoader.h"

#include <vector>
#include <string>

using namespace std;

	struct dataOut dataReturn;

	dataOut MSHP_Model_Loader::READ_OBJ_FILE() { //Not capable of processing every contents of '.obj' file format. 
		std::ifstream infile("test.obj");

		string vertexInfo = "";
		string faceInfo = "";

		dataOut dataReturn;
		string line;

		while (std::getline(infile, line))
		{
			cout << line << '\n';
			if (line[0] == 'f' && line[1] == ' ') {
				for (int i = 2; i < line.length(); i++)
				{
					if (line[i] != '\n')
						if (line[i] == ' ')
							dataReturn.fInfo += '/';
						else
							dataReturn.fInfo.push_back(line[i]);
				}
				dataReturn.fInfo += '/';
			}
			else if (line[0] == 'v' && line[1] == ' ') {
				dataReturn.vVec.push_back(line.erase(0,2));
			}
			else if (line[0] == 'v' && line[1] == 't' && line[2] == ' ') {
				dataReturn.vtVec.push_back(line.erase(0, 3));
			}
			else if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ') {
				dataReturn.vnVec.push_back(line.erase(0, 3));
			}
		}
		return dataReturn;
	}
	vector<float> MSHP_Model_Loader::OBJ_getVertices() {
		MSHP_Model_Loader  MSHPLoader;
		dataOut dataReturn = MSHPLoader.READ_OBJ_FILE();
		int phaseCounter = 0;
		bool countDigits = false;
		int digitCounter = 0;
		string holdData;
		string faceIndex;
		vector<float> vertices{};

		for (size_t i = 0; i < dataReturn.fInfo.length(); i++)
		{
			if (dataReturn.fInfo[i] != '/')
				faceIndex += dataReturn.fInfo[i];
			else {
				if (phaseCounter == 0) {
					for (int l = 0; l < dataReturn.vVec[stoi(faceIndex) - 1].length(); l++)
					{
						holdData += dataReturn.vVec[stoi(faceIndex) - 1][l];
						if (dataReturn.vVec[stoi(faceIndex) - 1][l] == '.') {
							countDigits = true;
						}
						if (countDigits) {
							digitCounter++;
							if (digitCounter == 7) {
								vertices.push_back(stof(holdData));
								digitCounter = 0;
								holdData = "";
								countDigits = false;
							}
						}
					}
					phaseCounter++;
				}
				else if (phaseCounter == 1) {
					for (int l = 0; l < dataReturn.vtVec[stoi(faceIndex) - 1].length(); l++)
					{
						holdData += dataReturn.vtVec[stoi(faceIndex) - 1][l];
						if (dataReturn.vtVec[stoi(faceIndex) - 1][l] == '.') {
							countDigits = true;
						}
						if (countDigits) {
							digitCounter++;
							if (digitCounter == 7) {
								vertices.push_back(stof(holdData));
								digitCounter = 0;
								holdData = "";
								countDigits = false;
							}
						}
					}
					phaseCounter++;
				}
				else if (phaseCounter == 2) {
					for (int l = 0; l < dataReturn.vnVec[stoi(faceIndex) - 1].length(); l++)
					{
						holdData += dataReturn.vnVec[stoi(faceIndex) - 1][l];
						if (dataReturn.vnVec[stoi(faceIndex) - 1][l] == '.') {
							countDigits = true;
						}
						if (countDigits) {
							digitCounter++;
							if (digitCounter == 5) {
								vertices.push_back(stof(holdData));
								digitCounter = 0;
								holdData = "";
								countDigits = false;
							}
						}
					}
					phaseCounter = 0;
				}
				faceIndex = "";
			}
		}
		return vertices;
	}