#include "Proiect.h"
#include "RoadGen.h"
#include "Building.h"
#include <vector>
#include <string>
#include <iostream>
#include <Core/Engine.h>

using namespace std;
RoadGen* roadgen;
std::vector<Building*> buildings;
int buildingsCount = 0;

// Parametri pentru generarea drumurilor
int height = 25, width = 25, lenMax = 125, turnMax = 13;
const string textureLoc = "Resources/Textures/";

Proiect::Proiect()
{
}

Proiect::~Proiect()
{
}

void Proiect::Init()
{
	auto camera = GetSceneCamera();
	camera->SetPositionAndRotation(glm::vec3(0, 8, 8), glm::quat(glm::vec3(-40 * TO_RADIANS, 0, 0)));
	camera->Update();

	{
		TextureManager::LoadTexture(RESOURCE_PATH::TEXTURES, "particle2.png");
	}

	// Incarcam shaderele
	LoadShader("Simple", false);
	LoadShader("Snow");
	LoadShader("Fireworks");
	LoadShader("Rain");

	// Declaram numarul de particule
	unsigned int nrParticlesSnow = 5000;
	unsigned int nrParticlesFireworks = 500;
	unsigned int nrParticlesRain = 5000;

	// Declar efectele pentru particule
	snowEffect = new ParticleEffect<Snow>();
	snowEffect->Generate(nrParticlesSnow, true);
	fireworksEffect = new ParticleEffect<Fireworks>();
	fireworksEffect->Generate(nrParticlesFireworks, true);
	rainEffect = new ParticleEffect<Rain>();
	rainEffect->Generate(nrParticlesRain, true);

	// Initializez buferele
	auto particleSSBOSnow = snowEffect->GetParticleBuffer();
	Snow* dataSnow = const_cast<Snow*>(particleSSBOSnow->GetBuffer());
	auto particleSSBOFireworks = fireworksEffect->GetParticleBuffer();
	Fireworks* dataFireworks = const_cast<Fireworks*>(particleSSBOFireworks->GetBuffer());
	auto particleSSBORain = rainEffect->GetParticleBuffer();
	Rain* dataRain = const_cast<Rain*>(particleSSBORain->GetBuffer());

	// Initializez datele pentru zapada
	for (unsigned int i = 0; i < nrParticlesSnow; i++)
	{
		glm::vec4 pos(1);
		pos.x = Randrange(-width / 2, width / 2);
		pos.y = Randrange(0, height);
		pos.z = Randrange(-height / 2, height / 2);
		glm::vec4 speed(0);

		speed.x = (rand() % 20 - 10) / 10.0f;
		speed.z = (rand() % 20 - 10) / 10.0f;
		speed.y = 0.0f;
		dataSnow[i].SetInitial(pos, speed);
	}

	// Dimensiunile pentru artificii
	int fireworksCSize = 9;
	int fireworksHSize = 1;

	// Initializeaza artificiile
	for (unsigned int i = 0; i < nrParticlesFireworks; i++)
	{
		glm::vec4 pos(1);
		pos.x = RandFireworksPos(width) - (rand() % fireworksCSize - fireworksHSize);
		pos.y = height / 2 + (rand() % fireworksCSize - fireworksHSize);
		pos.z = - height - (rand() % fireworksCSize - fireworksHSize);

		glm::vec4 speed(0);
		speed.x = (rand() % 20 - 10) / 10.0f;
		speed.z = (rand() % 20 - 10) / 10.0f;
		speed.y = rand() % 2 + 2.0f;
		dataFireworks[i].SetInitial(pos, speed);
	}

	// Initializez datele despre ploaie
	for (unsigned int i = 0; i < nrParticlesRain; i++)
	{
		glm::vec4 pos(1);
		pos.x = Randrange(-width / 2, width / 2);
		pos.y = Randrange(0, height);
		pos.z = Randrange(-height / 2, height / 2);

		glm::vec4 speed(0);
		speed.x = 0.0f;
		speed.z = 0.0f;
		speed.y = -15.0f; 
		dataRain[i].SetInitial(pos, speed);
	}

	particleSSBOSnow->SetBufferData(dataSnow);
	particleSSBOFireworks->SetBufferData(dataFireworks);
	particleSSBORain->SetBufferData(dataRain);

	// Creeaza un patrat
	{
		vector<glm::vec3> vertices
		{
			glm::vec3(0.5f,   0.5f, 0.0f),	// Top Right
			glm::vec3(0.5f,  -0.5f, 0.0f),	// Bottom Right
			glm::vec3(-0.5f, -0.5f, 0.0f),	// Bottom Left
			glm::vec3(-0.5f,  0.5f, 0.0f),	// Top Left
		};

		vector<glm::vec3> normals
		{
			glm::vec3(0, 1, 1),
			glm::vec3(1, 0, 1),
			glm::vec3(1, 0, 0),
			glm::vec3(0, 1, 0)
		};

		// TODO : Complete texture coordinates for the square
		vector<glm::vec2> textureCoords
		{
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 1.0f),
			glm::vec2(0.0f, 0.0f)
		};

		vector<unsigned short> indices =
		{
			0, 1, 3,
			1, 2, 3
		};

		Mesh* mesh = new Mesh("square");
		mesh->InitFromData(vertices, normals, textureCoords, indices);
		meshes[mesh->GetMeshID()] = mesh;
	}

	// Creeaza o prisma triunghiulara
	{
		vector<glm::vec3> vertices
		{
			glm::vec3(0, -0.5, 0.5),
			glm::vec3(0, 0.5, 0.5),
			glm::vec3(-0.5, -0.5, -0.5),
			glm::vec3(0.5, -0.5, -0.5),
			glm::vec3(-0.5, 0.5, -0.5),
			glm::vec3(0.5, 0.5, -0.5),
			glm::vec3(0.5, -0.5, -0.5),
			glm::vec3(0.5, 0.5, -0.5),
		};

		vector<glm::vec3> normals
		{
			glm::vec3(0, 1, 1),
			glm::vec3(0, 1, 0),
			glm::vec3(1, 1, 1),
			glm::vec3(0, 1, 1),
			glm::vec3(1, 0, 0),
			glm::vec3(0, 0, 1),
			glm::vec3(0, 1, 1),
			glm::vec3(0, 0, 1)
		};

		vector<glm::vec2> textureCoords
		{
			glm::vec2(0.0f, 0.0f),
			glm::vec2(0.0f, 1.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 0.0f),
			glm::vec2(0.0f, 1.0f),
		};

		vector<unsigned short> indices =
		{
			0, 2, 3,		1, 4, 5,
			0, 3, 5,		5, 1, 0,
			1, 2, 0,		1, 2, 4,
			7, 2, 4,		7, 2, 6,
		};

		Mesh* mesh = new Mesh("tri");
		mesh->InitFromData(vertices, normals, textureCoords, indices);
		meshes[mesh->GetMeshID()] = mesh;
	}

	// Creeaza un triunghi
	{
		vector<glm::vec3> vertices
		{
			glm::vec3(0, 0, 0.5),
			glm::vec3(-0.5, 0, -0.5),
			glm::vec3(0.5, 0, -0.5),
		};

		vector<glm::vec3> normals
		{
			glm::vec3(0, 1, 0),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 1, 0),
		};

		vector<glm::vec2> textureCoords
		{
			glm::vec2(0.5f, 0.0f),
			glm::vec2(0.0f, 1.0f),
			glm::vec2(1.0f, 1.0f),
		};

		vector<unsigned short> indices =
		{
			0, 1, 2
		};

		Mesh* mesh = new Mesh("triangle");
		mesh->InitFromData(vertices, normals, textureCoords, indices);
		meshes[mesh->GetMeshID()] = mesh;
	}

	// Creeaza o prisma hexagonala
	{
		vector<glm::vec3> vertices
		{
			glm::vec3(0, -0.5, 0.5),
			glm::vec3(0.5, -0.5, 0.25),
			glm::vec3(0.5, -0.5, -0.25),
			glm::vec3(0, -0.5, -0.5),
			glm::vec3(-0.5, -0.5, -0.25),
			glm::vec3(-0.5, -0.5, 0.25),
			glm::vec3(0, 0.5, 0.5),
			glm::vec3(0.5, 0.5, 0.25),
			glm::vec3(0.5, 0.5, -0.25),
			glm::vec3(0, 0.5, -0.5),
			glm::vec3(-0.5, 0.5, -0.25),
			glm::vec3(-0.5, 0.5, 0.25),
		};

		vector<glm::vec3> normals
		{
			glm::vec3(0, 1, 1),
			glm::vec3(0, 1, 0),
			glm::vec3(1, 1, 1),
			glm::vec3(0, 1, 1),
			glm::vec3(1, 0, 0),
			glm::vec3(0, 0, 1),
			glm::vec3(0, 1, 1),
			glm::vec3(0, 1, 0),
			glm::vec3(1, 1, 1),
			glm::vec3(0, 1, 1),
			glm::vec3(1, 0, 0),
			glm::vec3(0, 0, 1)
		};

		vector<glm::vec2> textureCoords
		{
			glm::vec2(0.25f, 0.0f),
			glm::vec2(0.75f, 0.0f),
			glm::vec2(0.25f, 0.0f),
			glm::vec2(0.75f, 0.0f),
			glm::vec2(0.25f, 0.0f),
			glm::vec2(0.75f, 0.0f),
			glm::vec2(0.25f, 1.0f),
			glm::vec2(0.75f, 1.0f),
			glm::vec2(0.25f, 1.0f),
			glm::vec2(0.75f, 1.0f),
			glm::vec2(0.25f, 1.0f),
			glm::vec2(0.75f, 1.0f),
		};

		vector<unsigned short> indices =
		{
			0, 7, 1,		0, 7, 6,
			1, 8, 7,		1, 8, 2,
			2, 9, 8,		2, 9, 3,
			3, 10, 9,		3, 10, 4,
			4, 11, 10,		4, 11, 5,
			5, 6, 11,		5, 6, 0,
			0, 1, 2,		0, 2, 3,
			0, 3, 4,		0, 4, 5,
			6, 7, 8,		6, 8, 9,
			6, 9, 10,		6, 10, 11,
		};

		Mesh* mesh = new Mesh("hexa");
		mesh->InitFromData(vertices, normals, textureCoords, indices);
		meshes[mesh->GetMeshID()] = mesh;
	}

	// Creeaza un hexagon
	{
		vector<glm::vec3> vertices
		{
			glm::vec3(0, 0, 0.5),
			glm::vec3(0.5, 0, 0.25),
			glm::vec3(0.5, 0, -0.25),
			glm::vec3(0, 0, -0.5),
			glm::vec3(-0.5, 0, -0.25),
			glm::vec3(-0.5, 0, 0.25),
		};

		vector<glm::vec3> normals
		{
			glm::vec3(0, 1, 0),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 1, 0),
		};

		vector<glm::vec2> textureCoords
		{
			glm::vec2(0.5f, 0.0f),
			glm::vec2(1.0f, 0.33f),
			glm::vec2(1.0f, 0.66f),
			glm::vec2(0.5f, 1.0f),
			glm::vec2(0.0f, 0.66f),
			glm::vec2(0.0f, 0.33f)
		};

		vector<unsigned short> indices =
		{
			0, 1, 2,	0, 2, 3,
			0, 3, 4,	0, 4, 5,
		};

		Mesh* mesh = new Mesh("hexagon");
		mesh->InitFromData(vertices, normals, textureCoords, indices);
		meshes[mesh->GetMeshID()] = mesh;
	}

	// Incarca texturile

	// Strada
	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "road.jpg").c_str(), GL_REPEAT);
		mapTextures["road"] = texture;
	}

	// Gazon
	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "lawn.jpg").c_str(), GL_REPEAT);
		mapTextures["lawn"] = texture;
	}

	// Intersectia de strazi
	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "intersection.jpg").c_str(), GL_REPEAT);
		mapTextures["intersection"] = texture;
	}

	// Texturi de cladiri
	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "facade_complex.jpg").c_str(), GL_REPEAT);
		mapTextures["building"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "facade_complex_blue.jpg").c_str(), GL_REPEAT);
		mapTextures["building_blue"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "facade_complex_yellow.jpg").c_str(), GL_REPEAT);
		mapTextures["building_yellow"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "facade_complex_green.jpg").c_str(), GL_REPEAT);
		mapTextures["building_green"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "facade_triangle.jpg").c_str(), GL_REPEAT);
		mapTextures["building_triangle"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "facade_hexa_complex.jpg").c_str(), GL_REPEAT);
		mapTextures["building_hexa"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "facade_hexa_complex_green.jpg").c_str(), GL_REPEAT);
		mapTextures["building_hexa_green"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "facade_hexa_complex_yellow.jpg").c_str(), GL_REPEAT);
		mapTextures["building_hexa_yellow"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "facade_hexa_complex_blue.jpg").c_str(), GL_REPEAT);
		mapTextures["building_hexa_blue"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "black.jpg").c_str(), GL_REPEAT);
		mapTextures["black"] = texture;
	}

	// Acoperis
	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "top.jpg").c_str(), GL_REPEAT);
		mapTextures["top"] = texture;
	}

	// Pista de elicopter
	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "helipad.jpg").c_str(), GL_REPEAT);
		mapTextures["helipad"] = texture;
	}

	// Luna
	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "moon.jpg").c_str(), GL_REPEAT);
		mapTextures["moon"] = texture;
	}

	// Lampi
	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "Street_Lamp_SM.png").c_str(), GL_REPEAT);
		mapTextures["streetlamp"] = texture;
	}

	// Bambus
	{
		Texture2D* texture = new Texture2D();
		texture->Load2D("Resources/Models/Vegetation/Bamboo/bamboo.png", GL_REPEAT);
		mapTextures["bamboo"] = texture;
	}

	// Incarca mesh uri
	{
		Mesh* mesh = new Mesh("bamboo");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Vegetation/Bamboo/", "bamboo.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("box");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("sphere");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("quad");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "quad.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("lamppost");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "StreetLamp.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}


	// Creeaza shadere
	{
		Shader *shader = new Shader("Shader");
		shader->AddShader("Source/Laboratoare/Proiect/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Proiect/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{
		Shader *shader = new Shader("SpotShader");
		shader->AddShader("Source/Laboratoare/Proiect/Shaders/SpotVertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Proiect/Shaders/SpotFragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	// Proprietati ale luminii si materialelor
	{
		lightPosition = glm::vec3(0, 1, 1);
		lightDirection = glm::vec3(0, -1, 0);
		materialShininess = 60;
		materialKd = 0.5;
		materialKs = 0.5;
	}

	// Generarea drumului
	roadgen = new RoadGen(height, width, lenMax, turnMax);

	// Numararea cladirilor si afisarea matricei
	for (int i = 0; i < roadgen->height; i++) {
		for (int j = 0; j < roadgen->width; j++) {
			cout << roadgen->road[i][j] << " ";
			if (roadgen->road[i][j] == 1) {
				buildingsCount++;
			}
		}
		cout << endl;
	}

	int one = buildingsCount;
	float floors[3][2];
	Building *dummy = new Building(2, floors, -1, -1, "dummy", "dummy", "dummy");

	// Creeaza cladirile generand random numarul si forma etajelor
	for (int i = 0; i < roadgen->height; i++) {
		for (int j = 0; j < roadgen->width; j++) {
			if (roadgen->road[i][j] == 1) {
				one--;
				buildings.push_back(dummy->RandomizeBuilding(i - height / 2, j - height / 2, one));
			}
		}
	}
}

void Proiect::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 4.0f / 254, 25.0f / 254, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Proiect::LoadShader(std::string name, bool hasGeomtery)
{
	static std::string shaderPath = "Source/Laboratoare/Proiect/Shaders/";

	// Incarca shaderele pentru particule
	{
		Shader* shader = new Shader(name.c_str());
		shader->AddShader((shaderPath + name + ".VS.glsl").c_str(), GL_VERTEX_SHADER);
		shader->AddShader((shaderPath + name + ".FS.glsl").c_str(), GL_FRAGMENT_SHADER);
		if (hasGeomtery)
		{
			shader->AddShader((shaderPath + name + ".GS.glsl").c_str(), GL_GEOMETRY_SHADER);
		}

		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
}

// Returneaza un numar random dintr-un interval
int Proiect::Randrange(int min_n, int max_n)
{
	return rand() % (max_n - min_n + 1) + min_n;
}

// Returneaza o pozitie random pe axa Ox
int Proiect::RandFireworksPos(int reper) 
{
	if (rand() % 2 == 0) {
		return reper;
	}
	else {
		return -reper;
	}
}

void Proiect::Update(float deltaTimeSeconds)
{
	for (int i = 0; i < roadgen->height; i++) {
		for (int j = 0; j < roadgen->width; j++) {
			if (roadgen->road[i][j] == 0) {
				// model matrix pentru drum
				glm::mat4 modelMatrix = glm::mat4(1);

				// model matrix pentru lampi
				glm::mat4 modelMatrixLamp1 = glm::mat4(1);
				glm::mat4 modelMatrixLamp2 = glm::mat4(1);

				modelMatrix = glm::translate(modelMatrix, glm::vec3(i - height / 2, 0.0f, j - width / 2));
				
				// Roteste textura drumului si directia lampii in functie de directia drumului
				if (roadgen->direct[i][j] == 1 || roadgen->direct[i][j] == 2) {
					modelMatrix = glm::rotate(modelMatrix, (float)M_PI / 2, glm::vec3(0, 1, 0));
					modelMatrixLamp1 = glm::translate(modelMatrixLamp1, glm::vec3(i - height / 2, 0.0f, j - width / 2 - 0.5f));
					modelMatrixLamp1 = glm::scale(modelMatrixLamp1, glm::vec3(0.003));
					modelMatrixLamp1 = glm::rotate(modelMatrixLamp1, (float)-M_PI / 2, glm::vec3(0, 1, 0));

					modelMatrixLamp2 = glm::translate(modelMatrixLamp2, glm::vec3(i - height / 2, 0.0f, j - width / 2 + 0.5f));
					modelMatrixLamp2 = glm::scale(modelMatrixLamp2, glm::vec3(0.003));
					modelMatrixLamp2 = glm::rotate(modelMatrixLamp2, (float)M_PI / 2, glm::vec3(0, 1, 0));
				}
				else {

					modelMatrixLamp1 = glm::translate(modelMatrixLamp1, glm::vec3(i - height / 2 - 0.5f, 0.0f, j - width / 2));
					modelMatrixLamp1 = glm::scale(modelMatrixLamp1, glm::vec3(0.003));

					modelMatrixLamp2 = glm::translate(modelMatrixLamp2, glm::vec3(i - height / 2 + 0.5f, 0.0f, j - width / 2));
					modelMatrixLamp2 = glm::scale(modelMatrixLamp2, glm::vec3(0.003));
					modelMatrixLamp2 = glm::rotate(modelMatrixLamp2, (float)M_PI, glm::vec3(0, 1, 0));
				}

				// Calculeaza cate si unde trebuie puse lampile
				modelMatrix = glm::rotate(modelMatrix, (float)M_PI / 2, glm::vec3(1, 0, 0));
				int sem = 0;
				if (i > 0) {
					if (roadgen->road[i - 1][j] == 0) {
						if (j > 0) {
							if (roadgen->road[i][j - 1] == 0 && roadgen->road[i - 1][j - 1] != 0) {
								sem++;
							}
						}

						if (j < roadgen->width - 1) {
							if (roadgen->road[i][j + 1] == 0 && roadgen->road[i - 1][j + 1] != 0) {
								sem++;
							}
						}
					}
				}
				if (i < roadgen->height - 1) {
					if (roadgen->road[i + 1][j] == 0) {
						if (j > 0) {
							if (roadgen->road[i][j - 1] == 0 && roadgen->road[i + 1][j - 1] != 0) {
								sem++;
							}
						}

						if (j < roadgen->width - 1) {
							if (roadgen->road[i][j + 1] == 0 && roadgen->road[i + 1][j + 1] != 0) {
								sem++;
							}
						}
					}
				}
				if (sem == 0) {
					{
						int s1 = 0, s2 = 0;
						if (roadgen->direct[i][j] == 1 || roadgen->direct[i][j] == 2) {
								if (j > 0) {
									if (roadgen->road[i][j - 1] != 0) {
										s1 = 1;
									}
								}
								if (j < width - 1) {
									if (roadgen->road[i][j + 1] != 0) {
										s2 = 1;
									}
								}
						}

						if (roadgen->direct[i][j] == -1 || roadgen->direct[i][j] == -2) {
							if (i > 0) {
								if (roadgen->road[i - 1][j] != 0) {
									s1 = 1;
								}
							}
							if (i < height - 1) {
								if (roadgen->road[i + 1][j] != 0) {
									s2 = 1;
								}
							}
						}
						if (s1 == 1) {
							RenderSimpleMesh(meshes["lamppost"], shaders["Shader"], modelMatrixLamp1, mapTextures["streetlamp"]);
						}
						if (s2 == 1) {
							RenderSimpleMesh(meshes["lamppost"], shaders["Shader"], modelMatrixLamp2, mapTextures["streetlamp"]);
						}
					}
					RenderSimpleMesh(meshes["square"], shaders["Shader"], modelMatrix, mapTextures["road"]);
				}
				else {
					RenderSimpleMesh(meshes["square"], shaders["Shader"], modelMatrix, mapTextures["intersection"]);
				}
			}
		}
	}

	// Randeaza luna
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(width, 20, -height * 3));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(7));
		RenderSimpleMesh(meshes["sphere"], shaders["Shader"], modelMatrix, mapTextures["moon"]);
	}

	// Randeaza cladirile
	for (int z = 0; z < buildingsCount; z++) {

		int i = buildings[z]->i, j = buildings[z]->j;
		string primitive  = buildings[z]->primitive,  texture  = buildings[z]->texture,
			   primitive1 = buildings[z]->primitive1, texture1 = buildings[z]->texture1,
			   primitive2 = buildings[z]->primitive2, texture2 = buildings[z]->texture2;

		// Randeaza gazonul
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(i, 0, j));
		modelMatrix = glm::rotate(modelMatrix, (float)M_PI / 2, glm::vec3(1, 0, 0));
		RenderSimpleMesh(meshes["square"], shaders["Shader"], modelMatrix, mapTextures["lawn"]);

		// Randeaza primul etaj al cladirii
		float h = buildings[z]->floor[0][0], w = buildings[z]->floor[0][1];
		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(i, h / 2, j));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(w, h, w));
		RenderSimpleMesh(meshes[primitive], shaders["Shader"], modelMatrix, mapTextures[texture]);

		// Randeaza al doilea etaj al cladirii
		float h1 = buildings[z]->floor[1][0], w1 = buildings[z]->floor[1][1];
		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(i, h + h1 / 2, j));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(w1, h1, w1));
		RenderSimpleMesh(meshes[primitive1], shaders["Shader"], modelMatrix, mapTextures[texture1]);

		// Randeaza al treilea etaj daca exista
		if (buildings[z]->floors == 3) {
			float h2 = buildings[z]->floor[2][0], w2 = buildings[z]->floor[2][1];
			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(i, h + h1 + h2 / 2, j));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(w2, h2, w2));
			lightPosition = glm::vec3(i, h + h1 + h2 / 2 + 2, j);
			RenderSimpleMesh(meshes[primitive2], shaders["Shader"], modelMatrix, mapTextures[texture2]);

			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(i, h + h1 + h2 / 2 + 1, j));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2));
		}

		// Plaseaza bambusii langa cladirile triunghi
		if (primitive.compare("tri") == 0) {
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(i + 0.2, 0, j + 0.33));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.005f));
			RenderSimpleMesh(meshes["bamboo"], shaders["Shader"], modelMatrix, mapTextures["bamboo"]);

			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(i + 0.255, 0, j + 0.21));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.006f));
			RenderSimpleMesh(meshes["bamboo"], shaders["Shader"], modelMatrix, mapTextures["bamboo"]);

			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(i + 0.31, 0, j + 0.09));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.007f));
			RenderSimpleMesh(meshes["bamboo"], shaders["Shader"], modelMatrix, mapTextures["bamboo"]);

			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(i + 0.365, 0, j - 0.03));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.006f));
			RenderSimpleMesh(meshes["bamboo"], shaders["Shader"], modelMatrix, mapTextures["bamboo"]);

			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(i + 0.420, 0, j - 0.15));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.005f));
			RenderSimpleMesh(meshes["bamboo"], shaders["Shader"], modelMatrix, mapTextures["bamboo"]);

			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(i - 0.2, 0, j + 0.33));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.005f));
			RenderSimpleMesh(meshes["bamboo"], shaders["Shader"], modelMatrix, mapTextures["bamboo"]);

			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(i - 0.255, 0, j + 0.21));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.006f));
			RenderSimpleMesh(meshes["bamboo"], shaders["Shader"], modelMatrix, mapTextures["bamboo"]);

			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(i - 0.31, 0, j + 0.09));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.007f));
			RenderSimpleMesh(meshes["bamboo"], shaders["Shader"], modelMatrix, mapTextures["bamboo"]);

			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(i - 0.365, 0, j - 0.03));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.006f));
			RenderSimpleMesh(meshes["bamboo"], shaders["Shader"], modelMatrix, mapTextures["bamboo"]);

			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(i - 0.420, 0, j - 0.15));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.005f));
			RenderSimpleMesh(meshes["bamboo"], shaders["Shader"], modelMatrix, mapTextures["bamboo"]);

		}

		if (primitive.compare("box") == 0) {
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(i, h + 0.001, j));
			modelMatrix = glm::rotate(modelMatrix, (float)M_PI / 2, glm::vec3(1, 0, 0));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(w, w, 0));
			RenderSimpleMesh(meshes["square"], shaders["Shader"], modelMatrix, mapTextures["black"]);
		}

		if (primitive1.compare("box") == 0) {
			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(i, h + h1 + 0.001, j));
			modelMatrix = glm::rotate(modelMatrix, (float)M_PI / 2, glm::vec3(1, 0, 0));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(w1, w1, 0));
			RenderSimpleMesh(meshes["square"], shaders["Shader"], modelMatrix, mapTextures["black"]);
		}

		// Plaseaza o pista de elicoptere daca cladirea are 3 etaje
		if (primitive2.compare("box") == 0) {
			if (buildings[z]->floors == 3) {
				float h2 = buildings[z]->floor[2][0], w2 = buildings[z]->floor[2][1];
				modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(i, h + h1 + h2 + 0.002, j));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(w2, 0.001, w2));
				lightPosition = glm::vec3(i, h + h1 + h2 + 0.3, j);
				RenderSimpleMesh(meshes["box"], shaders["SpotShader"], modelMatrix, mapTextures["helipad"]);
			}
		}

		string prim;
		if (buildings[z]->floors == 2) {
			prim = primitive1;
		}
		else {
			prim = primitive2;
		}

		// Randeaza luminile pe cladiri
		int fl = buildings[z]->floors - 1;
			if (prim.compare("box") == 0) {
				float h2 = buildings[z]->floor[fl][0], w2 = buildings[z]->floor[fl][1];
				modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(i, h + h2 + 0.002, j));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(w2, 0.001, w2));
				lightPosition = glm::vec3(i, h + h1 + (h2 - h1) + 0.3, j);
				RenderSimpleMesh(meshes["box"], shaders["SpotShader"], modelMatrix, mapTextures["top"]);
			}

			if (prim.compare("hexa") == 0) {
				float h2 = buildings[z]->floor[fl][0], w2 = buildings[z]->floor[fl][1];
				modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(i, h + h2 + 0.002, j));
				if (fl == 2) {
					modelMatrix = glm::translate(modelMatrix, glm::vec3(0, h1, 0));
				}
				modelMatrix = glm::scale(modelMatrix, glm::vec3(w2, 0.001, w2));
				lightPosition = glm::vec3(i, h + h1 + (h2 - h1) + 0.3, j);
				if (fl == 2) {
					lightPosition = glm::vec3(i, h + h1 + h2 + 0.3, j);
				}
				RenderSimpleMesh(meshes["hexagon"], shaders["SpotShader"], modelMatrix, mapTextures["top"]);
			}

			if (prim.compare("tri") == 0) {
				float h2 = buildings[z]->floor[fl][0], w2 = buildings[z]->floor[fl][1];
				modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(i, h + h2 + 0.002, j));
				if (fl == 2) {
					modelMatrix = glm::translate(modelMatrix, glm::vec3(0, h1, 0));
				}
				modelMatrix = glm::scale(modelMatrix, glm::vec3(w2, 0.001, w2));
				lightPosition = glm::vec3(i, h + h1 + (h2 - h1) + 0.3, j);
				if (fl == 2) {
					lightPosition = glm::vec3(i, h + h1 + h2 + 0.3, j);
				}
				RenderSimpleMesh(meshes["triangle"], shaders["SpotShader"], modelMatrix, mapTextures["top"]);
			}
	}

	glLineWidth(3);

	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);

	// Randeaza zapada
	{
		if (startSnow == 1)
		{
			auto shader = shaders["Snow"];
			if (shader->GetProgramID())
			{
				shader->Use();
				snowEffect->Render(GetSceneCamera(), shader);
			}
		}
	}

	// Randeaza artificiile
	{
		if (startFireworks == 1)
		{
			auto shader = shaders["Fireworks"];
			if (shader->GetProgramID())
			{
				shader->Use();
				TextureManager::GetTexture("particle2.png")->BindToTextureUnit(GL_TEXTURE0);
				fireworksEffect->Render(GetSceneCamera(), shader);
			}
		}
		else {
			auto particleSSBOFireworks = fireworksEffect->GetParticleBuffer();
			Fireworks* dataFireworks = const_cast<Fireworks*>(particleSSBOFireworks->GetBuffer());
			particleSSBOFireworks->SetBufferData(dataFireworks);
		}
	}

	// Randeaza ploaia
	{
		if (startRain == 1)
		{
			auto shader = shaders["Rain"];
			if (shader->GetProgramID())
			{
				shader->Use();
				rainEffect->Render(GetSceneCamera(), shader);
			}
		}
	}

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

}


