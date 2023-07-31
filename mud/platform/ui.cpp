#include "ui.hpp"

#include "graphics/application_graphics_context.hpp"
#include "graphics/color.hpp"
#include "math/matrix.hpp"
#include "math/matrix/transformations.hpp"
#include "utils/asset_manager.hpp"
#include "utils/logger.hpp"

namespace mud::ui
{
	Element::Element(Element * parent)
		: m_parent(nullptr), m_isVisible(true)
	{
		m_spriteRenderCommand.texture = nullptr;
		m_spriteRenderCommand.transform = Matrix4::identity;
		m_spriteRenderCommand.color = Color::white;

		m_textRenderCommand.text = "";
		m_textRenderCommand.position = Vector2::zero;
		m_textRenderCommand.fontFace = AssetManager::getInstance().getDefaultAsset<FontFamily>()->get()->getFontFace();
		m_textRenderCommand.color = Color::black;

		setParent(parent);
	}

	const Texture * Element::getTexture() const
	{
		return m_spriteRenderCommand.texture;
	}

	void Element::setTexture(const Texture * texture)
	{
		m_spriteRenderCommand.texture = texture;
	}

	bool Element::getIsVisible() const
	{
		return m_isVisible;
	}

	void Element::setIsVisible(bool isVisible)
	{
		m_isVisible = isVisible;
	}

	const Vector2 & Element::getPosition() const
	{
		return m_position;
	}

	void Element::setPosition(const Vector2 & position)
	{
		m_position = position;
		updateTransform();
	}

	const Vector2 & Element::getSize() const
	{
		return m_size;
	}

	void Element::setSize(const Vector2 & size)
	{
		m_size = size;
		updateTransform();
	}

	const Matrix4 & Element::getTransform() const
	{
		return m_spriteRenderCommand.transform;
	}

	const Color & Element::getColor() const
	{
		return m_spriteRenderCommand.color;
	}

	void Element::setColor(const Color & color)
	{
		m_spriteRenderCommand.color = color;
	}

	const Color & Element::getForegroundColor() const
	{
		return m_textRenderCommand.color;
	}

	void Element::setForegroundColor(const Color & color)
	{
		m_textRenderCommand.color = color;
	}

	const std::string & Element::getText() const
	{
		return m_textRenderCommand.text;
	}

	void Element::setText(const std::string & text)
	{
		m_textRenderCommand.text = text;
	}

	const FontFace * Element::getTextFontFace() const
	{
		return m_textRenderCommand.fontFace;
	}

	void Element::setTextFontFace(const FontFace * fontFace)
	{
		m_textRenderCommand.fontFace = fontFace;
	}

	const SpriteRenderCommand & Element::getSpriteRenderCommand() const
	{
		return m_spriteRenderCommand;
	}

	const TextRenderCommand & Element::getTextRenderCommand() const
	{
		return m_textRenderCommand;
	}

	Element * Element::getParent() const
	{
		return m_parent;
	}

	void Element::setParent(Element * parent)
	{
		if (m_parent == parent)
			return;

		if (m_parent != nullptr)
		{
			auto iter = std::find(m_parent->m_children.begin(), m_parent->m_children.end(), this);
			if (iter != m_parent->m_children.end())
				m_parent->m_children.erase(iter);
		}

		m_parent = parent;

		if (m_parent != nullptr)
			m_parent->m_children.push_back(this);

		updateTransform();
	}

	const std::vector<Element *> & Element::getChildren() const
	{
		return m_children;
	}

	bool Element::checkIntersection(const Vector2 & position) const
	{
		float left = m_spriteRenderCommand.transform.columns[3].x;
		float right = left + m_spriteRenderCommand.transform.columns[0].x;
		float top = m_spriteRenderCommand.transform.columns[3].y;
		float bottom = top + m_spriteRenderCommand.transform.columns[1].y;

		return !(position.x < left || position.x > right || position.y < top || position.y > bottom);
	}

	Element::EventOnMouseButton & Element::getEventOnMouseButton()
	{
		return m_eventOnMouseButton;
	}

