#pragma once

#include "CCamera.h"

#include <vulkan/vulkan.h>

struct FrameInfo
{
	size_t frameIndex;
	float frameTime;
	VkCommandBuffer commandBuffer;
	Camera& camera;
	VkDescriptorSet descriptorSet;

};