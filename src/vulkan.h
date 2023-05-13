#pragma once

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>

#include <ft2build.h>
#include FT_FREETYPE_H

//#define TINYOBJLOADER_IMPLEMENTATION
//#include <tiny_obj_loader.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <config.h>

const int MAX_FRAMES_IN_FLIGHT = 2;

const std::string shaders = "shaders/";

const bool enableValidationLayers = true;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct Vertex {
    glm::vec3 pos;
    glm::vec2 texCoord;
    glm::vec3 normal;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(3);

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, texCoord);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, normal);

        return attributeDescriptions;
    }

    bool operator==(const Vertex& other) const {
        return pos == other.pos && normal == other.normal && texCoord == other.texCoord;
    }
};

class VkMesh {
public:
    VkBuffer vertexBuffer;
    VmaAllocation vertexBufferMemory;
    VkBuffer indexBuffer;
    VmaAllocation indexBufferMemory;
    uint32_t indices;
    VkMesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
    ~VkMesh();
    void createVertexBuffer(std::vector<Vertex>& vertices);
    void createIndexBuffer(std::vector<uint32_t>& indices);
    void cleanMesh();
};

VkMesh* quad;

class VkUBO {
public:
    glm::vec2 pos;
    glm::vec2 size;
    float rot;
    float depth;
    glm::vec2 cameraPos;
    glm::vec4 color;
    VkUBO();
};

class VkMVP {
public:
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
    
    glm::vec2 pos;
    glm::vec2 size;
    float rot;
    float depth;
    glm::vec2 cameraPos;
    glm::vec4 color;

    VkMVP();
};

class VkTexture {
public:
    VkTexture() { }
    VkTexture(std::string filename);
    VkTexture(unsigned char* pixels, int width, int height, int channels);
    ~VkTexture();
    VkImage textureImage;
    VmaAllocation textureImageMemory;
    void loadImage(stbi_uc* pixels, int texWidth, int texHeight, int texChannels);
    void createTextureImageView(VkFormat format);
    void cleanup();
    VkImageView textureImageView;

};

class VkRender {
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VmaAllocation> uniformBuffersMemory;
    VkSampler* sampler;
    VkDescriptorPool descriptorPool;
public:
    VkUBO transform;
    VkTexture* tex;
    VkMesh* mesh = nullptr;
    std::vector<VkDescriptorSet> descriptorSets;

    VkRender(VkTexture* tex);
    VkRender(VkTexture* tex, VkSampler* sampler);
    ~VkRender();

    void cleanupSwapChain();
    void cleanup();
    void recreateSwapChain();
    void createUniformBuffers();
    void updateUniformBuffer(int imageIndex);
    void createDescriptorPool();
    void createDescriptorSets(VkSampler* sampler);
};

class GLTF {
public:
	std::vector<VkMesh*> meshes;
	std::vector<VkTexture*> textures;
    std::vector<int> meshMatIndices;
	
	void readGLTF(std::string filename);
	void readGLTFNode(tinygltf::Model &model, tinygltf::Node &node);
	void bindMesh(tinygltf::Model &model, tinygltf::Mesh &mesh);
    ~GLTF();
};

class VkRender3d {
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VmaAllocation> uniformBuffersMemory;
    VkDescriptorPool descriptorPool;
public:
    VkMVP transform;
    VkTexture* tex;
    VkMesh* mesh;
    std::vector<VkDescriptorSet> descriptorSets;

    VkRender3d(GLTF * gltf, int meshIndex);
    ~VkRender3d();

    void cleanupSwapChain();
    void cleanup();
    void recreateSwapChain();
    void createUniformBuffers();
    void updateUniformBuffer(int imageIndex);
    void createDescriptorPool();
    void createDescriptorSets();
};

GLFWwindow* window;

VkInstance instance;
VkDebugUtilsMessengerEXT debugMessenger;
VkSurfaceKHR surface;

VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
VkDevice device;

VmaAllocator alloc;

VkQueue graphicsQueue;
VkQueue presentQueue;

VkSwapchainKHR swapChain;
std::vector<VkImage> swapChainImages;
VkFormat swapChainImageFormat;
VkExtent2D swapChainExtent;
std::vector<VkImageView> swapChainImageViews;
std::vector<VkFramebuffer> swapChainFramebuffers;

VkDescriptorSetLayout descriptorSetLayout;
VkDescriptorPool descriptorPool;

VkRenderPass renderPass;

VkShaderModule vertexShader;
VkShaderModule fragmentShader;
VkPipelineLayout pipelineLayout;
VkPipeline graphicsPipeline;

VkShaderModule vertexShader3d;
VkShaderModule fragmentShader3d;
VkPipelineLayout pipelineLayout3d;
VkPipeline pipeline3d;

VkCommandPool commandPool;
std::vector<VkCommandBuffer> commandBuffers;

VkImage depthImage;
VkDeviceMemory depthImageMemory;
VkImageView depthImageView;

VkSampler textureSampler;
VkSampler unfiltered;

std::vector<VkSemaphore> imageAvailableSemaphores;
std::vector<VkSemaphore> renderFinishedSemaphores;
std::vector<VkFence> inFlightFences;
uint32_t currentFrame = 0;

bool framebufferResized = false;

std::vector<VkTexture*> textures;
std::vector<VkMesh*> meshes;
std::vector<VkRender*> renders;
std::vector<VkRender3d*> render3ds;

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
void setupDebugMessenger();

void createInstance();
void createSurface();
void pickPhysicalDevice();
void createLogicalDevice();
void createAllocator();
void createSwapChain();
void createImageViews();
void createDescriptorSetLayout();
void createRenderPass(VkRenderPass* rPass, VkFormat color, VkFormat depth);
void createShader(VkShaderModule& mod, std::string path);
void createGraphicsPipeline(VkPipelineLayout* layout, VkPipeline* pipeline, 
    VkShaderModule& vertMod, VkShaderModule& fragMod,VkRenderPass& rPass,VkExtent2D extent, bool is3d);
void createFramebuffers(VkFramebuffer* framebuffer, std::vector<VkImageView> attachments,
    VkRenderPass& renderPass, int width, int height);
void createCommandPool();
void createDepthResources();
void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
    bool isMappable, VkBuffer& buffer, VmaAllocation& bufferMemory);
void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
    VkBuffer& buffer, VkDeviceMemory& bufferMemory);
void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
void createCommandBuffers(std::vector<VkCommandBuffer>& commandBuffers, size_t size);
void createSyncObjects();
void createImage(uint32_t width, uint32_t height, VkFormat format,
    VkImageTiling tiling, VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
void createImage(uint32_t width, uint32_t height, VkFormat format,
    VkImageTiling tiling, VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties, VkImage& image, VmaAllocation& imageMemory);
VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout,
    VkImageLayout newLayout);
void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
void createTextureSampler(VkSampler* textureSampler, VkFilter filter);
void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
void drawFrame();

VkShaderModule createShaderModule(const std::vector<char>& code);

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
bool isDeviceSuitable(VkPhysicalDevice device);
bool checkDeviceExtensionSupport(VkPhysicalDevice device);
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling,
    VkFormatFeatureFlags features);
VkFormat findDepthFormat();
bool hasStencilComponent(VkFormat format);
std::vector<const char*> getRequiredExtensions();

bool checkValidationLayerSupport();
static std::vector<char> readFile(const std::string& filename);
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

VkCommandBuffer beginSingleTimeCommands();
void endSingleTimeCommands(VkCommandBuffer commandBuffer);

void cleanupSwapChain();
void recreateSwapChain();
static void framebufferResizeCallback(GLFWwindow* window, int width, int height);