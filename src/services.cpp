#include <string>
#include <json.hpp>
#include "consoleUtils.h"
#include "services.h"

std::string gnu::apiURL = "https://api.cinext.up.railway.app";

// fetching
json gnu::fetch(std::string url) {
    std::string command = "curl -s " + url;
    std::string fetchResult = gnu::exec(command);

    json data;
    try {
        data = json::parse(fetchResult);
    }
    catch (std::exception e) {
        throw std::runtime_error("Error de fetching a " + url);
    }
    return data;
}

json gnu::apifetch(std::string ednpoint) {
    return fetch(apiURL + ednpoint);
}
