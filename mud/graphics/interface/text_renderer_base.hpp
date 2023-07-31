#ifndef TEXT_RENDERER_BASE_HPP
#define TEXT_RENDERER_BASE_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include "graphics/color.hpp"
#include "graphics/font.hpp"
#include "graphics/render_pass.hpp"
#include "math/vector.hpp"

namespace mud
{
	struct TextRenderCommand
	{
		std::string text;
		Vector3 position;
		const FontFace * fontFace;
		Color color;
	};

	class Camera;

	class TextRendererBase
	{
	public:

		TextRendererBase(RenderPassOptions renderPassOptions);

		virtual ~TextRendererBase();

		virtual void submit(const TextRenderCommand & command) = 0;

		virtual void draw(const Camera & camera) = 0;

	protected:

		std::vector<ShaderModule *> m_shaderModules;
		RenderPass * m_renderPass;
	};
}

#endif