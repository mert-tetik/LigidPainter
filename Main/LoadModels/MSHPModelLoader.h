#pragma once
#include<iostream>
#include <vector>

struct dataOut
{
	std::string fInfo;
	std::vector <std::string> vVec;
	std::vector <std::string> vnVec;
	std::vector <std::string> vtVec;
};
class MSHP_Model_Loader {
public:
	dataOut READ_OBJ_FILE();
	std::vector<float> OBJ_getVertices();
};