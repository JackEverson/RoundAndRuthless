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
    int biomass = 0, tier = 0;
    std::string selected_seed;
    std::map<std::string, int> seeds;   // name -> count
    std::vector<TileSave> tiles;
};


namespace SaveSystem {
    void Save(const std::string& path, const GameState& s);          // GameState -> json -> file
    bool Load(const std::string& path, int version, GameState& out); // file -> json -> GameState; false if missing/version-mismatch
}


