#pragma once

#include <string>

#include <include/glm.h>
#include <Core/GPU/Mesh.h>
#include <Core/Engine.h>

using namespace std;

class RoadGen
{
public:
	int width, height, maxTunnels, maxLength;
	int **road;
	int **direct;
	RoadGen(int height, int width, int maxTunnels, int maxLength);
	void Roads();
	int chooseDirection(int oldDirection);
	bool checkMove(int direction, pair<int ,int> point, int tunnel_length);
	pair<int, int> move(int direction, pair<int, int> point);
	~RoadGen();
};

