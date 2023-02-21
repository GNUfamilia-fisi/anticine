#pragma once

#include "Box.h"

namespace gnu {

#define MAX_SYNOPSIS_LENGTH 500

class MovieCard : public Box {
public:
    json data;
    char type;

    MovieCard(json data, gnu::vec2d size) : Box({ 5, 5 }) {
        this->data = data;
        this->size = size;
        this->updateData(data);
    }

    void updateData(json newData) {
        this->content = newData["title"].get<std::string>();
    }

    void cardDraw() {
        if (this->content != "") this->draw();
        else {
            this->transparent = true;
            this->showBorder = false;
            this->flushBorders();
            this->draw();
        }
    }
};

}
