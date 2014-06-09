#pragma once

#define _USE_MATH_DEFINES

#include "noise.h"
#include <math.h>
#include "ioutil.h"

using namespace Noise;

cl::Program Noise::noiseProgram;

float interpolate(float f)
{
	//return (cos(f * M_PI) + 1.0f) * 0.5f;
	return 1.0f - f * f * (3 - f * 2);
}

void Noise::staticInit()
{
	Noise::noiseProgram.create(IOUtil::EXE_DIR + L"\\programs\\noise.cl");
}

unsigned char NoiseGenerator1D::noiseAt(int &x)
{
	const int andField = size - 1;
	return noiseMap[x & andField];
}

NoiseGenerator1D::NoiseGenerator1D()
{

}

NoiseGenerator1D::NoiseGenerator1D(int sizeExponent, int octaves, float smoothness, unsigned int seed)
	: sizeExponent(sizeExponent), size(1 << sizeExponent), octaves(octaves), smoothness(smoothness), seed(seed), noiseMap(new unsigned char[size])
{
	srand(seed);
	for(int i = 0; i < size; i++)
	{
		noiseMap[i] = rand() & 1;
	}
}

NoiseGenerator1D::~NoiseGenerator1D()
{
	delete[] noiseMap;
}

float NoiseGenerator1D::getNoise(float x)
{
	return getNoise(x, smoothness);
}

float NoiseGenerator1D::getNoise(float x, float smoothness)
{
	float noise = 0.0f;
	float value = 1.0f;
	float maxValue = 0.0f;

	int ix;
	int ix1;
	float xMix;
	float xMixi;

	for(int i = 0; i < octaves; i++)
	{
		maxValue += value;

		ix = (int)floorf(x);
		ix1 = ix + 1;
		xMix = interpolate(x - ix);
		xMixi = 1.0f - xMix;

		noise += (noiseAt(ix) * xMix
			+ noiseAt(ix1) * xMixi)
			* value;

		x *= 0.5f;
		value *= smoothness;
	}

	return noise / maxValue;
}

void NoiseGenerator1D::prepareForCL()
{
	noiseMapBuffer.create(sizeof(unsigned char) * size, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, noiseMap);
}

void NoiseGenerator1D::fillNoiseBuffer(cl::CommandQueue& queue, cl::Buffer& buffer, unsigned int xs, float x, float xd)
{
	fillNoiseBuffer(queue, buffer, xs, smoothness, x, xd);
}

void NoiseGenerator1D::fillNoiseBuffer(cl::CommandQueue& queue, cl::Buffer& buffer, unsigned int xs, float smoothness, float x, float xd)
{
	const size_t local_ws[] = {1};
	const size_t global_ws[] = {xs};
	
	noiseProgram.prepare("fillNoiseBuffer1d");
	noiseProgram.setArgument(sizeof(cl_mem), &buffer);
	noiseProgram.setArgument(sizeof(cl_mem), &noiseMapBuffer);
	noiseProgram.setArgument(sizeof(unsigned int), &sizeExponent);
	noiseProgram.setArgument(sizeof(float), &smoothness);
	noiseProgram.setArgument(sizeof(unsigned int), &octaves);
	noiseProgram.setArgument(sizeof(float), &x);
	noiseProgram.setArgument(sizeof(float), &xd);
	noiseProgram.setArgument(sizeof(unsigned int), &xs);
	noiseProgram.invoke(queue, 1, global_ws, local_ws);
}

void NoiseGenerator1D::fillNoiseBufferWithSmoothness(cl::CommandQueue& queue, cl::Buffer& buffer, unsigned int xs, cl::Buffer& smoothness, float x, float xd)
{
	const size_t local_ws[] = {1};
	const size_t global_ws[] = {xs};
	
	noiseProgram.prepare("fillNoiseBufferWithSmoothness1d");
	noiseProgram.setArgument(sizeof(cl_mem), &buffer);
	noiseProgram.setArgument(sizeof(cl_mem), &noiseMapBuffer);
	noiseProgram.setArgument(sizeof(unsigned int), &sizeExponent);
	noiseProgram.setArgument(sizeof(cl_mem), &smoothness);
	noiseProgram.setArgument(sizeof(unsigned int), &octaves);
	noiseProgram.setArgument(sizeof(float), &x);
	noiseProgram.setArgument(sizeof(float), &xd);
	noiseProgram.setArgument(sizeof(unsigned int), &xs);
	noiseProgram.invoke(queue, 2, global_ws, local_ws);
}

