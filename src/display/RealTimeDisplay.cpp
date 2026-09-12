#include "../../include/display/RealTimeDisplay.hpp"
#include <iostream>

RealTimeDisplay::RealTimeDisplay(unsigned int width, unsigned int height, const std::string& title)
    : m_width(width), m_height(height), m_title(title), m_active(false) {
#ifdef USE_SFML
    m_pixels.resize(width * height * 4, 0);
#endif
}

RealTimeDisplay::~RealTimeDisplay() {
    close();
}

bool RealTimeDisplay::init() {
#ifdef USE_SFML
    try {
        m_window.create(sf::VideoMode(m_width, m_height), m_title);
        m_window.setFramerateLimit(30);
        m_texture.create(m_width, m_height);
        m_sprite.setTexture(m_texture);
        m_active = true;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Erreur lors de l'initialisation de l'affichage SFML: " << e.what() << std::endl;
        return false;
    }
#else
    std::cerr << "SFML non disponible. L'affichage en temps réel est désactivé." << std::endl;
    return false;
#endif
}

bool RealTimeDisplay::processEvents() {
#ifdef USE_SFML
    if (!m_active) {
        return false;
    }
    sf::Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            close();
            return false;
        }
    }
    return true;
#else
    return false;
#endif
}

void RealTimeDisplay::setPixel([[maybe_unused]] unsigned int x,
                               [[maybe_unused]] unsigned int y,
                               [[maybe_unused]] const Color& color) {
#ifdef USE_SFML
    if (x >= m_width || y >= m_height || !m_active) {
        return;
    }
    unsigned int index = (y * m_width + x) * 4;
    m_pixels[index] = static_cast<sf::Uint8>(color.getR8());
    m_pixels[index + 1] = static_cast<sf::Uint8>(color.getG8());
    m_pixels[index + 2] = static_cast<sf::Uint8>(color.getB8());
    m_pixels[index + 3] = 255;
#endif
}

bool RealTimeDisplay::update([[maybe_unused]] const std::vector<Color>& imageData) {
#ifdef USE_SFML
    if (!m_active) {
        return false;
    }
    if (!processEvents()) {
        return false;
    }
    if (!imageData.empty()) {
        for (unsigned int y = 0; y < m_height; ++y) {
            for (unsigned int x = 0; x < m_width; ++x) {
                const Color& color = imageData[y * m_width + x];
                setPixel(x, y, color);
            }
        }
    }
    m_texture.update(m_pixels.data());
    m_window.clear();
    m_window.draw(m_sprite);
    m_window.display();
    return true;
#else
    return false;
#endif
}

void RealTimeDisplay::close() {
#ifdef USE_SFML
    if (m_window.isOpen()) {
        m_window.close();
    }
    m_active = false;
#endif
}

bool RealTimeDisplay::isActive() const {
#ifdef USE_SFML
    return m_active && m_window.isOpen();
#else
    return false;
#endif
}
