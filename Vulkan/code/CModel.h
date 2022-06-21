#pragma once

#include "CDevice.h"
#include "CBuffer.h"
// Glm //////////////////////////////////
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
// Std ///////////////////////////////////
#include <vector>
#include <memory>

class Model 
{
public:
	struct Vertex
	{
		glm::vec3 pos{};
		glm::vec3 color{};
		glm::vec3 normal{};
		glm::vec2 texCoord{};

		static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

		bool operator==(const Vertex& other) const
		{
			return pos == other.pos && color == other.color && normal == other.normal && texCoord == other.texCoord;
		}
	};

	class Builder
	{
	public:
		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};

		void loadModel(const std::string& modelpath);
	};


	Model(Device& device, const Model::Builder& builder);
	~Model();

	Model(const Model&) = delete;
	Model& operator = (const Model&) = delete;
	
	static std::unique_ptr<Model> createModelFromFile(Device& device, const std::string& modelpath);

	void bind(VkCommandBuffer commandBuffer);
	void draw(VkCommandBuffer commandBuffer);


private:
	void createVertexBuffer(const std::vector<Vertex>& vertices);
	void createIndexBuffer(const std::vector<uint32_t>& indices);
	
	Device&							m_device;
	std::unique_ptr<Buffer>			m_vertexBuffer;
	uint32_t						m_vertexCount;
	std::unique_ptr<Buffer>			m_indexBuffer;
	uint32_t						m_indexCount;
};


