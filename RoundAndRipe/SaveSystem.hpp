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

struct GameState {
    int version = 2;
    double elapsed = 0.0;
    int biomass = 0;
    int tier = 0;
    int harvest_count = 0;
    std::string selected_seed;
    std::map<std::string, int> seeds;   // name -> count
    std::vector<TileSave> tiles;
};


// Machine preferences — separate file from the game save so they survive
// New Game / save wipes / SAVE_VERSION bumps. Defaults here = first-run values.
struct GameSettings {
    float volume = 1.0f;
    float brightness = 1.0f;
    float sensitivity = 0.05f;
    bool  invert_y = false;
    float ui_scale = 2.0f;
    bool  borderless = true;   // fullscreen by default
};

namespace SaveSystem {
    void Save(const std::string& path, const GameState& s);          // GameState -> json -> file
    bool Load(const std::string& path, int version, GameState& out); // file -> json -> GameState; false if missing/version-mismatch

    void SaveSettings(const std::string& path, const GameSettings& s);
    bool LoadSettings(const std::string& path, GameSettings& out);   // false if missing/malformed (out keeps defaults)
}


