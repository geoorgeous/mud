#ifndef APPLICATION_GRAPHICS_CONTEXT_BASE_HPP
#define APPLICATION_GRAPHICS_CONTEXT_BASE_HPP

#include "platform/window.hpp"

namespace mud
{
	class ApplicationGraphicsContextBase
	{
	public:

		virtual void init(const WindowProperties & windowProperties) = 0;
	};
}

#endif