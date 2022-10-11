#include "CApp.h"

#include "CKeyboardMovemendControllerGLFW.h"
#include "CRenderSystem.h"
#include "CCamera.h"
// Std ////////////////////////////
#include <stdexcept>
#include <array>
#include <chrono>
#include <iostream>

struct GlobalUbo
{
	glm::mat4 projectionView{ 1.0f };
};


App::App()
{
	createTextureImage(); // TODO: Make this a image class
	m_textureImageView = createImageView(m_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
	createTextureSampler();

	m_descriptorPool = DescriptorPool::Builder(m_device)
		.setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
		.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
		.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1)
		.build();


	//m_texture = Texture::createTextureFromFile(m_device, "textures/psq.png");
	loadGameObjects();
} //App()


App::~App() 
{
	vkDestroySampler(m_device.getDevice(), m_textureSampler, nullptr);

	vkDestroyImageView(m_device.getDevice(), m_textureImageView, nullptr);

	vkDestroyImage(m_device.getDevice(), m_textureImage, nullptr);
	vkFreeMemory(m_device.getDevice(), m_textureImageMemory, nullptr);
}

void App::run()
{


	std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
	for (size_t i = 0; i < uboBuffers.size(); i++)
	{
		uboBuffers[i] = std::make_unique<Buffer>(m_device, sizeof(GlobalUbo), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		uboBuffers[i]->map();
	}

	auto setLayout = DescriptorSetLayout::Builder(m_device)
		.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
		.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.build();

	std::vector<VkDescriptorSet> descriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);

	VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = m_textureImageView;
		imageInfo.sampler = m_textureSampler;

	for (int i = 0; i < descriptorSets.size(); i++)
	{
		auto bufferInfo = uboBuffers[i]->descriptorInfoForBuffer();

		DescriptorWriter(*setLayout, *m_descriptorPool)
			.writeBuffer(0, &bufferInfo)
			.writeImage(1, &imageInfo)
			.build(descriptorSets[i]);
	}

	RenderSystem renderSystem { m_device, m_renderer.getSwapChainRenderPass(), setLayout->getDescriptorSetLayout() };
	Camera camera{};
	camera.setViewDirection(glm::vec3( 0.0f ), glm::vec3( 0.5f, 0.0f, 0.0f ));
	
	GameObject viewObject = GameObject::createGameObject("camera");
	m_gameObjects.push_back(std::move(viewObject));

	KeyboardMovmentController cameraController;

	//std::cout << "maxPushConstantSize = " << m_device.m_properties.limits.maxPushConstantsSize << '\n'; // TODO: Check on why it onely shows in debug
	std::cout << "Total number of GameObjects: " << m_gameObjects.size() << '\n';

	for (auto& obj : m_gameObjects)
	{
		std::cout << "GameObject ID: " << obj.getId() << " Name: " << obj.getName() << '\n';
	}

	auto currentTime = std::chrono::high_resolution_clock::now();

	static float time = 0;
	// GameLoop
	while (!m_window.shouldClose())
	{
		size_t currentGameObjectsSize = m_gameObjects.size();

		glfwPollEvents();

		auto newTime = std::chrono::high_resolution_clock::now();
		float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
		currentTime = newTime;

		cameraController.moveInPLaneXZ(m_window.getGLFWwindow(), frameTime, viewObject);
		camera.setViewYXZ(viewObject.transform.translation, viewObject.transform.rotation);

		float aspect = m_renderer.getAspectRatio();
		//camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 9999);
		camera.setPerspectiveProjection(glm::radians(45.f), aspect, 0.1f, 1000.f);


		if (VkCommandBuffer commandBuffer = m_renderer.beginFrame() ) // Returns nullptr if not completed
		{
			size_t frameIndex = m_renderer.getFrameIndex();
			
			FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, camera, descriptorSets[frameIndex] };

			//update
			GlobalUbo ubo{};
			ubo.projectionView = camera.getProjection() * camera.getView();
			uboBuffers[frameIndex]->writeToBuffer(&ubo);
			uboBuffers[frameIndex]->flush();
		

			//render
			m_renderer.beginSwapChainRenderPass(commandBuffer);
			renderSystem.renderGameObjects(frameInfo, m_gameObjects);
			m_renderer.endSwapChainRenderPass(commandBuffer);
			m_renderer.endFrame();
		}

		static int counter = 0;

		if (counter != (100 * 100))
		{
			for (size_t i = 0; i < 10; i++)
			{
				loadGameObjects();
				counter++;
			}
			time += frameTime;
		}

		if (currentGameObjectsSize != m_gameObjects.size())
		{
				std::cout << "GameObject ID: " << m_gameObjects[m_gameObjects.size() - 1].getId() << " Name: " << m_gameObjects[m_gameObjects.size() - 1].getName() << '\n';
		}
	}

	vkDeviceWaitIdle(m_device.getDevice());
} //run()

