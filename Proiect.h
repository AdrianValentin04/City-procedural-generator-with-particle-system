#pragma once
#include <Component/SimpleScene.h>
#include <Component/Transform/Transform.h>
#include <Core/GPU/Mesh.h>


class Proiect : public SimpleScene
{
	struct Snow
	{
		glm::vec4 position;
		glm::vec4 speed;
		glm::vec4 initialPos;
		glm::vec4 initialSpeed;

		Snow() {};

		Snow(const glm::vec4& pos, const glm::vec4& speed)
		{
			SetInitial(pos, speed);
		}

		void SetInitial(const glm::vec4& pos, const glm::vec4& speed)
		{
			position = pos;
			initialPos = pos;

			this->speed = speed;
			initialSpeed = speed;
		}
	};

	struct Fireworks
	{
		glm::vec4 position;
		glm::vec4 speed;
		glm::vec4 initialPos;
		glm::vec4 initialSpeed;

		Fireworks() {};

		Fireworks(const glm::vec4& pos, const glm::vec4& speed)
		{
			SetInitial(pos, speed);
		}

		void SetInitial(const glm::vec4& pos, const glm::vec4& speed)
		{
			position = pos;
			initialPos = pos;

			this->speed = speed;
			initialSpeed = speed;
		}
	};

	struct Rain
	{
		glm::vec4 position;
		glm::vec4 speed;
		glm::vec4 initialPos;
		glm::vec4 initialSpeed;

		Rain() {};

		Rain(const glm::vec4& pos, const glm::vec4& speed)
		{
			SetInitial(pos, speed);
		}

		void SetInitial(const glm::vec4& pos, const glm::vec4& speed)
		{
			position = pos;
			initialPos = pos;

			this->speed = speed;
			initialSpeed = speed;
		}
	};

	public:
		Proiect();
		~Proiect();

		void Init() override;

	private:
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;

		void RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, Texture2D* texture1 = NULL, Texture2D* texture2 = NULL);
		Texture2D* CreateRandomTexture(unsigned int width, unsigned int height);
		Mesh * CreateMesh(const char * name, const std::vector<VertexFormat> &vertices, const std::vector<unsigned short> &indices);

		void LoadShader(std::string name, bool hasGeomtery = true);
		int Randrange(int min_n, int max_n);
		int RandFireworksPos(int reper);

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;

		std::unordered_map<std::string, Texture2D*> mapTextures;
		GLuint randomTextureID;

		glm::vec3 lightPosition;
		glm::vec3 lightDirection;
		unsigned int materialShininess;
		float materialKd;
		float materialKs;

	protected:
		glm::mat4 modelMatrix;
		float translateX, translateY, translateZ;
		float scaleX, scaleY, scaleZ;
		float angularStepOX, angularStepOY, angularStepOZ;
		GLenum polygonMode;
		int startSnow = 0, startFireworks = 0, startRain = 0;

		ParticleEffect<Rain>* rainEffect;
		ParticleEffect<Fireworks>* fireworksEffect;
		ParticleEffect<Snow>* snowEffect;

};
