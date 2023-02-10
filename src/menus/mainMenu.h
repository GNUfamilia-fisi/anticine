#pragma once

#include <string>
#include <Windows.h>
#include <json.hpp>

namespace gnu {

    void logoDisplay3D(gnu::color color);
    void displayLogo2D(gnu::color color);
    void cinemaListDisplay(nlohmann::json closeData, size_t current, size_t namePos, size_t showSize);
    void cargandoDisplay();
    void displayDate(std::string fulldate, short opt);
    void chooseCinema();
    void carteleraFecha();

} // namespace gnu
