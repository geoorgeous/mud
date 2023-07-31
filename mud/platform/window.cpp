#include "window.hpp"

// GLFW
#if defined(MUD_USE_OPENGL)
#define GLFW_INCLUDE_OPENGL
#define MUD_GLFW_API GLFW_OPENGL_API
#elif defined(MUD_USE_VULKAN)
#define GLFW_INCLUDE_VULKAN
#define MUD_GLFW_API GLFW_NO_API
#endif
#include <glfw/glfw3.h>

#include "graphics/window_graphics_context.hpp"
#include "utils/logger.hpp"

namespace mud
{
    int toGlfwKey(Key key)
    {
        static const std::unordered_map<Key, int> map = {
                    {Key::Unknown, GLFW_KEY_UNKNOWN},
                    {Key::Escape, GLFW_KEY_ESCAPE},
                    {Key::F1, GLFW_KEY_F1},
                    {Key::F2, GLFW_KEY_F2},
                    {Key::F3, GLFW_KEY_F3},
                    {Key::F4, GLFW_KEY_F4},
                    {Key::F5, GLFW_KEY_F5},
                    {Key::F6, GLFW_KEY_F6},
                    {Key::F7, GLFW_KEY_F7},
                    {Key::F8, GLFW_KEY_F8},
                    {Key::F9, GLFW_KEY_F9},
                    {Key::F10, GLFW_KEY_F10},
                    {Key::F11, GLFW_KEY_F11},
                    {Key::F12, GLFW_KEY_F12},
                    {Key::Num0, GLFW_KEY_0},
                    {Key::Num1, GLFW_KEY_1},
                    {Key::Num2, GLFW_KEY_2},
                    {Key::Num3, GLFW_KEY_3},
                    {Key::Num4, GLFW_KEY_4},
                    {Key::Num5, GLFW_KEY_5},
                    {Key::Num6, GLFW_KEY_6},
                    {Key::Num7, GLFW_KEY_7},
                    {Key::Num8, GLFW_KEY_8},
                    {Key::Num9, GLFW_KEY_9},
                    {Key::A, GLFW_KEY_A},
                    {Key::B, GLFW_KEY_B},
                    {Key::C, GLFW_KEY_C},
                    {Key::D, GLFW_KEY_D},
                    {Key::E, GLFW_KEY_E},
                    {Key::F, GLFW_KEY_F},
                    {Key::G, GLFW_KEY_G},
                    {Key::H, GLFW_KEY_H},
                    {Key::I, GLFW_KEY_I},
                    {Key::J, GLFW_KEY_J},
                    {Key::K, GLFW_KEY_K},
                    {Key::L, GLFW_KEY_L},
                    {Key::M, GLFW_KEY_M},
                    {Key::N, GLFW_KEY_N},
                    {Key::O, GLFW_KEY_O},
                    {Key::P, GLFW_KEY_P},
                    {Key::Q, GLFW_KEY_Q},
                    {Key::R, GLFW_KEY_R},
                    {Key::S, GLFW_KEY_S},
                    {Key::T, GLFW_KEY_T},
                    {Key::U, GLFW_KEY_U},
                    {Key::V, GLFW_KEY_V},
                    {Key::W, GLFW_KEY_W},
                    {Key::X, GLFW_KEY_X},
                    {Key::Y, GLFW_KEY_Y},
                    {Key::Z, GLFW_KEY_Z},
                    {Key::GraveAccent, GLFW_KEY_GRAVE_ACCENT},
                    {Key::Minus, GLFW_KEY_MINUS},
                    {Key::Equal, GLFW_KEY_EQUAL},
                    {Key::Backspace, GLFW_KEY_BACKSPACE},
                    {Key::Tab, GLFW_KEY_TAB},
                    {Key::LeftBracket, GLFW_KEY_LEFT_BRACKET},
                    {Key::RightBracket, GLFW_KEY_RIGHT_BRACKET},
                    {Key::Enter, GLFW_KEY_ENTER},
                    {Key::CapsLock, GLFW_KEY_CAPS_LOCK},
                    {Key::SemiColon, GLFW_KEY_SEMICOLON},
                    {Key::Apostrophe, GLFW_KEY_APOSTROPHE},
                    {Key::Hash, GLFW_KEY_0},
                    {Key::LeftShift, GLFW_KEY_LEFT_SHIFT},
                    {Key::BackwardSlash, GLFW_KEY_BACKSLASH},
                    {Key::Comma, GLFW_KEY_COMMA},
                    {Key::Period, GLFW_KEY_PERIOD},
                    {Key::ForwardSlash, GLFW_KEY_SLASH},
                    {Key::RightShift, GLFW_KEY_RIGHT_SHIFT},
                    {Key::LeftControl, GLFW_KEY_LEFT_CONTROL},
                    {Key::LeftAlt, GLFW_KEY_LEFT_ALT},
                    {Key::Space, GLFW_KEY_SPACE},
                    {Key::RightAlt, GLFW_KEY_RIGHT_ALT},
                    {Key::RightControl, GLFW_KEY_RIGHT_CONTROL},
                    {Key::Insert, GLFW_KEY_INSERT},
                    {Key::Delete, GLFW_KEY_DELETE},
                    {Key::Home, GLFW_KEY_HOME},
                    {Key::End, GLFW_KEY_END},
                    {Key::PageUp, GLFW_KEY_PAGE_UP},
                    {Key::PageDown, GLFW_KEY_PAGE_DOWN},
                    {Key::Left, GLFW_KEY_LEFT},
                    {Key::Right, GLFW_KEY_RIGHT},
                    {Key::Up, GLFW_KEY_UP},
                    {Key::Down, GLFW_KEY_DOWN},
                    {Key::NumPad0, GLFW_KEY_KP_0},
                    {Key::NumPad1, GLFW_KEY_KP_1},
                    {Key::NumPad2, GLFW_KEY_KP_2},
                    {Key::NumPad3, GLFW_KEY_KP_3},
                    {Key::NumPad4, GLFW_KEY_KP_4},
                    {Key::NumPad5, GLFW_KEY_KP_5},
                    {Key::NumPad6, GLFW_KEY_KP_6},
                    {Key::NumPad7, GLFW_KEY_KP_7},
                    {Key::NumPad8, GLFW_KEY_KP_8},
                    {Key::NumPad9, GLFW_KEY_KP_9},
                    {Key::NumPadDivide, GLFW_KEY_KP_DIVIDE},
                    {Key::NumPadMultiply, GLFW_KEY_KP_MULTIPLY},
                    {Key::NumPadMinus, GLFW_KEY_KP_SUBTRACT},
                    {Key::NumPadPlus, GLFW_KEY_KP_ADD},
                    {Key::NumPadPeriod, GLFW_KEY_KP_DECIMAL},
                    {Key::NumPadEnter, GLFW_KEY_KP_ENTER}
        };

        auto glfwKey = map.find(key);

        if (glfwKey != map.end())
            return glfwKey->second;

        return GLFW_KEY_UNKNOWN;
    }

