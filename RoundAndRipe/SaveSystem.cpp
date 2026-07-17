#include "SaveSystem.hpp"

#include "json.hpp"

#include <fstream>

void SaveSystem::Save(const std::string& path, const GameState& s) {
    nlohmann::json j;
    j["version"]       = s.version;
    j["elapsed"]       = s.elapsed;
    j["biomass"]       = s.biomass;
    j["selected_seed"] = s.selected_seed;
    j["seeds"]         = s.seeds;                   // std::map<string,int> serializes directly
    j["tier"]          = s.tier; 
    j["harvest_count"] = s.harvest_count;
    j["structure_inv"] = s.structure_inv;           // std::map<string,int> serializes directly

    j["tiles"] = nlohmann::json::array();
    for (const auto& t : s.tiles) {
        j["tiles"].push_back({
            {"state",   t.state},
            {"watered", t.watered},
            {"seconds_growing",    t.seconds_growing},
            {"plant",   t.plant}
        });
    }

    j["structure_field"] = nlohmann::json::array();
    for (const auto& s : s.structures_field){
        j["structure_field"].push_back({
            {"name", s.name},
            {"col", s.col},
            {"row", s.row},
            {"crop", s.crop}
        });
    }

    std::ofstream(path) << j.dump(2);            // pretty-printed
}

void SaveSystem::SaveSettings(const std::string& path, const GameSettings& s) {
    nlohmann::json j;
    j["volume"]      = s.volume;
    j["brightness"]  = s.brightness;
    j["sensitivity"] = s.sensitivity;
    j["invert_y"]    = s.invert_y;
    j["ui_scale"]    = s.ui_scale;
    j["borderless"]  = s.borderless;
    j["crosshair"]   = s.crosshair;
    std::ofstream(path) << j.dump(2);
}

bool SaveSystem::LoadSettings(const std::string& path, GameSettings& out) {
    std::ifstream f(path);
    if (!f) return false;                         // no file -> keep defaults

    nlohmann::json j = nlohmann::json::parse(f, nullptr, false);
    if (j.is_discarded()) return false;           // malformed -> keep defaults

    GameSettings d; // defaults for missing keys
    out.volume      = j.value("volume",      d.volume);
    out.brightness  = j.value("brightness",  d.brightness);
    out.sensitivity = j.value("sensitivity", d.sensitivity);
    out.invert_y    = j.value("invert_y",    d.invert_y);
    out.ui_scale    = j.value("ui_scale",    d.ui_scale);
    out.borderless  = j.value("borderless",  d.borderless);
    out.crosshair   = j.value("crosshair",   d.crosshair);
    return true;
}

bool SaveSystem::Load(const std::string& path, int version, GameState& out) {
    std::ifstream f(path);
    if (!f) return false;                         // no save file -> fresh game

    nlohmann::json j = nlohmann::json::parse(f, nullptr, false);  // no-throw parse
    if (j.is_discarded()) return false;           // malformed JSON -> ignore
    if (j.value("version", 0) != version) return false;  // version mismatch -> ignore

    out.version       = version;
    out.elapsed       = j.value("elapsed", 0.0);
    out.biomass       = j.value("biomass", 0ll);    out.selected_seed = j.value("selected_seed", std::string{});
    out.seeds         = j.value("seeds", std::map<std::string, int>{});
    out.tier          = j.value("tier", 0);
    out.harvest_count = j.value("harvest_count", 0);
    out.structure_inv = j.value("structure_inv", std::map<std::string, int>{});

    out.tiles.clear();
    for (const auto& tj : j.value("tiles", nlohmann::json::array())) {
        TileSave t;
        t.state   = tj.value("state",   0);
        t.watered = tj.value("watered", false);
        t.seconds_growing = tj.value("seconds_growing", 0.0f);
        t.plant   = tj.value("plant",   std::string{});
        out.tiles.push_back(t);
    }

    out.structures_field.clear();
    for (const auto& sj : j.value("structure_field", nlohmann::json::array())){
        StructureSave s;
        s.name = sj.value("name", std::string{});
        s.col = sj.value("col", -1);
        s.row = sj.value("row", -1);
        s.crop = sj.value("crop", std::string{});
        out.structures_field.push_back(s);
    }

    return true;
}
