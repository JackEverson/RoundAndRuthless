#pragma once
 
#include "Engine.hpp"

class Scene {
public:
    virtual ~Scene() = default;
    
    // Called once when scene becomes active
    virtual void onEnter() {}
    
    // Called once when scene is about to change
    virtual void onExit() {}
    
    // Return next scene (nullptr = stay on current scene)
    virtual Scene* update() = 0;
    
    // Render the scene
    virtual void render(GLFWwindow& window, Renderer& renderer) = 0;
    
    // Handle input
    virtual void handleInput(GLFWwindow& window) = 0;
    };
