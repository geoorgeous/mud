#ifndef WINDOW_GRAPHICS_CONTEXT_BASE_HPP
#define WINDOW_GRAPHICS_CONTEXT_BASE_HPP

namespace mud
{
	class WindowGraphicsContextBase
	{
	public:

		virtual void makeDirty() = 0;

		virtual bool beginDraw() = 0;

		virtual void endDraw() = 0;
	};
}

#endif