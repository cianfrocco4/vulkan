#ifndef VULKANLITE_HPP
#define VULKANLITE_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include <stdexcept>
#include <functional>
#include <chrono>

namespace VulkanLite {

struct Vertex {
    float pos[2];
    static VkVertexInputBindingDescription getBindingDescription();
    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
};

class Buffer {
public:
    Buffer(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue,
           const void* data, VkDeviceSize size, VkBufferUsageFlags usage);
    ~Buffer();
    VkBuffer getBuffer() const { return buffer; }

    VkDeviceMemory memory;

private:
    VkDevice device;
    VkBuffer buffer;
};

class Shader {
public:
    Shader(VkDevice device, const std::string& filename);
    ~Shader();
    VkShaderModule getModule() const { return module; }

private:
    VkDevice device;
    VkShaderModule module;
};

class Renderer {
public:
    Renderer(VkDevice device, VkRenderPass renderPass, VkExtent2D extent);
    ~Renderer();
    void beginRenderPass(VkCommandBuffer cmdBuf, VkRenderPass renderPass, VkFramebuffer framebuffer, VkExtent2D extent);
    void endRenderPass(VkCommandBuffer cmdBuf);
    void bindPipeline(VkCommandBuffer cmdBuf);
    void draw(VkCommandBuffer cmdBuf, const Buffer& vertexBuffer, uint32_t vertexCount);

private:
    VkDevice device;
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;
};

class VulkanContext {
public:
    VulkanContext(const std::string& appName, uint32_t width, uint32_t height);
    ~VulkanContext();
    void run(std::function<void(VkCommandBuffer)> renderCallback);

    GLFWwindow* getWindow() const { return window; }
    VkDevice getDevice() const { return device; }
    VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }
    VkCommandPool getCommandPool() const { return commandPool; }
    VkQueue getGraphicsQueue() const { return graphicsQueue; }
    VkRenderPass getRenderPass() const { return renderPass; }
    VkExtent2D getSwapExtent() const { return swapChainExtent; }
    uint32_t getCurrentImageIndex() const { return currentImageIndex; } // New getter
    Renderer* createRenderer();
    std::vector<VkFramebuffer>& getSwapChainFrameBuffers();

private:
    struct tsQueueFamilyIndices {
        uint32_t graphicsFamily = UINT32_MAX;
        uint32_t presentFamily = UINT32_MAX;
        bool isComplete() { return graphicsFamily != UINT32_MAX && presentFamily != UINT32_MAX; }
    };

    tsQueueFamilyIndices 
    findQueueFamilies(VkPhysicalDevice device);

    GLFWwindow* window;
    VkInstance instance;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphicsQueue;
    VkSurfaceKHR surface;
    VkQueue presentQueue;
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    VkRenderPass renderPass;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;
    uint32_t currentImageIndex = 0; // New member to track swapchain image index

    void initWindow(const std::string& appName, uint32_t width, uint32_t height);
    void initVulkan();
    void cleanup();
};

inline std::vector<VkFramebuffer>&
VulkanContext::getSwapChainFrameBuffers()
{
    return swapChainFramebuffers;
}

} // namespace VulkanLite

#endif // VULKANLITE_HPP
