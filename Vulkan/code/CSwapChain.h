#pragma once
#include "CDevice.h"

// Vulkan /////////////////////////////
#include <vulkan/vulkan.h>
// Std ////////////////////////////
#include <memory>
#include <vector>


class SwapChain
{
public:

	static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

	SwapChain(Device& rDevice, VkExtent2D windowExtent);
	SwapChain(Device& rDevice, VkExtent2D windowExtent, std::shared_ptr<SwapChain> previous );
	~SwapChain();

	SwapChain(const SwapChain&) = delete;
	SwapChain& operator = (const SwapChain&) = delete;

	// Getters ///////////////////////////////////////////////////////////////////////////////
	VkFramebuffer			getFrameBuffer(int index)	{ return m_swapChainFramebuffers[index]; }
	VkRenderPass			getRenderPass()				{ return m_renderPass; }
	VkImageView				getImageView(int index)		{ return m_swapChainImageViews[index]; }
	size_t					getImageCount()				{ return m_swapChainImages.size(); }
	VkFormat				getSwapChainImageFormat()	{ return m_swapChainImageFormat; }
	VkExtent2D				getSwapchainExtent()		{ return m_swapChainExtent; }
	uint32_t				getWidth()					{ return m_windowExtent.width; }
	uint32_t				getHeight()					{ return m_windowExtent.height; }
	float					extentAspectRatio()			{ return static_cast<float>(m_swapChainExtent.width) / static_cast<float>(m_swapChainExtent.height); }
	
	// Helper ////////////////////////////////////////////////
	VkFormat		findDepthFormat();
	VkResult		acuireNextImage(uint32_t* imageIndex);
	VkResult		submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);
	VkImageView		createImageView(VkImage image, VkFormat format);

private:

	void			init();
	void			createSwapChain();
	void			createImageViews();
	void			createRenderPass();
	void			createDepthResources();
	void			createFramebuffers();
	void			createSyncObjects();

	// Helper /////////////////////////////////////
	VkSurfaceFormatKHR	chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& rAvailableFormats);
	VkPresentModeKHR	chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& rAvailablePresentModes);
	VkExtent2D			chooseSwapExtent(const VkSurfaceCapabilitiesKHR& rCapabilities);
	
	// Member //////////////////////////////////////////////
	VkFormat					m_swapChainImageFormat;
	VkFormat					m_swapChainDepthFormat;
	VkExtent2D					m_swapChainExtent;

	std::vector<VkFramebuffer>	m_swapChainFramebuffers;
	VkRenderPass				m_renderPass;

	std::vector<VkImage>		m_depthImages;
	std::vector<VkDeviceMemory> m_depthImageMemorys;
	std::vector<VkImageView>	m_depthImageViews;
	std::vector<VkImage>		m_swapChainImages;
	std::vector<VkImageView>	m_swapChainImageViews;

	Device&						m_device;
	VkExtent2D					m_windowExtent;

	VkSwapchainKHR				m_swapChain;
	std::shared_ptr<SwapChain>  m_oldSwapChain;

	std::vector<VkSemaphore>	m_imageAvailableSemaphores;
	std::vector<VkSemaphore>	m_renderFinishedSemaphores;
	std::vector<VkFence>		m_inFlightFences;
	std::vector<VkFence>		m_imagesInFlight;
	size_t						m_currentFrame = 0;

};