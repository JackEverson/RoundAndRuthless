#include "GardenScene.hpp"
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
        m_veg_top_texture("./res/textures/veg_top.png"),
        m_apple_texture("./res/textures/apple.png"),
        m_radish_texture("./res/textures/radish.png"),
        m_carrot_texture("./res/textures/carrot.png"),
        m_tomato_growing_texture("./res/textures/tomato.png"),
        m_tomato_ripe_texture("./res/textures/tomato_with_tomato.png"),
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
    seed_maker_trigger.time_to_trigger = 0.1f;
    seed_maker_trigger.type = TriggerType::Interact;
    seed_maker_trigger.on_triggered = [this]() { m_shop_open = true; };
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

    TriggerVolume house_trigger;
    house_trigger.position = HOUSE_POS;
    house_trigger.size = glm::vec3(HOUSE_SIZE, HOUSE_SIZE, 0.10f);
    house_trigger.time_to_trigger = 2.0f;
    house_trigger.type = TriggerType::Interact;
    house_trigger.on_triggered = [this]() { StartSleep(); };
    m_triggers.push_back(house_trigger);

    m_chest.texture = &m_chest_texture;
    m_chest.size = glm::vec2(CHEST_SIZE, CHEST_SIZE);
    m_chest.color = glm::vec4(1.0f);
    m_chest.position = CHEST_POS;
    m_static_sprites.push_back(m_chest);

    TriggerVolume chest_trigger;
    chest_trigger.position = CHEST_POS;
    chest_trigger.size = glm::vec3(CHEST_SIZE, CHEST_SIZE, 0.10f);
    chest_trigger.time_to_trigger = 2.0f;
    chest_trigger.type = TriggerType::Interact;
    chest_trigger.on_triggered = [this]() 
    { 
      if (m_outcome != Outcome::Playing) return;
      if (!SpendBiomass(TIER_COST[m_tier])){
          std::string message = "Not Enough Biomass for Tier Upgrade! " + std::to_string(m_tier) + "->" + std::to_string(m_tier + 1) + " costs: " + std::to_string(TIER_COST[m_tier]);
          m_notification_manager.Push(message);
          return;
        }

        m_tier++;
        if (m_tier >= (int)TIER_COST.size()) m_outcome = Outcome::Won;
        else {
          std::string message = "Upgrade to Tier " + std::to_string(m_tier) + " successful!";
          m_notification_manager.Push(message);
        }
    };
    m_triggers.push_back(chest_trigger);

    // seeds
    m_seeds.push_back({Radish(&m_veg_top_texture, &m_radish_texture), 10});
    m_seeds.push_back({Carrot(&m_veg_top_texture,&m_carrot_texture), 10});
    m_seeds.push_back({Tomato(&m_tomato_growing_texture, &m_tomato_ripe_texture), 10});

    Load();

    for (auto& e : RoundAndRipeEvents::GetDaysEvents(*this, m_day))
        StartEvent(std::move(e));
}