    static Key fromGlfwKey(int glfwKey)
    {
        static const std::unordered_map<int32_t, Key> map = {
                    {GLFW_KEY_UNKNOWN, Key::Unknown},
                    {GLFW_KEY_ESCAPE, Key::Escape},
                    {GLFW_KEY_F1, Key::F1},
                    {GLFW_KEY_F2, Key::F2},
                    {GLFW_KEY_F3, Key::F3},
                    {GLFW_KEY_F4, Key::F4},
                    {GLFW_KEY_F5, Key::F5},
                    {GLFW_KEY_F6, Key::F6},
                    {GLFW_KEY_F7, Key::F7},
                    {GLFW_KEY_F8, Key::F8},
                    {GLFW_KEY_F9, Key::F9},
                    {GLFW_KEY_F10, Key::F10},
                    {GLFW_KEY_F11, Key::F11},
                    {GLFW_KEY_F12, Key::F12},
                    {GLFW_KEY_0, Key::Num0},
                    {GLFW_KEY_1, Key::Num1},
                    {GLFW_KEY_2, Key::Num2},
                    {GLFW_KEY_3, Key::Num3},
                    {GLFW_KEY_4, Key::Num4},
                    {GLFW_KEY_5, Key::Num5},
                    {GLFW_KEY_6, Key::Num6},
                    {GLFW_KEY_7, Key::Num7},
                    {GLFW_KEY_8, Key::Num8},
                    {GLFW_KEY_9, Key::Num9},
                    {GLFW_KEY_A, Key::A},
                    {GLFW_KEY_B, Key::B},
                    {GLFW_KEY_C, Key::C},
                    {GLFW_KEY_D, Key::D},
                    {GLFW_KEY_E, Key::E},
                    {GLFW_KEY_F, Key::F},
                    {GLFW_KEY_G, Key::G},
                    {GLFW_KEY_H, Key::H},
                    {GLFW_KEY_I, Key::I},
                    {GLFW_KEY_J, Key::J},
                    {GLFW_KEY_K, Key::K},
                    {GLFW_KEY_L, Key::L},
                    {GLFW_KEY_M, Key::M},
                    {GLFW_KEY_N, Key::N},
                    {GLFW_KEY_O, Key::O},
                    {GLFW_KEY_P, Key::P},
                    {GLFW_KEY_Q, Key::Q},
                    {GLFW_KEY_R, Key::R},
                    {GLFW_KEY_S, Key::S},
                    {GLFW_KEY_T, Key::T},
                    {GLFW_KEY_U, Key::U},
                    {GLFW_KEY_V, Key::V},
                    {GLFW_KEY_W, Key::W},
                    {GLFW_KEY_X, Key::X},
                    {GLFW_KEY_Y, Key::Y},
                    {GLFW_KEY_Z, Key::Z},
                    {GLFW_KEY_GRAVE_ACCENT, Key::GraveAccent},
                    {GLFW_KEY_MINUS, Key::Minus},
                    {GLFW_KEY_EQUAL, Key::Equal},
                    {GLFW_KEY_BACKSPACE, Key::Backspace},
                    {GLFW_KEY_TAB, Key::Tab},
                    {GLFW_KEY_LEFT_BRACKET, Key::LeftBracket},
                    {GLFW_KEY_RIGHT_BRACKET, Key::RightBracket},
                    {GLFW_KEY_ENTER, Key::Enter},
                    {GLFW_KEY_CAPS_LOCK, Key::CapsLock},
                    {GLFW_KEY_SEMICOLON, Key::SemiColon},
                    {GLFW_KEY_APOSTROPHE, Key::Apostrophe},
                    {GLFW_KEY_0, Key::Hash},
                    {GLFW_KEY_LEFT_SHIFT, Key::LeftShift},
                    {GLFW_KEY_BACKSLASH, Key::BackwardSlash},
                    {GLFW_KEY_COMMA, Key::Comma},
                    {GLFW_KEY_PERIOD, Key::Period},
                    {GLFW_KEY_SLASH, Key::ForwardSlash},
                    {GLFW_KEY_RIGHT_SHIFT, Key::RightShift},
                    {GLFW_KEY_LEFT_CONTROL, Key::LeftControl},
                    {GLFW_KEY_LEFT_ALT, Key::LeftAlt},
                    {GLFW_KEY_SPACE, Key::Space},
                    {GLFW_KEY_RIGHT_ALT, Key::RightAlt},
                    {GLFW_KEY_RIGHT_CONTROL, Key::RightControl},
                    {GLFW_KEY_INSERT, Key::Insert},
                    {GLFW_KEY_DELETE, Key::Delete},
                    {GLFW_KEY_HOME, Key::Home},
                    {GLFW_KEY_END, Key::End},
                    {GLFW_KEY_PAGE_UP, Key::PageUp},
                    {GLFW_KEY_PAGE_DOWN, Key::PageDown},
                    {GLFW_KEY_LEFT, Key::Left},
                    {GLFW_KEY_RIGHT, Key::Right},
                    {GLFW_KEY_UP, Key::Up},
                    {GLFW_KEY_DOWN, Key::Down},
                    {GLFW_KEY_KP_0, Key::NumPad0},
                    {GLFW_KEY_KP_1, Key::NumPad1},
                    {GLFW_KEY_KP_2, Key::NumPad2},
                    {GLFW_KEY_KP_3, Key::NumPad3},
                    {GLFW_KEY_KP_4, Key::NumPad4},
                    {GLFW_KEY_KP_5, Key::NumPad5},
                    {GLFW_KEY_KP_6, Key::NumPad6},
                    {GLFW_KEY_KP_7, Key::NumPad7},
                    {GLFW_KEY_KP_8, Key::NumPad8},
                    {GLFW_KEY_KP_9, Key::NumPad9},
                    {GLFW_KEY_KP_DIVIDE, Key::NumPadDivide},
                    {GLFW_KEY_KP_MULTIPLY, Key::NumPadMultiply},
                    {GLFW_KEY_KP_SUBTRACT, Key::NumPadMinus},
                    {GLFW_KEY_KP_ADD, Key::NumPadPlus},
                    {GLFW_KEY_KP_DECIMAL, Key::NumPadPeriod},
                    {GLFW_KEY_KP_ENTER, Key::NumPadEnter}
        };

        auto key = map.find(glfwKey);

        if (key != map.end())
            return key->second;

        return Key::Unknown;
    }

