#pragma once

#include <stdlib.h>
#include "opencl.h"

namespace Noise
{
	void staticInit();

	extern cl::Program noiseProgram;

	class NoiseGenerator1D
	{
	private:
		int sizeExponent;
		int size;
		int octaves;
		float smoothness;
		int seed;

		unsigned char noiseAt(int &x);

		cl::Buffer noiseMapBuffer;
	public:
		unsigned char* noiseMap;

		NoiseGenerator1D();
		NoiseGenerator1D(int sizeExponent, int octaves, float smoothness, unsigned int seed);
		~NoiseGenerator1D();

		float getNoise(float x);
		float getNoise(float x, float smoothness);

		void prepareForCL();
		void fillNoiseBuffer(cl::CommandQueue& queue, cl::Buffer& buffer, unsigned int xs, float x, float xd);
		void fillNoiseBuffer(cl::CommandQueue& queue, cl::Buffer& buffer, unsigned int xs, float smoothness, float x, float xd);
		void fillNoiseBufferWithSmoothness(cl::CommandQueue& queue, cl::Buffer& buffer, unsigned int xs, cl::Buffer& smoothness, float x, float xd);
	};

	class NoiseGenerator2D
	{
	private:
		int sizeExponent;
		int size;
		int octaves;
		float smoothness;
		int seed;

		unsigned char noiseAt(int &x, int &y);

		cl::Buffer noiseMapBuffer;
	public:
		unsigned char* noiseMap;

		NoiseGenerator2D();
		NoiseGenerator2D(int sizeExponent, int octaves, float smoothness, unsigned int seed);
		~NoiseGenerator2D();

		float getNoise(float x, float y);
		float getNoise(float x, float y, float smoothness);

		void prepareForCL();
		void fillNoiseBuffer(cl::CommandQueue& queue, cl::Buffer& buffer, unsigned int xs, unsigned int ys, float x, float y, float xd, float yd);
		void fillNoiseBuffer(cl::CommandQueue& queue, cl::Buffer& buffer, unsigned int xs, unsigned int ys, float smoothness, float x, float y, float xd, float yd);
		void fillNoiseBufferWithSmoothness(cl::CommandQueue& queue, cl::Buffer& buffer, unsigned int xs, unsigned int ys, cl::Buffer& smoothness, float x, float y, float xd, float yd);
	};

	class NoiseGenerator3D
	{
	private:
		int sizeExponent;
		int size;
		int octaves;
		float smoothness;
		int seed;

		unsigned char noiseAt(int &x, int &y, int &z);

		cl::Buffer noiseMapBuffer;
	public:
		unsigned char* noiseMap;

		NoiseGenerator3D();
		NoiseGenerator3D(int sizeExponent, int octaves, float smoothness, unsigned int seed);
		~NoiseGenerator3D();

		float getNoise(float x, float y, float z);
		float getNoise(float x, float y, float z, float smoothness);

		void prepareForCL();
		void fillNoiseBuffer(cl::CommandQueue& queue, cl::Buffer& buffer, unsigned int xs, unsigned int ys, unsigned int zs, float x, float y, float z, float xd, float yd, float zd);
		void fillNoiseBuffer(cl::CommandQueue& queue, cl::Buffer& buffer, unsigned int xs, unsigned int ys, unsigned int zs, float smoothness, float x, float y, float z, float xd, float yd, float zd);
		void fillNoiseBufferWithSmoothness(cl::CommandQueue& queue, cl::Buffer& buffer, unsigned int xs, unsigned int ys, unsigned int zs, cl::Buffer& smoothness, float x, float y, float z, float xd, float yd, float zd);
	};
}