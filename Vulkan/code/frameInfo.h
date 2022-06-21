#pragma once

#include "CCamera.h"

#include <vulkan/vulkan.h>

struct FrameInfo
{
	int frameIndex;
	float frameTime;
	VkCommandBuffer commandBuffer;
	Camera& camera;
	VkDescriptorSet descriptorSet;

};