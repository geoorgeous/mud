#include "sprite_batch_renderer_base.hpp"

#include <unordered_map>
#include <vector>

#include "utils/file_io.hpp"

namespace mud
{
	SpriteBatchRendererBase::SpriteBatchRendererBase(RenderPassOptions renderPassOptions)
	{
		m_shaderModules.push_back(new ShaderModule(ShaderType::Vertex));
		m_shaderModules.back()->setSource(file::readText("./../mud/graphics/shaders/sprite.vert"));
		m_shaderModules.back()->compileSource();
		
		m_shaderModules.push_back(new ShaderModule(ShaderType::Fragment));
		m_shaderModules.back()->setSource(file::readText("./../mud/graphics/shaders/sprite.frag"));
		m_shaderModules.back()->compileSource();

		std::vector<FrameBufferAttachmentInfo> frameBufferAttachmentsInfo;
		frameBufferAttachmentsInfo.push_back(FrameBufferAttachmentInfo{});
		frameBufferAttachmentsInfo.back().imageFormat = ImageFormat::B8G8R8A8_SRGB;

		m_renderPass = new RenderPass({m_shaderModules}, renderPassOptions, frameBufferAttachmentsInfo);
	}

	SpriteBatchRendererBase::~SpriteBatchRendererBase()
	{
		delete m_renderPass;

		for (ShaderModule * shaderModule : m_shaderModules)
			delete shaderModule;
	}
}