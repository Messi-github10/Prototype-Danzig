//
// Created by nicefold on 2026/4/17.
//

#include "Window.hpp"
#include "Error.hpp"
#include "Logger.hpp"
#include <SDL2/SDL.h>

Window::Window(int width, int height, const std::string& title, bool fullscreen, Uint32 flags)
    : m_window(nullptr)
    , m_width(width)
    , m_height(height)
    , m_isFullscreen(fullscreen)
    , m_shouldClose(false)
{
    // 根据fullscreen参数调整标志
    if (fullscreen) {
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    } else {
        flags &= ~SDL_WINDOW_FULLSCREEN_DESKTOP;
    }

    m_window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        flags
    );

    SDL_CHECK(m_window != nullptr, "Failed to create SDL window");

    LOG_INFO("Window created: {}x{} '{}' (fullscreen: {}, flags: 0x{:x})",
             width, height, title, fullscreen, flags);
}

Window::~Window()
{
    if (m_window) {
        SDL_DestroyWindow(m_window);
        LOG_INFO("Window destroyed");
    }
}

Window::Window(Window&& other) noexcept
    : m_window(other.m_window)
    , m_width(other.m_width)
    , m_height(other.m_height)
    , m_isFullscreen(other.m_isFullscreen)
    , m_shouldClose(other.m_shouldClose)
{
    other.m_window = nullptr;
    other.m_width = 0;
    other.m_height = 0;
    other.m_isFullscreen = false;
    other.m_shouldClose = false;
}

Window& Window::operator=(Window&& other) noexcept
{
    if (this != &other) {
        if (m_window) {
            SDL_DestroyWindow(m_window);
        }

        m_window = other.m_window;
        m_width = other.m_width;
        m_height = other.m_height;
        m_isFullscreen = other.m_isFullscreen;
        m_shouldClose = other.m_shouldClose;

        other.m_window = nullptr;
        other.m_width = 0;
        other.m_height = 0;
        other.m_isFullscreen = false;
        other.m_shouldClose = false;
    }
    return *this;
}

void Window::toggleFullscreen()
{
    if (!m_window) return;

    Uint32 flags = SDL_GetWindowFlags(m_window);
    bool isFullscreen = (flags & SDL_WINDOW_FULLSCREEN) || (flags & SDL_WINDOW_FULLSCREEN_DESKTOP);

    if (SDL_SetWindowFullscreen(m_window, isFullscreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP) == 0) {
        m_isFullscreen = !isFullscreen;
        LOG_INFO("Window toggled fullscreen: {}", m_isFullscreen);
    } else {
        LOG_ERROR("Failed to toggle fullscreen: {}", SDL_GetError());
    }
}

bool Window::pollEvents()
{
    if (!m_window) return false;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                m_shouldClose = true;
                LOG_INFO("Window received quit event");
                break;

            case SDL_WINDOWEVENT:
                if (event.window.windowID == SDL_GetWindowID(m_window)) {
                    handleWindowEvent(event.window);
                }
                break;

            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    m_shouldClose = true;
                    LOG_INFO("Escape key pressed, closing window");
                }
                break;

            default:
                // Ignore other event types
                break;
        }
    }

    return !m_shouldClose;
}

bool Window::shouldClose() const
{
    return m_shouldClose;
}

void Window::resize(int width, int height)
{
    if (!m_window) return;

    SDL_SetWindowSize(m_window, width, height);
    m_width = width;
    m_height = height;

    LOG_INFO("Window resized to {}x{}", width, height);
}

void Window::setTitle(const std::string& title)
{
    if (!m_window) return;

    SDL_SetWindowTitle(m_window, title.c_str());
    LOG_INFO("Window title set to: {}", title);
}

int Window::getWidth() const
{
    return m_width;
}

int Window::getHeight() const
{
    return m_height;
}

bool Window::isFullscreen() const
{
    return m_isFullscreen;
}

bool Window::isMinimized() const
{
    if (!m_window) return false;

    Uint32 flags = SDL_GetWindowFlags(m_window);
    return (flags & SDL_WINDOW_MINIMIZED) != 0;
}

SDL_Window* Window::getSDLWindow() const
{
    return m_window;
}

// Private helper method
void Window::handleWindowEvent(const SDL_WindowEvent& event)
{
    switch (event.event) {
        case SDL_WINDOWEVENT_RESIZED:
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            m_width = event.data1;
            m_height = event.data2;
            LOG_DEBUG("Window size changed: {}x{}", m_width, m_height);
            break;

        case SDL_WINDOWEVENT_MINIMIZED:
        case SDL_WINDOWEVENT_RESTORED:
        case SDL_WINDOWEVENT_FOCUS_GAINED:
        case SDL_WINDOWEVENT_FOCUS_LOST:
            // 这些事件只记录调试信息
            {
                const char* msg = nullptr;
                switch (event.event) {
                    case SDL_WINDOWEVENT_MINIMIZED: msg = "Window minimized"; break;
                    case SDL_WINDOWEVENT_RESTORED: msg = "Window restored"; break;
                    case SDL_WINDOWEVENT_FOCUS_GAINED: msg = "Window gained focus"; break;
                    case SDL_WINDOWEVENT_FOCUS_LOST: msg = "Window lost focus"; break;
                    default: break; // 不会发生，因为外层switch已经过滤
                }
                if (msg) {
                    LOG_DEBUG("{}", msg);
                }
            }
            break;

        case SDL_WINDOWEVENT_CLOSE:
            m_shouldClose = true;
            LOG_INFO("Window close event received");
            break;

        default:
            // Ignore other window event types
            break;
    }
}
