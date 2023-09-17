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
#include "menuFormInicioSesion.h"
#include "menuFormRegistro.h"
#include "menuAsientos.h"
#include "menuCompraEntradas.h"

#include "globales.h"

using json = nlohmann::json;

namespace gnu {

void menuSelector() {
    json menuRawData = gnu::apifetch("/cines/cercanos");
    if (menuRawData["code"].get<int>() == 404) {
        g_cineID = "";
        g_cineData = json();
    }
    else {
        g_cineID = menuRawData["nearest_id"].get<std::string>();
        g_cineData = menuRawData["cinemas"][0]; // el primero es el más cercano
    }

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
        if (menuID == "menuFormularioRegistro") {
            menuID = gnu::menuFormularioRegistro();
        }
        if (menuID == "menuFormularioLogin") {
            menuID = gnu::menuFormularioLogin();
        }
        if (menuID == "menuCompraEntradas") {
            menuID = gnu::menuCompraEntradas();
        }
        if (menuID == "exit") {
            break;
        }
    }
}

} // namespace gnu
