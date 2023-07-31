#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "graphics/application_graphics_context.hpp"

namespace mud
{
	class Application
	{
	public:

		~Application();

		void run();
		void update(float deltaTime);
		void render();

	private:

		ApplicationGraphicsContext * m_applicationGraphicsContext;
	};
}

#endif