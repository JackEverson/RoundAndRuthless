#include "SaveSystem.hpp"

#include "json.hpp"

#include <fstream>

void SaveSystem::Save(const std::string& path, const GameState& s) {
    nlohmann::json j;
    j["version"]       = s.version;
    j["day"]           = s.day;
    j["energy"]        = s.energy;
    j["biomass"]       = s.biomass;
    j["selected_seed"] = s.selected_seed;
    j["seeds"]         = s.seeds;                 // std::map<string,int> serializes directly
    j["tier"]          = s.tier; 

    j["tiles"] = nlohmann::json::array();
    for (const auto& t : s.tiles) {
        j["tiles"].push_back({
            {"state",   t.state},
            {"watered", t.watered},
            {"days",    t.days},
            {"plant",   t.plant}
        });
    }

    std::ofstream(path) << j.dump(2);            // pretty-printed
}

bool SaveSystem::Load(const std::string& path, int version, GameState& out) {
    std::ifstream f(path);
    if (!f) return false;                         // no save file -> fresh game

    nlohmann::json j = nlohmann::json::parse(f, nullptr, false);  // no-throw parse
    if (j.is_discarded()) return false;           // malformed JSON -> ignore
    if (j.value("version", 0) != version) return false;  // version mismatch -> ignore

    out.version       = version;
    out.day           = j.value("day", 1);
    out.energy        = j.value("energy", 0);
    out.biomass       = j.value("biomass", 0);    out.selected_seed = j.value("selected_seed", std::string{});
    out.seeds         = j.value("seeds", std::map<std::string, int>{});
    out.tier          = j.value("tier", 0);  

    out.tiles.clear();
    for (const auto& tj : j.value("tiles", nlohmann::json::array())) {
        TileSave t;
        t.state   = tj.value("state",   0);
        t.watered = tj.value("watered", false);
        t.days    = tj.value("days",    0);
        t.plant   = tj.value("plant",   std::string{});
        out.tiles.push_back(t);
    }
    return true;
}