    static KeyAction fromGlfwAction(int glfwAction)
    {
        static const std::unordered_map<int32_t, KeyAction> map = {
                    {GLFW_PRESS, KeyAction::Pressed},
                    {GLFW_REPEAT, KeyAction::Repeating},
                    {GLFW_RELEASE, KeyAction::Released},
        };

        auto key = map.find(glfwAction);

        if (key != map.end())
            return key->second;

        return KeyAction::Released;
    }

    static MouseButton fromGlfwMouseButton(int glfwMouseButton)
    {
        switch (glfwMouseButton)
        {
        case GLFW_MOUSE_BUTTON_LEFT:
            return MouseButton::Left;

        case GLFW_MOUSE_BUTTON_RIGHT:
            return MouseButton::Right;

        case GLFW_MOUSE_BUTTON_MIDDLE:
            return MouseButton::Middle;
        }

        return MouseButton::Unknown;
    }
    
    static int toGlfwCursorInputMode(CursorInputMode cursorInputMode)
    {
        switch (cursorInputMode)
        {
        case CursorInputMode::Hidden:
            return GLFW_CURSOR_HIDDEN;

        case CursorInputMode::Disabled:
            return GLFW_CURSOR_DISABLED;

        default:
            return GLFW_CURSOR_NORMAL;
        }
    }

    static int toGlfwCursorShape(CursorShape cursorShape)
    {
        switch (cursorShape)
        {
        case CursorShape::TextInput:
            return GLFW_IBEAM_CURSOR;
            
        case CursorShape::Crosshair:
            return GLFW_CROSSHAIR_CURSOR;
            
        case CursorShape::Hand:
            return GLFW_HAND_CURSOR;
            
        case CursorShape::ResizeHorizontal:
            return GLFW_HRESIZE_CURSOR;
            
        case CursorShape::ResizeVertical:
            return GLFW_VRESIZE_CURSOR;
            
        default:
            return GLFW_ARROW_CURSOR;
        }
    }

