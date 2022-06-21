#include "CRenderSystem.h"


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// Std ////////////////////////////
#include <stdexcept>
#include <array>
#include <iostream>

struct PushConstantData
{
	glm::mat4 modelMatrix{ 1.f };
};

RenderSystem::RenderSystem(Device& device, VkRenderPass renderPass , VkDescriptorSetLayout setLayout)
	: m_device(device)
{
	createPipelineLayout(setLayout);
	createPipeline(renderPass);

} //RenderSystem::RenderSystem()


RenderSystem::~RenderSystem()
{
	vkDestroyPipelineLayout(m_device.getDevice(), m_pipelineLayout, nullptr);

} //:~RenderSystem()



void RenderSystem::createPipelineLayout(VkDescriptorSetLayout setLayout)
{
	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(PushConstantData);

	std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ setLayout }; // can add mor layouts later. order = set number ( setLayout = set0)

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
	pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

	if (vkCreatePipelineLayout(m_device.getDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
		throw std::runtime_error("failed to create pipeline layout");

} //RenderSystem::createPipelineLayout()


void RenderSystem::createPipeline(VkRenderPass renderPass)
{
	PipelineConfigInfo pipelineConfig{};
	Pipeline::defaultPipelineConfigInfo(pipelineConfig);
	pipelineConfig.renderPass = renderPass;
	pipelineConfig.pipelineLayout = m_pipelineLayout;

	m_pipeline = std::make_unique<Pipeline>(m_device, pipelineConfig, "shaders/vert.spv", "shaders/frag.spv");

} //RenderSystem::createPipeline()

void RenderSystem::renderGameObjects(FrameInfo& frameInfo, std::vector<GameObject>& gameObjects)
{
	m_pipeline->bind(frameInfo.commandBuffer);

	vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &frameInfo.descriptorSet, 0, nullptr);

	for (GameObject& obj : gameObjects)
	{

		if (obj.getName() == "camera")
			continue;

		PushConstantData push{};
		push.modelMatrix = obj.transform.mat4();

		vkCmdPushConstants(frameInfo.commandBuffer, m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantData), &push);

		obj.model->bind(frameInfo.commandBuffer);
		obj.model->draw(frameInfo.commandBuffer);
	}

} //RenderSystem::renderGameObjects()
