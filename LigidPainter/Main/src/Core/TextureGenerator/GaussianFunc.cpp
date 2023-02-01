#include <iostream>
#include <vector>
#include <math.h>
#include "TextureGenerator.h"


std::vector<float> TextureGenerator::gaussianFunc(int radius,float mean){
	std::vector<float> oneDimensionalGauss;
	
	double sigma = ((double)radius / sqrt(-2.0 * log(1 - 0.995))) ;

	const float pi = 3.14159265359;

	for (int i = 0; i < radius*2 + 1; i++)
	{
    	double denominator = sigma * sqrt(2.0 * pi);
    	double expon = exp( (-1.0/2.0) * (pow(((double)i - (double)radius) - mean , 2.0) / pow(sigma,2.0)));

    	double a = 1.0 / denominator;
    	double result = a * expon;
		oneDimensionalGauss.push_back(result);
	}

    return oneDimensionalGauss;
}