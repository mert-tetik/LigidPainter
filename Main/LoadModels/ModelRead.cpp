#include<iostream>
#include<GLFW/glfw3.h>
#include<windows.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "MSHPApp.h"
#include "MSHPModelLoader.h"

#include <vector>
#include <string>

using namespace std;

	struct dataOut dataReturn;

	dataOut MSHP_Model_Loader::READ_OBJ_FILE() { //Not capable of processing every contents of '.obj' file format.
		MSHPApp MSHP;
		string source = MSHP.READ_FILE("test.obj");

		string vertexInfo = "";
		string faceInfo = "";

		dataOut dataReturn;


		bool enableReading_v = false;
		bool enableReading_vn = false;
		bool enableReading_vt = false;
		bool enableReading_f = false;

		vector <string> vertexVec;
		vector <string> normalVec;
		vector <string> txtrCoorVec;

		int dot_counter = 0;
		bool lastDigits = false;
		int digit_counter = 0;
		for (size_t i = 0; i < source.length(); i++)
		{
			if (source[i] == 'v' && source[i + 1] == ' ') {
				enableReading_v = true;
				i += 2;
			}
			if (source[i] == 'v' && source[i + 1] == 'n') {
				enableReading_vn = true;
				i += 2;
			}
			if (source[i] == 'v' && source[i + 1] == 't') {
				enableReading_vt = true;
				i += 2;
			}
			if (source[i] == 'f' && source[i + 1] == ' ') {
				enableReading_f = true;
				i += 2;
			}
			if (enableReading_v) {
				vertexInfo += source[i];
				if (source[i] == '.') {
					dot_counter++;
					if (dot_counter == 3) {
						lastDigits = true;
					}
				}
				if (lastDigits) {
					digit_counter++;
					if (digit_counter == 7) {
						vertexVec.push_back(vertexInfo);
						vertexInfo = "";
						enableReading_v = false;
						lastDigits = false;
						digit_counter = 0;
						dot_counter = 0;
					}
				}
			}
			if (enableReading_vn) {
				vertexInfo += source[i];
				if (source[i] == '.') {
					dot_counter++;
					if (dot_counter == 3) {
						lastDigits = true;
					}
				}
				if (lastDigits) {
					digit_counter++;
					if (digit_counter == 5) {
						normalVec.push_back(vertexInfo);
						vertexInfo = "";
						enableReading_vn = false;
						lastDigits = false;
						digit_counter = 0;
						dot_counter = 0;
					}
				}
			}
			if (enableReading_vt) {
				vertexInfo += source[i];
				if (source[i] == '.') {
					dot_counter++;
					if (dot_counter == 2) {
						lastDigits = true;
					}
				}
				if (lastDigits) {
					digit_counter++;
					if (digit_counter == 7) {
						txtrCoorVec.push_back(vertexInfo);
						vertexInfo = "";
						enableReading_vt = false;
						lastDigits = false;
						digit_counter = 0;
						dot_counter = 0;
					}
				}
			}
			if (enableReading_f) {
				if (source[i] == '\n' || source[i] == ' ') {
					faceInfo += '/';
				}
				else
					faceInfo += source[i];
			}
		}
		dataReturn.fInfo = faceInfo;
		dataReturn.vVec = vertexVec;
		dataReturn.vtVec = txtrCoorVec;
		dataReturn.vnVec = normalVec;

		return dataReturn;
	}
	vector<float> MSHP_Model_Loader::OBJ_getVertices() {
		MSHP_Model_Loader  MSHPLoader;
		dataOut dataReturn = MSHPLoader.READ_OBJ_FILE();
		int phaseCounter = 0;
		bool countDigits = 0;
		int digitCounter = 0;
		string holdData;
		string faceIndex;
		vector<float> vertices{};

		for (size_t i = 0; i < dataReturn.fInfo.length(); i++)
		{
			if (dataReturn.fInfo[i] != '/')
				faceIndex += dataReturn.fInfo[i];
			else {
				//cout << faceIndex << '\n';
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