void Proiect::FrameEnd()
{
	//DrawCoordinatSystem();
}

// Creeaza iluminarea scenei
void Proiect::RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, Texture2D* texture1, Texture2D* texture2)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// Randeaza un obiect folosind shaderul specific
	glUseProgram(shader->program);

	// Seteaza iluminarea in functie de pozitia luminii si a materialului
	// Lumina uniforma
	int light_position = glGetUniformLocation(shader->program, "light_pos");
	glUniform3f(light_position, lightPosition.x, lightPosition.y, lightPosition.z);

	// Lumina specifica
	// Seteaza pozitia privitorului
	glm::vec3 eyePosition = GetSceneCamera()->transform->GetWorldPosition();
	int eye_position = glGetUniformLocation(shader->program, "eye_position");
	glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

	// Proprietati de material
	int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
	glUniform1i(material_shininess, materialShininess);

	int material_kd = glGetUniformLocation(shader->program, "material_kd");
	glUniform1f(material_kd, materialKd);

	int material_ks = glGetUniformLocation(shader->program, "material_ks");
	glUniform1f(material_ks, materialKs);

	// Model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// View matrix
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Projection matrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	float time = Engine::GetElapsedTime();
	int location = glGetUniformLocation(shader->program, "Time");
	glUniform1f(location, time);

	if (texture1)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());
		glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);
	}

	if (texture2)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2->GetTextureID());
		glUniform1i(glGetUniformLocation(shader->program, "texture_2"), 1);
	}
	else {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());
		glUniform1i(glGetUniformLocation(shader->program, "texture_2"), 1);
	}

	// Deseneaza obiectul
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}

