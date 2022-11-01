#ifndef RGDMODELLOADER_H
#define RGDMODELLOADER_H
#include <vector>

struct ModelData
{
	std::string fInfo;//Face info
	std::vector <std::string> vVec; //vertex 
	std::vector <std::string> vnVec; //vertex normal 
	std::vector <std::string> vtVec; //vertex texture coord
};
class ModelLoader { //3D Model file processing
public:
	ModelData READ_OBJ_FILE(std::string path, bool autoTriangulate);//Distinguish 
	std::vector<float> OBJ_getVertices(std::string path, bool autoTriangulate);//Create a vector of the 3d model using distinguished data
};
#endif