#pragma once

#include <map>
#include <string>
#include <vector>


struct TileSave { 
    int state = 0; 
    bool watered = false; 
    float seconds_growing = 0; 
    std::string plant; 
};

struct StructureSave
{
    std::string name; 
    int row, col; 
    std::string crop;
};

struct GameState {
    int version = 5;
    double elapsed = 0.0;
    long long biomass = 0;
    int tier = 0;
    int harvest_count = 0;
    std::string selected_seed;
    std::map<std::string, int> seeds;   // name -> count
    std::map<std::string, int> structure_inv;
    std::vector<StructureSave> structures_field;
    std::vector<TileSave> tiles;
};

struct GameSettings {
    float volume = 1.0f;
    float brightness = 1.0f;
    float sensitivity = 0.05f;
    bool  invert_y = false;
    float ui_scale = 2.0f;
    bool  borderless = true;   // fullscreen by default
    bool  crosshair = true;
};

inline const std::string SAVE_PATH = "./save.json";
inline const std::string SETTINGS_PATH = "./settings.json";

namespace SaveSystem {
    void Save(const std::string& path, const GameState& s);          // GameState -> json -> file
    bool Load(const std::string& path, int version, GameState& out); // file -> json -> GameState; false if missing/version-mismatch

    void SaveSettings(const std::string& path, const GameSettings& s);
    bool LoadSettings(const std::string& path, GameSettings& out);   // false if missing/malformed (out keeps defaults)
}
