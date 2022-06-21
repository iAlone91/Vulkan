#include "CRenderer.h"

// Std ////////////////////////////
#include <stdexcept>
#include <array>
#include <iostream>

Renderer::Renderer(Window& window, Device& device)
	: m_window(window), m_device(device)
{
	recreateSwapChain();
	createCommandBuffers();
}


Renderer::~Renderer()
{
	freeCommandBuffers();
}

void Renderer::recreateSwapChain()
{
	VkExtent2D extent = m_window.getExtent();
	while (extent.width == 0 || extent.height == 0) // check to see if window size is > 0 else pause the window. 
	{
		extent = m_window.getExtent();
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(m_device.getDevice());

	if (m_swapChain == nullptr)
		m_swapChain = std::make_unique<SwapChain>(m_device, extent);
	else
	{
		std::shared_ptr<SwapChain> oldSwapChain = std::move(m_swapChain);
		m_swapChain = std::make_unique<SwapChain>(m_device, extent, oldSwapChain);

	}

} //recreateSwapChain()

void Renderer::createCommandBuffers()
{
	m_commandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_device.getCommandPool();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)m_commandBuffers.size();

	if (vkAllocateCommandBuffers(m_device.getDevice(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS)
		throw std::runtime_error("failed to create command buffers");

} //createCommandBuffers()

void Renderer::freeCommandBuffers()
{
	vkFreeCommandBuffers(m_device.getDevice(), m_device.getCommandPool(), static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());
	m_commandBuffers.clear();
} //freeCommandBuffers()

VkCommandBuffer Renderer::beginFrame()
{
	VkResult result = m_swapChain->acuireNextImage(&m_currentImageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapChain();
		return nullptr;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		throw std::runtime_error("failed to acquire swap chain image");

	VkCommandBuffer commandBuffer = getCurrentCommandBuffer();

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		throw std::runtime_error("failed to begin recording command buffer");

	return commandBuffer;

} //beginFrame()

void Renderer::endFrame()
{
	VkCommandBuffer commandBuffer = getCurrentCommandBuffer();

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		throw std::runtime_error("failed to record command buffer");

	VkResult result = m_swapChain->submitCommandBuffers(&commandBuffer, &m_currentImageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_window.wasWindowResized())
	{
		m_window.resetWindowResizeFlag();
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS)
		throw std::runtime_error("failed to present swap chain image");

	m_currentFrameIndex = (m_currentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
} //endFrame()

void Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
	std::array<VkClearValue, 2> clearValues{}; // the order of clearValues should be identical to the order of your attachments
	clearValues[0].color = { 1.0f, 1.0f, 1.0f, 1.0f };
	clearValues[1].depthStencil = { 1.0, 0 };

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_swapChain->getRenderPass();
	renderPassInfo.framebuffer = m_swapChain->getFrameBuffer(m_currentImageIndex);
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = m_swapChain->getSwapchainExtent();
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_swapChain->getSwapchainExtent().width);
	viewport.height = static_cast<float>(m_swapChain->getSwapchainExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = m_swapChain->getSwapchainExtent();

	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

} //beginSwapChainRenderPass()

void Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
	vkCmdEndRenderPass(commandBuffer);

} //endSwapChainRenderPass()