    static MonitorInfo * setupGlfwMonitorUserPointer(GLFWmonitor * monitor)
    {
        MonitorInfo * monitorInfo;
        void * userPointer = glfwGetMonitorUserPointer(monitor);

        if (userPointer == nullptr)
        {
            monitorInfo = new MonitorInfo;
            glfwSetMonitorUserPointer(monitor, static_cast<void *>(monitorInfo));
            monitorInfo->uuid.generate();
        }
        else
        {
            monitorInfo = reinterpret_cast<MonitorInfo *>(userPointer);
        }

        monitorInfo->name = std::string(glfwGetMonitorName(monitor));

        int count;
        const GLFWvidmode * glfwVidModes = glfwGetVideoModes(monitor, &count);
        monitorInfo->availableResolutions.resize(count);
        for (int idx = 0; idx < count; ++idx)
        {
            monitorInfo->availableResolutions[idx].width = glfwVidModes[idx].width;
            monitorInfo->availableResolutions[idx].height = glfwVidModes[idx].height;
            monitorInfo->availableResolutions[idx].refreshRate = glfwVidModes[idx].refreshRate;
        }

        glfwSetMonitorUserPointer(monitor, static_cast<void *>(monitorInfo));

        return monitorInfo;
    }

    static GLFWimage toGlfwImage(const Texture * texture)
    {
        GLFWimage glfwImage;
        glfwImage.width = texture->getWidth();
        glfwImage.height = texture->getHeight();
        glfwImage.pixels = const_cast<unsigned char *>(texture->getData());
        return glfwImage;
    }
    
    void KeyState::reset()
    {
        action = KeyAction::None;
        modifiers = 0;
    }

    void MouseState::reset()
    {
        cursorDelta = Vector2::zero;
        scrollDelta = Vector2::zero;
        for (auto & mouseButton : buttons)
            mouseButton.second.action = MouseButtonAction::None;
    }

    WindowProperties::WindowProperties()
        : mode(WindowMode::Windowed), width(0), height(0), cursorInputMode(CursorInputMode::Normal), isHidden(false), isResizable(true), isDecorated(true)
    { }
    
    uint16_t Window::s_windowCount = 0;

    std::unordered_map<CursorShape, GLFWcursor *> Window::s_glfwCursors;

