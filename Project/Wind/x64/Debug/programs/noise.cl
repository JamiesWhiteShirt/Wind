float interpolate(float f)
{
	return 1.0f - f * f * (3.0f - f * 2.0f);
}

float noise2d(__global const uchar* noiseMap, float x, float y, uint octaves, const float smoothness)
{
	const int andField = 0b1111;
	const int sizeExponent = 4;

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

		ix = floor(x);
		ix1 = ix + 1;
		iy = floor(y);
		iy1 = iy + 1;
		xMix = interpolate(x - ix);
		xMixi = 1.0f - xMix;
		yMix = interpolate(y - iy);
		yMixi = 1.0f - yMix;

		ix = (ix & andField) << sizeExponent;
		ix1 = (ix1 & andField) << sizeExponent;
		iy = iy & andField;
		iy1 = iy1 & andField;

		noise += (noiseMap[ix | iy] * xMix * yMix
			+ noiseMap[ix1 | iy] * xMixi * yMix
			+ noiseMap[ix | iy1] * xMix * yMixi
			+ noiseMap[ix1 | iy1] * xMixi * yMixi)
			* value;

		x *= 0.5f;
		y *= 0.5f;
		value *= smoothness;
	}

	return noise / maxValue;
}

float noise3d(__global const uchar* noiseMap, float x, float y, float z, uint octaves, const float smoothness)
{
	const int andField = 0b1111;
	const int sizeExponent = 4;

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

		ix = floor(x);
		ix1 = ix + 1;
		iy = floor(y);
		iy1 = iy + 1;
		iz = floor(z);
		iz1 = iz + 1;
		xMix = interpolate(x - ix);
		xMixi = 1.0f - xMix;
		yMix = interpolate(y - iy);
		yMixi = 1.0f - yMix;
		zMix = interpolate(z - iz);
		zMixi = 1.0f - zMix;

		ix = (ix & andField) << (sizeExponent * 2);
		ix1 = (ix1 & andField) << (sizeExponent * 2);
		iy = (iy & andField) << sizeExponent;
		iy1 = (iy1 & andField) << sizeExponent;
		iz = iz & andField;
		iz1 = iz1 & andField;

		noise += (noiseMap[ix | iy | iz] * xMix * yMix * zMix
			+ noiseMap[ix1 | iy | iz] * xMixi * yMix * zMix
			+ noiseMap[ix | iy1 | iz] * xMix * yMixi * zMix
			+ noiseMap[ix1 | iy1 | iz] * xMixi * yMixi * zMix
			+ noiseMap[ix | iy | iz1] * xMix * yMix * zMixi
			+ noiseMap[ix1 | iy | iz1] * xMixi * yMix * zMixi
			+ noiseMap[ix | iy1 | iz1] * xMix * yMixi * zMixi
			+ noiseMap[ix1 | iy1 | iz1] * xMixi * yMixi * zMixi)
			* value;

		x *= 0.5f;
		y *= 0.5f;
		z *= 0.5f;
		value *= smoothness;
	}

	return noise / maxValue;
}

__kernel void fillNoiseBuffer2d(__global float* buffer, __global const uchar* noiseMap, const float smoothness, const uint octaves, const float xo, const float yo, const float xd, const float yd, const int xs, const int ys)
{
	const int index = get_global_id(0) * xs + get_global_id(1);
	const float x = (get_global_id(0)) * xd + xo;
	const float y = (get_global_id(1)) * yd + yo;

	buffer[index] = noise2d(noiseMap, x, y, octaves, smoothness);
}

__kernel void fillNoiseBufferWithSmoothness2d(__global float* buffer, __global const uchar* noiseMap, const __global float* smoothness, const uint octaves, const float xo, const float yo, const float xd, const float yd, const int xs, const int ys)
{
	const int index = get_global_id(0) * xs + get_global_id(1);
	const float x = (get_global_id(0)) * xd + xo;
	const float y = (get_global_id(1)) * yd + yo;

	buffer[index] = noise2d(noiseMap, x, y, octaves, 2.0f + smoothness[index]);
}

__kernel void fillNoiseBuffer3d(__global float* buffer, __global const uchar* noiseMap, const float smoothness, const uint octaves, const float xo, const float yo, const float zo, const float xd, const float yd, const float zd, const int xs, const int ys, const int zs)
{
	const int index = (get_global_id(0) * ys + get_global_id(1)) * xs + get_global_id(2);
	const float x = (get_global_id(0)) * xd + xo;
	const float y = (get_global_id(1)) * yd + yo;
	const float z = (get_global_id(2)) * zd + zo;

	buffer[index] = noise3d(noiseMap, x, y, z, octaves, smoothness);
}

__kernel void fillNoiseBufferWithSmoothness3d(__global float* buffer, __global const uchar* noiseMap, const __global float* smoothness, const uint octaves, const float xo, const float yo, const float zo, const float xd, const float yd, const float zd, const int xs, const int ys, const int zs)
{
	const int index = (get_global_id(0) * ys + get_global_id(1)) * xs + get_global_id(2);
	const float x = (get_global_id(0)) * xd + xo;
	const float y = (get_global_id(1)) * yd + yo;
	const float z = (get_global_id(2)) * zd + zo;

	buffer[index] = noise3d(noiseMap, x, y, z, octaves, 2.0f + smoothness[index]);
}