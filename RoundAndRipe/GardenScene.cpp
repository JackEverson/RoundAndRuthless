#include "GardenScene.hpp"
#include "Engine.hpp"
#include "FPSController.hpp"
#include "GLFW/glfw3.h"
#include "Plants.hpp"
#include "Renderer.hpp"
#include "RoundAndRipeEvents.hpp"
#include "Structures.hpp"
#include "TriggerVolume.hpp"


#include "glm/ext/matrix_float4x4.hpp"
#include "glm/geometric.hpp"
#include "imgui.h"
#include <json.hpp>
#include <string>


GardenScene::GardenScene()
    : sound_manager(SimpleSoundManager::Instance()),
      m_wall_texture("./res/textures/concrete_wall.png"),
      m_floor_texture("./res/textures/gravel_floor.png"),
      m_sushi_texture("./res/textures/sushi.png"),
      m_sushi_eat_texture("./res/textures/sushi_eat.png"),
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
      m_wrench_texture("./res/textures/wrench.png"),
      m_sprinkler_texture("./res/textures/sprinkler.png"),
      m_veg_top_texture("./res/textures/veg_top.png"),
      m_apple_texture("./res/textures/apple.png"),
      m_radish_texture("./res/textures/radish.png"),
      m_turnip_texture("./res/textures/turnip.png"),
      m_bush_texture("./res/textures/bush.png"),
      m_tomato_texture("./res/textures/tomato_bush.png"),
      m_blueberry_texture("./res/textures/blueberry_bush.png"),
      m_kidneybean_texture("./res/textures/kidneybean_bush.png"),
      m_staring_mellon_growing_texture("./res/textures/eye_plant.png"),
      m_staring_mellon_ripe_texture("./res/textures/eye_plant_ripe.png"),
      m_liverbounty_grow("./res/textures/liver_grow.png"),
      m_liverbounty_ripe("./res/textures/liver_ripe.png"),

      m_field(glm::vec3(-5.0f, 0.0f, -5.0f), 10, 30, 1.0f, &m_soil_texture,
              &m_rock_texture, &m_till_texture, &m_seeded_texture, 10) {};

