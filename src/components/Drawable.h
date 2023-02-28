#pragma once

#include "../consoleUtils.h"

namespace gnu {

/**
 * @brief Una clase para representar componentes posisionables y dibujables
 *
 * @note Es buena práctica heredar de esta clase para crear componentes dibujables.
 *       La función draw() debe ser implementada.
 */
class Drawable {
  public:
    gnu::vec2d size = { 5, 5 };
    gnu::vec2d position = { 0, 0 };

    Drawable() {}
    Drawable(gnu::vec2d size) {
        this->size = size;
    }
    void setPosition(gnu::vec2d position) {
        this->position = position;
    }

    void draw() = delete;

    void centerHorizontal() {
        this->position.x = (gnu::getConsoleSize().x - this->size.x) / 2;
    }
    void centerVertical() {
        this->position.y = (gnu::getConsoleSize().y - this->size.y) / 2;
    }
};

} // namespace gnu
