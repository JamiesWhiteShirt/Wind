#pragma once
#include <random>

class Random
{
private:
	unsigned int seed;
	//std::linear_congruential_engine<unsigned int, 2, 0, 0> randomEngine;
	std::mersenne_twister_engine<unsigned int, 32, 624, 397, 31, 0x9908b0df, 11, 0xffffffff, 7, 0x9d2c5680, 15, 0xefc60000, 18, 1812433253> randomEngine;

public:
	Random(unsigned int seed = 0U);

	unsigned int getUint(unsigned int min, unsigned int max);
	unsigned int getUint();
	float getFloat(float min = 0.0f, float max = 1.0f);
	double getDouble(double min = 0.0, double max = 1.0);
};