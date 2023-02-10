#include <string>
#include <json.hpp>

using json = nlohmann::json;

namespace gnu {

    extern std::string apiURL;

    json fetch(std::string url);

    json apifetch(std::string endpoint);

} // namespace gnu
