//Frustum culling of transformed grid coordinates
__kernel void arrayInsideCheck(const uint renderDistance, __global uchar* grid, __global uchar* bools)
{
	const uint dimensionSize1 = renderDistance * 2 + 1;
	const uint index1 = (get_global_id(0) * dimensionSize1 + get_global_id(1)) * dimensionSize1 + get_global_id(2);
	const uint dimensionSize2 = renderDistance * 2 + 2;
	const uint index2 = (get_global_id(0) * dimensionSize2 + get_global_id(1)) * dimensionSize2 + get_global_id(2);

	uchar bitField = 0;

	for(uint i = 0; i < 2; i++)
	{
		for(uint j = 0; j < 2; j++)
		{
			for(uint k = 0; k < 2; k++)
			{
				const int newIndex = index2 + (i * dimensionSize2 + j) * dimensionSize2 + k;
				bitField |= bools[newIndex];
			}
		}
	}

	//grid[index1] = maxZ >= -1.0F && minZ <= 1.0F && maxY >= -1.0F && minY <= 1.0F;// && maxZ >= -1.0F && minZ <= 1.0F;
	grid[index1] = bitField == 0b111111;
}

//Takes an entire grid of coordinates and spits them out as transformed coordinates.
__kernel void gridTransform(const uint renderDistance, __global const float* projectionMatrix, const int xCam, const int yCam, const int zCam, __global uchar* bools)
{
	const uint dimensionSize = renderDistance * 2 + 2;
	const uint index = (get_global_id(0) * dimensionSize + get_global_id(1)) * dimensionSize + get_global_id(2);

	const int f1 = (xCam + get_global_id(0) - renderDistance) * 16;
	const int f2 = (yCam + get_global_id(1) - renderDistance) * 16;
	const int f3 = (zCam + get_global_id(2) - renderDistance) * 16;

	float w = f1 * projectionMatrix[12] + f2 * projectionMatrix[13] + f3 * projectionMatrix[14] + projectionMatrix[15];
	float x = (f1 * projectionMatrix[0] + f2 * projectionMatrix[1] + f3 * projectionMatrix[2] + projectionMatrix[3]) / w;
	float y = (f1 * projectionMatrix[4] + f2 * projectionMatrix[5] + f3 * projectionMatrix[6] + projectionMatrix[7]) / w;
	float z = (f1 * projectionMatrix[8] + f2 * projectionMatrix[9] + f3 * projectionMatrix[10] + projectionMatrix[11]) / w;

	bools[index] = (x >= -1.0f ? 0b100000 : 0) | (x <= 1.0f ? 0b10000 : 0) | (y >= -1.0f ? 0b1000 : 0) | (y <= 1.0f ? 0b100 : 0) | (z >= -1.0f ? 0b10 : 0) | (z <= 1.0f ? 0b1 : 0);
}