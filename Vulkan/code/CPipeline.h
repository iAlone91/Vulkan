#pragma once

#include "CDevice.h"

// Std /////////////////////////////////////
#include <string>
#include <vector>

struct PipelineConfigInfo 
{
	PipelineConfigInfo() = default;
	PipelineConfigInfo(const PipelineConfigInfo&) = delete;
	PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

	std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

	VkPipelineViewportStateCreateInfo viewportInfo;
	VkPipelineInputAssemblyStateCreateInfo inputAssembly;
	VkPipelineRasterizationStateCreateInfo rasterizer;
	VkPipelineMultisampleStateCreateInfo multisampling;
	VkPipelineColorBlendAttachmentState colorBlendAttachment;
	VkPipelineColorBlendStateCreateInfo colorBlending;
	VkPipelineDepthStencilStateCreateInfo depthStencil;
	std::vector<VkDynamicState> dynamicStateEnables;
	VkPipelineDynamicStateCreateInfo dynamicState;
	VkPipelineLayout pipelineLayout = nullptr;
	VkRenderPass renderPass = nullptr;
	uint32_t subpass = 0;
};

class Pipeline
{
public:

	Pipeline( Device& rDevice, const PipelineConfigInfo& rConfigInfo, const std::string& rVertFilepath, const std::string& rFragFilepath);
	~Pipeline();

	Pipeline(const Pipeline&) = delete;
	Pipeline operator= (const Pipeline&) = delete;

	void bind(VkCommandBuffer commandBuffer);
	static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

private:

	void createGraphicsPipeline( const PipelineConfigInfo& rConfigInfo, const std::string& rVertFilepath, const std::string& rFragFilepath);
	void createShaderModule(const std::vector<char>& rCode, VkShaderModule* pShaderModule);

	// Helpers ///////////////////////////////////////////////////////
	static std::vector<char> readFile(const std::string& sFilename);

	Device&							m_rDevice;
	VkPipeline						m_GraphicsPipeline;
	VkShaderModule					m_VertShaderModule;
	VkShaderModule					m_FragShaderModule;
};