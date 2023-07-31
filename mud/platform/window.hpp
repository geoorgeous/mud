#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <string>
#include <unordered_map>

#include "event.hpp"
#include "graphics/texture.hpp"
#include "graphics/window_graphics_context.hpp"
#include "math/vector.hpp"
#include "utils/uuid.hpp"

struct GLFWcursor;
struct GLFWmonitor;
struct GLFWwindow;

namespace mud
{
    enum class WindowMode
    {
        Windowed,
        Fullscreen,
        WindowedFullscreen
    };

    enum class Key
    {
        Unknown = -1,

        Escape,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,

        Num0,
        Num1,
        Num2,
        Num3,
        Num4,
        Num5,
        Num6,
        Num7,
        Num8,
        Num9,

        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,

        GraveAccent,
        Minus,
        Equal,
        Backspace,

        Tab,
        LeftBracket,
        RightBracket,
        Enter,

        CapsLock,
        SemiColon,
        Apostrophe,
        Hash,

        LeftShift,
        BackwardSlash,
        Comma,
        Period,
        ForwardSlash,
        RightShift,

        LeftControl,
        LeftAlt,
        Space,
        RightAlt,
        RightControl,

        Insert,
        Delete,
        Home,
        End,
        PageUp,
        PageDown,

        Left,
        Right,
        Up,
        Down,

        NumPad0,
        NumPad1,
        NumPad2,
        NumPad3,
        NumPad4,
        NumPad5,
        NumPad6,
        NumPad7,
        NumPad8,
        NumPad9,
        NumPadDivide,
        NumPadMultiply,
        NumPadMinus,
        NumPadPlus,
        NumPadPeriod,
        NumPadEnter,
    };

    enum class KeyAction
    {
        None,
        Pressed,
        Released,
        Repeating
    };

    enum class KeyModifier
    {
        Shift       = 1 << 0,
        Control     = 1 << 1,
        Alt         = 1 << 2,
        CapsLock    = 1 << 3,
        NumLock     = 1 << 4
    };

    enum class MouseButton
    {
        Unknown = -1,
        Left,
        Right,
        Middle
    };

    enum class MouseButtonAction
    {
        None,
        Pressed,
        Released
    };

    enum class CursorInputMode
    {
        Normal,
        Hidden,
        Disabled
    };

    enum class CursorShape
    {
        Default,
        TextInput,
        Crosshair,
        Hand,
        ResizeHorizontal,
        ResizeVertical
    };

    struct KeyState
    {
        KeyAction action;
        bool isPressed;
        uint8_t modifiers;

        void reset();
    };

    struct MouseButtonState
    {
        MouseButtonAction action;
        bool isPressed;
    };

    struct MouseState
    {
        Vector2 cursorPosition;
        Vector2 cursorDelta;
        Vector2 scrollPosition;
        Vector2 scrollDelta;
        std::unordered_map<MouseButton, MouseButtonState> buttons{
            {MouseButton::Left, MouseButtonState{}},
            {MouseButton::Right, MouseButtonState{}},
            {MouseButton::Middle, MouseButtonState{}}
        };

        void reset();
    };

    struct WindowProperties
    {
        WindowProperties();

        WindowMode mode;
        uint16_t width;
        uint16_t height;
        std::string title;
        std::vector<Texture *> iconCandidates;
        CursorInputMode cursorInputMode;
        bool isHidden;
        bool isResizable;
        bool isDecorated;
    };

    struct MonitorResolution
    {
        uint16_t width;
        uint16_t height;
        uint16_t refreshRate;
    };

    struct MonitorInfo
    {
        UUID uuid;
        std::string name;
        std::vector<MonitorResolution> availableResolutions;
    };

    class Window
    {
    public:

        Window(const WindowProperties & properties);

        ~Window();

        Window(const Window &) = delete;
        Window & operator=(const Window &) = delete;

        GLFWwindow * getGLFWHandle() const;

        bool getIsHidden() const;

        void setIsHidden(bool isHidden);

        bool getIsFocused() const;

        void giveFocus();

        void requestAttention() const;

        bool getIsResizable() const;

        void setIsResizable(bool isResizable);

        bool getIsDecorated() const;

        void setIsDecorated(bool isDecorated);

        bool getIsMinimised() const;

        void setIsMinimised(bool isMinimised);

        const std::string getTitle() const;

        void setTitle(const std::string & title);

        void setIcon(const Texture * iconTexture);

        void setIcon(const std::vector<Texture *> & iconTextureCandidates);

        void setPosition(uint16_t left, uint16_t top);

