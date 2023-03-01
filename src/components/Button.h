#pragma once

#include <utf8.hpp>
#include "./Box.h"
#include "../consoleUtils.h"

namespace gnu {

#define BUTTON_TEXT_DEFAULT_COLOR style::rgb({ 161, 148, 202 })
#define BUTTON_BORDER_DEFAULT_COLOR ANTICINE_LOGO_COLOR

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
        this->text_color = BUTTON_TEXT_DEFAULT_COLOR;
        this->defaultBorderColor = false;
        this->border_color = BUTTON_BORDER_DEFAULT_COLOR;
    }

    Button(std::string content, gnu::vec2d size) : Box(size) {
        this->content = content;
        this->transparent = true;
        this->showBorder = false;
        this->defaultFontColor = false;
        this->text_color = BUTTON_TEXT_DEFAULT_COLOR;
        this->defaultBorderColor = false;
        this->border_color = BUTTON_BORDER_DEFAULT_COLOR;
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
