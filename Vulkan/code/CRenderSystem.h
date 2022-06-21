#pragma once
#include "CCamera.h"
#include "CGameObject.h"
#include "CPipeline.h"
#include "CDevice.h"
#include "frameInfo.h"

// Std //////////////////////////////
#include <memory>
#include <vector>

class RenderSystem
{
public:

	RenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout setLayout);
	~RenderSystem();

	RenderSystem(const RenderSystem&) = delete;
	RenderSystem& operator= (const RenderSystem&) = delete;

	void renderGameObjects(FrameInfo& frameInfo, std::vector<GameObject>& gameObjects);

private:
	void createPipelineLayout(VkDescriptorSetLayout setLayout);
	void createPipeline( VkRenderPass renderPass );

	Device& m_device;

	std::unique_ptr<Pipeline> m_pipeline;
	VkPipelineLayout m_pipelineLayout;

};
