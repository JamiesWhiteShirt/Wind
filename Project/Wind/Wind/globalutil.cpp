#include "globalutil.h"

Random::Random(unsigned int seed)
	: seed(seed), randomEngine()
{
	randomEngine.seed(seed);
}

unsigned int Random::getUint(unsigned int min, unsigned int max)
{
	std::uniform_int_distribution<unsigned int> distribution(min, max);
	return distribution(randomEngine);
}

unsigned int Random::getUint()
{
	return randomEngine();
}

float Random::getFloat(float min, float max)
{
	std::uniform_real_distribution<float> distribution(min, max);
	return distribution(randomEngine);
}

double Random::getDouble(double min, double max)
{
	std::uniform_real_distribution<double> distribution(min, max);
	return distribution(randomEngine);
}