void GardenScene::onEnter(GLFWwindow &window) {
  glfwSetInputMode(&window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  m_controller.Init(window);

  srand((unsigned)time(nullptr));  

  // settings: load (defaults if no file) and apply
  GameSettings cfg;
  SaveSystem::LoadSettings(SETTINGS_PATH, cfg);
  m_master_volume = cfg.volume;
  sound_manager.SetMasterVolume(cfg.volume);
  m_brightness = cfg.brightness;
  m_controller.MouseSensitivity = cfg.sensitivity;
  m_controller.InvertY = cfg.invert_y;
  m_font_size = cfg.ui_scale;
  m_borderless = cfg.borderless;
  if (m_borderless)
    GardenEngine::SetBorderless(
        window, true); // window is created windowed; only switch if needed

  sound_manager.Initialize();
  sound_manager.LoadSound("background_noise", "./res/sounds/ambient-noise.ogg");
  sound_manager.PlayBackgroundMusic("background_noise");

  sound_manager.LoadSound("meow_talk", "./res/sounds/meow_talk.wav");
  sound_manager.LoadSound("meow_angry", "./res/sounds/meow_angry.wav");
  sound_manager.LoadSound("meow_sad", "./res/sounds/meow_sad.wav");
  sound_manager.LoadSound("eating", "./res/sounds/eating.wav");

  sound_manager.LoadSound("water", "./res/sounds/water.ogg");
  sound_manager.LoadSound("dig", "./res/sounds/dig.wav");
  sound_manager.LoadSound("bell", "./res/sounds/fire_truck_bell-clean.wav");
  sound_manager.LoadSound("boom", "./res/sounds/big_dynamite_explode.ogg");
  sound_manager.LoadSound("pop", "./res/sounds/pop.wav");

  glm::vec3 seed_maker_position = glm::vec3(0.0f, HALF_SUSHI_SIZE, -10.0f);
  m_camera.SetCamera(glm::vec3(0.0f, PLAYER_HEIGHT, 0.0f));

  // lights
  glm::vec3 light_color = glm::vec3(0.85f, 0.92f, 1.0f) / 3.0f;
  AddLight(glm::vec3(seed_maker_position.x, 0, seed_maker_position.z),
           HALF_SUSHI_SIZE, light_color);
  AddLight(glm::vec3(0), FLOOR_TILE_SIZE / 2, light_color);

  m_highlight.color = glm::vec3(0.5);
  m_highlight.position =
      glm::vec3(seed_maker_position.x, 1, seed_maker_position.z);
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
  seed_maker_trigger.on_triggered = [this]() {
    m_menu_mode = MenuMode::SeedShop;
  };
  m_triggers.push_back(seed_maker_trigger);

  // house
  m_house.texture = &m_house_texture;
  m_house.size = glm::vec2(HOUSE_SIZE, HOUSE_SIZE);
  m_house.color = glm::vec4(1.0f);
  m_house.position = HOUSE_POS;
  m_static_sprites.push_back(m_house);

  TriggerVolume house_trigger;
  house_trigger.position = HOUSE_POS;
  house_trigger.size = glm::vec3(HOUSE_SIZE, HOUSE_SIZE, 0.10f);
  house_trigger.time_to_trigger = 0.01f;
  house_trigger.type = TriggerType::Interact;
  house_trigger.on_triggered = [this]() { m_menu_mode = MenuMode::StructureShop; };
  m_triggers.push_back(house_trigger);

  // chest
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
  chest_trigger.on_triggered = [this]() {
    m_menu_mode = MenuMode::UpgradeShop;
  };
  m_triggers.push_back(chest_trigger);

  // Apple
  float apple_size = 0.25f;
  m_apple.texture = &m_apple_texture;
  m_apple.size = glm::vec2(apple_size, apple_size);
  m_apple.color = glm::vec4(1.0f);

  m_apple_trigger.time_to_trigger = 0.1f;
  m_apple_trigger.type = TriggerType::Interact;
  m_apple_trigger.size = glm::vec3(apple_size, apple_size, apple_size);
  m_apple_trigger.on_triggered = [this]() {
    if (m_apple_collected)
      return;
    m_apple_collected = true;
    m_biomass += 5;
  };
  m_apple_trigger_index = m_triggers.size();
  m_triggers.push_back(m_apple_trigger);
  PlaceApple();

  // seeds
  m_seeds.push_back({Radish(&m_veg_top_texture, &m_radish_texture), 0});
  m_seeds.push_back({Blueberry(&m_bush_texture, &m_blueberry_texture), 0});

  m_seeds.push_back({Turnip(&m_veg_top_texture, &m_turnip_texture), 0});
  m_seeds.push_back({Tomato(&m_bush_texture, &m_tomato_texture), 0});
  m_seeds.push_back({KidneyBean(&m_bush_texture, &m_kidneybean_texture), 0});
  m_seeds.push_back({StaringMellon(&m_staring_mellon_growing_texture,&m_staring_mellon_ripe_texture),0});
  m_seeds.push_back({LiverBounty(&m_liverbounty_grow, &m_liverbounty_ripe), 0});


  m_structure_inv.push_back({Sprinkler(&m_sprinkler_texture), 0});
  m_structure_inv.push_back({Harvester(&m_sprinkler_texture), 0});
  m_structure_inv.push_back({Hoer(&m_sprinkler_texture), 0});
  m_structure_inv.push_back({Planter(&m_sprinkler_texture), 0});


  bool loaded_save = Load();
  if (!loaded_save) { StartEvent(std::make_unique<RoundAndRipeEvents::TutorialEvent>(*this)); }

}

void GardenScene::onExit(GLFWwindow &window) {
  
  if (m_new_game) { std::remove(SAVE_PATH.c_str()); }
  else if (m_outcome == Outcome::Playing) { Save(); }

  // settings persist regardless of outcome
  GameSettings cfg;
  cfg.volume = m_master_volume;
  cfg.brightness = m_brightness;
  cfg.sensitivity = m_controller.MouseSensitivity;
  cfg.invert_y = m_controller.InvertY;
  cfg.ui_scale = m_font_size;
  cfg.borderless = m_borderless;
  SaveSystem::SaveSettings(SETTINGS_PATH, cfg);

  glfwSetInputMode(&window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

Scene *GardenScene::update(GLFWwindow &window, float delta) {
  if (m_new_game)
    return new GardenScene();

  m_notification_manager.Update(delta);

  m_elapsed += delta;
  m_random_event_timer += delta;
  if (m_elapsed >= TIME_LIMIT && m_outcome == Outcome::Playing)
    m_outcome = Outcome::Lost;

  m_save_timer += delta;
  if (m_save_timer >= SAVE_INTERVAL) {
    Save();
    m_save_timer = 0.0f;
  }

  for (auto &t : m_field.Tiles())
    t.Update(delta);

  // event updates
  for (auto &e : m_events)
    e->Update(delta);
  for (auto &e : m_events)
    if (e->IsComplete())
      e->OnComplete();
  std::erase_if(m_events, [](auto &e) { return e->IsComplete(); });

  if (m_apple_collected) {
    m_apple_timer += delta;
    if (m_apple_timer >= APPLE_RESPAWN_TIME) {
      PlaceApple();
      m_apple_timer = 0.0f;
    }
  }

  StructureReport machines = m_field.RunStructures(delta);
  m_biomass += machines.collected;
  if (machines.tilled)    PlaySound("dig");
  if (machines.planted)   PlaySound("pop");
  if (machines.harvested) PlaySound("pop");
  if (machines.watered)   PlaySound("water");

  // random events
  if (m_random_event_timer >= m_next_random_event) {
    if (m_events.empty()) {
      auto e = RoundAndRipeEvents::GetRandomBackgroundEvent(*this);
      if (e)
        StartEvent(std::move(e));
      m_random_event_timer = 0.0f;
      m_next_random_event =
          RANDOM_EVENT_MIN + (float)(rand() % RANDOM_EVENT_SPREAD);
    } else {
      // busy (tutorial/dialogue up): hold just short of the threshold so
      // ambience waits a breather after events clear, not fires instantly
      m_random_event_timer = m_next_random_event - 20.0f;
    }
  }

  // look down quips
  m_sushi_quip_cooldown -= delta;
  bool looking = IsLookingDown();
  if (looking && !m_was_looking_down            // edge: just looked down, not "is holding gaze"
      && m_sushi_quip_cooldown <= 0.0f
      && m_events.empty()) {                    // never talk over tutorial/dialogue/ending
    StartEvent(RoundAndRipeEvents::GetLookDownQuip(*this));
    m_sushi_quip_cooldown = 60.0f + rand() % 60;  // then at most ~once a minute
  }
  m_was_looking_down = looking;

  // TODO: Theses scenes
  // if (m_outcome == Outcome::Won) {
  //     return new WinScene();
  // } else if (m_outcome == Outcome::Lost) {
  //     return new LoseScene();
  // }
  if (m_outcome != Outcome::Playing) {
    m_controller.DisableInput();
  }
  if (m_quit_game){
    glfwSetWindowShouldClose(&window, true);
  }

  return nullptr;
}

void GardenScene::handleInput(GLFWwindow &window, float delta) {
  HandleCommonInput(window, delta);

  // ESC: edge-detected toggle — opens Settings, or closes whatever menu is up
  bool esc = glfwGetKey(&window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
  if (esc && !m_esc_held) {
    m_menu_mode =
        (m_menu_mode == MenuMode::None) ? MenuMode::Settings : MenuMode::None;
  }
  m_esc_held = esc;

  // water-can loop: must run BEFORE the InputDisabled early-return,
  // or the stop-branch can't fire while a menu is open
  bool watering = (m_tool == Tool::WateringCan) &&
                  m_controller.InteractionHeld() &&
                  !m_controller.InputDisabled();
  if (watering && !sound_manager.IsSoundPlaying("water"))
    sound_manager.PlaySound("water", true);
  else if (!watering && sound_manager.IsSoundPlaying("water"))
    sound_manager.StopSound("water");

  if (m_controller.InputDisabled())
    return;

  for (auto &e : m_events)
    e->HandleInput(window, delta);

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

  if (glfwGetKey(&window, GLFW_KEY_1) == GLFW_PRESS)
    m_tool = Tool::None;
  if (glfwGetKey(&window, GLFW_KEY_2) == GLFW_PRESS)
    m_tool = Tool::Hoe;
  if (glfwGetKey(&window, GLFW_KEY_3) == GLFW_PRESS)
    m_tool = Tool::WateringCan;
  if (glfwGetKey(&window, GLFW_KEY_4) == GLFW_PRESS)
    m_tool = Tool::SeedPacket;
  if (glfwGetKey(&window, GLFW_KEY_5) == GLFW_PRESS)
    m_tool = Tool::Wrench;
  if (glfwGetKey(&window, GLFW_KEY_6) == GLFW_PRESS)
    m_tool = Tool::Shovel;

  // temp scroll wheel seed selection
  float wheel = ImGui::GetIO().MouseWheel; // + up / - down, this frame
  if (wheel > 0)
    CycleTool(-1);
  else if (wheel < 0)
    CycleTool(+1);

  if (glfwGetMouseButton(&window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS){
    if (m_tool == Tool::SeedPacket){ m_menu_mode = MenuMode::SeedSelection; }
    else if (m_tool == Tool::Wrench){ m_menu_mode = MenuMode::StructureSelection; }
  }
}

void GardenScene::render(GLFWwindow &window, Renderer &renderer) {
  int w, h;
  glfwGetWindowSize(&window, &w, &h);
  if (w == 0 || h == 0) { return; } 

  const float ui = GetUiScale(h);
  
  float day01 = 0.5f + 0.5f * std::sin(m_elapsed * glm::two_pi<float>() / DAY_LENGTH); // 0 = midnight, 1 = noon; smooth ebb and flow
  glm::vec3 sky = glm::mix(NIGHT_SKY, DAY_SKY, day01);
  renderer.Clear(sky.r, sky.g, sky.b, 1.0f);
  renderer.BeginBatchDraw(500, 500);

  std::vector<PointLight> lights = m_lights;

  if (m_show_highlight)
    lights.push_back(m_highlight);

  // player torch — a pool of light that follows you, fading in as night falls
  // so the ground stays readable without a daytime spotlight artifact
  PointLight player_light;
  player_light.position = m_camera.GetLocation();
  player_light.color = PLAYER_LIGHT_COLOR * (1.0f - day01);
  player_light.radius = PLAYER_LIGHT_RADIUS;
  lights.push_back(player_light);

  float ambient = glm::mix(NIGHT_AMBIENT, DAY_AMBIENT, day01);
  renderer.SetLights(lights, ambient * m_brightness);

  SetupRenderingObjects(renderer);

  for (auto &e : m_events)
    e->Render(renderer, w, h);

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
    glm::vec3 forward_horiz =
        glm::normalize(glm::vec3(forward.x, -0.0f, forward.z));

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
    sushi.position = campos + glm::vec3(0.0f, -EMBED_SUSHI_DROP, 0.0f) +
                     forward_horiz * EMBED_SUSHI_FORWARD +
                     right * EMBED_SUSHI_SIDE;

    sushi.model_mat = m;
    renderer.SubmitTransparentSprite(sushi);
  }

  if (Texture *tex = ToolTexture(m_tool)) {
    glm::vec3 up = glm::normalize(glm::cross(right, forward));

    // full billboard (faces camera), then a 45° in-plane roll for the "held"
    // tilt
    glm::mat4 billboard = glm::transpose(glm::mat4(glm::mat3(view)));
    billboard = billboard * glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f),
                                        glm::vec3(0, 0, 1));

    SpriteInstance toolspr;
    toolspr.texture = tex;
    toolspr.size = TOOL_SIZE;
    toolspr.color = glm::vec4(1.0f);
    toolspr.position =
        campos + forward * TOOL_FWD + right * TOOL_SIDE - up * TOOL_DROP;
    toolspr.model_mat = billboard;
    renderer.SubmitTransparentSprite(toolspr);
  }

  renderer.RendBatch(view, projection, campos, glm::mix(NIGHT_FOG, DAY_FOG, day01) * (1 / (m_brightness)));

  // HUD and UI (ImGUI)

  ImGui::SetNextWindowPos(ImVec2(w * 0.75f, h - 60.0f), ImGuiCond_Always,
                            ImVec2(0.5f, 0.0f));
  ImGui::SetNextWindowBgAlpha(0.0f);
  ImGui::Begin("##task", nullptr,
                ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
                    ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_AlwaysAutoResize);

  if (!m_task_text.empty()) {
    const char *task_text = m_task_text.c_str();
    ImGui::Text("Task: %s", task_text);
  }
  else if (m_tier < (int)TIER_COST.size()){
    std::string t = "Reach tier " + std::to_string(m_tier) + " - " + std::to_string(TIER_COST[m_tier]) + "g";
    ImGui::Text("Task: %s", t.c_str());
  }
  ImGui::SetWindowFontScale(ui);
  ImGui::End();

  std::string hud_text = std::string("Tool: ") + GetToolName(m_tool);

  if (m_tool == Tool::SeedPacket) {
    hud_text += "\nSeed: ";
    hud_text += (m_selected_seed == -1)
        ? "none"
        : m_seeds[m_selected_seed].def.name + " x" +
              std::to_string(m_seeds[m_selected_seed].count);
  }
  if (m_tool == Tool::Wrench) {
    hud_text += "\nStructure: ";
    hud_text += (m_selected_structure == -1)
        ? "none"
        : m_structure_inv[m_selected_structure].def.name + " x" +
              std::to_string(m_structure_inv[m_selected_structure].count);
  }
  hud_text += "\nTime: " + std::to_string((int)m_elapsed) +
              "\nBiomass: " + std::to_string(m_biomass) +
              " g\nTier: " + std::to_string(m_tier);

  ImGui::SetNextWindowPos(ImVec2(w * 0.05f, h - 250.0f), ImGuiCond_Always,
                          ImVec2(0.0f, 0.0f));
  ImGui::SetNextWindowBgAlpha(0.0f);
  ImGui::Begin("##hud", nullptr,
               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::Text("%s", hud_text.c_str());
  ImGui::SetWindowFontScale(ui);
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
    ImGui::SetWindowFontScale(ui);

    ImGui::Text("Biomass: %lld g", m_biomass);
    ImGui::Separator();

    for (int n = 0; n < (int)m_seeds.size(); n++) {
      Seed &seed = m_seeds[n];

      ImGui::PushID(n);
      if (seed.def.tier > m_tier) {
        ImGui::Text("%s  (locked, tier %d)", seed.def.name.c_str(),
                    seed.def.tier);
        ImGui::PopID();
        continue;
      }
      auto buy = [&](int qty) {
        long long cost = (long long)seed.def.biomass_cost * qty;
        if (m_biomass >= cost) {
          m_biomass -= cost;
          seed.count += qty;
        } else
          m_notification_manager.Push("Not enough biomass", 1.5f);
      };
      if (ImGui::Button("Buy")) buy(1);
      ImGui::SameLine();
      if (ImGui::Button("x5")) buy(5);
      ImGui::SameLine();
      if (ImGui::Button("x10")) buy(10);
      ImGui::SameLine();
      if (ImGui::Button("x100")) buy(100);
      ImGui::SameLine();
      ImGui::Text("%s  (have %d)  -  %d g", seed.def.name.c_str(), seed.count,
                  seed.def.biomass_cost);
      ImGui::PopID();
    }
    ImGui::Separator();
    if (ImGui::Button("Close")) {
      m_menu_mode = MenuMode::None;
    }
    ImGui::End();
  } else if (m_menu_mode == MenuMode::UpgradeShop) {
    ImGui::SetNextWindowPos(ImVec2(w * 0.5f, h * 0.5f), ImGuiCond_Always,
                            ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowBgAlpha(0.9f);
    ImGui::Begin("Upgrade Store", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SetWindowFontScale(ui);
    ImGui::Text("Biomass: %lld g", m_biomass);
    ImGui::Separator();
    if (m_tier >= (int)TIER_COST.size()) {
      ImGui::Text("Max tier reached.");
    } else {
      ImGui::Text("Tier %d -> Tier %d - %lld g", m_tier, m_tier + 1, TIER_COST[m_tier]);
      if (ImGui::Button("Upgrade")) {
        if (m_biomass >= TIER_COST[m_tier]) {
          m_biomass -= TIER_COST[m_tier];
          m_tier++;
          sound_manager.PlaySound("bell");

          if (m_tier >= (int)TIER_COST.size()) {
            StartEvent(std::make_unique<RoundAndRipeEvents::EndEvent>(*this));
          } else {
            StartEvent(std::make_unique<RoundAndRipeEvents::NotificationEvent>(
                *this, RoundAndRipeEvents::TierUpLines(m_tier)));
            PlaySound("meow_talk");
            m_menu_mode = MenuMode::None;
          }
        } else
          m_notification_manager.Push("Not enough biomass", 1.5f);
      }
    }
    if (ImGui::Button("Close"))
      m_menu_mode = MenuMode::None;
    ImGui::End();
  } else if (m_menu_mode == MenuMode::SeedSelection) {
    ImGui::SetNextWindowPos(ImVec2(w * 0.5f, h * 0.5f), ImGuiCond_Always,
                            ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowBgAlpha(0.9f);
    ImGui::Begin("Seeds", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SetWindowFontScale(ui);

    bool any_shown = false;
    for (int n = 0; n < (int)m_seeds.size(); n++) {
      Seed &seed = m_seeds[n];
      if (seed.count <= 0) continue; // only seeds the player actually has in stock
      ImGui::PushID(n);
      any_shown = true;

      if (ImGui::Button("Select")) {
        m_selected_seed = n;
        m_menu_mode =
            MenuMode::None; // pick -> close, straight back to planting
      }
      ImGui::SameLine();
      ImGui::Text("%s%s  x%d", (m_selected_seed == n) ? "> " : "",
                  seed.def.name.c_str(), seed.count);

      ImGui::PopID();
    }
    if (!any_shown)
      ImGui::Text("No seeds. Buy some at the seed maker.");

    ImGui::Separator();
    if (ImGui::Button("Close"))
      m_menu_mode = MenuMode::None;
    ImGui::End();
  } else if (m_menu_mode == MenuMode::StructureSelection){
    ImGui::SetNextWindowPos(ImVec2(w * 0.5f, h * 0.5f), ImGuiCond_Always,
                                ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowBgAlpha(0.9f);
        ImGui::Begin("Structures", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::SetWindowFontScale(ui);

        bool any_shown = false;
        for (int n = 0; n < (int)m_structure_inv.size(); n++) {
          StructureInv &s = m_structure_inv[n];
          if (s.count <= 0) continue; 
          ImGui::PushID(n);
          any_shown = true;

          if (ImGui::Button("Select")) {
            m_selected_structure = n;
            m_menu_mode =
                MenuMode::None; 
          }
          ImGui::SameLine();
          ImGui::Text("%s%s  x%d : %s", (m_selected_structure == n) ? "> " : "",
                      s.def.name.c_str(), s.count, s.def.description.c_str());
          ImGui::PopID();
        }
        if (!any_shown)
          ImGui::Text("No structures. Buy some at the structure maker.");

        ImGui::Separator();
        if (ImGui::Button("Close"))
          m_menu_mode = MenuMode::None;
        ImGui::End();
  }
  else if (m_menu_mode == MenuMode::StructureShop){
    ImGui::SetNextWindowPos(ImVec2(w * 0.5f, h * 0.5f), ImGuiCond_Always,
                                ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowBgAlpha(
            0.9f); // visible panel (your HUD used 0 = invisible)
        ImGui::Begin("Structure Store", nullptr,
                    ImGuiWindowFlags_AlwaysAutoResize); // NOTE: no NoInputs — it
                                                        // must take clicks
        ImGui::SetWindowFontScale(ui);

        ImGui::Text("Biomass: %lld g", m_biomass);
        ImGui::Separator();

        for (int n = 0; n < (int)m_structure_inv.size(); n++) {
          StructureInv &st = m_structure_inv[n];

          ImGui::PushID(n);
          if (st.def.tier > m_tier) {
            ImGui::Text("%s  (locked, tier %d)", st.def.name.c_str(),
                        st.def.tier);
            ImGui::PopID();
            continue;
          }
          auto buy = [&](int qty) {
            long long cost = (long long)st.def.biomass_cost * qty;
            if (m_biomass >= cost) {
              m_biomass -= cost;
              st.count += qty;
            } else
              m_notification_manager.Push("Not enough biomass", 1.5f);
          };
          if (ImGui::Button("Buy")) buy(1);
          ImGui::SameLine();
          if (ImGui::Button("x5")) buy(5);
          ImGui::SameLine();
          if (ImGui::Button("x10")) buy(10);
          ImGui::SameLine();
          ImGui::Text("%s  (have %d)  -  %d g", st.def.name.c_str(), st.count,
                      st.def.biomass_cost);
          ImGui::PopID();
    }
    if (ImGui::Button("Close"))
      m_menu_mode = MenuMode::None;
    ImGui::End();
  }
  else if (m_menu_mode == MenuMode::Settings) {
    ImGui::SetNextWindowPos(ImVec2(w * 0.5f, h * 0.5f), ImGuiCond_Always,
                            ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowBgAlpha(0.9f);
    ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SetWindowFontScale(ui);

    if (ImGui::SliderFloat("Volume", &m_master_volume, 0.0f, 1.0f))
      sound_manager.SetMasterVolume(m_master_volume);

    ImGui::SliderFloat("Brightness", &m_brightness, 0.3f,
                       2.0f); // applied via SetLights each frame

    ImGui::SliderFloat("Mouse Sensitivity", &m_controller.MouseSensitivity,
                       0.01f, 0.15f);
    ImGui::Checkbox("Invert Y", &m_controller.InvertY);

    ImGui::SliderFloat("UI Scale", &m_font_size, 1.0f, 3.0f);

    if (ImGui::Checkbox("Windowed Fullscreen", &m_borderless))
      GardenEngine::SetBorderless(window, m_borderless);

    ImGui::Separator();
    if (ImGui::Button("Close Menu"))
      m_menu_mode = MenuMode::None;
    
    ImGui::Separator();
    if (!m_confirm_new) {
      if (ImGui::Button("New Game"))
        m_confirm_new = true;
    } else {
      ImGui::Text("Erase your run?");
      ImGui::SameLine();
      if (ImGui::Button("Yes, erase")) {
        m_new_game = true;
        m_confirm_new = false;
      }
      ImGui::SameLine();
      if (ImGui::Button("Cancel"))
        m_confirm_new = false;
    }
    ImGui::Separator();
    if (ImGui::Button("Quit Game"))
      glfwSetWindowShouldClose(&window, true); 
    ImGui::End();
  }

  if (m_outcome == Outcome::Lost) {
    ImGui::GetForegroundDrawList()->AddRectFilled(
        ImVec2(0, 0), ImVec2((float)w, (float)h), IM_COL32(0, 0, 0, 200));
    const char *msg = "You have been recycled.";
    ImGui::SetNextWindowPos(ImVec2(w * 0.5f, h * 0.5f), ImGuiCond_Always,
                            ImVec2(0.5f, 0.5f));
    ImGui::Begin("##end", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
                     ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SetWindowFontScale(ui * 1.5f);
    ImGui::Text("%s", msg);
    ImGui::End();
  }

  // tool bar
  const float slot = 48.0f * ui / 2.0f;   // icon size, tracks UI scale
  ImGui::SetNextWindowPos(ImVec2(w * 0.5f, h - 10.0f), ImGuiCond_Always, ImVec2(0.5f, 1.0f));
  ImGui::SetNextWindowBgAlpha(0.35f);
  ImGui::Begin("##hotbar", nullptr,
              ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
                  ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
  for (int i = 0; i < 6; i++) {
    Tool t = (Tool)i;
    if (i) ImGui::SameLine();
    bool selected = (t == m_tool);
    ImVec4 tint   = selected ? ImVec4(1,1,1,1) : ImVec4(1,1,1,0.35f);   // dim unselected
    ImVec4 border = selected ? ImVec4(1,1,0,1) : ImVec4(0,0,0,0);       // gold ring on current

    ImGui::BeginGroup();
    if (Texture* tex = ToolTexture(t))
      ImGui::Image((ImTextureID)(intptr_t)tex->GetID(), ImVec2(slot, slot),
                  ImVec2(0, 1), ImVec2(1, 0), tint, border);
    else {   // Hands has no texture — placeholder slot
      ImGui::TextColored(tint, "  X  ");
    }
    ImGui::SetWindowFontScale(ui * 0.6f);
    ImGui::Text("  %d", i + 1);          // key label under each slot
    ImGui::SetWindowFontScale(ui);
    ImGui::EndGroup();
  }
  ImGui::End();



  m_notification_manager.Render(w, h, ui);
}
