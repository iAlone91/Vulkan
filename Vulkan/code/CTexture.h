#pragma once

#include "CDevice.h"
#include "CBuffer.h"

#include <vector>
#include <memory>

class Texture
{
public:

	struct  Builder
	{
		unsigned char* pixels;
		int texWidth, texHeight, texChannels;

		void loadTexture(const std::string& modelpath);
	};


	Texture(Device& device, const Texture::Builder& builder);
	~Texture();

	Texture(const Texture&) = delete;
	Texture& operator = (const Texture&) = delete;

	static std::unique_ptr<Texture> createTextureFromFile(Device& device, const std::string& modelpath);

	void bind(VkCommandBuffer commandBuffer);


private:
	void createImage(int texWidth, int texHeight, unsigned char* pixels);


	Device& m_device;

	VkImage			m_textureImage;
	VkDeviceMemory	m_textureImageMemory;
};