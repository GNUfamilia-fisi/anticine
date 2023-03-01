#pragma once

#include <string>
#include <json.hpp>
#include "consoleUtils.h"
#include "services.h"

using json = nlohmann::json;

namespace gnu {

// La URL de la API donde anticine obtiene sus datos
// Repositorio: https://github.com/GNUfamilia-fisi/anticine-server
std::string apiURL = "http://[2001:1388:19e8:4b8b:4aa9:3f91:9ea6:49e4]:6969";

/**
 * Hace una petición a una URL y devuelve su respuesta.
 *
 * Por defecto solo funciona con JSONs, por ejemplo,
 * un `fetch("www.google.com")` terminaría en error, pues
 * Google te devuelve una página web en formato HTML,
 * no texto en formato JSON.
 *
 * Por detrás, esta función usa gnu::exec() para ejecutar
 * el comando "curl", que sirve para hacer peticiones HTTP.
 *
 * @example
 * json response = gnu::fetch("https://api.anticine.up.railway.app")
 */
json fetch(std::string url) {
    std::string command = "curl -s " + url;
    std::string fetchResult = gnu::exec(command);
    json data;

    try {
        data = json::parse(fetchResult);
    }
    catch (...) {
        throw std::runtime_error("Error al intentar hacer fetch a " + url);
    }
    return data;
}

/**
 * Usa a "fetch" con la url de nuestra API como base
 *
 * Por ejemplo, el siguiente código:
 * json response = apifetch("/cines/cercanos")
 *
 * Es equivalente a:
 * json response = fetch(apiURL + "/cines/cercanos")
 */
json apifetch(std::string endpoint) {
    return fetch(apiURL + endpoint);
}

json apipost(std::string endpoint, json data) {
    std::string url = apiURL + endpoint;
    std::string command = "curl -s -X POST -H \"Content-Type: application/json\" -d '" + data.dump() + "' " + url;
    LOG_FILE("Comando: " << command << std::endl);
    std::string fetchResult = gnu::exec(command);

    json response;

    try {
        response = json::parse(fetchResult);
    }
    catch (...) {
        throw std::runtime_error("Error al intentar hacer fetch a " + url);
    }
    return response;
}

} // namespace gnu
