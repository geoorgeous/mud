#ifndef RENDER_PASS_BASE_HPP
#define RENDER_PASS_BASE_HPP

#include <unordered_map>
#include <vector>

#include "graphics/render_sub_pass.hpp"
#include "graphics/color.hpp"
#include "graphics/image_format.hpp"

namespace mud
{
    enum class FaceCullMode
    {
        None,
        Front,
        Back,
        FrontAndBack
    };

    enum class FramebufferAttachmentUsage
    {
        Color,
        DepthStencil
    };

    struct RenderPassOptions
    {
        RenderPassOptions();

        bool clearColorBuffer;
        bool clearDepthBuffer;
        bool wireframe;
        FaceCullMode faceCullMode;
    };

    struct FrameBufferAttachmentInfo {
		ImageFormat imageFormat;
	};

    class RenderPassBase
    {
    public:

        RenderPassBase(const std::vector<std::vector<ShaderModule *>> & subpassesShaderModules, const RenderPassOptions & options, const std::vector<FrameBufferAttachmentInfo> & frameBufferAttachmentsInfo);

        virtual ~RenderPassBase();

        const std::vector<RenderSubpass *> & getSubpasses() const;

        const std::vector<RenderSubpass *> & getSubpasses();

    protected:

        std::vector<RenderSubpass *> m_subpasses;
        RenderPassOptions m_options;
    };
}

#endif