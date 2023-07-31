#include "deferred_renderer_base.hpp"

#include "utils/file_io.hpp"

namespace mud
{
	DeferredRendererBase::DeferredRendererBase(RenderPassOptions renderPassOptions)
		: m_directionalLight{ Vector3(1, -3, 2).normal(), Color::white }
	{
		m_shaderModules.push_back(new ShaderModule(ShaderType::Vertex));
		m_shaderModules.back()->setSource(file::readText("./../mud/graphics/shaders/deferred_geometry.vert"));
		m_shaderModules.back()->compileSource();
		
		m_shaderModules.push_back(new ShaderModule(ShaderType::Fragment));
		m_shaderModules.back()->setSource(file::readText("./../mud/graphics/shaders/deferred_geometry"));
		m_shaderModules.back()->compileSource();
        
		m_shaderModules.push_back(new ShaderModule(ShaderType::Vertex));
		m_shaderModules.back()->setSource(file::readText("./../mud/graphics/shaders/deferred_lighting.vert"));
		m_shaderModules.back()->compileSource();
		
		m_shaderModules.push_back(new ShaderModule(ShaderType::Fragment));
		m_shaderModules.back()->setSource(file::readText("./../mud/graphics/shaders/deferred_lighting"));
		m_shaderModules.back()->compileSource();
        
        std::vector<ShaderModule *> geometryPassShaderModules {
            m_shaderModules[0], m_shaderModules[1]
        };
        
        std::vector<ShaderModule *> lightingPassShaderModules {
            m_shaderModules[2], m_shaderModules[3]
        };

		std::vector<FrameBufferAttachmentInfo> frameBufferAttachmentsInfo;
		frameBufferAttachmentsInfo.push_back(FrameBufferAttachmentInfo{});
		frameBufferAttachmentsInfo.back().imageFormat = ImageFormat::R16G16B16_SFLOAT;
		frameBufferAttachmentsInfo.push_back(FrameBufferAttachmentInfo{});
		frameBufferAttachmentsInfo.back().imageFormat = ImageFormat::R16G16B16_SFLOAT;
		frameBufferAttachmentsInfo.push_back(FrameBufferAttachmentInfo{});
		frameBufferAttachmentsInfo.back().imageFormat = ImageFormat::R8G8B8_UNORM;

		m_renderPass = new RenderPass({ geometryPassShaderModules, lightingPassShaderModules }, renderPassOptions, frameBufferAttachmentsInfo);
	}
	
	DeferredRendererBase::~DeferredRendererBase()
	{
		delete m_renderPass;

		for (ShaderModule * shaderModule : m_shaderModules)
			delete shaderModule;
	}

	void DeferredRendererBase::submit(RenderCommand command)
	{
		m_commands.push_back(command);
	}

	void DeferredRendererBase::submit(PointLight light)
	{
		m_lights.push_back(light);
	}

	const DirectionalLight & DeferredRendererBase::getDirectionalLight() const
	{
		return m_directionalLight;
	}

	DirectionalLight & DeferredRendererBase::getDirectionalLight()
	{
		return m_directionalLight;
	}
}