void GardenScene::onExit(GLFWwindow &window) {
    glfwSetInputMode(&window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }

Scene *GardenScene::update(GLFWwindow &window, float delta) {
    m_notification_manager.Update(delta);
    
    // event updates
    for (auto& e : m_events) e->Update(delta);
    for (auto& e : m_events) if (e->IsComplete()) e->OnComplete();
    std::erase_if(m_events, [](auto& e){ return e->IsComplete(); });

    // animate sleep
    if (m_sleep == SleepPhase::GoingDark) {
      m_fade += FADE_SPEED * delta;
      if (m_fade >= 1.0f) {
        m_fade = 1.0f;
        AdvanceDay();
        m_sleep = SleepPhase::Waking;
      }
    } else if (m_sleep == SleepPhase::Waking) {
      m_fade -= FADE_SPEED * delta;
      if (m_fade <= 0.0f) {
        m_fade = 0.0f;
        m_sleep = SleepPhase::Awake;
      }
    }

    return nullptr;
}


void GardenScene::handleInput(GLFWwindow &window, float delta) {
    HandleCommonInput(window, delta);

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

    if (m_menu_tile) {
      if (glfwGetKey(&window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        CloseMenu();
      } else if (m_menu_mode == MenuMode::Tend) {
        if (glfwGetKey(&window, GLFW_KEY_Y) == GLFW_PRESS) {
          m_menu_tile->PullUp();
          CloseMenu();
        } else if (glfwGetKey(&window, GLFW_KEY_N) == GLFW_PRESS) {
          CloseMenu();
        }
      }
    } else {
      if (glfwGetKey(&window, GLFW_KEY_1) == GLFW_PRESS)
        m_tool = Tool::Shovel;
      if (glfwGetKey(&window, GLFW_KEY_2) == GLFW_PRESS)
        m_tool = Tool::Hoe;
      if (glfwGetKey(&window, GLFW_KEY_3) == GLFW_PRESS)
        m_tool = Tool::WateringCan;
      if (glfwGetKey(&window, GLFW_KEY_4) == GLFW_PRESS)
        m_tool = Tool::SeedPacket;
      if (glfwGetKey(&window, GLFW_KEY_5) == GLFW_PRESS)
        m_tool = Tool::None;
    }

    // Skip day debugging
    bool s = glfwGetKey(&window, GLFW_KEY_K) == GLFW_PRESS;
    if (s && !m_sleep_held)
      AdvanceDay();
    m_sleep_held = s;

    // temp scroll wheel seed selection
    float wheel = ImGui::GetIO().MouseWheel; // + up / - down, this frame
    if (wheel > 0)
      CycleSeed(+1);
    else if (wheel < 0)
      CycleSeed(-1);
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

    // Sushi 'merchant' billboard
    // if (IsMerchantDay()) {
    //   m_sushi_observer.model_mat = billboard;
    //   renderer.SubmitTransparentSprite(m_sushi_observer);
    // }

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

    renderer.RendBatch(view, projection, campos, 0.05f);

    // HUD and UI (ImGUI)
    const char *task_text = "Just cooperate and hand over the liver!";

    ImGui::SetNextWindowPos(ImVec2(w * 0.75f, h - 40.0f), ImGuiCond_Always,
                            ImVec2(0.5f, 0.0f));
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("##task", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("%s", task_text);
    ImGui::SetWindowFontScale(m_font_size);
    ImGui::End();

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
    ImGui::Text("Tool: %s\nSeed: %s\nDay: %d\nBiomass: %d g\nEnergy: %d/%d",
                GetToolName(m_tool), seed_text.c_str(), m_day, m_biomass,
                m_energy, m_max_energy);
    ImGui::SetWindowFontScale(m_font_size);
    ImGui::End();

    // rendering tending menu
    if (m_menu_mode != MenuMode::None) {
      ImGui::SetNextWindowPos(ImVec2(w * 0.5f, h * 0.5f), ImGuiCond_Always,
                              ImVec2(0.5f, 0.5f));
      ImGui::Begin("##plant", nullptr,
                   ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
                       ImGuiWindowFlags_AlwaysAutoResize);
      ImGui::SetWindowFontScale(m_font_size);

      if (m_menu_mode == MenuMode::Tend) {
        ImGui::Text("[Y] Pull up?");
        ImGui::Text("[N] Cancel");
      }
      ImGui::End();
    }

    // rendering shop menu
    if (m_shop_open && m_cursor_captured) {
      EnterSelectionMode(window);
    } else if (!m_shop_open && !m_cursor_captured) {
      ExitSelectionMode(window);
    }

    if (m_shop_open) {
      ImGui::SetNextWindowPos(ImVec2(w * 0.5f, h * 0.5f), ImGuiCond_Always,
                              ImVec2(0.5f, 0.5f));
      ImGui::SetNextWindowBgAlpha(
          0.9f); // visible panel (your HUD used 0 = invisible)
      ImGui::Begin("Merchant", nullptr,
                   ImGuiWindowFlags_AlwaysAutoResize); // NOTE: no NoInputs — it
                                                       // must take clicks
      ImGui::SetWindowFontScale(m_font_size);

      ImGui::Text("Biomass: %d g", m_biomass);
      ImGui::Separator();

      for (int n = 0; n < (int)m_seeds.size(); n++) {
        Seed &seed = m_seeds[n];
        int cost = seed.def.biomass_yield / 2; // placeholder pricing

        ImGui::PushID(n); // (!) unique id per row — see below
        ImGui::Text("%s  (have %d)  -  %d g", seed.def.name.c_str(), seed.count,
                    cost);
        ImGui::SameLine();
        if (ImGui::Button("Buy")) { // returns true the frame it's clicked
          if (m_biomass >= cost) {
            m_biomass -= cost;
            seed.count++; // bumps count on the existing entry — pointer-safe
          }
        }
        ImGui::PopID();
      }

      ImGui::Separator();
      if (ImGui::Button("Close")) {
        m_shop_open = false; // your ExitSelectionMode recaptures next frame
      }
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

    // sleep screen, TODO: probably should replace this with my own rects
    if (m_fade > 0.0f) {
      ImU32 col = IM_COL32(0, 0, 0, (int)(m_fade * 255));
      ImGui::GetForegroundDrawList()->AddRectFilled(
          ImVec2(0, 0), ImVec2((float)w, (float)h), col);
    }

    m_notification_manager.Render(w, h);
  }

void GardenScene::AdvanceDay() {
  m_day++;
  PlaceApple();
  m_energy = m_max_energy;
  m_field.Advance();
  if (m_day > 100 && m_outcome == Outcome::Playing) m_outcome = Outcome::Lost;
  Save();
  
  for (auto& e : RoundAndRipeEvents::GetDaysEvents(*this, m_day))
    StartEvent(std::move(e));
}