void App::loadGameObjects()
{
	static int a = 0;

	if (a == 0)
	{
		m_model = Model::createModelFromFile(m_device, "models/vikingRoom.obj");
		a = 1;
	}

	static float offx = 0.f;
	static float offy = 0.f;

	GameObject cube = GameObject::createGameObject(std::string("vikingRoom"));
	cube.model = m_model;
	cube.transform.translation = { 0.0f + offx , 2.0f , 0.0f + offy };
	cube.transform.scale = { 4.0f, 4.0f, 4.0f };

	offx += 5.3f;
	if (offx > 530)
	{
		offx = 0;
		offy += 5.85f;
	}
	m_gameObjects.push_back(std::move(cube));
}

#include "stb_image.h"

void App::createTextureImage()
{
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load("textures/vikingRoom.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = static_cast<uint64_t>(texWidth * texHeight * 4);

	if (!pixels) {
		throw std::runtime_error("failed to load texture image!");
	}

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	m_device.createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(m_device.getDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(m_device.getDevice(), stagingBufferMemory);


	stbi_image_free(pixels);

	createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_textureImage , m_textureImageMemory);

	m_device.transitionImageLayout(m_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	m_device.copyBufferToImage(stagingBuffer, m_textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
	m_device.transitionImageLayout(m_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(m_device.getDevice(), stagingBuffer, nullptr);
	vkFreeMemory(m_device.getDevice(), stagingBufferMemory, nullptr);
}

void App::createTextureSampler()
{
	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(m_device.getPhysicalDevice(), &properties);

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR; // specify how to interpolate texels that are magnified (nearest neighbour or bi linear)
	samplerInfo.minFilter = VK_FILTER_LINEAR; // specify how to interpolate texels that are minified
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT; // -x determines what happens when you try to read texels outside the image
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT; // y
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT; // z
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
	samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE; // texels are addressed using the [0, 1] range on all axes
	samplerInfo.compareEnable = VK_FALSE; // if VK_TRUE texels will first be compared to a value, and the result of that comparison is used in filtering operations
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	if (vkCreateSampler(m_device.getDevice(), &samplerInfo, nullptr, &m_textureSampler) != VK_SUCCESS)
		throw std::runtime_error("failed to create texture sampler");
}

void App::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.flags = 0;

	if (vkCreateImage(m_device.getDevice(), &imageInfo, nullptr, &image) != VK_SUCCESS)
		throw std::runtime_error("faild to create image");

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(m_device.getDevice(), image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (vkAllocateMemory(m_device.getDevice(), &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate image memory");

	vkBindImageMemory(m_device.getDevice(), image, imageMemory, 0);
}

uint32_t App::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(m_device.getPhysicalDevice(), &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter && (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;
	}

	throw std::runtime_error("failed to find suitable memory type");
}

VkImageView App::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image = image;
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.format = format;
	createInfo.subresourceRange.aspectMask = aspectFlags;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;

	if (vkCreateImageView(m_device.getDevice(), &createInfo, nullptr, &imageView) != VK_SUCCESS)
		throw std::runtime_error("faild to create image view");

	return imageView;

} //createImageView()
