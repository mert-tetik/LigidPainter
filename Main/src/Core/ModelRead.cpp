#include<iostream>
#include<GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include "MSHPApp.h"
#include <vector>
#include <string>
#include <glm/gtx/string_cast.hpp>
using namespace std;

	struct dataOut dataReturn;

	dataOut MSHP_Model_Loader::READ_OBJ_FILE() { //Not capable of processing every contents of '.obj' file format. 
		std::ifstream infile("untriangulatedSphere.obj");

		int slashCounter = 0;
		vector<glm::vec3> hold1F;
		glm::vec3 holdFV;
		string holdF;
		string holdVtoLine;

		string vertexInfo = "";
		string faceInfo = "";

		dataOut dataReturn;
		string line;

		while (std::getline(infile, line))
		{
			if (line[0] == 'f' && line[1] == ' ') {
				for (size_t i = 2; i < line.length(); i++)
				{
					if (line[i] == '/' || line[i] == ' ' || line[i] == '\n') {
						slashCounter++;
						if (slashCounter == 1) {
							holdFV.x = stoi(holdF);
						}
						else if (slashCounter == 2) {
							holdFV.y = stoi(holdF);
						}
						else if (slashCounter == 3) {
							holdFV.z = stoi(holdF);
							hold1F.push_back(holdFV);
							slashCounter = 0;
						}
						holdF = "";
					}
					else{
						holdF += line[i];
					}
				}
				holdFV.z = stoi(holdF);
				hold1F.push_back(holdFV);
				slashCounter = 0;
				holdF = "";
				for (size_t i = 0; i < hold1F.size(); i++)
				{
					cout << glm::to_string(hold1F[i]) << '\n';
				}
				for (int x = 0; x < hold1F.size()-2; x++)
				{
					holdVtoLine = to_string((int)hold1F[0].x) + "/" + to_string((int)hold1F[0].y) + "/" + to_string((int)hold1F[0].z) + " " + to_string((int)hold1F[x+1].x) + "/" + to_string((int)hold1F[x+1].y) + "/" + to_string((int)hold1F[x+1].z) + " " + to_string((int)hold1F[x+2].x) + "/" + to_string((int)hold1F[x+2].y) + "/" + to_string((int)hold1F[x+2].z);
					cout << holdVtoLine << '\n';
					for (int i = 0; i < holdVtoLine.length(); i++)
					{
						if (holdVtoLine[i] != '\n')
							if (holdVtoLine[i] == ' ')
								dataReturn.fInfo += '/';
							else
								dataReturn.fInfo.push_back(holdVtoLine[i]);
					}
					dataReturn.fInfo += '/';
				}

				//cout << dataReturn.fInfo << '\n';

				hold1F.clear();
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
		//cout << dataReturn.fInfo;
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