#ifndef RENDER_SUB_PASS_BASE_HPP
#define RENDER_SUB_PASS_BASE_HPP

#include <unordered_map>
#include <vector>

#include "graphics/shader_module.hpp"

namespace mud
{
    class RenderSubpassBase
    {
    public:

        RenderSubpassBase(const std::vector<ShaderModule *> & shaderModules);

        ~RenderSubpassBase();

        const ShaderModule * getShaderModule(ShaderType type) const;

        const ShaderModulePushConstantBlock * getPushConstantBlock(ShaderType shaderType, std::string id) const;

        ShaderModulePushConstantBlock * getPushConstantBlock(ShaderType shaderType, std::string id);

    protected:

        std::unordered_map<ShaderType, ShaderModule *> m_shaderModules;
        std::unordered_map<ShaderType, std::unordered_map<std::string, ShaderModulePushConstantBlock *>> m_shaderModulesPushConstantBlocks;
    };
}

#endif