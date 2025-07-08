#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

void initUI(GLFWwindow* window, VkInstance instance, VkDevice device, VkPhysicalDevice gpu, VkQueue queue, uint32_t queueFamily, VkRenderPass renderPass) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplGlfw_InitForVulkan(window, true);

    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = instance;
    init_info.PhysicalDevice = gpu;
    init_info.Device = device;
    init_info.Queue = queue;
    init_info.QueueFamily = queueFamily;
    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.DescriptorPool = /* your Vulkan descriptor pool */;
    ImGui_ImplVulkan_Init(&init_info, renderPass);

    // Upload fonts
    VkCommandBuffer cmd = /* begin single-time command buffer */;
    ImGui_ImplVulkan_CreateFontsTexture(cmd);
    /* submit and wait */;
    ImGui_ImplVulkan_DestroyFontUploadObjects();
}