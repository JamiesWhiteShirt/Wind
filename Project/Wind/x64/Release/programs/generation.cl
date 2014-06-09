__kernel void generateTerrain_1(__global ushort* blocks, const int chunkX, const int chunkY, const int chunkZ, __global const float* minHeightNoise, __global const float* maxHeightNoise, __global const float* smoothnessNoise, __global const float* solidNoise, __global const float* stoneNoise, __global const float* temperatureNoise, __global const float* humidityNoise)
{
	const int x = chunkX * 16 + get_global_id(0);
	const int y = chunkY * 16 + get_global_id(1);
	const int z = chunkZ * 16 + get_global_id(2);

	const int blockIndex = (get_global_id(0) << 8) | (get_global_id(1) << 4) | get_global_id(2);
	const int index2d = get_global_id(0) * 18 + get_global_id(2);
	const int index3d = ((get_global_id(0) + 1) * 18 + (get_global_id(1) + 1)) * 18 + get_global_id(2) + 1;

	if(y < -64)
	{
		blocks[blockIndex] = 1;
	}
	else if(y >= 128)
	{
		blocks[blockIndex] = 0;
	}
	else
	{
		const float minHeight = minHeightNoise[index2d] * 95.0f - 64.0f;
		const float maxHeight = maxHeightNoise[index2d] * 96.0f + minHeight + 1.0f;
		const float heightDiff = maxHeight - minHeight;

		const float tolerance = (y - minHeight) / heightDiff;

		const float n1 = smoothnessNoise[index3d];
		const float n2 = solidNoise[index3d];
		const float n3 = stoneNoise[index3d];

		const float f1 = (heightDiff / 384.0f) * ((1.0f - tolerance) * (n1 + 1.0f) / 2.0f + 1.0f);
		const float f2 = (n2 - tolerance) * 16.0f * (1.0f - n1);

		const float stoneChance = f1 > f2 ? f1 : f2;
		const float gravelChance = (1.0f + n1) * (96.0f - heightDiff) / 192.0f;
		const float sandLine = (1.0f - maxHeightNoise[index2d]) * 16.0f - 4.0f;

		if(tolerance < n2)
		{
			if(n3 < stoneChance * (1.0f - gravelChance))
			{
				blocks[blockIndex] = 1;
			}
			else if(n3 < stoneChance)
			{
				blocks[blockIndex] = 5;
			}
			else if(n3 < stoneChance * (1.0f + gravelChance))
			{
				blocks[blockIndex] = 6;
			}
			else
			{
				if(y < sandLine && y > -4.0 - sandLine)
				{
					blocks[blockIndex] = 7;
				}
				else
				{
					if(y < -1)
					{
						blocks[blockIndex] = 6;
					}
					else if((y + 1 - minHeight) / heightDiff < solidNoise[index3d + 18])
					{
						blocks[blockIndex] = 3;
					}
					else
					{
						blocks[blockIndex] = 4;
					}
				}
			}
		}
		else
		{
			if(y < 0)
			{
				blocks[blockIndex] = 2;
			}
			else
			{
				blocks[blockIndex] = 0;
			}
		}
	}
}