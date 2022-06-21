#include "CDescriptors.h"

#include <stdexcept>

DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::addBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t count)
{
	VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = binding;
		layoutBinding.descriptorType = descriptorType;
		layoutBinding.descriptorCount = count;
		layoutBinding.stageFlags = stageFlags;

	m_bindings[binding] = layoutBinding;

	return *this;

} //DescriptorSetLayout::Builder::addBinding()

std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::Builder::build() const
{
	return std::make_unique<DescriptorSetLayout>(m_device, m_bindings);
} //DescriptorSetLayout::Builder::build()

DescriptorSetLayout::DescriptorSetLayout(Device& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
	: m_device(device), m_bindings(bindings)
{
	std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};

	for (auto binding : bindings)
	{
		setLayoutBindings.push_back(binding.second);
	}

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
		descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutInfo.bindingCount =  static_cast<uint32_t>(setLayoutBindings.size());
		descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

		if (vkCreateDescriptorSetLayout(m_device.getDevice(), &descriptorSetLayoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS)
			throw std::runtime_error("failed to create descriptor set layout");

} //DescriptorSetLayout::DescriptorSetLayout()

DescriptorSetLayout::~DescriptorSetLayout()
{
	vkDestroyDescriptorSetLayout(m_device.getDevice(), m_descriptorSetLayout, nullptr);

} //DescriptorSetLayout::~DescriptorSetLayout()

DescriptorPool::Builder& DescriptorPool::Builder::addPoolSize(VkDescriptorType descriptorType, uint32_t count)
{
	m_poolSizes.push_back({ descriptorType, count });

	return *this;

} //DescriptorPool::Builder::addPoolSize()

DescriptorPool::Builder& DescriptorPool::Builder::setPoolFlags(VkDescriptorPoolCreateFlags flags)
{
	m_poolFlags = flags;

	return *this;

} //DescriptorPool::Builder::setPoolFlags()

DescriptorPool::Builder& DescriptorPool::Builder::setMaxSets(uint32_t count)
{
	m_maxSets = count;

	return *this;

} //DescriptorPool::Builder::setMaxSets()

std::unique_ptr<DescriptorPool> DescriptorPool::Builder::build() const
{
	return std::make_unique<DescriptorPool>(m_device, m_maxSets, m_poolFlags, m_poolSizes);

} //DescriptorPool::Builder::build()

DescriptorPool::DescriptorPool(Device& device, uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize>& poolSizes)
	: m_device(device)
{
	VkDescriptorPoolCreateInfo descriptorPoolInfo{};
		descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		descriptorPoolInfo.pPoolSizes = poolSizes.data();
		descriptorPoolInfo.maxSets = maxSets;
		//descriptorPoolInfo.flags = poolFlags;

	if (vkCreateDescriptorPool(m_device.getDevice(), &descriptorPoolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS)
		throw std::runtime_error("faild to create discriptor pool");

} //DescriptorPool::DescriptorPool()

DescriptorPool::~DescriptorPool()
{
	vkDestroyDescriptorPool(m_device.getDevice(), m_descriptorPool, nullptr);

} //DescriptorPool::~DescriptorPool()

bool DescriptorPool::allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const
{
	VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_descriptorPool;
		allocInfo.pSetLayouts = &descriptorSetLayout;
		allocInfo.descriptorSetCount = 1;

	if (vkAllocateDescriptorSets(m_device.getDevice(), &allocInfo, &descriptor) != VK_SUCCESS)
		return false;

	return true;

} //DescriptorPool::allocateDescriptor()

void DescriptorPool::freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const
{
	vkFreeDescriptorSets(m_device.getDevice(), m_descriptorPool, static_cast<uint32_t>(descriptors.size()), descriptors.data());

} //DescriptorPool::freeDescriptors()

void DescriptorPool::resetPool()
{
	vkResetDescriptorPool(m_device.getDevice(), m_descriptorPool, 0);

} //DescriptorPool::resetPool()

DescriptorWriter::DescriptorWriter(DescriptorSetLayout& setLayout, DescriptorPool& pool)
	: m_setLayout(setLayout), m_pool(pool)
{
} //DescriptorWriter::DescriptorWriter()

DescriptorWriter& DescriptorWriter::writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo)
{
	auto& bindingDescription = m_setLayout.m_bindings[binding];

	VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.descriptorCount = 1;
		write.pBufferInfo = bufferInfo;

	m_writers.push_back(write);

	return *this;

} //DescriptorWriter::writeBuffer()

DescriptorWriter& DescriptorWriter::writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo)
{
	auto& bindingDescription = m_setLayout.m_bindings[binding];

	VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.descriptorCount = 1;
		write.pImageInfo = imageInfo;

	m_writers.push_back(write);

	return *this;

} //DescriptorWriter::writeImage()

bool DescriptorWriter::build(VkDescriptorSet& set)
{
	bool succses = m_pool.allocateDescriptor(m_setLayout.getDescriptorSetLayout(), set);

	if (!succses)
		return false;

	overwrite(set);

	return true;

}

void DescriptorWriter::overwrite(VkDescriptorSet& set)
{
	for (auto& write : m_writers)
		write.dstSet = set;

	vkUpdateDescriptorSets(m_pool.m_device.getDevice(), static_cast<uint32_t>(m_writers.size()), m_writers.data(), 0, nullptr);
}
