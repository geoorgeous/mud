#include "text_input_buffer.hpp"

#include <codecvt>

#include "logger.hpp"

namespace mud
{
	TextInputBuffer::TextInputBuffer(Window & window, const Config & config)
		: m_window(&window), m_callbackOnKey(this, &TextInputBuffer::onKey), m_callbackOnChar(this, &TextInputBuffer::onChar), m_isFocussed(config.startFocussed), m_cursorPosition(0), m_maxBufferLength(config.maxBufferLength)
	{
		m_window->getEventOnKey().addListener(&m_callbackOnKey);
		m_window->getEventOnChar().addListener(&m_callbackOnChar);

		insert(config.startContent);
	}

	TextInputBuffer::~TextInputBuffer()
	{
		m_window->getEventOnKey().removeListener(&m_callbackOnKey);
		m_window->getEventOnChar().removeListener(&m_callbackOnChar);
	}

	bool TextInputBuffer::getIsFocussed() const
	{
		return m_isFocussed;
	}

	void TextInputBuffer::setIsFocussed(bool isFocussed)
	{
		m_isFocussed = isFocussed;
	}

	const std::string & TextInputBuffer::getContent() const
	{
		return m_buffer;
	}

	void TextInputBuffer::setContent(const std::string & content)
	{
		m_buffer = content;
		onBufferChanged();
		setCursorPosition(m_buffer.length());
	}

	void TextInputBuffer::insert(const std::string & content)
	{
		if (content.empty())
			return;
		m_buffer.insert(m_cursorPosition, content);
		onBufferChanged();
		offsetCursorPosition(content.length());
	}

	void TextInputBuffer::reset()
	{
		setContent(std::string());
	}
	
	TextInputBuffer::EventOnContentChanged & TextInputBuffer::getEventOnContentChanged()
	{
		return m_eventOnContentChanged;
	}

	TextInputBuffer::EventOnSubmit & TextInputBuffer::getEventOnSubmit()
	{
		return m_eventOnSubmit;
	}

	void TextInputBuffer::onKey(Key key, KeyAction state, uint8_t mods)
	{
		if (!m_isFocussed || state == KeyAction::Released)
			return;

		switch (key)
		{
		case Key::Left:
			offsetCursorPosition(-1);
			break;
		case Key::Right:
			offsetCursorPosition(1);
			break;
		case Key::Home:
			setCursorToStart();
			break;
		case Key::End:
			setCursorToEnd();
			break;
		case Key::Backspace:
			deleteFromCursorPosition(-1);
			break;
		case Key::Delete:
			deleteFromCursorPosition(1);
			break;
		case Key::Enter:
			m_eventOnSubmit.broadcast(*this);
			break;
		}
	}

	void TextInputBuffer::onChar(unsigned int codepoint)
	{
		if (!m_isFocussed)
			return;

		char32_t c = static_cast<char32_t>(codepoint);
		std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
		insert(convert.to_bytes(&c, &c + 1));
	}

	void TextInputBuffer::setCursorPosition(size_t position)
	{
		if (position > m_buffer.length())
			position = m_buffer.length();
		m_cursorPosition = position;

		std::string str = m_buffer;
		str.insert(m_cursorPosition, "_");
	}

	void TextInputBuffer::offsetCursorPosition(int32_t offset)
	{
		int32_t position = static_cast<int32_t>(m_cursorPosition) + offset;

		if (position < 0)
			position = 0;

		setCursorPosition(static_cast<size_t>(position));
	}

	void TextInputBuffer::deleteFromCursorPosition(int32_t offset)
	{
		if (offset == 0)
			return;

		size_t eraseStart = m_cursorPosition;
		size_t eraseEnd = eraseStart;

		if (offset < 0)
		{
			int32_t signedEraseStart = static_cast<int32_t>(eraseEnd) + offset;
			if (signedEraseStart < 0)
				eraseStart = 0;
			else
				eraseStart = static_cast<size_t>(signedEraseStart);
		}
		else
		{
			eraseEnd = eraseStart + static_cast<size_t>(offset);
			if (eraseEnd > m_buffer.length())
				eraseEnd = m_buffer.length();
		}

		if (eraseEnd - eraseStart == 0)
			return;

		m_buffer.erase(eraseStart, eraseEnd - eraseStart);

		setCursorPosition(eraseStart);
	}

	void TextInputBuffer::setCursorToStart()
	{
		setCursorPosition(0);
	}

	void TextInputBuffer::setCursorToEnd()
	{
		setCursorPosition(m_buffer.length());
	}

	void TextInputBuffer::onBufferChanged()
	{
		if (m_maxBufferLength != 0 && m_buffer.length() > m_maxBufferLength)
			m_buffer = m_buffer.substr(0, m_maxBufferLength);
		m_eventOnContentChanged.broadcast(*this);
	}
}