unsigned char NoiseGenerator2D::noiseAt(int &x, int &y)
{
	const int andField = size - 1;
	return noiseMap[(x & andField) << sizeExponent | y & andField];
}

NoiseGenerator2D::NoiseGenerator2D()
{

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

		ix = (int)floorf(x);
		ix1 = ix + 1;
		iy = (int)floorf(y);
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

void NoiseGenerator2D::prepareForCL()
{
	noiseMapBuffer.create(sizeof(unsigned char) * size * size, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, noiseMap);
}

void NoiseGenerator2D::fillNoiseBuffer(cl::CommandQueue& queue, cl::Buffer& buffer, unsigned int xs, unsigned int ys, float x, float y, float xd, float yd)
{
	fillNoiseBuffer(queue, buffer, xs, ys, smoothness, x, y, xd, yd);
}

void NoiseGenerator2D::fillNoiseBuffer(cl::CommandQueue& queue, cl::Buffer& buffer, unsigned int xs, unsigned int ys, float smoothness, float x, float y, float xd, float yd)
{
	const size_t local_ws[] = {1, 1};
	const size_t global_ws[] = {xs, ys};
	
	noiseProgram.prepare("fillNoiseBuffer2d");
	noiseProgram.setArgument(sizeof(cl_mem), &buffer);
	noiseProgram.setArgument(sizeof(cl_mem), &noiseMapBuffer);
	noiseProgram.setArgument(sizeof(unsigned int), &sizeExponent);
	noiseProgram.setArgument(sizeof(float), &smoothness);
	noiseProgram.setArgument(sizeof(unsigned int), &octaves);
	noiseProgram.setArgument(sizeof(float), &x);
	noiseProgram.setArgument(sizeof(float), &y);
	noiseProgram.setArgument(sizeof(float), &xd);
	noiseProgram.setArgument(sizeof(float), &yd);
	noiseProgram.setArgument(sizeof(unsigned int), &xs);
	noiseProgram.setArgument(sizeof(unsigned int), &ys);
	noiseProgram.invoke(queue, 2, global_ws, local_ws);
}

void NoiseGenerator2D::fillNoiseBufferWithSmoothness(cl::CommandQueue& queue, cl::Buffer& buffer, unsigned int xs, unsigned int ys, cl::Buffer& smoothness, float x, float y, float xd, float yd)
{
	const size_t local_ws[] = {1, 1};
	const size_t global_ws[] = {xs, ys};
	
	noiseProgram.prepare("fillNoiseBufferWithSmoothness2d");
	noiseProgram.setArgument(sizeof(cl_mem), &buffer);
	noiseProgram.setArgument(sizeof(cl_mem), &noiseMapBuffer);
	noiseProgram.setArgument(sizeof(unsigned int), &sizeExponent);
	noiseProgram.setArgument(sizeof(cl_mem), &smoothness);
	noiseProgram.setArgument(sizeof(unsigned int), &octaves);
	noiseProgram.setArgument(sizeof(float), &x);
	noiseProgram.setArgument(sizeof(float), &y);
	noiseProgram.setArgument(sizeof(float), &xd);
	noiseProgram.setArgument(sizeof(float), &yd);
	noiseProgram.setArgument(sizeof(unsigned int), &xs);
	noiseProgram.setArgument(sizeof(unsigned int), &ys);
	noiseProgram.invoke(queue, 2, global_ws, local_ws);
}

unsigned char NoiseGenerator3D::noiseAt(int &x, int &y, int &z)
{
	const int andField = size - 1;
	return noiseMap[((x & andField) << sizeExponent | y & andField) << sizeExponent | z & andField];
}

NoiseGenerator3D::NoiseGenerator3D()
{

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

		ix = (int)floorf(x);
		ix1 = ix + 1;
		iy = (int)floorf(y);
		iy1 = iy + 1;
		iz = (int)floorf(z);
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

void NoiseGenerator3D::prepareForCL()
{
	noiseMapBuffer.create(sizeof(unsigned char) * size * size * size, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, noiseMap);
}

void NoiseGenerator3D::fillNoiseBuffer(cl::CommandQueue& queue, cl::Buffer& buffer, unsigned int xs, unsigned int ys, unsigned int zs, float x, float y, float z, float xd, float yd, float zd)
{
	fillNoiseBuffer(queue, buffer, xs, ys, zs, smoothness, x, y, z, xd, yd, zd);
}

void NoiseGenerator3D::fillNoiseBuffer(cl::CommandQueue& queue, cl::Buffer& buffer, unsigned int xs, unsigned int ys, unsigned int zs, float smoothness, float x, float y, float z, float xd, float yd, float zd)
{
	const size_t local_ws[] = {1, 1, 1};
	const size_t global_ws[] = {xs, ys, zs};
	
	noiseProgram.prepare("fillNoiseBuffer3d");
	noiseProgram.setArgument(sizeof(cl_mem), &buffer);
	noiseProgram.setArgument(sizeof(cl_mem), &noiseMapBuffer);
	noiseProgram.setArgument(sizeof(unsigned int), &sizeExponent);
	noiseProgram.setArgument(sizeof(float), &smoothness);
	noiseProgram.setArgument(sizeof(unsigned int), &octaves);
	noiseProgram.setArgument(sizeof(float), &x);
	noiseProgram.setArgument(sizeof(float), &y);
	noiseProgram.setArgument(sizeof(float), &z);
	noiseProgram.setArgument(sizeof(float), &xd);
	noiseProgram.setArgument(sizeof(float), &yd);
	noiseProgram.setArgument(sizeof(float), &zd);
	noiseProgram.setArgument(sizeof(unsigned int), &xs);
	noiseProgram.setArgument(sizeof(unsigned int), &ys);
	noiseProgram.setArgument(sizeof(unsigned int), &zs);
	noiseProgram.invoke(queue, 3, global_ws, local_ws);
}

void NoiseGenerator3D::fillNoiseBufferWithSmoothness(cl::CommandQueue& queue, cl::Buffer& buffer, unsigned int xs, unsigned int ys, unsigned int zs, cl::Buffer& smoothness, float x, float y, float z, float xd, float yd, float zd)
{
	const size_t local_ws[] = {1, 1, 1};
	const size_t global_ws[] = {xs, ys, zs};
	
	noiseProgram.prepare("fillNoiseBufferWithSmoothness3d");
	noiseProgram.setArgument(sizeof(cl_mem), &buffer);
	noiseProgram.setArgument(sizeof(cl_mem), &noiseMapBuffer);
	noiseProgram.setArgument(sizeof(unsigned int), &sizeExponent);
	noiseProgram.setArgument(sizeof(cl_mem), &smoothness);
	noiseProgram.setArgument(sizeof(unsigned int), &octaves);
	noiseProgram.setArgument(sizeof(float), &x);
	noiseProgram.setArgument(sizeof(float), &y);
	noiseProgram.setArgument(sizeof(float), &z);
	noiseProgram.setArgument(sizeof(float), &xd);
	noiseProgram.setArgument(sizeof(float), &yd);
	noiseProgram.setArgument(sizeof(float), &zd);
	noiseProgram.setArgument(sizeof(unsigned int), &xs);
	noiseProgram.setArgument(sizeof(unsigned int), &ys);
	noiseProgram.setArgument(sizeof(unsigned int), &zs);
	noiseProgram.invoke(queue, 3, global_ws, local_ws);
}