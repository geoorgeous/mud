#ifndef TEXT_INPUT_BUFFER_HPP
#define TEXT_INPUT_BUFFER_HPP

#include "platform/event.hpp"
#include "platform/window.hpp"

#include <sstream>

namespace mud
{
	class TextInputBuffer
	{
	public:

		struct Config
		{
			bool startFocussed;
			std::string startContent;
			uint32_t maxBufferLength;
		};

		TextInputBuffer(Window & window, const Config & config = Config{ false, "", 0 });

		~TextInputBuffer();

		bool getIsFocussed() const;

		void setIsFocussed(bool isFocussed);

		const std::string & getContent() const;

		void setContent(const std::string & content);

		void insert(const std::string & content);

		void reset();

		typedef Event<TextInputBuffer &> EventOnContentChanged;
		EventOnContentChanged & getEventOnContentChanged();

		typedef Event<TextInputBuffer &> EventOnSubmit;
		EventOnSubmit & getEventOnSubmit();

	private:

		Window * m_window;

		bool m_isFocussed;
		size_t m_cursorPosition;
		std::string m_buffer;
		uint32_t m_maxBufferLength;

		EventOnSubmit m_eventOnSubmit;
		EventOnContentChanged m_eventOnContentChanged;

		Window::EventOnKey::TListenerCallback<TextInputBuffer> m_callbackOnKey;
		void onKey(Key key, KeyAction state, uint8_t mods);

		Window::EventOnChar::TListenerCallback<TextInputBuffer> m_callbackOnChar;
		void onChar(unsigned int codepoint);

		void setCursorPosition(size_t position);

		void offsetCursorPosition(int32_t offset);

		void deleteFromCursorPosition(int32_t offset);

		void setCursorToStart();

		void setCursorToEnd();

		void onBufferChanged();
	};
}

#endif