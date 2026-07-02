#include "GardenScene.hpp"
#include "GLFW/glfw3.h"
#include "RoundAndRipeEvents.hpp"   
#include "FPSController.hpp"
#include "TriggerVolume.hpp"

#include "glm/geometric.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include <json.hpp>



GardenScene::GardenScene()
      : sound_manager(SimpleSoundManager::Instance()),
        m_wall_texture("./res/textures/concrete_wall.png"),
        m_floor_texture("./res/textures/gravel_floor.png"),
        m_sushi_texture("./res/textures/sushi.png"),
        m_house_texture("./res/textures/house.png"),
        m_chest_texture("./res/textures/chest.png"),
        m_human_texture("./res/textures/human_hazsuit.png"),
        m_seed_maker_texture("./res/textures/seed_maker.png"),
        m_soil_texture("./res/textures/gravel_floor.png"),
        m_rock_texture("./res/textures/rock.png"),
        m_till_texture("./res/textures/hole.png"),
        m_seeded_texture("./res/textures/covered_hole.png"),
        m_shovel_texture("./res/textures/shovel.png"),
        m_hoe_texture("./res/textures/hoe.png"),
        m_can_texture("./res/textures/watering_can.png"),
        m_packet_texture("./res/textures/seed_packet.png"),
        m_veg_top_texture("./res/textures/veg_top.png"),
        m_apple_texture("./res/textures/apple.png"),
        m_radish_texture("./res/textures/radish.png"),
        m_turnip_texture("./res/textures/turnip.png"),
        m_bush_texture("./res/textures/bush.png"),
        m_tomato_texture("./res/textures/tomato_bush.png"),
        m_blueberry_texture("./res/textures/blueberry_bush.png"),
        m_staring_cabbage_growing_texture("./res/textures/staring_cabbage_growing.png"),
        m_staring_cabbage_ripe_texture("./res/textures/staring_cabbage_ripe.png"),
        m_field(glm::vec3(-5.0f, 0.0f, -5.0f), 10, 10, 1.0f, &m_soil_texture,
                &m_rock_texture, &m_till_texture, &m_seeded_texture) {};


