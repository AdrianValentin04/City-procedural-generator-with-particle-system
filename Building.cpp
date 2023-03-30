#include "Building.h"

#include <Core/Engine.h>
#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>
#include <ctime>

using namespace std;

Building::Building(int floors, float floor[3][2], int i, int j, string primitive,
	string primitive1, string primitive2) {
	
	// Salveaza datele cladirii
	this->floors = floors;
	this->i = i;
	this->j = j;
	this->primitive = primitive;
	this->primitive1 = primitive1;
	this->primitive2 = primitive2;

	// Leaga textura de fiecare corp
	if (primitive.compare("box") == 0) {
		this->texture = "building";
	} 
	else if (primitive.compare("tri") == 0) {
		this->texture = "building";
	}
	else if (primitive.compare("hexa") == 0) {
		this->texture = "building_hexa";
	}

	if (primitive1.compare("box") == 0) {
		this->texture1 = "building";
	}
	else if (primitive1.compare("tri") == 0) {
		this->texture1 = "building";
	}
	else if (primitive1.compare("hexa") == 0) {
		this->texture1 = "building_hexa";
	}

	if (primitive2.compare("box") == 0) {
		this->texture2 = "building";
	}
	else if (primitive2.compare("tri") == 0) {
		this->texture2 = "building";
	}
	else if (primitive2.compare("hexa") == 0) {
		this->texture2 = "building_hexa";
	}

	// Randomizeaza textura aplicata cladirii
	int r = rand() % 4;
	if (r == 1) {
		this->texture.append("_blue");
		this->texture1.append("_blue");
		this->texture2.append("_blue");
	}
	else if (r == 2) {
			this->texture.append("_green");
			this->texture1.append("_green");
			this->texture2.append("_green");
	}
	else if (r == 3) {
		this->texture.append("_yellow");
		this->texture1.append("_yellow");
		this->texture2.append("_yellow");
	}

	// Salveaza informatiile despre etaje
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++) {
			if (i < floors) {
				this->floor[i][j] = floor[i][j];
			}
			else {
				this->floor[i][j] = 0;
			}
		}
	}
}

// Returneaza numarul de etaje si forma
Building * Building::RandomizeBuilding(int i, int j, int ind)
{
	int prim = rand() % 3;
	string primitive, primitive1, primitive2;

	if (prim == 0) {
		primitive = "box";
		prim = rand() % 15;
		if (prim < 8) {
			primitive1 = "box";
			prim = rand() % 10;
			if (prim < 8) {
				primitive2 = "box";
			}
			else if (prim >= 8 && prim < 10) {
				primitive2 = "tri";
			}
			else if (prim >= 10 && prim < 15) {
				primitive2 = "hexa";
			}
		}
		else if (prim >= 8 && prim < 10) {
			primitive1 = "tri";
			primitive2 = "tri";
		}
		else if (prim >= 10 && prim < 15) {
			primitive1 = "hexa";
			primitive2 = "hexa";
		}
	}
	else if (prim == 1) {
		primitive = "tri";
		primitive1 = "tri";
		primitive2 = "tri";
	}
	else if (prim == 2) {
		primitive = "hexa";
		primitive1 = "hexa";
		primitive2 = "hexa";
	}

	// Genereaza dimensiunile fiecarui corp
	float floor[3][2];
	int floors = rand() % 2;
	floors += 2;
	float h = rand() % 51;
	h += 50;
	h /= 100;
	floor[0][0] = h;
	floor[0][1] = 0.9f;

	float w, w1;
		h = rand() % 51;
		h += 50;
		h /= 100;
		floor[1][0] = h;
		w = rand() % 26;
		w += 40;
		w /= 100;
		floor[1][1] = w;

	if (floors == 3) {
		h = rand() % 51;
		h += 50;
		h /= 100;
		floor[2][0] = h;
		w1 = rand() % (int(w * 100) / 4);
		w1 += int(w * 100) / 2;
		w1 /= 100;
		floor[2][1] = w1;
	}

	// Creeaza cladirea
	Building *b = new Building(floors, floor, i,  j, primitive, primitive1, primitive2);
	return b;
}

