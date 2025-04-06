#include <VulkanLite.hpp>
#include <iostream>
#include <vector>
#include <algorithm>

struct GameState {
    float paddleX = 0.0f;
    float ballX = 0.0f, ballY = -0.45f;
    float ballVelX = 0.005f, ballVelY = 0.05f;
    int score = 0;
};

int main() {
    VulkanLite::VulkanContext context("Pong", 800, 600);
    VulkanLite::Renderer* renderer = context.createRenderer();

    std::vector<VulkanLite::Vertex> paddleVertices = {
        {{0.0f, -0.55f}}, {{0.2f, -0.55f}}, {{0.2f, -0.45f}}, {{0.0f, -0.45f}}
    };
    std::vector<VulkanLite::Vertex> ballVertices = {
        {{0.0f, -0.5f}}, {{0.05f, -0.4f}}, {{-0.05f, -0.4f}}
    };

    const auto ballHeight = 0.35f;

    VulkanLite::Buffer paddleBuffer(context.getDevice(), context.getPhysicalDevice(), context.getCommandPool(),
                                    context.getGraphicsQueue(), paddleVertices.data(), sizeof(VulkanLite::Vertex) * paddleVertices.size(),
                                    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    VulkanLite::Buffer ballBuffer(context.getDevice(), context.getPhysicalDevice(), context.getCommandPool(),
                                  context.getGraphicsQueue(), ballVertices.data(), sizeof(VulkanLite::Vertex) * ballVertices.size(),
                                  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

    std::vector<VulkanLite::Vertex> updatedPaddle = paddleVertices;
    std::vector<VulkanLite::Vertex> updatedBall = ballVertices;
    VulkanLite::Buffer stagingPaddle(context.getDevice(), context.getPhysicalDevice(), context.getCommandPool(),
                                     context.getGraphicsQueue(), updatedPaddle.data(), sizeof(VulkanLite::Vertex) * updatedPaddle.size(),
                                     VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    VulkanLite::Buffer stagingBall(context.getDevice(), context.getPhysicalDevice(), context.getCommandPool(),
                                   context.getGraphicsQueue(), updatedBall.data(), sizeof(VulkanLite::Vertex) * updatedBall.size(),
                                   VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

    GameState state;

    context.run([&](VkCommandBuffer cmdBuf) {
        // Update game state
        double mouseX, mouseY;
        glfwGetCursorPos(context.getWindow(), &mouseX, &mouseY);
        state.paddleX = (float)(mouseX / 800.0f) * 2.0f - 1.0f;
        state.paddleX = std::max(-0.99f, std::min(0.99f, state.paddleX));

        state.ballX += state.ballVelX;
        state.ballY += state.ballVelY;
        if (state.ballX < -0.95f || state.ballX > 0.95f) state.ballVelX = -state.ballVelX;
        if (state.ballY > (1.0f + ballHeight)) {
            state.ballVelY = -state.ballVelY;
            std::cout << "Top bounce at ballY: " << state.ballY << std::endl;
        }
        if (state.ballY < -1.05f - ballHeight) {
            state.ballX = 0.0f;
            state.ballY = -0.9f;
            state.ballVelY = -state.ballVelY;
            std::cout << "Reset ball" << std::endl;
        }

        // Paddle collision
        float paddleLeft = state.paddleX;
        float paddleRight = state.paddleX + 0.2f;
        float paddleTop = -0.45f;
        float paddleBottom = -0.55f;
        float ballLeft = state.ballX - 0.05f;
        float ballRight = state.ballX + 0.05f;
        float ballTop = state.ballY;
        float ballBottom = state.ballY - 0.1f;

        std::cout << "Ball: (" << state.ballX << ", " << state.ballY << "), VelY: " << state.ballVelY 
                  << ", Paddle: (" << paddleLeft << " to " << paddleRight << ")" << std::endl;

        if (state.ballVelY < 0) {
            std::cout << "Falling: ballBottom=" << ballBottom << ", ballTop=" << ballTop 
                      << ", paddleTop=" << paddleTop << ", paddleBottom=" << paddleBottom << std::endl;

            if (ballRight > paddleLeft && ballLeft < paddleRight &&
                ballBottom <= (paddleBottom+ballHeight)) {
                std::cout << "Collision detected!" << std::endl;
                state.ballVelY = -state.ballVelY;
                // state.ballY = paddleTop - 0.001f;
                state.score++;
                std::cout << "Score: " << state.score << std::endl;
            }
        }

        // Update vertex data
        for (auto& v : updatedPaddle) v.pos[0] = paddleVertices[&v - updatedPaddle.data()].pos[0] + state.paddleX;
        for (auto& v : updatedBall) {
            v.pos[0] = ballVertices[&v - updatedBall.data()].pos[0] + state.ballX;
            v.pos[1] = ballVertices[&v - updatedBall.data()].pos[1] + state.ballY;
        }

        void* data;
        // Allocate memory to write to
        vkMapMemory(
            context.getDevice(), 
            stagingPaddle.memory, 
            0, 
            sizeof(VulkanLite::Vertex) * updatedPaddle.size(), 
            0, 
            &data);
        // Write to the memory
        // The staging buffer (stagingPaddle) is CPU-writable. 
        // Mapping lets us directly update its contents with the new 
        // paddle vertex positions computed earlier.
        memcpy(data, updatedPaddle.data(), sizeof(VulkanLite::Vertex) * updatedPaddle.size());
        vkUnmapMemory(context.getDevice(), stagingPaddle.memory);

        vkMapMemory(
            context.getDevice(), 
            stagingBall.memory, 
            0, 
            sizeof(VulkanLite::Vertex) * updatedBall.size(), 
            0, 
            &data);
        memcpy(data, updatedBall.data(), sizeof(VulkanLite::Vertex) * updatedBall.size());
        vkUnmapMemory(context.getDevice(), stagingBall.memory);

        VkBufferCopy paddleCopy{};
        paddleCopy.size = sizeof(VulkanLite::Vertex) * updatedPaddle.size();

        // The device buffer (paddleBuffer) has VK_BUFFER_USAGE_VERTEX_BUFFER_BIT 
        // and is optimized for GPU access, but not CPU writes.
        // This command stages the transfer, executed when the command buffer is submitted, 
        // updating the GPU’s vertex data for rendering.
        vkCmdCopyBuffer(cmdBuf, stagingPaddle.getBuffer(), paddleBuffer.getBuffer(), 1, &paddleCopy);

        VkBufferCopy ballCopy{};
        ballCopy.size = sizeof(VulkanLite::Vertex) * updatedBall.size();
        vkCmdCopyBuffer(cmdBuf, stagingBall.getBuffer(), ballBuffer.getBuffer(), 1, &ballCopy);

        uint32_t imageIndex = context.getCurrentImageIndex();
        renderer->beginRenderPass(
            cmdBuf, 
            context.getRenderPass(), 
            context.getSwapChainFrameBuffers()[imageIndex], 
            context.getSwapExtent());
        renderer->bindPipeline(cmdBuf);
        renderer->draw(cmdBuf, paddleBuffer, 4);
        renderer->draw(cmdBuf, ballBuffer, 3);
        renderer->endRenderPass(cmdBuf);
    });

    delete renderer;
    return 0;
}