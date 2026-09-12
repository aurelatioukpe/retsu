#ifndef REAL_TIME_DISPLAY_HPP_
#define REAL_TIME_DISPLAY_HPP_

#include <string>
#include <vector>
#include "../core/Color.hpp"

#ifdef USE_SFML
    #include <SFML/Graphics.hpp>
#endif

class RealTimeDisplay {
private:
#ifdef USE_SFML
    sf::RenderWindow m_window;
    sf::Texture m_texture;
    sf::Sprite m_sprite;
    std::vector<sf::Uint8> m_pixels;
#endif
    unsigned int m_width;
    unsigned int m_height;
    std::string m_title;
    bool m_active;

    bool processEvents();

public:
    RealTimeDisplay(unsigned int width, unsigned int height, const std::string& title);
    ~RealTimeDisplay();
    bool init();
    bool isActive() const;
    bool update(const std::vector<Color>& imageData);
    void setPixel(unsigned int x, unsigned int y, const Color& color);
    void close();
};

#endif /* REAL_TIME_DISPLAY_HPP_ */