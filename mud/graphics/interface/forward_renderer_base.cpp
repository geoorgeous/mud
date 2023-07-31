#include "forward_renderer_base.hpp"

#include "utils/file_io.hpp"

namespace mud
{
	ForwardRendererBase::ForwardRendererBase(RenderPassOptions renderPassOptions)
		: m_directionalLight{ Vector3(1, -3, 2).normal(), Color::white }
	{
		m_shaderModules.push_back(new ShaderModule(ShaderType::Vertex));
		m_shaderModules.back()->setSource(file::readText("./../mud/graphics/shaders/forward.vert"));
		m_shaderModules.back()->compileSource();
		
		m_shaderModules.push_back(new ShaderModule(ShaderType::Fragment));
		m_shaderModules.back()->setSource(file::readText("./../mud/graphics/shaders/pbr.frag"));
		m_shaderModules.back()->compileSource();

		std::vector<FrameBufferAttachmentInfo> frameBufferAttachmentsInfo;
		frameBufferAttachmentsInfo.push_back(FrameBufferAttachmentInfo{});
		frameBufferAttachmentsInfo.back().imageFormat = ImageFormat::B8G8R8A8_SRGB;

		m_renderPass = new RenderPass({m_shaderModules}, renderPassOptions, frameBufferAttachmentsInfo);
	}
	
	ForwardRendererBase::~ForwardRendererBase()
	{
		delete m_renderPass;

		for (ShaderModule * shaderModule : m_shaderModules)
			delete shaderModule;
	}

	void ForwardRendererBase::submit(RenderCommand command)
	{
		m_commands.push_back(command);
	}

	void ForwardRendererBase::submit(PointLight light)
	{
		m_lights.push_back(light);
	}

	const DirectionalLight & ForwardRendererBase::getDirectionalLight() const
	{
		return m_directionalLight;
	}

	DirectionalLight & ForwardRendererBase::getDirectionalLight()
	{
		return m_directionalLight;
	}
}