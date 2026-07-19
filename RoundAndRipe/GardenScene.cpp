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
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include "imgui.h"
#include <cctype>
#include <json.hpp>
#include <string>


GardenScene::GardenScene()
    : sound_manager(SimpleSoundManager::Instance()),
      m_wall_texture("./res/textures/concrete_wall.png"),
      m_fence_texture("./res/textures/fence.png", true),   // repeat-wrapped for tiling
      m_floor_texture("./res/textures/gravel_floor.png"),
      m_sushi_texture("./res/textures/sushi.png"),
      m_sushi_eat_texture("./res/textures/sushi_eat.png"),
      m_structure_maker_texture("./res/textures/structure_maker.png"),
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
      m_hand_texture("./res/textures/hand.png"),
      m_sprinkler_texture("./res/textures/sprinkler.png"),
      m_hoer_texture("./res/textures/hoer.png"),
      m_harvester_texture("./res/textures/harvester.png"),
      m_planter_loaded_texture("./res/textures/planter_loaded.png"),
      m_planter_unloaded_texture("./res/textures/planter_unloaded.png"),
      m_veg_top_texture("./res/textures/veg_top.png"),
      m_apple_texture("./res/textures/apple.png"),
      m_radish_texture("./res/textures/radish.png"),
      m_turnip_texture("./res/textures/turnip.png"),
      m_bush_texture("./res/textures/bush.png"),
      m_tomato_texture("./res/textures/tomato_bush.png"),
      m_blueberry_texture("./res/textures/blueberry_bush.png"),
      m_kidneybean_texture("./res/textures/kidneybean_bush.png"),
      m_staring_melon_growing_texture("./res/textures/eye_plant.png"),
      m_staring_melon_ripe_texture("./res/textures/eye_plant_ripe.png"),
      m_liverbounty_grow("./res/textures/liver_grow.png"),
      m_liverbounty_ripe("./res/textures/liver_ripe.png"),
      m_field(glm::vec3(-5.0f, 0.0f, -5.0f), 12, 30, 1.0f, &m_soil_texture, SOIL_COLOR,
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
  m_show_crosshair = cfg.crosshair;
  GardenEngine::SetBorderless(window, m_borderless);

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

  m_highlight.color = glm::vec3(1.0f, 1.0f, 1.0f);
  m_highlight.position =
      glm::vec3(seed_maker_position.x, 0.1f, seed_maker_position.z);
  m_highlight.radius = 0.25f;

  AddFloor(glm::vec3(0.0f), glm::vec2(FLOOR_TILE_SIZE), &m_floor_texture, FLOOR_COLOR);
  AddWall(glm::vec3(0.0f, 0.0f, FLOOR_TILE_SIZE / 2),
          glm::vec2(FLOOR_TILE_SIZE, 4.0f), &m_wall_texture);
  AddWall(glm::vec3(0.0f, 0.0f, -FLOOR_TILE_SIZE / 2),
          glm::vec2(FLOOR_TILE_SIZE, 4.0f), &m_wall_texture);
  AddWallRotated(glm::vec3(FLOOR_TILE_SIZE / 2, 0.0f, 0.0f),
                 glm::vec2(FLOOR_TILE_SIZE, 4.0f), 90.0f, &m_wall_texture);
  AddWallRotated(glm::vec3(-FLOOR_TILE_SIZE / 2, 0.0f, 0.0f),
                 glm::vec2(FLOOR_TILE_SIZE, 4.0f), 90.0f, &m_wall_texture);

  // ── the pen: low fence you see over. Collision tests at CAMERA height
  // (1.6), so the visible rail alone is walk-through — each side also gets
  // an invisible 6-high collider when solid.
  // rails use the TRANSPARENT batch (via AddGlassRotated) so the picket gaps
  // show what's behind them — opaque quads write depth even at alpha-0 pixels
  auto add_fence = [this](glm::vec2 mn, glm::vec2 mx, bool solid) {
    glm::vec2 c((mn.x + mx.x) * 0.5f, (mn.y + mx.y) * 0.5f);
    glm::vec2 sz(mx.x - mn.x, mx.y - mn.y);
    float y = FENCE_HEIGHT * 0.5f;   // rail bottom sits on the ground
    AddGlassRotated(glm::vec3(c.x, y, mx.y), glm::vec2(sz.x, FENCE_HEIGHT), 0.0f, 1.0f, &m_fence_texture)
        .uv_scale = glm::vec2(sz.x / FENCE_TILE_WIDTH, 1.0f);
    AddGlassRotated(glm::vec3(c.x, y, mn.y), glm::vec2(sz.x, FENCE_HEIGHT), 0.0f, 1.0f, &m_fence_texture)
        .uv_scale = glm::vec2(sz.x / FENCE_TILE_WIDTH, 1.0f);
    AddGlassRotated(glm::vec3(mx.x, y, c.y), glm::vec2(sz.y, FENCE_HEIGHT), 90.0f, 1.0f, &m_fence_texture)
        .uv_scale = glm::vec2(sz.y / FENCE_TILE_WIDTH, 1.0f);
    AddGlassRotated(glm::vec3(mn.x, y, c.y), glm::vec2(sz.y, FENCE_HEIGHT), 90.0f, 1.0f, &m_fence_texture)
        .uv_scale = glm::vec2(sz.y / FENCE_TILE_WIDTH, 1.0f);
    if (solid) {
      AddWall(glm::vec3(c.x, 0.0f, mx.y), glm::vec2(sz.x, 6.0f), &m_wall_texture).visible = false;
      AddWall(glm::vec3(c.x, 0.0f, mn.y), glm::vec2(sz.x, 6.0f), &m_wall_texture).visible = false;
      AddWallRotated(glm::vec3(mx.x, 0.0f, c.y), glm::vec2(sz.y, 6.0f), 90.0f, &m_wall_texture).visible = false;
      AddWallRotated(glm::vec3(mn.x, 0.0f, c.y), glm::vec2(sz.y, 6.0f), 90.0f, &m_wall_texture).visible = false;
    }
  };
  add_fence(PEN_MIN, PEN_MAX, true);

  // ── the neighbours: mock pens — dirt patch, fake crops, a parked machine.
  // Nothing out here is simulated; it only has to survive being looked at
  // over a fence.
  struct MockPen { glm::vec2 mn, mx; Texture *crop; Texture *machine; };
  // first four host a wanderer; no staring melon out here — that crop is a
  // tier-3 surprise, not scenery
  const MockPen pens[] = {
      {{20.0f, -5.0f},  {33.0f, 25.0f},  &m_tomato_texture,     &m_sprinkler_texture},
      {{-33.0f, -5.0f}, {-20.0f, 25.0f}, &m_kidneybean_texture, &m_harvester_texture},
      {{-24.0f, 36.0f}, {-4.0f, 46.0f},  &m_turnip_texture,     &m_hoer_texture},
      {{4.0f, -28.0f},  {24.0f, -18.0f}, &m_radish_texture,     &m_sprinkler_texture},
      {{4.0f, 36.0f},   {24.0f, 46.0f},  &m_blueberry_texture,  &m_sprinkler_texture},
      {{-24.0f, -28.0f},{-4.0f, -18.0f}, &m_tomato_texture,     &m_harvester_texture},
  };
  for (const auto &p : pens) {
    glm::vec2 c((p.mn.x + p.mx.x) * 0.5f, (p.mn.y + p.mx.y) * 0.5f);
    glm::vec2 sz(p.mx.x - p.mn.x, p.mx.y - p.mn.y);
    AddFloor(glm::vec3(c.x, 0.01f, c.y), sz, &m_soil_texture, SOIL_COLOR);
    add_fence(p.mn, p.mx, false);   // pure decor: no colliders needed out there

    // ragged crop rows: jittered grid so it doesn't read as copy-paste
    for (float x = p.mn.x + 1.5f; x < p.mx.x - 1.0f; x += 3.0f) {
      for (float z = p.mn.y + 1.5f; z < p.mx.y - 1.0f; z += 4.0f) {
        AmbientSprite a;
        float s = 0.7f + (rand() % 40) / 100.0f;   // 0.7–1.1
        a.sprite.texture = p.crop;
        a.sprite.size = glm::vec2(s, s);
        a.sprite.color = glm::vec4(1.0f);
        a.sprite.position =
            glm::vec3(x + (rand() % 60 - 30) / 100.0f, s / 2.0f,
                      z + (rand() % 60 - 30) / 100.0f);
        a.phase = (rand() % 628) / 100.0f;         // 0..2π
        m_ambient_sprites.push_back(a);
      }
    }

    AmbientSprite machine;                         // parked mid-pen, rigid
    machine.sprite.texture = p.machine;
    machine.sprite.size = glm::vec2(0.8f);
    machine.sprite.color = glm::vec4(1.0f);
    machine.sprite.position = glm::vec3(c.x, 0.4f, c.y);
    machine.sway = 0.0f;
    m_ambient_sprites.push_back(machine);
  }

  // ── fellow prisoners: one pacing each of the first four pens
  for (int i = 0; i < 4; i++) {
    const MockPen &p = pens[i];
    Wanderer w;
    w.sprite.texture = &m_human_texture;
    w.sprite.size = BODY_SIZE;
    w.sprite.color = glm::vec4(1.0f);
    w.pen_min = p.mn + glm::vec2(1.0f);
    w.pen_max = p.mx - glm::vec2(1.0f);
    glm::vec2 c = (w.pen_min + w.pen_max) * 0.5f;
    w.sprite.position = glm::vec3(c.x, BODY_SIZE.y / 2.0f, c.y);
    w.target = w.sprite.position;
    w.pause = (rand() % 300) / 100.0f;   // desync their first moves
    m_wanderers.push_back(w);
  }

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
  seed_maker_trigger.label = "Seed Maker";
  seed_maker_trigger.on_triggered = [this]() {
    m_menu_mode = MenuMode::SeedShop;
  };
  m_triggers.push_back(seed_maker_trigger);

  // house
  m_structure_maker.texture = &m_structure_maker_texture;
  m_structure_maker.size = glm::vec2(HOUSE_SIZE, HOUSE_SIZE);
  m_structure_maker.color = glm::vec4(1.0f);
  m_structure_maker.position = HOUSE_POS;
  m_static_sprites.push_back(m_structure_maker);

  TriggerVolume house_trigger;
  house_trigger.position = HOUSE_POS;
  house_trigger.size = glm::vec3(HOUSE_SIZE, HOUSE_SIZE, 0.10f);
  house_trigger.time_to_trigger = 0.01f;
  house_trigger.type = TriggerType::Interact;
  house_trigger.label = "Structure Maker";
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
  chest_trigger.label = "Quota Chest";
  chest_trigger.on_triggered = [this]() {
    m_menu_mode = MenuMode::UpgradeShop;
  };
  m_triggers.push_back(chest_trigger);

  // Apple
  float apple_size = 0.25f;
  m_apple.texture = &m_apple_texture;
  m_apple.size = glm::vec2(apple_size, apple_size);
  m_apple.color = glm::vec4(1.0f);

  m_apple_trigger.time_to_trigger = 0.01f;
  m_apple_trigger.type = TriggerType::Interact;
  m_apple_trigger.size = glm::vec3(apple_size * 1.5f);
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
  m_seeds.push_back({StaringMelon(&m_staring_melon_growing_texture,&m_staring_melon_ripe_texture),0});
  m_seeds.push_back({LiverBouquet(&m_liverbounty_grow, &m_liverbounty_ripe), 0});

  m_structure_inv.push_back({Sprinkler(&m_sprinkler_texture), 0});
  m_structure_inv.push_back({Harvester(&m_harvester_texture), 0});
  m_structure_inv.push_back({Hoer(&m_hoer_texture), 0});
  m_structure_inv.push_back({Planter(&m_planter_unloaded_texture, &m_planter_loaded_texture), 0});

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
  cfg.crosshair = m_show_crosshair;
  SaveSystem::SaveSettings(SETTINGS_PATH, cfg);

  glfwSetInputMode(&window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

Scene *GardenScene::update(GLFWwindow &window, float delta) {
  if (m_new_game)
    return new GardenScene();

  m_notification_manager.Update(delta);

  m_elapsed += delta;
  m_random_event_timer += delta;
  // if (m_elapsed >= TIME_LIMIT && m_outcome == Outcome::Playing)
  //   m_outcome = Outcome::Lost;

  m_save_timer += delta;
  if (m_save_timer >= SAVE_INTERVAL) {
    Save();
    m_save_timer = 0.0f;
  }

  for (auto &t : m_field.Tiles())
    t.Update(delta);

  // neighbours pace their pens: walk → pause (chores) → pick a new spot
  for (auto &w : m_wanderers) {
    if (w.pause > 0.0f) {
      w.pause -= delta;
      if (w.pause <= 0.0f) {
        float tx = w.pen_min.x +
                   (rand() % 100) / 100.0f * (w.pen_max.x - w.pen_min.x);
        float tz = w.pen_min.y +
                   (rand() % 100) / 100.0f * (w.pen_max.y - w.pen_min.y);
        w.target = glm::vec3(tx, w.sprite.position.y, tz);
      }
      continue;
    }
    glm::vec3 to = w.target - w.sprite.position;
    float dist = glm::length(to);
    if (dist < 0.05f) {
      w.pause = 2.0f + (rand() % 400) / 100.0f;   // 2–6s of standing around
    } else {
      w.sprite.position += (to / dist) * std::min(WANDERER_SPEED * delta, dist);
    }
  }

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
  m_harvest_count += machines.harvests;
  if (machines.tilled)                        PlaySound("dig", 0.35f);
  if (machines.planted || machines.harvested) PlaySound("pop", 0.35f);

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
  // (dead once the ending is running — the end screen must stay up)
  bool esc = glfwGetKey(&window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
  if (esc && !m_esc_held && m_outcome == Outcome::Playing) {
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
  // PointLight player_light;
  // player_light.position = m_camera.GetLocation();
  // player_light.color = PLAYER_LIGHT_COLOR * (1.0f - day01);
  // player_light.radius = PLAYER_LIGHT_RADIUS;
  // lights.push_back(player_light);

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

  // ambient dressing: neighbours' crops sway in the wind, machines stand rigid
  for (auto &a : m_ambient_sprites) {
    glm::vec3 to_cam = campos - a.sprite.position;
    float yaw = std::atan2(to_cam.x, to_cam.z);
    glm::mat4 m = glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0, 1, 0));
    if (a.sway > 0.0f) {
      float lean = std::sin((float)m_elapsed * 1.5f + a.phase) * 0.06f * a.sway;
      m = m * glm::rotate(glm::mat4(1.0f), lean, glm::vec3(0, 0, 1));
    }
    a.sprite.model_mat = m;
    renderer.SubmitTransparentSprite(a.sprite);
  }

  // fellow prisoners: yaw-billboard + a small bob while walking
  for (auto &w : m_wanderers) {
    glm::vec3 to_cam = campos - w.sprite.position;
    float yaw = std::atan2(to_cam.x, to_cam.z);
    SpriteInstance s = w.sprite;   // copy: the bob must not accumulate
    s.model_mat = glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0, 1, 0));
    if (w.pause <= 0.0f)
      s.position.y += std::sin((float)m_elapsed * 10.0f) * 0.03f;
    renderer.SubmitTransparentSprite(s);
  }

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

  ImGui::SetNextWindowPos(ImVec2(w * 0.95f, h - 100.0f), ImGuiCond_Always,
                            ImVec2(1.0f, 0.0f));
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
    std::string t = "Reach tier " + std::to_string(m_tier) + " - " + FormatBiomass(TIER_COST[m_tier]);
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
  hud_text += "\nBiomass: " + FormatBiomass(m_biomass) +
              "\nTier: " + std::to_string(m_tier);

  ImGui::SetNextWindowPos(ImVec2(w * 0.05f, h - 250.0f), ImGuiCond_Always,
                          ImVec2(0.0f, 0.0f));
  ImGui::SetNextWindowBgAlpha(0.0f);
  ImGui::Begin("##hud", nullptr,
               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::Text("%s", hud_text.c_str());
  ImGui::SetWindowFontScale(ui);
  ImGui::End();

  // crosshair — screen-center cross, hidden while a menu has the cursor
  if (m_show_crosshair && m_menu_mode == MenuMode::None) {
    ImDrawList *dl = ImGui::GetForegroundDrawList();
    ImVec2 c(w * 0.5f, h * 0.5f);
    float arm = 7.0f * ui;
    float gap = 3.0f * ui;                       // hollow center so it doesn't hide the aim point
    ImU32 col = IM_COL32(235, 210, 170, 120);   // warm parchment, faded
    float th = 2.0f;
    dl->AddLine(ImVec2(c.x - arm, c.y), ImVec2(c.x - gap, c.y), col, th);
    dl->AddLine(ImVec2(c.x + gap, c.y), ImVec2(c.x + arm, c.y), col, th);
    dl->AddLine(ImVec2(c.x, c.y - arm), ImVec2(c.x, c.y - gap), col, th);
    dl->AddLine(ImVec2(c.x, c.y + gap), ImVec2(c.x, c.y + arm), col, th);
  }

  // interaction note — what you're aiming at: building > structure > tile
  if (m_menu_mode == MenuMode::None && m_outcome == Outcome::Playing) {
    std::string aim_text;

    if (const TriggerVolume *tv = m_controller.AimedInteractable(m_triggers)) {
      aim_text = tv->label;
    } else if (Tile *t = m_field.TileAtRay(campos, forward)) {
      if (t->HasStructure()) {
        if (Structure *s = m_field.StructureAtTile(*t)) {
          aim_text = s->def->name;
          if (!aim_text.empty()) aim_text[0] = (char)toupper(aim_text[0]);
          if (s->def->kind == StructureKind::Planter)
            aim_text += s->crop ? "\nLoaded: " + s->crop->name
                                : "\nEmpty - load with a seed packet";
        }
      } else if (t->IsRefuse()) {
        aim_text = "Rock - clear it with the shovel";
      } else if (t->IsTilled()) {
        aim_text = "Tilled soil - ready for seeds";
      } else if (t->HasPlant()) {
        aim_text = t->GetPlantName();
        aim_text += t->IsHarvestable() ? " - ready to harvest" : " - growing";
      }
      // bare empty soil: no note — silence keeps the field calm
    }

    if (!aim_text.empty()) {
      ImGui::SetNextWindowPos(ImVec2(w * 0.5f, h * 0.5f + 30.0f * ui),
                              ImGuiCond_Always, ImVec2(0.5f, 0.0f));
      ImGui::SetNextWindowBgAlpha(0.35f);
      ImGui::Begin("##aimnote", nullptr,
                   ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
                       ImGuiWindowFlags_NoMove |
                       ImGuiWindowFlags_AlwaysAutoResize);
      ImGui::SetWindowFontScale(ui);
      // centre each line: window is as wide as the longest line, so
      // shorter lines need their cursor nudged to the middle
      size_t start = 0;
      while (start <= aim_text.size()) {
        size_t end = aim_text.find('\n', start);
        std::string line = aim_text.substr(
            start, end == std::string::npos ? std::string::npos : end - start);
        float pad = (ImGui::GetWindowSize().x -
                     ImGui::CalcTextSize(line.c_str()).x) * 0.5f;
        ImGui::SetCursorPosX(pad > 0.0f ? pad : 0.0f);
        ImGui::TextUnformatted(line.c_str());
        if (end == std::string::npos) break;
        start = end + 1;
      }
      ImGui::End();
    }
  }

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
                 ImGuiWindowFlags_AlwaysAutoResize |
                     ImGuiWindowFlags_NoCollapse); // NOTE: no NoInputs — it
                                                   // must take clicks
    ImGui::SetWindowFontScale(ui);

    ImGui::Text("Biomass: %s", FormatBiomass(m_biomass).c_str());
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
    ImGui::Begin("Upgrade Store", nullptr,
                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
    ImGui::SetWindowFontScale(ui);
    ImGui::Text("Biomass: %s", FormatBiomass(m_biomass).c_str());
    ImGui::Separator();
    if (m_tier >= (int)TIER_COST.size()) {
      int total = (int)(m_final_time > 0.0 ? m_final_time : m_elapsed);
      int harvests = m_final_harvests > 0 ? m_final_harvests : m_harvest_count;
      ImGui::Text("Max tier reached.");
      ImGui::Text("Your sentence was %d minutes %d seconds", total / 60,
                  total % 60);
      ImGui::Text("Plants harvested: %d", harvests);
    } else {
      ImGui::Text("Tier %d -> Tier %d - %s", m_tier, m_tier + 1,
                  FormatBiomass(TIER_COST[m_tier]).c_str());
      if (ImGui::Button("Upgrade")) {
        if (m_biomass >= TIER_COST[m_tier]) {
          m_biomass -= TIER_COST[m_tier];
          m_tier++;
          sound_manager.PlaySound("bell");

          if (m_tier >= (int)TIER_COST.size()) {
            m_final_time = m_elapsed;
            m_final_harvests = m_harvest_count;
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
    if (m_tier < (int)TIER_COST.size()) {   // ending on screen — no way out
      if (ImGui::Button("Close"))
        m_menu_mode = MenuMode::None;
    }
    ImGui::End();
  } else if (m_menu_mode == MenuMode::SeedSelection) {
    ImGui::SetNextWindowPos(ImVec2(w * 0.5f, h * 0.5f), ImGuiCond_Always,
                            ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowBgAlpha(0.9f);
    ImGui::Begin("Seeds", nullptr,
                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
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
        ImGui::Begin("Structures", nullptr,
                     ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
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
                    ImGuiWindowFlags_AlwaysAutoResize |
                        ImGuiWindowFlags_NoCollapse); // NOTE: no NoInputs — it
                                                      // must take clicks
        ImGui::SetWindowFontScale(ui);

        ImGui::Text("Biomass: %s", FormatBiomass(m_biomass).c_str());
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
    ImGui::Begin("Settings", nullptr,
                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
    ImGui::SetWindowFontScale(ui);

    if (ImGui::SliderFloat("Volume", &m_master_volume, 0.0f, 1.0f))
      sound_manager.SetMasterVolume(m_master_volume);

    ImGui::SliderFloat("Brightness", &m_brightness, 0.3f,
                       2.0f); // applied via SetLights each frame

    ImGui::SliderFloat("Mouse Sensitivity", &m_controller.MouseSensitivity,
                       0.01f, 0.15f);
    ImGui::Checkbox("Invert Y", &m_controller.InvertY);
    ImGui::Checkbox("Crosshair", &m_show_crosshair);

    ImGui::SliderFloat("UI Scale", &m_font_size, 1.0f, 3.0f);

    bool windowed = !m_borderless;
    if (ImGui::Checkbox("Windowed", &windowed)) {
      m_borderless = !windowed;
      GardenEngine::SetBorderless(window, m_borderless);
    }

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
      { ImGui::Image((ImTextureID)(intptr_t)tex->GetID(), ImVec2(slot, slot),
                  ImVec2(0, 1), ImVec2(1, 0), tint, border);}

    ImGui::SetWindowFontScale(ui * 0.6f);
    ImGui::Text("  %d", i + 1);          // key label under each slot
    ImGui::SetWindowFontScale(ui);
    ImGui::EndGroup();
  }
  ImGui::End();



  m_notification_manager.Render(w, h, ui);
}
