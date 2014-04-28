#include <stdlib.h>

namespace Noise
{
	class NoiseGenerator2D
	{
	private:
		int sizeExponent;
		int size;
		int octaves;
		float smoothness;
		int seed;

		unsigned char noiseAt(int &x, int &y);
		float interpolate(float f);
	public:
		unsigned char* noiseMap;

		NoiseGenerator2D();
		NoiseGenerator2D(int sizeExponent, int octaves, float smoothness, unsigned int seed);
		~NoiseGenerator2D();

		float getNoise(float x, float y);
		float getNoise(float x, float y, float smoothness);
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
		float interpolate(float f);
	public:
		unsigned char* noiseMap;

		NoiseGenerator3D();
		NoiseGenerator3D(int sizeExponent, int octaves, float smoothness, unsigned int seed);
		~NoiseGenerator3D();

		float getNoise(float x, float y, float z);
		float getNoise(float x, float y, float z, float smoothness);
	};
}