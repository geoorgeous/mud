#ifndef UI_HPP
#define UI_HPP

#include <vector>

#include "platform/event.hpp"
#include "platform/window.hpp"
#include "math/vector.hpp"
#include "graphics/camera.hpp"
#include "graphics/color.hpp"
#include "graphics/sprite_batch_renderer.hpp"
#include "graphics/text_renderer.hpp"
#include "graphics/texture.hpp"

namespace mud::ui
{
	class Element
	{
	public:

		Element(Element * parent = nullptr);

		const Texture * getTexture() const;

		void setTexture(const Texture * texture);

		bool getIsVisible() const;

		void setIsVisible(bool isVisible);

		const Vector2 & getPosition() const;

		void setPosition(const Vector2 & position);

		const Vector2 & getSize() const;

		void setSize(const Vector2 & size);

		const Matrix4 & getTransform() const;

		const Color & getColor() const;

		void setColor(const Color & color);

		const Color & getForegroundColor() const;

		void setForegroundColor(const Color & color);

		const std::string & getText() const;

		void setText(const std::string & text);

		const FontFace * getTextFontFace() const;

		void setTextFontFace(const FontFace * fontFace);

		const SpriteRenderCommand & getSpriteRenderCommand() const;

		const TextRenderCommand & getTextRenderCommand() const;

		Element * getParent() const;

		void setParent(Element * parent);

		const std::vector<Element *> & getChildren() const;

		bool checkIntersection(const Vector2 & position) const;

		typedef Event<Element &> EventOnMouseEnter;
		EventOnMouseEnter & getEventOnMouseEnter();

		typedef Event<Element &> EventOnMouseLeave;
		EventOnMouseLeave & getEventOnMouseLeave();

		typedef Event<Element &, MouseButton, MouseButtonState> EventOnMouseButton;
		EventOnMouseButton & getEventOnMouseButton();

	private:

		bool m_isVisible;
		Vector2 m_position;
		Vector2 m_size;

		SpriteRenderCommand m_spriteRenderCommand;
		TextRenderCommand m_textRenderCommand;

		Element * m_parent;
		std::vector<Element *> m_children;

		EventOnMouseEnter m_eventOnMouseEnter;
		EventOnMouseLeave m_eventOnMouseLeave;
		EventOnMouseButton m_eventOnMouseButton;

		void updateTransform();
	};

	class UIContext
	{
	public:

		UIContext();

		~UIContext();

		Element * newElement();

		void render();

	private:

		Camera m_camera;
		SpriteBatchRenderer * m_spriteRenderer;
		TextRenderer * m_textRenderer;

		Element m_root;
		std::vector<Element> m_elements;

		Element * m_elementHovered;

		void submitElementDrawCommand(Element & element, const Matrix4 & parentTransform);

		Window::EventOnMouseMove::TListenerCallback<UIContext> m_callbackOnMouseMove;
		void onMouseMove(Vector2 mousePosition, Vector2 mouseDelta);

		Window::EventOnMouseButton::TListenerCallback<UIContext> m_callbackOnMouseButton;
		void onMouseButton(MouseButton button, MouseButtonState state);
	};

	namespace widgets
	{
		class WidgetBase
		{
		public:

			WidgetBase(Element * element);

			virtual ~WidgetBase() { }

		protected:

			Element * m_element;
		};

		class Button : public WidgetBase
		{
		public:

			enum class State
			{
				Default,
				Disabled,
				Hovered,
				Pressed
			};

			Button(Element * element);

			~Button();

			State getState() const;

			void setIsDisabled(bool isDisabled);

			typedef Event<MouseButton> EventOnClicked;
			EventOnClicked & getEventOnClicked();

		private:

			State m_state;

			EventOnClicked m_eventOnClicked;

			void setState(State state);

			Element::EventOnMouseEnter::TListenerCallback<Button> m_eventListenerElementOnMouseEnter;
			void elementEventOnMouseEnterCallback(Element & element);

			Element::EventOnMouseLeave::TListenerCallback<Button> m_eventListenerElementOnMouseLeave;
			void elementEventOnMouseLeaveCallback(Element & element);

			Element::EventOnMouseButton::TListenerCallback<Button> m_eventListenerElementOnMouseButton;
			void elementEventOnMouseButtonCallback(Element & element, MouseButton button, MouseButtonState state);
		};
	}
}

#endif