	Element::EventOnMouseEnter & Element::getEventOnMouseEnter()
	{
		return m_eventOnMouseEnter;
	}

	Element::EventOnMouseLeave & Element::getEventOnMouseLeave()
	{
		return m_eventOnMouseLeave;
	}

	void Element::updateTransform()
	{
		m_spriteRenderCommand.transform = Matrix4::identity;
		m_spriteRenderCommand.transform.columns[3].x = m_position.x;
		m_spriteRenderCommand.transform.columns[3].y = m_position.y;
		m_spriteRenderCommand.transform.columns[3].z = 0.0f;
		m_spriteRenderCommand.transform.columns[0].x = m_size.x;
		m_spriteRenderCommand.transform.columns[1].y = m_size.y;
		m_spriteRenderCommand.transform.columns[2].z = 1.0f;

		m_textRenderCommand.position = m_position;

		if (m_parent != nullptr)
		{
			m_spriteRenderCommand.transform.columns[3].x += m_parent->m_spriteRenderCommand.transform.columns[3].x;
			m_spriteRenderCommand.transform.columns[3].y += m_parent->m_spriteRenderCommand.transform.columns[3].y;
			m_spriteRenderCommand.transform.columns[3].z += m_parent->m_spriteRenderCommand.transform.columns[3].z;
		}

		for (Element * child : m_children)
			child->updateTransform();
	}

	UIContext::UIContext()
		: m_elementHovered(nullptr), m_callbackOnMouseMove(this, &UIContext::onMouseMove), m_callbackOnMouseButton(this, &UIContext::onMouseButton)
	{
        RenderPassOptions renderPassOptions;
        renderPassOptions.clearColorBuffer = false;
        renderPassOptions.clearDepthBuffer = true;
        renderPassOptions.wireframe = false;
        renderPassOptions.faceCullMode = FaceCullMode::Back;
		m_spriteRenderer = new SpriteBatchRenderer(renderPassOptions);

        renderPassOptions.clearColorBuffer = false;
        renderPassOptions.clearDepthBuffer = true;
        renderPassOptions.wireframe = false;
        renderPassOptions.faceCullMode = FaceCullMode::Back;
		m_textRenderer = new TextRenderer(renderPassOptions);

		m_elements.reserve(1024);

		Window * window = ApplicationGraphicsContext::getInstance()->getMainWindow();

		window->getEventOnMouseMove().addListener(&m_callbackOnMouseMove);
		window->getEventOnMouseButton().addListener(&m_callbackOnMouseButton);

		m_camera.setOrthographic(window->getWidth(), window->getHeight(), -1.0f, 1.0f);
		m_camera.setPosition(Vector3(window->getWidth() * 0.5, window->getHeight() * 0.5, 0));

		m_root.setSize(Vector2(window->getWidth(), window->getHeight()));
	}

	UIContext::~UIContext()
	{
		delete m_spriteRenderer;
		delete m_textRenderer;

		Window * window = ApplicationGraphicsContext::getInstance()->getMainWindow();

		window->getEventOnMouseMove().removeListener(&m_callbackOnMouseMove);
		window->getEventOnMouseButton().removeListener(&m_callbackOnMouseButton);
	}

	Element * UIContext::newElement()
	{
		Element & element = m_elements.emplace_back();
		element.setParent(&m_root);
		return &element;
	}

	void UIContext::render()
	{
		Window * window = ApplicationGraphicsContext::getInstance()->getMainWindow();
		
		m_camera.setOrthographic(window->getWidth(), window->getHeight(), -1.0f, 1.0f);
		m_camera.setPosition(Vector3(window->getWidth() * 0.5, window->getHeight() * 0.5, 0));

		submitElementDrawCommand(m_root, Matrix4::identity);

		m_spriteRenderer->draw(m_camera);
		m_textRenderer->draw(m_camera);
	}

