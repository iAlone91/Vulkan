#pragma once

#include "CModel.h"
#include "CTexture.h"

#include "glm/gtc/matrix_transform.hpp"

#include <memory>

struct TransformComponent
{
	glm::vec3 translation{};
	glm::vec3 scale{ 1.f, 1.f, 1.f };
	glm::vec3 rotation{};

	glm::mat4 mat4()
	{
		glm::highp_mat4 transform = glm::translate(glm::mat4{ 1.f }, translation);

		transform = glm::rotate(transform, rotation.x, { 1.f, 0.f, 0.f });
		transform = glm::rotate(transform, rotation.y, { 0.f, 1.f, 0.f });
		transform = glm::rotate(transform, rotation.z, { 0.f, 0.f, 1.f });

		transform = glm::scale(transform, scale);

		return transform;
	}
};

class GameObject
{
public:

	using id_t = unsigned int;

	static GameObject createGameObject(std::string name)
	{
		static id_t currentId = 0;
		return GameObject(currentId++, name);
	}

	GameObject(const GameObject&) = delete;
	GameObject& operator=(const GameObject&) = delete;
	GameObject(GameObject&&) = default;
	GameObject& operator=(GameObject&&) = default;

	id_t getId() { return m_id; }
	std::string getName() { return m_name; }

	std::shared_ptr<Model> model{};
	TransformComponent transform{};

private:

	GameObject(id_t objId, std::string name) : m_id(objId), m_name(name) {}

	id_t m_id;
	std::string m_name;
};