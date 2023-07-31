#ifndef SPRITE_BATCH_RENDERER_BASE_HPP
#define SPRITE_BATCH_RENDERER_BASE_HPP

#include <vector>

#include "graphics/color.hpp"
#include "graphics/render_pass.hpp"
#include "graphics/texture.hpp"
#include "math/matrix.hpp"

namespace mud
{
	struct SpriteRenderCommand
	{
		const Texture * texture;
		Matrix4 transform;
		Color color;
	};

	class Camera;

	class SpriteBatchRendererBase
	{
	public:
	
		SpriteBatchRendererBase(RenderPassOptions renderPassOptions);

		virtual ~SpriteBatchRendererBase();

		virtual void submit(const SpriteRenderCommand & command) = 0;

		virtual void draw(const Camera & camera) = 0;

	protected:
	
		std::vector<ShaderModule *> m_shaderModules;
		RenderPass * m_renderPass;
	};
}

#endif