#pragma once

#include <utf8.hpp>
#include "./Box.h"
#include "../consoleUtils.h"

namespace gnu {

/**
 * @brief Una clase para simplificar la creación de botones genéricos
 */
class Button : public Box {
  public:
    // La box se inicia según su contenido
    Button(std::string content) : Box({ int(utf8::str_length(content) + 4), 1 }) {
        this->content = content;
        this->transparent = true;
        this->showBorder = false;
        this->defaultFontColor = false;
        this->text_color = { 255, 138, 208 };
    }

    Button(std::string content, gnu::vec2d size) : Box(size) {
        this->content = content;
        this->transparent = true;
        this->showBorder = false;
        this->defaultFontColor = false;
        this->text_color = { 255, 138, 208 };
    }

    void focus() {
        this->defaultFontColor = false;
        // Color de focus, un rosado elegido por nuestro diseñador @PandaDiestro
        this->setFontColor(this->text_color);
        this->setBordersVisible(true);
    }

    void unfocus() {
        this->defaultFontColor = true;
        this->setBordersVisible(false);
    }

    void setFocus(bool focus) {
        focus ? this->focus() : this->unfocus();
    }

};

} // namespace gnu