        uint16_t getWidth() const;

        uint16_t getHeight() const;

        float getAspectRatio() const;

        std::string getClipboardString() const;

        void setClipboardString(const std::string & string) const;

        void pollEvents();

        const KeyState & getKeyState(Key key);

        const MouseState & getMouseState() const;

        bool getShouldClose() const;

        WindowGraphicsContext & getGraphicsContext();

        void setMode(WindowMode mode);

        void maximizeSize();

        void requestUserAttention();

        void setCursorInputMode(CursorInputMode cursorInputMode);

        void setCursorImageType(CursorShape cursorImageType);

        void setCursorPosition(Vector2 position);

        Vector2 getNormalisedDeviceCoordinates(Vector2 screenPosition) const;

        const MonitorInfo & getCurrentMonitor() const;

        void setCurrentMonitor(const UUID & monitorUuid);

        static std::vector<MonitorInfo *> getAvailableMonitors();

        typedef Event<const MonitorInfo &, bool> EventOnMonitor;
        static EventOnMonitor & getEventOnMonitor();

        typedef Event<bool> EventOnFocusChanged;
        EventOnFocusChanged & getEventOnFocusChanged();

        typedef Event<bool> EventOnIconificationChanged;
        EventOnIconificationChanged & getEventOnIconificationChanged();

        typedef Event<uint32_t, uint32_t> EventOnResized;
        EventOnResized & getEventOnResized();

        typedef Event<Key, KeyAction, uint8_t> EventOnKey;
        EventOnKey & getEventOnKey();

        typedef Event<unsigned int> EventOnChar;
        EventOnChar & getEventOnChar();

        typedef Event<Vector2, Vector2> EventOnMouseMove;
        EventOnMouseMove & getEventOnMouseMove();

        typedef Event<bool> EventOnMouseEnterExit;
        EventOnMouseEnterExit & getEventOnMouseEnterExit();

        typedef Event<MouseButton, MouseButtonState> EventOnMouseButton;
        EventOnMouseButton & getEventOnMouseButton();

        typedef Event<std::vector<std::string>> EventOnDrop;
        EventOnDrop & getEventOnDrop();

    private:

        static void glfwCallback_Monitor(GLFWmonitor * monitor, int event);

        static void glfwCallback_Error(int error, const char * description);

        static void glfwCallback_WindowFocus(GLFWwindow * window, int focused);

        static void glfwCallback_WindowIconify(GLFWwindow * window, int iconified);

        static void glfwCallback_WindowMaximize(GLFWwindow * window, int maximized);

        static void glfwCallback_WindowSize(GLFWwindow * window, int width, int height);

        static void glfwCallback_FramebufferResize(GLFWwindow * window, int width, int height);

        static void glfwCallback_Key(GLFWwindow * window, int key, int scancode, int glfwAction, int glfwMods);

        static void glfwCallback_Char(GLFWwindow * window, unsigned int codepoint);

        static void glfwCallback_CursorPos(GLFWwindow * window, double xpos, double ypos);

        static void glfwCallback_CursorEnter(GLFWwindow * window, int entered);

        static void glfwCallback_MouseButton(GLFWwindow * window, int glfwButton, int glfwAction, int glfwMods);

        static void glfwCallback_Scroll(GLFWwindow * window, double xoffset, double yoffset);

        static void glfwCallback_Drop(GLFWwindow * window, int count, const char ** paths);

        static uint16_t s_windowCount;

        GLFWwindow * m_glfwWindow;

        WindowProperties m_properties;
        uint16_t m_cachedWindowedModePosLeft;
        uint16_t m_cachedWindowedModePosTop;
        uint16_t m_cachedWindowedModeWidth;
        uint16_t m_cachedWindowedModeHeight;
        bool m_isMinimised;

        WindowGraphicsContext m_graphicsContext;

        std::unordered_map<Key, KeyState> m_keyStates;

        MouseState m_mouseState;

        static std::unordered_map<CursorShape, GLFWcursor *> s_glfwCursors;

        static EventOnMonitor s_eventOnMonitor;
        EventOnFocusChanged m_eventOnFocusChanged;
        EventOnIconificationChanged m_eventOnIconificationChanged;
        EventOnResized m_eventOnResized;
        EventOnKey m_eventOnKey;
        EventOnChar m_eventOnChar;
        EventOnMouseMove m_eventOnMouseMove;
        EventOnMouseEnterExit m_eventOnMouseEnterExit;
        EventOnMouseButton m_eventOnMouseButton;
        EventOnDrop m_eventOnDrop;
    };
}

#endif