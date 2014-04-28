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

__kernel void generateTerrain_p1(__global ushort* blocks, const int chunkX, const int chunkY, const int chunkZ, __global const uchar* noiseMap1, __global const uchar* noiseMap2)
{
	const int x = chunkX * 16 + get_global_id(0);
	const int y = chunkY * 16 + get_global_id(1);
	const int z = chunkZ * 16 + get_global_id(2);

	const int index = (get_global_id(0) << 8) | (get_global_id(1) << 4) | get_global_id(2);

	if(y < -32)
	{
		blocks[index] = 1;
	}
	else if(y >= 64)
	{
		blocks[index] = 0;
	}
	else
	{
		float smoothness = 2.0f + noise3d(noiseMap2, x * 4.0f, y * 4.0f, z * 4.0f, 12, 2.5f);
		float n2 = noise3d(noiseMap1, x * 20.0f, y * 25.0f, z * 20.0f, 12, smoothness);

		if((y + 32) / 96.0f < n2)
		//if((y + 32) / 96.0f < smoothness)
		{
			blocks[index] = 1;
		}
		else
		{
			if(y < 0)
			{
				blocks[index] = 2;
			}
			else
			{
				blocks[index] = 0;
			}
		}
	}
}