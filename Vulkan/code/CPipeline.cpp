#include "CPipeline.h"
#include "CModel.h"

// Std ///////////////////////////////////////////////
#include <fstream>
#include <stdexcept>


Pipeline::Pipeline(Device& rDevice, const PipelineConfigInfo& rConfigInfo, const std::string& rVertFilepath, const std::string& rFragFilepath)
	: m_rDevice(rDevice)
{
	createGraphicsPipeline( rConfigInfo, rVertFilepath, rFragFilepath);
} //Pipeline()

Pipeline::~Pipeline()
{
	vkDestroyShaderModule(m_rDevice.getDevice(), m_VertShaderModule, nullptr);
	vkDestroyShaderModule(m_rDevice.getDevice(), m_FragShaderModule, nullptr);
	vkDestroyPipeline(m_rDevice.getDevice(), m_GraphicsPipeline, nullptr);

} //~Pipeline()

void Pipeline::createGraphicsPipeline(const PipelineConfigInfo& rConfigInfo, const std::string& rVertFilepath, const std::string& rFragFilepath)
{
	auto vertShaderCode = readFile(rVertFilepath);
	auto fragShaderCode = readFile(rFragFilepath);

	createShaderModule(vertShaderCode, &m_VertShaderModule);
	createShaderModule(fragShaderCode, &m_FragShaderModule);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = m_VertShaderModule;
		vertShaderStageInfo.pName = "main";
		vertShaderStageInfo.pSpecializationInfo = nullptr; //optional

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = m_FragShaderModule;
		fragShaderStageInfo.pName = "main";
		fragShaderStageInfo.pSpecializationInfo = nullptr; //optional

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	auto bindingDescriptions = Model::Vertex::getBindingDescriptions();
	auto attributeDescriptions = Model::Vertex::getAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &rConfigInfo.inputAssembly;
		pipelineInfo.pViewportState = &rConfigInfo.viewportInfo;
		pipelineInfo.pRasterizationState = &rConfigInfo.rasterizer;
		pipelineInfo.pMultisampleState = &rConfigInfo.multisampling;
		pipelineInfo.pDepthStencilState = &rConfigInfo.depthStencil;
		pipelineInfo.pColorBlendState = &rConfigInfo.colorBlending;
		pipelineInfo.pDynamicState = &rConfigInfo.dynamicState;
		pipelineInfo.layout = rConfigInfo.pipelineLayout;
		pipelineInfo.renderPass = rConfigInfo.renderPass;
		pipelineInfo.pTessellationState = nullptr;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

	if ( vkCreateGraphicsPipelines(m_rDevice.getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS)
		throw std::runtime_error("failed to create graphics pipeline");

} //Pipeline::createGraphicsPipeline()

void Pipeline::bind(VkCommandBuffer commandBuffer)
{
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);

} //Pipeline::bind()

void Pipeline::defaultPipelineConfigInfo(PipelineConfigInfo& configInfo)
{
	configInfo.inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	configInfo.inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	configInfo.inputAssembly.primitiveRestartEnable = VK_FALSE;

	configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	configInfo.viewportInfo.viewportCount = 1;
	configInfo.viewportInfo.pViewports = nullptr;
	configInfo.viewportInfo.scissorCount = 1;
	configInfo.viewportInfo.pScissors = nullptr;

	configInfo.rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	configInfo.rasterizer.depthClampEnable = VK_FALSE;
	configInfo.rasterizer.rasterizerDiscardEnable = VK_FALSE;
	configInfo.rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	configInfo.rasterizer.lineWidth = 1.0f;
	configInfo.rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	configInfo.rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	configInfo.rasterizer.depthBiasEnable = VK_FALSE; // if you want to use shadowmaps enable this
	configInfo.rasterizer.depthBiasConstantFactor = 0.0f; //optional
	configInfo.rasterizer.depthBiasClamp = 0.0f; // optional
	configInfo.rasterizer.depthBiasSlopeFactor = 0.0f; // optional

	configInfo.multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	configInfo.multisampling.sampleShadingEnable = VK_FALSE;
	configInfo.multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	configInfo.multisampling.minSampleShading = 1.0f;
	configInfo.multisampling.pSampleMask = nullptr;
	configInfo.multisampling.alphaToCoverageEnable = VK_FALSE;
	configInfo.multisampling.alphaToOneEnable = VK_FALSE;

	configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
	configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
	configInfo.colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	configInfo.colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	configInfo.colorBlending.logicOpEnable = VK_FALSE;
	configInfo.colorBlending.logicOp = VK_LOGIC_OP_COPY;
	configInfo.colorBlending.attachmentCount = 1;
	configInfo.colorBlending.pAttachments = &configInfo.colorBlendAttachment;
	configInfo.colorBlending.blendConstants[0] = 0.0f;
	configInfo.colorBlending.blendConstants[1] = 0.0f;
	configInfo.colorBlending.blendConstants[2] = 0.0f;
	configInfo.colorBlending.blendConstants[3] = 0.0f;

	configInfo.depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	configInfo.depthStencil.depthTestEnable = VK_TRUE;
	configInfo.depthStencil.depthWriteEnable = VK_TRUE;
	configInfo.depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	configInfo.depthStencil.depthBoundsTestEnable = VK_FALSE;
	configInfo.depthStencil.minDepthBounds = 0.0f;
	configInfo.depthStencil.maxDepthBounds = 1.0f;
	configInfo.depthStencil.stencilTestEnable = VK_FALSE;
	configInfo.depthStencil.front = {};
	configInfo.depthStencil.back = {};

	configInfo.dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

	VkPipelineDynamicStateCreateInfo dynamicState{};
	configInfo.dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	configInfo.dynamicState.dynamicStateCount = static_cast<uint32_t>(configInfo.dynamicStateEnables.size());
	configInfo.dynamicState.pDynamicStates = configInfo.dynamicStateEnables.data();

	configInfo.bindingDescriptions = Model::Vertex::getBindingDescriptions();
	configInfo.attributeDescriptions = Model::Vertex::getAttributeDescriptions();

} //Pipeline::defaultPipelineConfigInfo()

void Pipeline::createShaderModule(const std::vector<char>& rCode, VkShaderModule* pShaderModule)
{
	VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = rCode.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(rCode.data());

	if (vkCreateShaderModule(m_rDevice.getDevice(), &createInfo, nullptr, pShaderModule) != VK_SUCCESS)
		throw std::runtime_error("faild to create shader module");

} //Pipeline::createShaderModule()

std::vector<char> Pipeline::readFile(const std::string& sFilename)
{
	std::ifstream file(sFilename, std::ios::ate | std::ios::binary); // ate: Start reading at the end of the file to get the size of the file;

	if (!file.is_open())
		throw std::runtime_error("faild to open file: " + sFilename);

	size_t fileSize = (size_t)file.tellg();
	std::vector <char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;

} //Pipeline::readFile()