Mesh* Proiect::CreateMesh(const char *name, const std::vector<VertexFormat> &vertices, const std::vector<unsigned short> &indices)
{
	unsigned int VAO = 0;
	// Creeaza VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Creeaza VBO
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Trimite vertecsii in VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// Creeaza IBO
	unsigned int IBO;
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	// Trimite indecii in IBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// ========================================================================
	// This section describes how the GPU Shader Vertex Shader program receives data

	// set vertex position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

	// set vertex normal attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

	// set texture coordinate attribute
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

	// set vertex color attribute
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));

	// ========================================================================

	// Unbind the VAO
	glBindVertexArray(0);

	// Check for OpenGL errors
	CheckOpenGLError();

	// Mesh information is saved into a Mesh object
	meshes[name] = new Mesh(name);
	meshes[name]->InitFromBuffer(VAO, static_cast<unsigned short>(indices.size()));
	meshes[name]->vertices = vertices;
	meshes[name]->indices = indices;
	return meshes[name];
}

Texture2D* Proiect::CreateRandomTexture(unsigned int width, unsigned int height)
{
	GLuint textureID = 0;
	unsigned int channels = 3;
	unsigned int size = width * height * channels;
	unsigned char* data = new unsigned char[size];

	for (int i = 0; i < size; i++)
	{
		data[i] = rand() % 256;
	}
	// Genereaza texturi
	glGenTextures(1, &textureID);

	// Genereaza id ul texturii si il leaga
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Seteaza parametri texturii
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	CheckOpenGLError();

	// Seteaza texture data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	CheckOpenGLError();

	// Salveaza textura
	Texture2D* texture = new Texture2D();
	texture->Init(textureID, width, height, channels);

	SAFE_FREE_ARRAY(data);
	return texture;
}

void Proiect::OnInputUpdate(float deltaTime, int mods)
{
}

void Proiect::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_1)
	{
		startSnow = 1;
		startRain = 0;
	}

	if (key == GLFW_KEY_2)
	{
		if (startFireworks == 0)
			startFireworks = 1;
		else
			startFireworks = 0;
	}

	if (key == GLFW_KEY_3)
	{
		startRain = 1;
		startSnow = 0;

	}

	if (key == GLFW_KEY_R)
	{
		startSnow = 0;
		startFireworks = 0;
		startRain = 0;
	}
}

void Proiect::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Proiect::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
}

void Proiect::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Proiect::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Proiect::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Proiect::OnWindowResize(int width, int height)
{
}