    Window::Window(const WindowProperties & properties)
        : m_properties(properties), m_isMinimised(false)
    {
        static bool glfwInitialized = false;
        if (!glfwInitialized)
        {
            if (glfwInit())
            {
                glfwInitialized = true;
                
                glfwWindowHint(GLFW_CLIENT_API, MUD_GLFW_API);

                glfwSetErrorCallback(glfwCallback_Error);
                glfwSetMonitorCallback(glfwCallback_Monitor);

                int monitorCount;
                GLFWmonitor ** monitors = glfwGetMonitors(&monitorCount);
                for (int idx = 0; idx < monitorCount; ++idx)
                    setupGlfwMonitorUserPointer(monitors[idx]);

                s_glfwCursors[CursorShape::Default] = NULL;
                s_glfwCursors[CursorShape::TextInput] = glfwCreateStandardCursor(toGlfwCursorShape(CursorShape::TextInput));
                s_glfwCursors[CursorShape::Crosshair] = glfwCreateStandardCursor(toGlfwCursorShape(CursorShape::Crosshair));
                s_glfwCursors[CursorShape::Hand] = glfwCreateStandardCursor(toGlfwCursorShape(CursorShape::Hand));
                s_glfwCursors[CursorShape::ResizeHorizontal] = glfwCreateStandardCursor(toGlfwCursorShape(CursorShape::ResizeHorizontal));
                s_glfwCursors[CursorShape::ResizeVertical] = glfwCreateStandardCursor(toGlfwCursorShape(CursorShape::ResizeVertical));
            }
            else
            {
                log(LogLevel::Error, "Failed to initialise GLFW\n", "Platform");
                return;
            }
        }
         
        int desktopX, desktopY, desktopWidth, desktopHeight;
        glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), &desktopX, &desktopY, &desktopWidth, &desktopHeight);

        if (m_properties.width == 0 || m_properties.height == 0)
        {
            const MonitorInfo * primaryMonitorInfo = reinterpret_cast<MonitorInfo *>(glfwGetMonitorUserPointer(glfwGetPrimaryMonitor()));
            m_properties.width = primaryMonitorInfo->availableResolutions.front().width;
            m_properties.height = primaryMonitorInfo->availableResolutions.front().height;
        }
        
        glfwWindowHint(GLFW_VISIBLE, m_properties.isHidden ? GLFW_FALSE : GLFW_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, m_properties.isResizable ? GLFW_TRUE : GLFW_FALSE);
        glfwWindowHint(GLFW_DECORATED, m_properties.isDecorated ? GLFW_TRUE : GLFW_FALSE);

        switch (m_properties.mode)
        {
        case WindowMode::Windowed:
        {
            m_glfwWindow = glfwCreateWindow(static_cast<int>(m_properties.width), static_cast<int>(m_properties.height), m_properties.title.c_str(), nullptr, nullptr);
            setPosition((static_cast<uint16_t>(desktopWidth) - m_properties.width) / 2, (static_cast<uint16_t>(desktopHeight) - m_properties.height) / 2);
            break;
        }
            
        case WindowMode::WindowedFullscreen:
        {
            const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            glfwWindowHint(GLFW_RED_BITS, mode->redBits);
            glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
            glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
            glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
            m_glfwWindow = glfwCreateWindow(mode->width, mode->height, m_properties.title.c_str(), glfwGetPrimaryMonitor(), nullptr);
            break;
        }
            
        case WindowMode::Fullscreen:
        {
            m_glfwWindow = glfwCreateWindow(static_cast<int>(m_properties.width), static_cast<int>(m_properties.height), m_properties.title.c_str(), glfwGetPrimaryMonitor(), nullptr);
            break;
        }   
        }

        if (m_glfwWindow == nullptr)
        {
            log(LogLevel::Error, "Failed to create GLFW Window\n", "Window");
            return;
        }

        setCursorInputMode(m_properties.cursorInputMode);
        setIcon(m_properties.iconCandidates);

        glfwSetWindowUserPointer(m_glfwWindow, static_cast<void *>(this));

        glfwSetWindowFocusCallback(m_glfwWindow, glfwCallback_WindowFocus);
        glfwSetWindowIconifyCallback(m_glfwWindow, glfwCallback_WindowIconify);
        glfwSetWindowMaximizeCallback(m_glfwWindow, glfwCallback_WindowMaximize);
        glfwSetWindowSizeCallback(m_glfwWindow, glfwCallback_WindowSize);
        glfwSetFramebufferSizeCallback(m_glfwWindow, glfwCallback_FramebufferResize);
        glfwSetKeyCallback(m_glfwWindow, glfwCallback_Key);
        glfwSetCharCallback(m_glfwWindow, glfwCallback_Char);
        glfwSetCursorPosCallback(m_glfwWindow, glfwCallback_CursorPos);
        glfwSetCursorEnterCallback(m_glfwWindow, glfwCallback_CursorEnter);
        glfwSetMouseButtonCallback(m_glfwWindow, glfwCallback_MouseButton);
        glfwSetScrollCallback(m_glfwWindow, glfwCallback_Scroll);
        glfwSetDropCallback(m_glfwWindow, glfwCallback_Drop);

        m_cachedWindowedModePosLeft = (static_cast<uint16_t>(desktopWidth) - m_properties.width) / 2;
        m_cachedWindowedModePosTop = (static_cast<uint16_t>(desktopHeight) - m_properties.height) / 2;
        m_cachedWindowedModeWidth = m_properties.width;
        m_cachedWindowedModeHeight = m_properties.height;

        log(LogLevel::Info, fmt::format("GLFW window initialised: title='{0}', size={1}x{2}, display='{3}'\n", m_properties.title, m_properties.width, m_properties.height, getCurrentMonitor().name), "Window");

        s_windowCount++;
        
        return;
    }

    Window::~Window()
    {
        glfwDestroyWindow(m_glfwWindow);
        s_windowCount--;

        if (s_windowCount == 0)
        {
            int monitorCount;
            GLFWmonitor ** monitors = glfwGetMonitors(&monitorCount);
            for (int idx = 0; idx < monitorCount; ++idx)
                delete reinterpret_cast<MonitorInfo *>(glfwGetMonitorUserPointer(monitors[idx]));
                
            glfwTerminate();
        }
    }

    GLFWwindow * Window::getGLFWHandle() const
    {
        return m_glfwWindow;
    }

    bool Window::getIsHidden() const
    {
        return glfwGetWindowAttrib(m_glfwWindow, GLFW_VISIBLE) == GLFW_FALSE;
    }

    void Window::setIsHidden(bool isHidden)
    {
        if (m_properties.mode != WindowMode::Windowed)
            return;
        if (isHidden)
            glfwHideWindow(m_glfwWindow);
        else
            glfwShowWindow(m_glfwWindow);
            
        m_properties.isHidden = isHidden;
    }

    bool Window::getIsFocused() const
    {
        return glfwGetWindowAttrib(m_glfwWindow, GLFW_FOCUSED) == GLFW_TRUE;
    }

    void Window::giveFocus()
    {
        glfwFocusWindow(m_glfwWindow);
    }

    void Window::requestAttention() const
    {
        glfwRequestWindowAttention(m_glfwWindow);
    }
    
    bool Window::getIsResizable() const
    {
        return glfwGetWindowAttrib(m_glfwWindow, GLFW_RESIZABLE) == GLFW_TRUE;
    }

    void Window::setIsResizable(bool isResizable)
    {
        glfwSetWindowAttrib(m_glfwWindow, GLFW_RESIZABLE, isResizable ? GLFW_TRUE : GLFW_FALSE);
        m_properties.isResizable = isResizable;
    }

    bool Window::getIsDecorated() const
    {
        return glfwGetWindowAttrib(m_glfwWindow, GLFW_DECORATED) == GLFW_TRUE;
    }

    void Window::setIsDecorated(bool isDecorated)
    {
        glfwSetWindowAttrib(m_glfwWindow, GLFW_DECORATED, isDecorated ? GLFW_TRUE : GLFW_FALSE);
        m_properties.isDecorated = isDecorated;
    }

    bool Window::getIsMinimised() const
    {
        return m_isMinimised;
    }

    void Window::setIsMinimised(bool isMinimised)
    {
        if (isMinimised)
            glfwIconifyWindow(m_glfwWindow);
        else
            glfwRestoreWindow(m_glfwWindow);
    }

    const std::string Window::getTitle() const
    {
        return m_properties.title;
    }

    void Window::setTitle(const std::string & title)
    {
        glfwSetWindowTitle(m_glfwWindow, title.data());
    }
    
    void Window::setIcon(const Texture * iconTexture)
    {
        GLFWimage glfwImage = toGlfwImage(iconTexture);
        glfwSetWindowIcon(m_glfwWindow, iconTexture == nullptr ? 0 : 1, &glfwImage);
    }
    
    void Window::setIcon(const std::vector<Texture *> & iconTextureCandidates)
    {
        if (iconTextureCandidates.size() == 0)
            glfwSetWindowIcon(m_glfwWindow, 0, nullptr);

        std::vector<GLFWimage> glfwImages(iconTextureCandidates.size());
        for (size_t idx = 0; idx < iconTextureCandidates.size(); ++idx)
            glfwImages[idx] = toGlfwImage(iconTextureCandidates[idx]);

        glfwSetWindowIcon(m_glfwWindow, static_cast<int>(glfwImages.size()), glfwImages.data());
    }
    
    void Window::setPosition(uint16_t left, uint16_t top)
    {
        glfwSetWindowPos(m_glfwWindow, static_cast<int>(left), static_cast<int>(top));
    }

    uint16_t Window::getWidth() const
    {
        return m_properties.width;
    }

    uint16_t Window::getHeight() const
    {
        return m_properties.height;
    }

    float Window::getAspectRatio() const
    {
        return static_cast<float>(m_properties.width) / m_properties.height;
    }

    std::string Window::getClipboardString() const
    {
        const char * text = glfwGetClipboardString(NULL);
        if (text == NULL)
            return "";
        return std::string(text);
    }

    void Window::setClipboardString(const std::string & string) const
    {
        glfwSetClipboardString(NULL, "A string with words in it");
    }

    void Window::pollEvents()
    {
        for (auto & keyState : m_keyStates)
            keyState.second.reset();
        m_mouseState.reset();
        glfwPollEvents();
    }

    const KeyState & Window::getKeyState(Key key)
    {
        return m_keyStates[key];
    }

    const MouseState & Window::getMouseState() const
    {
        return m_mouseState;
    }

    bool Window::getShouldClose() const
    {
        return glfwWindowShouldClose(m_glfwWindow);
    }

    WindowGraphicsContext & Window::getGraphicsContext()
    {
        return m_graphicsContext;
    }

    void Window::setMode(WindowMode mode)
    {
        if (m_properties.mode == mode)
            return;

        if (m_properties.mode == WindowMode::Windowed)
        {
            int xpos, ypos;
            glfwGetWindowPos(m_glfwWindow, &xpos, &ypos);
            m_cachedWindowedModePosLeft = static_cast<uint16_t>(xpos);
            m_cachedWindowedModePosTop = static_cast<uint16_t>(ypos);
            m_cachedWindowedModeWidth = m_properties.width;
            m_cachedWindowedModeHeight = m_properties.height;
        }

        switch (mode)
        {
        case WindowMode::Windowed:
        {
            glfwSetWindowMonitor(m_glfwWindow, nullptr, 0, 0, static_cast<int>(m_cachedWindowedModeWidth), static_cast<int>(m_cachedWindowedModeHeight), GLFW_DONT_CARE);
            const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            setPosition(m_cachedWindowedModePosLeft, m_cachedWindowedModePosTop);
            break;
        }

        case WindowMode::Fullscreen:
            glfwSetWindowMonitor(m_glfwWindow, glfwGetPrimaryMonitor(), 0, 0, static_cast<int>(m_cachedWindowedModeWidth), static_cast<int>(m_cachedWindowedModeWidth), GLFW_DONT_CARE);
            m_properties.isHidden = false;
            break;

        case WindowMode::WindowedFullscreen:
        {
            const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            glfwSetWindowMonitor(m_glfwWindow, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
            m_properties.isHidden = false;
            break;
        }
        }

        m_properties.mode = mode;
    }

    void Window::maximizeSize()
    {
        glfwMaximizeWindow(m_glfwWindow);
    }

    void Window::requestUserAttention()
    {
        glfwRequestWindowAttention(m_glfwWindow);
    }

    void Window::setCursorInputMode(CursorInputMode cursorInputMode)
    {
        glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, toGlfwCursorInputMode(cursorInputMode));
        m_properties.cursorInputMode = cursorInputMode;
    }

    void Window::setCursorImageType(CursorShape cursorImageType)
    {
        glfwSetCursor(m_glfwWindow, s_glfwCursors[cursorImageType]);
    }

    void Window::setCursorPosition(Vector2 position)
    {
        glfwSetCursorPos(m_glfwWindow, static_cast<double>(position.x), static_cast<double>(position.y));
    }

    Vector2 Window::getNormalisedDeviceCoordinates(Vector2 screenPosition) const
    {
        return (screenPosition / Vector2(m_properties.width, m_properties.height)) * 2.0f - Vector2::one;
    }

    const MonitorInfo & Window::getCurrentMonitor() const
    {
        GLFWmonitor * monitor = glfwGetWindowMonitor(m_glfwWindow);
        if (monitor == NULL)
            monitor = glfwGetPrimaryMonitor();

        return *reinterpret_cast<MonitorInfo *>(glfwGetMonitorUserPointer(monitor));
    }
    
    void Window::setCurrentMonitor(const UUID & monitorUuid)
    {
        if (m_properties.mode == WindowMode::Windowed)
            return;

        int count;
        GLFWmonitor** monitors = glfwGetMonitors(&count);

        for (int idx = 0; idx < count; ++idx)
            if (reinterpret_cast<MonitorInfo *>(glfwGetMonitorUserPointer(monitors[idx]))->uuid == monitorUuid)
                if (m_properties.mode == WindowMode::WindowedFullscreen)
                {
                    const GLFWvidmode * mode = glfwGetVideoMode(monitors[idx]);
                    glfwSetWindowMonitor(m_glfwWindow, monitors[idx], 0, 0, mode->width, mode->height, mode->refreshRate);
                }
                else
                {
                    glfwSetWindowMonitor(m_glfwWindow, monitors[idx], 0, 0, static_cast<int>(m_properties.width), static_cast<int>(m_properties.height), GLFW_DONT_CARE);
                }
    }

    std::vector<MonitorInfo *> Window::getAvailableMonitors()
    {
        int count;
        GLFWmonitor** monitors = glfwGetMonitors(&count);

        std::vector<MonitorInfo *> monitorInfos(count);

        for (int idx = 0; idx < count; ++idx)
            monitorInfos[idx] = reinterpret_cast<MonitorInfo *>(glfwGetMonitorUserPointer(monitors[idx]));

        return monitorInfos;
    }

    Window::EventOnMonitor Window::s_eventOnMonitor;

    Window::EventOnMonitor & Window::getEventOnMonitor()
    {
        return s_eventOnMonitor;
    }

    Window::EventOnFocusChanged & Window::getEventOnFocusChanged()
    {
        return m_eventOnFocusChanged;
    }

    Window::EventOnIconificationChanged & Window::getEventOnIconificationChanged()
    {
        return m_eventOnIconificationChanged;
    }

    Window::EventOnResized & Window::getEventOnResized()
    {
        return m_eventOnResized;
    }

    Window::EventOnKey & Window::getEventOnKey()
    {
        return m_eventOnKey;
    }

    Window::EventOnChar & Window::getEventOnChar()
    {
        return m_eventOnChar;
    }

    Window::EventOnMouseMove & Window::getEventOnMouseMove()
    {
        return m_eventOnMouseMove;
    }

    Window::EventOnMouseEnterExit & Window::getEventOnMouseEnterExit()
    {
        return m_eventOnMouseEnterExit;
    }

    Window::EventOnMouseButton & Window::getEventOnMouseButton()
    {
        return m_eventOnMouseButton;
    }

    void Window::glfwCallback_FramebufferResize(GLFWwindow * window, int width, int height)
    {
        Window * userWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        
        userWindow->m_properties.width = static_cast<uint16_t>(width);
        userWindow->m_properties.height = static_cast<uint16_t>(height);

        userWindow->m_eventOnResized.broadcast(static_cast<uint32_t>(width), static_cast<uint32_t>(height));

        log(LogLevel::Trace, fmt::format("Window framebuffer resized: {0} x {1}\n", width, height), "Platform");
    }
    
    void Window::glfwCallback_Monitor(GLFWmonitor * monitor, int event)
    {
        MonitorInfo * monitorInfo = nullptr;

        if (event == GLFW_CONNECTED)
        {
            monitorInfo = setupGlfwMonitorUserPointer(monitor);

            s_eventOnMonitor.broadcast(*monitorInfo, true);

            std::string availableResolutionsString;
            for (const MonitorResolution & monitorResolution : monitorInfo->availableResolutions)
                availableResolutionsString += fmt::format("\t{0} x {1} ({2}Hz)\n", monitorResolution.width, monitorResolution.height, monitorResolution.refreshRate);

            log(LogLevel::Trace, fmt::format("Monitor connection event: UUID='{0}', name='{1}', available resolutions:\n{2}", monitorInfo->uuid.getString(), monitorInfo->name, availableResolutionsString), "Platform");
        }
        else
        {
            monitorInfo = reinterpret_cast<MonitorInfo *>(glfwGetMonitorUserPointer(monitor));
            
            s_eventOnMonitor.broadcast(*monitorInfo, false);
            
            log(LogLevel::Trace, fmt::format("Monitor disconnection event: UUID='{0}', name='{1}'\n", monitorInfo->uuid.getString(), monitorInfo->name), "Platform");

            delete monitorInfo;
        }
    }

    void Window::glfwCallback_Error(int error, const char * description)
    {
        log(LogLevel::Error, fmt::format("GLFW Error {0}: {1}\n", error, std::string(description)), "Platform");
    }

    void Window::glfwCallback_WindowFocus(GLFWwindow * window, int focused)
    {
        Window * userWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

        userWindow->m_eventOnFocusChanged.broadcast(focused == GLFW_TRUE);
    }

    void Window::glfwCallback_WindowIconify(GLFWwindow * window, int iconified)
    {
        Window * userWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

        userWindow->m_isMinimised = iconified == GLFW_TRUE;

        userWindow->m_eventOnIconificationChanged.broadcast(userWindow->m_isMinimised);
    }

    void Window::glfwCallback_WindowMaximize(GLFWwindow * window, int maximized)
    {
    }

    void Window::glfwCallback_WindowSize(GLFWwindow * window, int width, int height)
    {
    }

    void Window::glfwCallback_Key(GLFWwindow * window, int glfwKey, int scancode, int glfwAction, int glfwModBits)
    {
        Window * userWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

        const Key key = fromGlfwKey(glfwKey);
        const KeyAction keyAction = fromGlfwAction(glfwAction);
        const uint8_t modifiers = static_cast<uint8_t>(glfwModBits);

        userWindow->m_keyStates[key].action = keyAction;
        userWindow->m_keyStates[key].isPressed = keyAction == KeyAction::Pressed || keyAction == KeyAction::Repeating;
        userWindow->m_keyStates[key].modifiers = modifiers;

        userWindow->m_eventOnKey.broadcast(key, keyAction, modifiers);

        //log(LogLevel::Trace, fmt::format("Key event: key={0}, action={1}, isPressed={2}, modifiers={3}\n", static_cast<int>(key), static_cast<int>(keyAction), userWindow->m_keyStates[key].isPressed, modifiers), "Input");
    }

    void Window::glfwCallback_Char(GLFWwindow * window, unsigned int codepoint)
    {
        Window * userWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

        userWindow->m_eventOnChar.broadcast(codepoint);
    }

    void Window::glfwCallback_CursorPos(GLFWwindow * window, double xpos, double ypos)
    {
        Window * userWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

        Vector2 newCursorPosition(xpos, ypos);
        userWindow->m_mouseState.cursorDelta = userWindow->m_mouseState.cursorPosition - newCursorPosition;
        userWindow->m_mouseState.cursorPosition = newCursorPosition;

        userWindow->m_eventOnMouseMove.broadcast(userWindow->m_mouseState.cursorPosition, userWindow->m_mouseState.cursorDelta);
    }

    void Window::glfwCallback_CursorEnter(GLFWwindow * window, int entered)
    {
        Window * userWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

        userWindow->m_eventOnMouseEnterExit.broadcast(entered == GLFW_TRUE);
    }

    void Window::glfwCallback_MouseButton(GLFWwindow * window, int glfwButton, int glfwAction, int glfwModBits)
    {
        Window * userWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

        MouseButton mouseButton = fromGlfwMouseButton(glfwButton);
        MouseButtonState & state = userWindow->m_mouseState.buttons[mouseButton];
        state.action = (glfwAction == GLFW_PRESS) ? MouseButtonAction::Pressed : MouseButtonAction::Released;
        state.isPressed = state.action == MouseButtonAction::Pressed;

        userWindow->m_eventOnMouseButton.broadcast(mouseButton, state);
        
        //log(LogLevel::Trace, fmt::format("Mouse button event: button={0}, action={1}, isPressed={2}, modifiers={3}\n", static_cast<int>(mouseButton), static_cast<int>(state.action), state.isPressed, glfwModBits), "Input");
    }

    void Window::glfwCallback_Scroll(GLFWwindow * window, double xoffset, double yoffset)
    {
        Window * userWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

        Vector2 newscrollPosition(xoffset, yoffset);
        userWindow->m_mouseState.scrollDelta = userWindow->m_mouseState.scrollPosition - newscrollPosition;
        userWindow->m_mouseState.scrollPosition = newscrollPosition;
    }

    void Window::glfwCallback_Drop(GLFWwindow * window, int count, const char ** paths)
    {
        Window * userWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

        std::vector<std::string> pathStrings(count);

        for (size_t idx = 0; idx < pathStrings.size(); ++idx)
            pathStrings.emplace_back(paths[idx]);

        userWindow->m_eventOnDrop.broadcast(pathStrings);
    }
}