#include "CBuffer.h"

// Std ///////////////////////////////////////////////////////////////
#include <cstring>

VkDeviceSize Buffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment)
{
	if (minOffsetAlignment > 0)
	{
		return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
	}
	return instanceSize;
}

Buffer::Buffer(Device& device, VkDeviceSize instanceSize, uint32_t instanceCount, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize minOffsetAlignment)
	: m_device(device), m_instanceSize(instanceSize), m_instanceCount(instanceCount), m_usageFlags(usageFlags), m_memoryPropertyFlags(memoryPropertyFlags)
{
	m_alignmentSize = getAlignment(m_instanceSize, minOffsetAlignment);
	m_bufferSize = m_alignmentSize * m_instanceCount;
	m_device.createBuffer(m_bufferSize, m_usageFlags, m_memoryPropertyFlags, m_buffer, m_memory);

} //Buffer()

Buffer::~Buffer()
{
	unmap();
	vkDestroyBuffer(m_device.getDevice(), m_buffer, nullptr);
	vkFreeMemory(m_device.getDevice(), m_memory, nullptr);

} //~Buffer()

VkResult Buffer::map(VkDeviceSize size, VkDeviceSize offset)
{
	return vkMapMemory(m_device.getDevice(), m_memory, offset, size, 0, &m_mapped);

} //map()

void ::Buffer::unmap()
{
	if (m_mapped)
	{
		vkUnmapMemory(m_device.getDevice(), m_memory);
		m_mapped = nullptr;
	}

} //unmap()

void Buffer::writeToBuffer(void* data, VkDeviceSize size, VkDeviceSize offset)
{
	if (size == VK_WHOLE_SIZE)
	{
		memcpy(m_mapped, data, m_bufferSize);
	}
	else
	{
		char* memOffset = static_cast<char*>(m_mapped);
		memOffset += offset;
		memcpy(memOffset, data, size);
	}

} //writeToBuffer()

VkResult Buffer::flush(VkDeviceSize size, VkDeviceSize offset)
{
	VkMappedMemoryRange mappedRange{};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = m_memory;
		mappedRange.offset = offset;
		mappedRange.size = size;

	return vkFlushMappedMemoryRanges(m_device.getDevice(), 1, &mappedRange);

} //flush()

VkResult Buffer::invalidate(VkDeviceSize size, VkDeviceSize offset)
{
	VkMappedMemoryRange mappedRange{};
	mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedRange.memory = m_memory;
	mappedRange.offset = offset;
	mappedRange.size = size;
	return vkInvalidateMappedMemoryRanges(m_device.getDevice(), 1, &mappedRange);

} //invalidate()

VkDescriptorBufferInfo Buffer::descriptorInfoForBuffer(VkDeviceSize size, VkDeviceSize offset)
{
	return VkDescriptorBufferInfo{ m_buffer, offset, size };

} //descriptorBufferInfo()

void Buffer::writeToIndex(void* data, int index)
{
	writeToBuffer(data, m_instanceSize, index * m_alignmentSize);

} //writeToIndex()

VkResult Buffer::flushIndex(int index)
{
	return flush(m_alignmentSize, index * m_alignmentSize);

} //flushIndex()

VkResult Buffer::invalidateIndex(int index)
{
	return invalidate(m_alignmentSize, index * m_alignmentSize);

} //invalidateIndex()

VkDescriptorBufferInfo Buffer::descriptorInfoForIndex(int index)
{
	return descriptorInfoForBuffer(m_alignmentSize, index * m_alignmentSize);

} //descriptorInfoForIndex()