void GardenScene::onEnter(GLFWwindow &window) {
    glfwSetInputMode(&window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    m_controller.Init(window);

    sound_manager.Initialize();
    sound_manager.LoadSound("background_noise",
                            "./res/sounds/ambient-noise.ogg");

    sound_manager.PlayBackgroundMusic("background_noise");

    glm::vec3 seed_maker_position = glm::vec3(0.0f, HALF_SUSHI_SIZE, -10.0f);
    m_camera.SetCamera(glm::vec3(0.0f, PLAYER_HEIGHT, 0.0f));

    // lights
    glm::vec3 light_color = glm::vec3(0.85f, 0.92f, 1.0f) / 3.0f;
    AddLight(glm::vec3(seed_maker_position.x, 0, seed_maker_position.z), HALF_SUSHI_SIZE,
             light_color);
    AddLight(glm::vec3(0), FLOOR_TILE_SIZE / 2, light_color);

    m_highlight.color = glm::vec3(0.5);
    m_highlight.position = glm::vec3(seed_maker_position.x, 1, seed_maker_position.z);
    m_highlight.radius = 0.25f;

    AddFloor(glm::vec3(0.0f), glm::vec2(FLOOR_TILE_SIZE), &m_floor_texture);
    AddWall(glm::vec3(0.0f, 0.0f, FLOOR_TILE_SIZE / 2),
            glm::vec2(FLOOR_TILE_SIZE, 4.0f), &m_wall_texture);
    AddWall(glm::vec3(0.0f, 0.0f, -FLOOR_TILE_SIZE / 2),
            glm::vec2(FLOOR_TILE_SIZE, 4.0f), &m_wall_texture);
    AddWallRotated(glm::vec3(FLOOR_TILE_SIZE / 2, 0.0f, 0.0f),
                   glm::vec2(FLOOR_TILE_SIZE, 4.0f), 90.0f, &m_wall_texture);
    AddWallRotated(glm::vec3(-FLOOR_TILE_SIZE / 2, 0.0f, 0.0f),
                   glm::vec2(FLOOR_TILE_SIZE, 4.0f), 90.0f, &m_wall_texture);

    // Seed maker
    m_seed_maker.texture = &m_seed_maker_texture;
    m_seed_maker.size = glm::vec2(SUSHI_SIZE, SUSHI_SIZE);
    m_seed_maker.color = glm::vec4(1.0f);
    m_seed_maker.position = seed_maker_position;
    m_static_sprites.push_back(m_seed_maker);

    TriggerVolume seed_maker_trigger;
    seed_maker_trigger.position = seed_maker_position;
    seed_maker_trigger.size = glm::vec3(SUSHI_SIZE, SUSHI_SIZE, SUSHI_SIZE);
    seed_maker_trigger.time_to_trigger = 0.01f;
    seed_maker_trigger.type = TriggerType::Interact;
    seed_maker_trigger.on_triggered = [this]() { m_menu_mode = MenuMode::SeedShop; };
    m_triggers.push_back(seed_maker_trigger);

    // Apple
    float apple_size = 0.25f;
    m_apple.texture = &m_apple_texture;
    m_apple.size = glm::vec2(apple_size, apple_size);
    m_apple.color = glm::vec4(1.0f);

    m_apple_trigger.time_to_trigger = 0.1f;
    m_apple_trigger.type = TriggerType::Interact;
    m_apple_trigger.size = glm::vec3(apple_size, apple_size, apple_size);
    m_apple_trigger.on_triggered = [this]() {
      if (m_apple_collected) return;
      m_apple_collected = true;
      m_biomass += 5;
    };
    m_apple_trigger_index = m_triggers.size();
    m_triggers.push_back(m_apple_trigger);
    PlaceApple();

    // house + chest
    m_house.texture = &m_house_texture;
    m_house.size = glm::vec2(HOUSE_SIZE, HOUSE_SIZE);
    m_house.color = glm::vec4(1.0f);
    m_house.position = HOUSE_POS;
    m_static_sprites.push_back(m_house);

    // TriggerVolume house_trigger;
    // house_trigger.position = HOUSE_POS;
    // house_trigger.size = glm::vec3(HOUSE_SIZE, HOUSE_SIZE, 0.10f);
    // house_trigger.time_to_trigger = 2.0f;
    // house_trigger.type = TriggerType::Interact;
    // house_trigger.on_triggered = [this]() { StartSleep(); };
    // m_triggers.push_back(house_trigger);

    m_chest.texture = &m_chest_texture;
    m_chest.size = glm::vec2(CHEST_SIZE, CHEST_SIZE);
    m_chest.color = glm::vec4(1.0f);
    m_chest.position = CHEST_POS;
    m_static_sprites.push_back(m_chest);

    TriggerVolume chest_trigger;
    chest_trigger.position = CHEST_POS;
    chest_trigger.size = glm::vec3(CHEST_SIZE, CHEST_SIZE, 0.10f);
    chest_trigger.time_to_trigger = 0.01f;
    chest_trigger.type = TriggerType::Interact;
    chest_trigger.on_triggered = [this]() { m_menu_mode = MenuMode::UpgradeShop; };
    m_triggers.push_back(chest_trigger);

    // seeds
    m_seeds.push_back({Radish(&m_veg_top_texture, &m_radish_texture), 5});
    m_seeds.push_back({Blueberry(&m_bush_texture, &m_blueberry_texture), 0});
    
    m_seeds.push_back({Turnip(&m_veg_top_texture,&m_turnip_texture), 0});
    m_seeds.push_back({Tomato(&m_bush_texture, &m_tomato_texture), 0});

    m_seeds.push_back({StaringCabbage(&m_staring_cabbage_growing_texture, &m_staring_cabbage_ripe_texture), 0});

    if (!Load()) {
      StartEvent(std::make_unique<RoundAndRipeEvents::TutorialEvent>(*this));
    }

    // for (auto& e : RoundAndRipeEvents::GetDaysEvents(*this, m_day))
    //     StartEvent(std::move(e));
}

void GardenScene::onExit(GLFWwindow &window) {
  if (m_outcome == Outcome::Playing) Save(); 

  glfwSetInputMode(&window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

Scene *GardenScene::update(GLFWwindow &window, float delta) {
    m_notification_manager.Update(delta);

    m_elapsed += delta;
    if (m_elapsed >= TIME_LIMIT && m_outcome == Outcome::Playing) m_outcome = Outcome::Lost;
    
    m_save_timer += delta;
    if (m_save_timer >= SAVE_INTERVAL) {
        Save();
        m_save_timer = 0.0f;
    }

    for (auto& t : m_field.Tiles()) t.Update(delta);

    // event updates
    for (auto& e : m_events) e->Update(delta);
    for (auto& e : m_events) if (e->IsComplete()) e->OnComplete();
    std::erase_if(m_events, [](auto& e){ return e->IsComplete(); });

    if (m_apple_collected) {
      m_apple_timer += delta;
      if (m_apple_timer >= APPLE_RESPAWN_TIME) {
        PlaceApple();
        m_apple_timer = 0.0f;
      }
    }


    // TODO: Theses scenes
    // if (m_outcome == Outcome::Won) {
    //     return new WinScene();
    // } else if (m_outcome == Outcome::Lost) {
    //     return new LoseScene();
    // }

    return nullptr;
}


void GardenScene::handleInput(GLFWwindow &window, float delta) {
    HandleCommonInput(window, delta);
    
    if (glfwGetKey(&window, GLFW_KEY_ESCAPE) == GLFW_PRESS) m_menu_mode = MenuMode::None;

    if (m_controller.InputDisabled())
      return;

    for (auto& e : m_events) e->HandleInput(window, delta);

    glm::vec3 campos = m_camera.GetLocation();
    glm::vec3 forward = m_camera.GetForward();
    auto tile = m_field.TileAtRay(campos, forward);

    if (tile) {
      m_highlight.position = tile->Position();
      m_show_highlight = true;
    } else {
      m_show_highlight = false;
    }

    if (m_controller.InteractionHeld()) {
      if (tile)
        UseToolOn(*tile);
    }

    if (glfwGetKey(&window, GLFW_KEY_1) == GLFW_PRESS)m_tool = Tool::None;
    if (glfwGetKey(&window, GLFW_KEY_2) == GLFW_PRESS)m_tool = Tool::Hoe;
    if (glfwGetKey(&window, GLFW_KEY_3) == GLFW_PRESS)m_tool = Tool::WateringCan;
    if (glfwGetKey(&window, GLFW_KEY_4) == GLFW_PRESS)m_tool = Tool::SeedPacket;
    if (glfwGetKey(&window, GLFW_KEY_5) == GLFW_PRESS)m_tool = Tool::Shovel;


    // temp scroll wheel seed selection
    float wheel = ImGui::GetIO().MouseWheel; // + up / - down, this frame
    if (wheel > 0)
      CycleTool(+1);
    else if (wheel < 0)
      CycleTool(-1);

    if (m_tool == Tool::SeedPacket && glfwGetMouseButton(&window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
      m_menu_mode = MenuMode::SeedSelection;
    }


  }

  void GardenScene::render(GLFWwindow &window, Renderer &renderer) {
    int w, h;
    glfwGetWindowSize(&window, &w, &h);
    if (w == 0 || h == 0)
      return;

    renderer.Clear(0.05f, 0.05f, 0.05f, 1.0f);
    renderer.BeginBatchDraw(30, 400);

    std::vector<PointLight> lights = m_lights;

    if (m_show_highlight)
      lights.push_back(m_highlight);

    renderer.SetLights(lights, 0.15f);

    SetupRenderingObjects(renderer);

    for (auto& e : m_events) e->Render(renderer);

    glm::vec3 campos = m_camera.GetLocation();
    glm::mat4 view = m_camera.GetViewMat();
    glm::mat4 projection = m_camera.GetProjectionMat(w, h);
    glm::vec3 forward = m_camera.GetForward();
    glm::vec3 right = m_camera.GetRight();
    right = glm::normalize(right);

    m_field.Render(renderer, campos);

    glm::mat4 billboard = glm::transpose(glm::mat4(glm::mat3(view)));

    // apple
    if (m_apple_collected == false) {
      m_apple.model_mat = billboard;
      renderer.SubmitTransparentSprite(m_apple);
    }

    {
      auto yaw_billboard = [&](const glm::vec3 &pos) {
        glm::vec3 to_cam = campos - pos;
        float yaw = std::atan2(to_cam.x, to_cam.z); // angle around Y only
        return glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0, 1, 0));
      };
      glm::vec3 forward_horiz = glm::normalize(glm::vec3(forward.x, -0.0f, forward.z));

      SpriteInstance body;
      body.texture = &m_human_texture;
      body.size = BODY_SIZE;
      body.color = glm::vec4(1.0f);
      body.position = campos + glm::vec3(0.0f, -BODY_DROP, 0.0f) +
                      forward_horiz * BODY_FORWARD;
      body.model_mat = yaw_billboard(body.position);
      renderer.SubmitTransparentSprite(body);

      float yaw = std::atan2(forward.x, forward.z);                       
      glm::mat4 m = glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0, 1, 0)); 
      m = glm::rotate(m, glm::radians(90.0f), glm::vec3(1, 0, 0));        

      SpriteInstance sushi;
      sushi.texture = &m_sushi_texture;
      sushi.size = EMBED_SUSHI_SIZE;
      sushi.color = glm::vec4(1.0f);
      sushi.position = campos + glm::vec3(0.0f, -EMBED_SUSHI_DROP, 0.0f)
        + forward_horiz * EMBED_SUSHI_FORWARD + right * EMBED_SUSHI_SIDE;

      sushi.model_mat = m;
      renderer.SubmitTransparentSprite(sushi);
    }

    if (Texture* tex = ToolTexture(m_tool)) {
    glm::vec3 up = glm::normalize(glm::cross(right, forward));

    // full billboard (faces camera), then a 45° in-plane roll for the "held" tilt
    glm::mat4 billboard = glm::transpose(glm::mat4(glm::mat3(view)));
    billboard = billboard * glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0, 0, 1));

    SpriteInstance toolspr;
    toolspr.texture = tex;
    toolspr.size = TOOL_SIZE;
    toolspr.color = glm::vec4(1.0f);
    toolspr.position = campos + forward * TOOL_FWD + right * TOOL_SIDE - up * TOOL_DROP;
    toolspr.model_mat = billboard;
    renderer.SubmitTransparentSprite(toolspr);
    }

    renderer.RendBatch(view, projection, campos, 0.05f);

    // HUD and UI (ImGUI)
    if (!m_task_text.empty()){
      const char *task_text = m_task_text.c_str();

      ImGui::SetNextWindowPos(ImVec2(w * 0.75f, h - 40.0f), ImGuiCond_Always,
                              ImVec2(0.5f, 0.0f));
      ImGui::SetNextWindowBgAlpha(0.0f);
      ImGui::Begin("##task", nullptr,
                  ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
                      ImGuiWindowFlags_NoMove |
                      ImGuiWindowFlags_AlwaysAutoResize);
      ImGui::Text("Task: %s", task_text);
      ImGui::SetWindowFontScale(m_font_size);
      ImGui::End();
    }


    std::string seed_text;

    if (m_selected_seed == -1) {
      seed_text = "none";
    } else {
      seed_text = m_seeds[m_selected_seed].def.name + " x" +
                  std::to_string(m_seeds[m_selected_seed].count);
    }

    ImGui::SetNextWindowPos(ImVec2(w * 0.15f, h - 150.0f), ImGuiCond_Always,
                            ImVec2(0.5f, 0.0f));
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("##hud", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("Tool: %s\nSeed: %s\nTime: %d\nBiomass: %d g\nTier: %d",
                GetToolName(m_tool), seed_text.c_str(), (int)m_elapsed, m_biomass, m_tier);
    ImGui::SetWindowFontScale(m_font_size);
    ImGui::End();

    // rendering shop menu
    bool panel_open = (m_menu_mode != MenuMode::None);
    if (panel_open && m_cursor_captured) {
      EnterSelectionMode(window);
    } else if (!panel_open && !m_cursor_captured) {
      ExitSelectionMode(window);
    }

    if (m_menu_mode == MenuMode::SeedShop) {
      ImGui::SetNextWindowPos(ImVec2(w * 0.5f, h * 0.5f), ImGuiCond_Always,
                              ImVec2(0.5f, 0.5f));
      ImGui::SetNextWindowBgAlpha(
          0.9f); // visible panel (your HUD used 0 = invisible)
      ImGui::Begin("Seed Store", nullptr,
                   ImGuiWindowFlags_AlwaysAutoResize); // NOTE: no NoInputs — it
                                                       // must take clicks
      ImGui::SetWindowFontScale(m_font_size);

      ImGui::Text("Biomass: %d g", m_biomass);
      ImGui::Separator();

      for (int n = 0; n < (int)m_seeds.size(); n++) {
        Seed &seed = m_seeds[n];

        ImGui::PushID(n);
        if (seed.def.tier > m_tier) {
            ImGui::Text("%s  (locked, tier %d)", seed.def.name.c_str(), seed.def.tier);
            ImGui::PopID();
            continue;
        }
        ImGui::Text("%s  (have %d)  -  %d g", seed.def.name.c_str(), seed.count, seed.def.biomass_cost);
        ImGui::SameLine();
        if (ImGui::Button("Buy")) {
            if (m_biomass >= seed.def.biomass_cost) { m_biomass -= seed.def.biomass_cost; seed.count++; }
            else m_notification_manager.Push("Not enough biomass", 1.5f);
        }
        ImGui::PopID();
      }
      ImGui::Separator();
      if (ImGui::Button("Close")) {
        m_menu_mode = MenuMode::None; 
      }
      ImGui::End();
    }
    else if (m_menu_mode == MenuMode::UpgradeShop) {
      ImGui::SetNextWindowPos(ImVec2(w * 0.5f, h * 0.5f), ImGuiCond_Always,
                              ImVec2(0.5f, 0.5f));
      ImGui::SetNextWindowBgAlpha(
          0.9f); // visible panel (your HUD used 0 = invisible)
      ImGui::Begin("Upgrade Store", nullptr,
                   ImGuiWindowFlags_AlwaysAutoResize); // NOTE: no NoInputs — it
                                                       // must take clicks
      ImGui::SetWindowFontScale(m_font_size);

      ImGui::Text("Biomass: %d g", m_biomass);
      ImGui::Separator();
      if (m_tier >= (int)TIER_COST.size()) {
          ImGui::Text("Max tier reached.");
      } else {
          ImGui::Text("Tier %d -> Tier %d - %d g", m_tier, m_tier + 1, (int)TIER_COST[m_tier]);
          if (ImGui::Button("Upgrade")) {
              if (m_biomass >= TIER_COST[m_tier]) {
                  m_biomass -= TIER_COST[m_tier];
                  m_tier++;
                  if (m_tier >= (int)TIER_COST.size()) m_outcome = Outcome::Won;
              } else m_notification_manager.Push("Not enough biomass", 1.5f);
          }
      }
      if (ImGui::Button("Close")) m_menu_mode = MenuMode::None;
      ImGui::End();
    }
    else if (m_menu_mode == MenuMode::SeedSelection) {
      ImGui::SetNextWindowPos(ImVec2(w * 0.5f, h * 0.5f), ImGuiCond_Always,
                              ImVec2(0.5f, 0.5f));
      ImGui::SetNextWindowBgAlpha(0.9f);
      ImGui::Begin("Seeds", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
      ImGui::SetWindowFontScale(m_font_size);

      bool any_in_stock = false;
      for (int n = 0; n < (int)m_seeds.size(); n++) {
        Seed &seed = m_seeds[n];
        if (seed.count <= 0) continue; // only seeds the player actually has
        any_in_stock = true;

        ImGui::PushID(n);
        ImGui::Text("%s%s  x%d", (m_selected_seed == n) ? "> " : "",
                    seed.def.name.c_str(), seed.count);
        ImGui::SameLine();
        if (ImGui::Button("Select")) {
          m_selected_seed = n;
          m_menu_mode = MenuMode::None; // pick -> close, straight back to planting
        }
        ImGui::PopID();
      }
      if (!any_in_stock)
        ImGui::Text("No seeds. Buy some at the seed maker.");

      ImGui::Separator();
      if (ImGui::Button("Close")) m_menu_mode = MenuMode::None;
      ImGui::End();
    }

    if (m_outcome != Outcome::Playing) {
        ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(0,0), ImVec2((float)w,(float)h), IM_COL32(0,0,0,200));
        const char* msg = (m_outcome == Outcome::Won) ? "Sentence served." : "You have been recycled.";
        ImGui::SetNextWindowPos(ImVec2(w*0.5f, h*0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::Begin("##end", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::SetWindowFontScale(m_font_size * 1.5f);
        ImGui::Text("%s", msg);
        ImGui::End();
    }

    m_notification_manager.Render(w, h);
  }