	void UIContext::submitElementDrawCommand(Element & element, const Matrix4 & parentTransform)
	{
		if (element.getIsVisible())
		{
			const SpriteRenderCommand & elementSpriteRenderCommand = element.getSpriteRenderCommand();

			if (elementSpriteRenderCommand.texture != nullptr)
				m_spriteRenderer->submit(elementSpriteRenderCommand);

			const TextRenderCommand & elementTextRenderCommand = element.getTextRenderCommand();

			if (!elementTextRenderCommand.text.empty() && elementTextRenderCommand.fontFace != nullptr)
				m_textRenderer->submit(elementTextRenderCommand);
		}

		for (Element * child : element.getChildren())
			submitElementDrawCommand(*child, element.getSpriteRenderCommand().transform);
	}

	void UIContext::onMouseMove(Vector2 mousePosition, Vector2 mouseDelta)
	{
		Element * oldElementHovered = m_elementHovered;
		m_elementHovered = nullptr;

		for (Element & element : m_elements)
			if (element.checkIntersection(mousePosition))
				m_elementHovered = &element;

		if (oldElementHovered != m_elementHovered)
		{
			if (oldElementHovered != nullptr)
				oldElementHovered->getEventOnMouseLeave().broadcast(*oldElementHovered);
			if (m_elementHovered != nullptr)
				m_elementHovered->getEventOnMouseEnter().broadcast(*m_elementHovered);
		}
	}

	void UIContext::onMouseButton(MouseButton button, MouseButtonState state)
	{
		if (m_elementHovered == nullptr)
			return;

		m_elementHovered->getEventOnMouseButton().broadcast(*m_elementHovered, button, state);
	}

	namespace widgets
	{
		WidgetBase::WidgetBase(Element * element)
			: m_element(element)
		{ }

		Button::Button(Element * element)
			: WidgetBase(element)
			, m_eventListenerElementOnMouseEnter(this, &Button::elementEventOnMouseEnterCallback)
			, m_eventListenerElementOnMouseLeave(this, &Button::elementEventOnMouseLeaveCallback)
			, m_eventListenerElementOnMouseButton(this, &Button::elementEventOnMouseButtonCallback)
		{
			m_element->getEventOnMouseEnter().addListener(&m_eventListenerElementOnMouseEnter);
			m_element->getEventOnMouseLeave().addListener(&m_eventListenerElementOnMouseLeave);
			m_element->getEventOnMouseButton().addListener(&m_eventListenerElementOnMouseButton);
		}

		Button::~Button()
		{
			m_element->getEventOnMouseEnter().removeListener(&m_eventListenerElementOnMouseEnter);
			m_element->getEventOnMouseLeave().removeListener(&m_eventListenerElementOnMouseLeave);
			m_element->getEventOnMouseButton().removeListener(&m_eventListenerElementOnMouseButton);
		}

		Button::State Button::getState() const
		{
			return m_state;
		}

		void Button::setIsDisabled(bool isDisabled)
		{
			setState(isDisabled ? State::Disabled : State::Default);
		}

		void Button::setState(State state)
		{
			m_state = state;

			switch (m_state)
			{
			case State::Default:
				m_element->setColor(Color::white);
				break;

			case State::Disabled:
				m_element->setColor(Color::grey);
				break;

			case State::Hovered:
				m_element->setColor(Color(0.5f, 0.5f, 1.0f, 1.0f));
				break;

			case State::Pressed:
				m_element->setColor(Color(0.2f, 0.2f, 1.0f, 1.0f));
				break;
			};
		}

		void Button::elementEventOnMouseEnterCallback(Element & element)
		{
			setState(State::Hovered);
		}

		void Button::elementEventOnMouseLeaveCallback(Element & element)
		{
			setState(State::Default);
		}

		void Button::elementEventOnMouseButtonCallback(Element & element, MouseButton button, MouseButtonState state)
		{
			if (m_state == State::Hovered && state.isPressed)
			{
				setState(State::Pressed);
			}
			else if (m_state == State::Pressed && !state.isPressed)
			{
				setState(State::Hovered);
				log(LogLevel::Trace, "Button clicked\n");
				m_eventOnClicked.broadcast(button);
			}
		}
	}
}