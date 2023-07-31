#ifndef FORWARD_RENDERER_BASE_HPP
#define FORWARD_RENDERER_BASE_HPP

#include <vector>

#include "graphics/lights.hpp"
#include "graphics/material.hpp"
#include "graphics/mesh.hpp"
#include "graphics/render_pass.hpp"
#include "graphics/texture.hpp"
#include "math/matrix.hpp"

namespace mud
{
	struct RenderCommand
	{
		const Mesh * mesh;
		Matrix4 transform;
		const Material * material;

		bool operator <(const RenderCommand & rhs)
		{
			size_t textureAsInt = reinterpret_cast<size_t>(material);
			size_t rhsTextureAsInt = reinterpret_cast<size_t>(rhs.material);

			if (textureAsInt != rhsTextureAsInt)
				return textureAsInt < rhsTextureAsInt;

			size_t meshAsInt = reinterpret_cast<size_t>(material);
			size_t rhsMeshAsInt = reinterpret_cast<size_t>(rhs.material);

			return meshAsInt < rhsMeshAsInt;
		}
	};

	class Camera;

	class ForwardRendererBase
	{
	public:

		ForwardRendererBase(RenderPassOptions renderPassOptions);

		virtual ~ForwardRendererBase();

		virtual void submit(RenderCommand command);

		virtual void submit(PointLight light);

		virtual void draw(const Camera & camera) = 0;

		const DirectionalLight & getDirectionalLight() const;

		DirectionalLight & getDirectionalLight();

	protected:

		std::vector<ShaderModule *> m_shaderModules;
		RenderPass * m_renderPass;

		std::vector<RenderCommand> m_commands;

		DirectionalLight m_directionalLight;
		std::vector<PointLight> m_lights;
	};
}

#endif