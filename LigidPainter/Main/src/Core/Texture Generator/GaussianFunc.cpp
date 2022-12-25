#include <iostream>
#include <vector>
#include "TextureGenerator.h"


std::vector<float> TextureGenerator::gaussianFunc(float radius,float mean){
	// vector<float> oneDimensionalGauss = {0.0003,	0.0004,	0.0007,	0.0012,	0.0019,	0.0029,	0.0044,	0.0064,	0.0090,	0.0124,	0.0166,	0.0216,	0.0274,	0.0337,	0.0404,	0.0470,	0.0532,	0.0587,	0.0629,	0.0655,	0.0665,	0.0655,	0.0629,	0.0587,	0.0532,	0.0470,	0.0404,	0.0337,	0.0274,	0.0216,	0.0166,	0.0124,	0.0090,	0.0064,	0.0044,	0.0029,	0.0019,	0.0012,	0.0007,	0.0004,	0.0003};
	std::vector<float> oneDimensionalGauss;
	
	float sigma = (radius / sqrt(-2.0 * log(1 - 0.995))) ;

	const float pi = 3.14159265359f;

	for (int i = 0; i < radius*2 + 1; i++)
	{
    	float denominator = sigma * sqrt(2.0f * pi);
    	float expon = exp( (-1.0f/2.0f) * (pow((float)i-((float)radius) - mean , 2.0f) / pow(sigma,2.0f)));

    	float a = 1.0f / denominator;
    	float result = a * expon;
		oneDimensionalGauss.push_back(result);
	}

    return oneDimensionalGauss;
}