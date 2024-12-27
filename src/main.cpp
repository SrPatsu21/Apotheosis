#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include "VulkanApp.hpp"

// Forward declaration for ImGui setup
void SetupImGui(GLFWwindow* window);

int main() {
    VulkanApp app;

    if (!app.Initialize()) {
        return EXIT_FAILURE;
    }

    GLFWwindow* window = app.GetWindow();

    SetupImGui(window);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        app.RenderFrame();
    }

    app.Cleanup();
    return EXIT_SUCCESS;
}

// Placeholder for ImGui setup (implement this as needed)
void SetupImGui(GLFWwindow* window) {
    // TODO: Integrate ImGui Vulkan backend setup
}
