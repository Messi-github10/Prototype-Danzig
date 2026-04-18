//
// Created by nicefold on 2026/4/17.
//

#pragma once

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>
#include <string>
#include "common/NonCopyable.hpp"

class Window : private common::NonCopyable {
public:
    Window(int width, int height, const std::string& title = "Danzig", bool fullscreen = false,
           Uint32 flags = SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
    ~Window();

    Window(Window&& other) noexcept;
    Window& operator=(Window&& other) noexcept;

    void toggleFullscreen();
    bool pollEvents();
    [[nodiscard]] bool shouldClose() const;

    void resize(int width, int height);
    void setTitle(const std::string& title);

    [[nodiscard]] int getWidth() const;
    [[nodiscard]] int getHeight() const;
    [[nodiscard]] bool isFullscreen() const;
    [[nodiscard]] bool isMinimized() const;
    [[nodiscard]] SDL_Window* getSDLWindow() const;

private:
    void handleWindowEvent(const SDL_WindowEvent& event);

    SDL_Window* m_window;
    int m_width, m_height;
    bool m_isFullscreen;
    bool m_shouldClose;
};

