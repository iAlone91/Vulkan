#include "CTexture.h"

// stb_image ////////////////////////////
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stdexcept>

Texture::Texture(Device& device, const Texture::Builder& builder)
	: m_device(device)
{
	createImage(builder.texWidth, builder.texHeight, builder.pixels);

}

Texture::~Texture()
{
	vkDestroyImage(m_device.getDevice(), m_textureImage, nullptr);
	vkFreeMemory(m_device.getDevice(), m_textureImageMemory, nullptr);
}

std::unique_ptr<Texture> Texture::createTextureFromFile(Device& device, const std::string& filepath)
{
	
	Builder builder{};
	builder.loadTexture(filepath);

	return std::make_unique<Texture>(device, builder);
}

void Texture::bind(VkCommandBuffer commandBuffer)
{
}

void Texture::createImage(int texWidth, int texHeight, unsigned char* pixels)
{
	
}


void Texture::Builder::loadTexture(const std::string& filepath)
{
	
	pixels = stbi_load(filepath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = static_cast<uint64_t>(texWidth * texHeight * 4);

	if (!pixels) {
		throw std::runtime_error("failed to load texture image!");
	}
}
