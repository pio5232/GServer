#include "pch.h"
#include <random>

Vector3 GenerateRandomPos()
{
	static std::random_device rd;
	static std::mt19937_64 generator(rd()); // ½Ãµå ¼¯À½.

	static std::uniform_real_distribution<float> xPos(sectorMinX, sectorMaxX);
	static std::uniform_real_distribution<float> zPos(sectorMinZ, sectorMaxZ);

	return Vector3(xPos(generator), 0.0f, zPos(generator));

}
