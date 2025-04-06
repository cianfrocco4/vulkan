#ifndef TCVULKANTRIANGLEAPP_HPP
#define TCVULKANTRIANGLEAPP_HPP

#define GLFW_INCLUDE_VULKAN

#include <vector>
#include <stdexcept>
#include <cstring>
#include <string>

#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

class tcVulkanTriangleApp 
{
public:
    void 
    run();

private:
    
    struct tsQueueFamilyIndices {
        uint32_t graphicsFamily = UINT32_MAX;
        uint32_t presentFamily = UINT32_MAX;
        bool isComplete() { return graphicsFamily != UINT32_MAX && presentFamily != UINT32_MAX; }
    };

    void
    initWindow();

    void
    initVulkan();

    void
    createInstance();

    void
    createSurface();

    void
    pickPhysicalDevice();

    tsQueueFamilyIndices 
    findQueueFamilies(VkPhysicalDevice device);

    void
    createLogicalDevice();

    void
    createSwapChain();

    void
    createImageViews();

    void
    createRenderPass();

    VkShaderModule 
    createShaderModule(const std::vector<char>& code);

    std::vector<char> 
    readFile(const std::string& filename);

    void
    createGraphicsPipeline();

    void
    createFramebuffers();

    void
    createCommandPool();

    void
    createCommandBuffer();

    void
    createSyncObjects();

    void
    mainLoop();

    void
    drawFrame();

    void
    cleanup();

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
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;
};

#endif