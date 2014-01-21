#define _USE_MATH_DEFINES

#include "noise.h"
#include <math.h>

using namespace Noise;

unsigned char NoiseGenerator2D::noiseAt(int &x, int &y)
{
	int andField = size - 1;
	return noiseMap[(x & andField) | ((y & andField) << sizeExponent)];
}

float NoiseGenerator2D::interpolate(float f)
{
	return (cos(f * M_PI) + 1.0f) * 0.5f;
}

NoiseGenerator2D::NoiseGenerator2D(int sizeExponent, int octaves, float smoothness, unsigned int seed)
	: sizeExponent(sizeExponent), size(1 << sizeExponent), octaves(octaves), smoothness(smoothness), seed(seed), noiseMap(new unsigned char[size * size])
{
	srand(seed);
	for(int i = 0; i < size * size; i++)
	{
		noiseMap[i] = rand() & 1;
	}
}

NoiseGenerator2D::~NoiseGenerator2D()
{
	delete[] noiseMap;
}

float NoiseGenerator2D::getNoise(float x, float y)
{
	return getNoise(x, y, smoothness);
}

float NoiseGenerator2D::getNoise(float x, float y, float smoothness)
{
	float noise = 0.0f;
	float value = 1.0f;
	float maxValue = 0.0f;

	int ix;
	int ix1;
	int iy;
	int iy1;
	float xMix;
	float xMixi;
	float yMix;
	float yMixi;

	for(int i = 0; i < octaves; i++)
	{
		maxValue += value;

		ix = (int)x;
		ix1 = ix + 1;
		iy = (int)y;
		iy1 = iy + 1;
		xMix = interpolate(x - ix);
		xMixi = 1.0f - xMix;
		yMix = interpolate(y - iy);
		yMixi = 1.0f - yMix;

		noise += (noiseAt(ix, iy) * xMix * yMix
			+ noiseAt(ix1, iy) * xMixi * yMix
			+ noiseAt(ix, iy1) * xMix * yMixi
			+ noiseAt(ix1, iy1) * xMixi * yMixi)
			* value;

		x *= 0.5f;
		y *= 0.5f;
		value *= smoothness;
	}

	return noise / maxValue;
}

unsigned char NoiseGenerator3D::noiseAt(int &x, int &y, int &z)
{
	int andField = size - 1;
	int pos = (x & andField) | ((y & andField) << sizeExponent) | ((z & andField) << (sizeExponent * 2));
	unsigned char c = noiseMap[pos];
	return noiseMap[(x & andField) | ((y & andField) << sizeExponent) | ((z & andField) << (sizeExponent * 2))];
}

float NoiseGenerator3D::interpolate(float f)
{
	//return (cos(f * M_PI) + 1.0f) * 0.5f;
	return 1.0f - f * f * (3 - f * 2);
}

NoiseGenerator3D::NoiseGenerator3D(int sizeExponent, int octaves, float smoothness, unsigned int seed)
	: sizeExponent(sizeExponent), size(1 << sizeExponent), octaves(octaves), smoothness(smoothness), seed(seed), noiseMap(new unsigned char[size * size * size])
{
	srand(seed);
	for(int i = 0; i < size * size * size; i++)
	{
		noiseMap[i] = rand() & 1;
	}
}

NoiseGenerator3D::~NoiseGenerator3D()
{
	delete[] noiseMap;
}

float NoiseGenerator3D::getNoise(float x, float y, float z)
{
	return getNoise(x, y, z, smoothness);
}

float NoiseGenerator3D::getNoise(float x, float y, float z, float smoothness)
{
	float noise = 0.0f;
	float value = 1.0f;
	float maxValue = 0.0f;

	int ix;
	int ix1;
	int iy;
	int iy1;
	int iz;
	int iz1;
	float xMix;
	float xMixi;
	float yMix;
	float yMixi;
	float zMix;
	float zMixi;

	for(int i = 0; i < octaves; i++)
	{
		maxValue += value;

		ix = (int)x;
		ix1 = ix + 1;
		iy = (int)y;
		iy1 = iy + 1;
		iz = (int)z;
		iz1 = iz + 1;
		xMix = interpolate(x - ix);
		xMixi = 1.0f - xMix;
		yMix = interpolate(y - iy);
		yMixi = 1.0f - yMix;
		zMix = interpolate(z - iz);
		zMixi = 1.0f - zMix;

		noise += (noiseAt(ix, iy, iz) * xMix * yMix * zMix
			+ noiseAt(ix1, iy, iz) * xMixi * yMix * zMix
			+ noiseAt(ix, iy1, iz) * xMix * yMixi * zMix
			+ noiseAt(ix1, iy1, iz) * xMixi * yMixi * zMix
			+ noiseAt(ix, iy, iz1) * xMix * yMix * zMixi
			+ noiseAt(ix1, iy, iz1) * xMixi * yMix * zMixi
			+ noiseAt(ix, iy1, iz1) * xMix * yMixi * zMixi
			+ noiseAt(ix1, iy1, iz1) * xMixi * yMixi * zMixi)
			* value;

		x *= 0.5f;
		y *= 0.5f;
		z *= 0.5f;
		value *= smoothness;
	}

	return noise / maxValue;
}