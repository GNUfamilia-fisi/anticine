#pragma once

#include <style.hpp>
#include "Box.h"
#include "../menus/menuDetallePelicula.h"

namespace gnu {

#define MAX_SYNOPSIS_LENGTH 500

class MovieCard : public Box {
public:
    char type;

    MovieCard(json movieData, gnu::vec2d size) : Box({ 5, 5 }) {
        this->size = size;
        this->updateData(movieData);
    }

    void updateData(json newMovieData) {
        this->content = newMovieData["title"].get<std::string>();
        style::rgb averageThumbnailColor = {
            newMovieData["average_thumbnail_color"]["r"].get<unsigned char>(),
            newMovieData["average_thumbnail_color"]["g"].get<unsigned char>(),
            newMovieData["average_thumbnail_color"]["b"].get<unsigned char>()
        };
        this->setBoxColor(averageThumbnailColor);
        this->setFontColor(style::getComplementaryColor(averageThumbnailColor));
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
