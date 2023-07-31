#include "text_renderer_base.hpp"

#include <unordered_map>
#include <vector>

#include "utils/file_io.hpp"

namespace mud
{
	TextRendererBase::TextRendererBase(RenderPassOptions renderPassOptions)
	{
		m_shaderModules.push_back(new ShaderModule(ShaderType::Vertex));
		m_shaderModules.back()->setSource(file::readText("./../mud/graphics/shaders/text.vert"));
		m_shaderModules.back()->compileSource();
		
		m_shaderModules.push_back(new ShaderModule(ShaderType::Fragment));
		m_shaderModules.back()->setSource(file::readText("./../mud/graphics/shaders/text.frag"));
		m_shaderModules.back()->compileSource();

		std::vector<FrameBufferAttachmentInfo> frameBufferAttachmentsInfo;
		frameBufferAttachmentsInfo.push_back(FrameBufferAttachmentInfo{});
		frameBufferAttachmentsInfo.back().imageFormat = ImageFormat::B8G8R8A8_SRGB;

		m_renderPass = new RenderPass({m_shaderModules}, renderPassOptions, frameBufferAttachmentsInfo);
	}

	TextRendererBase::~TextRendererBase()
	{
		delete m_renderPass;

		for (ShaderModule * shaderModule : m_shaderModules)
			delete shaderModule;
	}
}