#pragma once
#include "CWindow.h"

#include <string>
#include <vector>
#include <optional>



struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR			capabilities;
	std::vector<VkSurfaceFormatKHR>		formats;
	std::vector<VkPresentModeKHR>		presentModes;
};

struct QueueFamilyIndices
{
	std::optional<uint32_t>		graphicsFamily;
	std::optional<uint32_t>		presentFamily;

	bool isComplete()
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

class Device
{
public:
#ifdef  DEBUG
	const bool enableValidationLayers = true;
#else 
	const bool enableValidationLayers = false;
#endif //  DEBUG

	Device(Window& window);
	~Device();

	Device( const Device& ) = delete;
	void operator= ( const Device& ) = delete;
	Device ( Device&& ) = delete;
	Device& operator= ( Device&& ) = delete;

	VkDevice				getDevice()				{ return m_device; }
	VkSurfaceKHR			getSurface()			{ return m_surface; }
	VkPhysicalDevice		getPhysicalDevice()		{ return m_physicalDevice; }
	VkQueue					getGraphicsQueue()		{ return m_graphicsQueue; }
	VkQueue					getPresentQueue()		{ return m_presentQueue; }
	VkCommandPool			getCommandPool()		{ return m_commandPool; }
	SwapChainSupportDetails getSwapChainSupport()	{ return querySwapChainSupport(m_physicalDevice); }

	QueueFamilyIndices findPhysicalQueueFamilies() { return  findQueueFamilies(m_physicalDevice); }
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlagBits features);
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	void createImageWithInfo(const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

	VkPhysicalDeviceProperties m_properties;

private:
	void createInstance();
	void setupDebugMessenger();
	void createSurface();
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createCommandPool();
	

	// Helper /////////////////////////////////////////////////////
	bool isDeviceSuitable(VkPhysicalDevice device);
	int rateDeviceSuitability(VkPhysicalDevice device);
	std::vector<const char*> getRequiredExtensions();
	bool checkValidationLayerSupport();
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice devise);
	bool hasStencilComponent(VkFormat format);

	


	VkInstance						m_instance;
	VkDebugUtilsMessengerEXT		m_debugMessenger = NULL;
	VkPhysicalDevice				m_physicalDevice = VK_NULL_HANDLE;
	Window&							m_rWindow;
	VkCommandPool					m_commandPool;

	VkDevice						m_device = NULL;
	VkSurfaceKHR					m_surface;
	VkQueue							m_graphicsQueue = NULL;
	VkQueue							m_presentQueue = NULL;

	const std::vector<const char*> validationLayers { "VK_LAYER_KHRONOS_validation" };
	const std::vector<const char*> deviceExtensions { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
};
