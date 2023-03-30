#pragma once

#include <string>

#include <include/glm.h>
#include <Core/GPU/Mesh.h>
#include <Core/Engine.h>

using namespace std;

class Building
{
public:
	int floors, i, j;
	float floor[3][2];
	string primitive, texture, primitive1, texture1, primitive2, texture2;
	Building(int floors, float floor[3][2], int i, int j, string primitive, string primitive1, string primitive2);
	Building* RandomizeBuilding(int i, int j, int ind);
	~Building();
};

