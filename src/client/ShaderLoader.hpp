#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <fstream>

struct ShaderModules {
    VkShaderModule vertModule;
    VkShaderModule fragModule;
};

class ShaderLoader {
public:
    ShaderLoader(VkDevice device) : device(device) {}

    ShaderModules loadGraphicsShaders(const std::string& vertPath, const std::string& fragPath) {
        auto vertCode = readFile(vertPath);
        auto fragCode = readFile(fragPath);

        ShaderModules shaders;
        shaders.vertModule = createShaderModule(vertCode);
        shaders.fragModule = createShaderModule(fragCode);

        return shaders;
    }

    void destroy(ShaderModules& shaders) {
        vkDestroyShaderModule(device, shaders.vertModule, nullptr);
        vkDestroyShaderModule(device, shaders.fragModule, nullptr);
    }

private:
    VkDevice device;

    std::vector<char> readFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();
        return buffer;
    }

    VkShaderModule createShaderModule(const std::vector<char>& code) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
        }

        return shaderModule;
    }
};