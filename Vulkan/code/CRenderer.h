#pragma once

#include "CWindow.h"
#include "CDevice.h"
#include "CSwapChain.h"

// Std //////////////////////////////
#include <memory>
#include <vector>

class Renderer
{
public:


	Renderer(Window& window, Device& device);
	~Renderer();

	Renderer(const Renderer&) = delete;
	Renderer& operator= (const Renderer&) = delete;

	VkRenderPass getSwapChainRenderPass() const { return m_swapChain->getRenderPass(); }
	float getAspectRatio() const { return m_swapChain->extentAspectRatio(); }
	VkCommandBuffer getCurrentCommandBuffer() const { return m_commandBuffers[m_currentFrameIndex]; }
	int getFrameIndex() const { return m_currentFrameIndex; }

	VkCommandBuffer beginFrame();
	void endFrame();
	void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
	void endSwapChainRenderPass(VkCommandBuffer commandBuffer);


private:

	void createCommandBuffers();
	void freeCommandBuffers();
	void recreateSwapChain();

	Window& m_window;
	Device& m_device;
	std::unique_ptr<SwapChain> m_swapChain;
	std::vector<VkCommandBuffer> m_commandBuffers;

	uint32_t m_currentImageIndex;
	int m_currentFrameIndex = 0;
};
