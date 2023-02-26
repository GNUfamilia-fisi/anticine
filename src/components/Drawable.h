#pragma once

#include <string>
#include <vector>
#include <utf8.hpp>
#include "../consoleUtils.h"

namespace gnu {

class Drawable {
  public:
    gnu::vec2d size = { 5, 5 };
    gnu::vec2d position = { 0, 0 };

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
