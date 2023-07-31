#include "render_sub_pass_base.hpp"

namespace mud
{
    RenderSubpassBase::RenderSubpassBase(const std::vector<ShaderModule *> & shaderModules)
    {
        for (ShaderModule * shaderModule : shaderModules)
        {
            m_shaderModules[shaderModule->getType()] = shaderModule;
            auto & shaderModulePushConstantBlocksMap = m_shaderModulesPushConstantBlocks[shaderModule->getType()];

            for (const auto & pair : shaderModule->getPushConstantBlocksDetails())
            {
                ShaderModulePushConstantBlock * newBlock = new ShaderModulePushConstantBlock(pair.second, shaderModule->getType());
                shaderModulePushConstantBlocksMap[pair.first] = newBlock;
            }
        }
    }

    RenderSubpassBase::~RenderSubpassBase()
    {
        for (auto blocks : m_shaderModulesPushConstantBlocks)
            for (auto block : blocks.second)
                delete block.second;
    }
    
    const ShaderModule * RenderSubpassBase::getShaderModule(ShaderType type) const
    {
        auto iter = m_shaderModules.find(type);
        if (iter != m_shaderModules.end())
            return iter->second;
        return nullptr;
    }
    
    const ShaderModulePushConstantBlock * RenderSubpassBase::getPushConstantBlock(ShaderType shaderType, std::string id) const
    {
        auto shaderModulesPushConstantBlocksIter = m_shaderModulesPushConstantBlocks.find(shaderType);
        if (shaderModulesPushConstantBlocksIter == m_shaderModulesPushConstantBlocks.end())
            return nullptr;
        auto blocksIter = shaderModulesPushConstantBlocksIter->second.find(id);
        if (blocksIter == shaderModulesPushConstantBlocksIter->second.end())
            return nullptr;
        return blocksIter->second;
    }
    
    ShaderModulePushConstantBlock * RenderSubpassBase::getPushConstantBlock(ShaderType shaderType, std::string id)
    {
        auto shaderModulesPushConstantBlocksIter = m_shaderModulesPushConstantBlocks.find(shaderType);
        if (shaderModulesPushConstantBlocksIter == m_shaderModulesPushConstantBlocks.end())
            return nullptr;
        auto blocksIter = shaderModulesPushConstantBlocksIter->second.find(id);
        if (blocksIter == shaderModulesPushConstantBlocksIter->second.end())
            return nullptr;
        return blocksIter->second;
    }
}