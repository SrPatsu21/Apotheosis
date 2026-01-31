// UI.cpp
#include "UI.hpp"
#include "../CoreVulkan.hpp"

UI::UI():
    window(nullptr),
    device(VK_NULL_HANDLE),
    descriptorPool(VK_NULL_HANDLE)
{};

UI::~UI() {};

void UI::initContext() {
    // 1. Create ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    // 2. Init GLFW
    ImGui_ImplGlfw_InitForVulkan(window, true);

    // 3. Descriptor pool for ImGui
    VkDescriptorPoolSize pool_sizes[] = {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };
    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
    pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;
    vkCreateDescriptorPool(device, &pool_info, nullptr, &this->descriptorPool);
}

void UI::initSwapchainResources(
    VkInstance instance,
    VkPhysicalDevice physicalDevice,
    QueueFamilyIndices graphicsQueueFamilyIndices,
    VkQueue GraphicsQueue,
    VkRenderPass renderPass,
    uint32_t imageCount,
    VkSampleCountFlagBits msaaSamples
) {
    // 4. Init Vulkan backend
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = instance;
    init_info.PhysicalDevice = physicalDevice;
    init_info.Device = device;
    init_info.QueueFamily = graphicsQueueFamilyIndices.graphicsFamily.value();
    init_info.Queue = GraphicsQueue;
    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.DescriptorPool = this->descriptorPool;
    init_info.RenderPass = renderPass;
    init_info.Subpass = 0;
    init_info.MinImageCount = imageCount;
    init_info.ImageCount = imageCount;
    init_info.MSAASamples = msaaSamples;
    init_info.Allocator = nullptr;
    ImGui_ImplVulkan_Init(&init_info);

    // fonts
    ImGui::GetIO().Fonts->AddFontDefault();
}

void UI::init(
    GLFWwindow* window,
    VkInstance instance,
    VkPhysicalDevice physicalDevice,
    VkDevice device,
    QueueFamilyIndices graphicsQueueFamilyIndices,
    VkQueue GraphicsQueue,
    VkRenderPass renderPass,
    uint32_t imageCount,
    VkSampleCountFlagBits msaaSamples
)
{
    this->window = window;
    this->device = device;

    initContext();
    initSwapchainResources(
        instance,
        physicalDevice,
        graphicsQueueFamilyIndices,
        GraphicsQueue,
        renderPass,
        imageCount,
        msaaSamples
    );
}

void UI::newFrame() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UI::build() {
    // Example window
    ImGui::Begin("Demo Window");
    ImGui::Text("Hello from ImGui inside Vulkan!");
    ImGui::End();
}

void UI::render(VkCommandBuffer cmd) {
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
}

void UI::cleanup() {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    if (descriptorPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(device, this->descriptorPool, nullptr);
        this->descriptorPool = VK_NULL_HANDLE;
    }
}