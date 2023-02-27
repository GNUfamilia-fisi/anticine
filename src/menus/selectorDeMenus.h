#pragma once

#include <string>
#include <cmath>
#include <fstream>
#include <json.hpp>
#include <cstdlib>
#include <ctime>

#include "../consoleUtils.h"
#include "../services.h"

// Menús
#include "menuElegirCine.h"
#include "menuCartelera.h"
#include "menuDetallePelicula.h"
#include "menuFormulario.h"
#include "menuAsientos.h"

#include "globales.h"

using json = nlohmann::json;

namespace gnu {

void menuSelector() {
    json menuRawData = gnu::apifetch("/cines/cercanos");
    g_cineID = menuRawData["nearest_id"].get<std::string>();

    std::string menuID = gnu::menuCartelera();

    while (true) {
        if (menuID == "selectorCineAux") {
            menuID = gnu::menuElegirCine();
        }
        if (menuID == "menuCartelera") {
            menuID = gnu::menuCartelera();
        }
        if (menuID == "menuDetalles") {
            menuID = gnu::menuDetalles();
        }
        if (menuID == "submenuSeleccionarFecha") {
            menuID = gnu::submenuSeleccionarFecha();
        }
        if (menuID == "Asientos") {
            menuID = gnu::menuAsientos();
        }
        if (menuID == "menuFormulario") {
            menuID = gnu::menuFormulario();
        }
        if (menuID == "exit") {
            break;
        }
    }
}

} // namespace gnu
