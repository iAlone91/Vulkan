#include "CModel.h"
#include "OBJLoader.h"
#include "utils.h"
        
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <stdexcept>
#include <unordered_map>
#include <string>


namespace std
{
    template<>
    struct hash<Model::Vertex>
    {
        size_t operator()(Model::Vertex const& vertex) const
        {
            size_t seed = 0;
            hashCombine(seed, vertex.pos, vertex.color, vertex.normal, vertex.texCoord);
            return seed;
        }
    };
    
}

Model::Model(Device& device, const Model::Builder& builder)
    : m_device(device)
{
    createVertexBuffer(builder.vertices);
    createIndexBuffer(builder.indices);
} //Model()

Model::~Model()
{
} //~Model()

void Model::createVertexBuffer(const std::vector<Vertex>& vertices)
{
    m_vertexCount = static_cast<uint32_t>(vertices.size());

    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    uint32_t vertexSize = sizeof(vertices[0]);

    Buffer stagingBuffer{ m_device, vertexSize, m_vertexCount , VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };
    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void*)vertices.data());

    m_vertexBuffer = std::make_unique<Buffer>( m_device, vertexSize, m_vertexCount , VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

    m_device.copyBuffer(stagingBuffer.getBuffer(), m_vertexBuffer->getBuffer() , bufferSize);

} //createVertexBuffers()

void Model::createIndexBuffer(const std::vector<uint32_t>& indices)
{
    m_indexCount = static_cast<uint32_t>(indices.size());

    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
    uint32_t indexSize = sizeof(indices[0]);

    Buffer stagingBuffer{ m_device, indexSize, m_indexCount , VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };
    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void*)indices.data());

    m_indexBuffer = std::make_unique<Buffer>(m_device, indexSize, m_indexCount, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);


    m_device.copyBuffer(stagingBuffer.getBuffer(), m_indexBuffer->getBuffer(), bufferSize);


} //createIndexBuffer()

std::unique_ptr<Model> Model::createModelFromFile(Device& device, const std::string& modelpath)
{
    Builder builder{};
    builder.loadModel(modelpath);

    return std::make_unique<Model>(device, builder);
} //createModelFromFile()

void Model::bind(VkCommandBuffer commandBuffer)
{
    VkBuffer buffers[] = { m_vertexBuffer->getBuffer() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
    
} //bind()

void Model::draw(VkCommandBuffer commandBuffer)
{
    vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 0);

} //draw()

std::vector<VkVertexInputBindingDescription> Model::Vertex::getBindingDescriptions()
{
    std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescriptions;

} //getBindingDescriptions()

std::vector<VkVertexInputAttributeDescription> Model::Vertex::getAttributeDescriptions()
{
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions(4);
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, normal);

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[3].offset = offsetof(Vertex, texCoord);

    return attributeDescriptions;

} //getAttributeDescriptions()

void Model::Builder::loadModel(const std::string& modelpath)
{
    OBJLoader::attrib attrib;
    std::vector<OBJLoader::model> models;
    
    OBJLoader::loadOBJ(attrib, models , modelpath);

    vertices.clear();
    indices.clear();

    std::unordered_map<Vertex, uint32_t> uniqueVertices;
    for (const auto& model : models)
    {
        for (const auto& index : model.mesh.indices)
        {
            Vertex vertex{};

            if (index.vertexIndex >= 0)
            {
                vertex.pos =
                {
                    attrib.vertices[3 * index.vertexIndex + 0],
                    attrib.vertices[3 * index.vertexIndex + 1],
                    attrib.vertices[3 * index.vertexIndex + 2],
                };
            }

            if (index.normalIndex >= 0)
            {
                vertex.normal =
                {
                    attrib.normals[3 * index.normalIndex + 0],
                    attrib.normals[3 * index.normalIndex + 1],
                    attrib.normals[3 * index.normalIndex + 2],
        
                };
            }

            if (index.texcoordIndex >= 0)
            {
                vertex.texCoord =
                {
                    attrib.texcoords[2 * index.texcoordIndex + 0],
                    attrib.texcoords[2 * index.texcoordIndex + 1],
                };
            }

            vertex.color = { 0.5f, 0.0f, 0.1f };

            if (uniqueVertices.count(vertex) == 0)
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }

            indices.push_back(uniqueVertices[vertex]);
        }
    }

}