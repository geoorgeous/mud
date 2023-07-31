#include "render_pass_base.hpp"

namespace mud
{
    RenderPassOptions::RenderPassOptions()
        : clearColorBuffer(true),
        clearDepthBuffer(true),
        wireframe(false),
        faceCullMode(FaceCullMode::Back)
    { }

    RenderPassBase::RenderPassBase(const std::vector<std::vector<ShaderModule *>> & subpassesShaderModules, const RenderPassOptions & options, const std::vector<FrameBufferAttachmentInfo> & frameBufferAttachmentsInfo)
        : m_options(options)
    {
        for (size_t idx = 0; idx < subpassesShaderModules.size(); ++idx)
            m_subpasses.push_back(new RenderSubpass(subpassesShaderModules[idx]));
    }

    RenderPassBase::~RenderPassBase()
    {
        for (RenderSubpass * subpass : m_subpasses)
            delete subpass;
    }

    const std::vector<RenderSubpass *> & RenderPassBase::getSubpasses() const
    {
        return m_subpasses;
    }

    const std::vector<RenderSubpass *> & RenderPassBase::getSubpasses()
    {
        return m_subpasses;
    }
}