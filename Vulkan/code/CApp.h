#pragma once
#include "CGameObject.h"
#include "CWindow.h"
#include "CDevice.h"
#include "CRenderer.h"
#include "CDescriptors.h"

// Std //////////////////////////////
#include <memory>
#include <vector>

class App
{
public:
	const uint32_t WIDTH = 1280;
	const uint32_t HEIGHT = 720;

	App();
	~App();

	App(const App&) = delete;
	App& operator= (const App&) = delete;

	void run();

private:
	void loadGameObjects();

	Window m_window { WIDTH, HEIGHT, "Vulkan" };
	Device m_device{ m_window };
	Renderer m_renderer{ m_window, m_device };

	std::unique_ptr<DescriptorPool> m_descriptorPool;
	std::vector<GameObject> m_gameObjects;
	std::shared_ptr<Model> m_model;
	std::shared_ptr<Texture> m_texture;

	VkImage							m_textureImage;
	VkDeviceMemory					m_textureImageMemory;
	VkImageView						m_textureImageView;
	VkSampler						m_textureSampler;

	void createTextureImage();
	void createTextureSampler();
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
};
