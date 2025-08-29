#include <chrono>
#include <cstdio>
#include <print>
#include <string>
#include <thread>

#include <glad/glad.h>

// #include "Renderer.hpp"
#include "GLFW/glfw3.h"
#include "glm/trigonometric.hpp"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Engine.hpp"
#include "gl_debug.hpp"
#include "imgui_internal.h"

GardenEngine::GardenEngine(std::string name, int win_width, int win_height) {
  std::println("Creating GardenEngine...");

  setupGlfwWindow(name, win_width, win_height);
  setupOpenGl();
  setupImGui();
  setupGameState();

  test_texture = new Texture("./res/textures/sushi.png");

  std::println("GardenEngine Created!");
}

GardenEngine::~GardenEngine() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  delete m_renderer;
  m_renderer = nullptr;

  delete m_clickCounter;
  m_clickCounter = nullptr;

  glfwDestroyWindow(m_window);
  glfwTerminate();
}

int GardenEngine::Start(float fps) {
  std::println("GardenEngine Starting...");

  // Render loop!!!!!
  glClearColor(0.1, 0.6f, 0.2f, 1.0f);

  int frame_time_limit_ms = (int)((1 / fps) * 1000);

  while (!glfwWindowShouldClose(m_window)) {

    auto start_time = std::chrono::high_resolution_clock::now();

    processInput();
    updateGameState();
    renderScene();

    // finish and fps limit
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = end_time - start_time;
    int ms =
        (int)std::chrono::duration_cast<std::chrono::milliseconds>(duration)
            .count();
    // std::println("Time elapsed to generate frame: {}", ms);
    int waittime = frame_time_limit_ms - ms;
    if (waittime < frame_time_limit_ms * -0.5)
      std::println("WARNING, cannot keep up with set FPS. FPS currently at {}",
                   1000 / ms);
    else
      std::this_thread::sleep_for(std::chrono::milliseconds(waittime));
  }

  // glfwTerminate();
  std::println("GardenEngine Exited Successfully!");
  return 0;
}

void GardenEngine::setupGlfwWindow(std::string win_name, int win_width,
                                   int win_height) {
  std::println("Creating GLFW Window...");

  if (!glfwInit()) {
    std::println("Failed to Initialize GLFW");
    throw std::runtime_error("Failed to Initialize GLFW");
  };

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  int monitor_count;
  GLFWmonitor **_monitors = glfwGetMonitors(&monitor_count);
  if (!monitor_count) {
    std::println("GLFWmonitor found monitors to be null");
    throw std::runtime_error("GLFWMonitor found monitors to be null");
  }

  std::println("GLFWmonitors has found {} monitors", monitor_count);
  for (int i = 0; i < monitor_count; i++) {
    const GLFWvidmode *mode = glfwGetVideoMode(_monitors[i]);
    std::println("Monitor {} has resolution {} x {}", i, mode->width,
                 mode->height);
  }

  //// set fullscreen
  // GLFWmonitor* pMonitor = glfwGetPrimaryMonitor();
  // std::cout << "Using monitor: " << pMonitor << std::endl;
  // m_window = glfwCreateWindow(win_width, win_height, name.c_str(), pMonitor,
  // NULL);

  // set windowed
  m_window =
      glfwCreateWindow(win_width, win_height, win_name.c_str(), NULL, NULL);
  // glfwSwapInterval(0);

  if (!m_window) {
    std::println("Failed to create GLFW window");
    glfwTerminate();
    throw std::runtime_error("Failed to create GLFW window");
  }

  glfwMakeContextCurrent(m_window);

  glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
  std::println("Creating GLFW Window...");
}

void GardenEngine::setupOpenGl() {
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::println("Failed to initialize OpenGL context");
    throw std::runtime_error(
        "Failed to initialize OpenGL context through gladLoadGLLoader");
  }

  m_renderer = new Renderer();
}

void GardenEngine::setupImGui() {
  std::println("Creating ImGui Context...");
  ImGui::CreateContext();
  ImGuiIO io = ImGui::GetIO();
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
  // m_imgui_io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF
  // using Docking Branch
  ImGui_ImplGlfw_InitForOpenGL(
      m_window, true); // Second param install_callback=true will install GLFW
                       // callbacks and chain to existing ones.
  ImGui_ImplOpenGL3_Init();
  std::println("ImGui Context Created!");
}

void GardenEngine::setupGameState() { m_clickCounter = new ClickCounter(); }

void GardenEngine::processInput() {

  GLCall(glfwPollEvents());

  if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(m_window, true);

  if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
    double xpos, ypos;
    glfwGetCursorPos(m_window, &xpos, &ypos);

    int w, h;
    glfwGetWindowSize(m_window, &w, &h);
    float aspect_ratio = (float)w / (float)h;

    glm::vec3 cam_loc = m_camera.GetLocation();

    double nxpos = (xpos / w + cam_loc.x);
    double nypos = (1 - (ypos / h)) + cam_loc.y;

    if (m_first_click == true) {
      m_first_click = false;
      m_clickCounter->click();
    }
  } else {
    m_first_click = true;
  }
  if (glfwGetKey(m_window, GLFW_KEY_E)) {
    m_clickCounter->click();
  }

  float sensitivity = 1.0f;
  if (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    sensitivity = 1.5f;
  }
  float move = 0.01f * sensitivity;

  if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) {
    // locy += 0.001f;
    m_camera.ShiftCamera(0.0f, move, 0.0f);
  }
  if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) {
    // locy -= 0.001f;
    m_camera.ShiftCamera(0.0f, -move, 0.0f);
  }
  if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) {
    // locx -= 0.001f;
    m_camera.ShiftCamera(-move, 0.0f, 0.0f);
  }
  if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) {
    // locx += 0.001f;
    m_camera.ShiftCamera(move, 0.0f, 0.0f);
  }
  if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    m_camera.ShiftCamera(0.0f, 0.0f, -move);
  }
  if (glfwGetKey(m_window, GLFW_KEY_C) == GLFW_PRESS) {
    m_camera.ShiftCamera(0.0f, 0.0f, move);
  }

  // if (approach) {
  //     //locz -= 0.0001f;
  //     if (red < 1.0f) red += 0.00001;
  //     if (other > 0.0f) other -= 0.00001;
  // }
}

void GardenEngine::updateGameState() {

  static int cycles = 0;
  cycles++;

  if (cycles % 100 == 0) {
    m_clickCounter->removeClicks(1);
  }
}

void GardenEngine::renderScene() {

  m_renderer->Clear(0.1f, 0.1f, 0.1f, 1.0f);

  float clicks = (float)m_clickCounter->GetClicks();
  double xpos, ypos;
  glfwGetCursorPos(m_window, &xpos, &ypos);

  glm::vec3 campos = m_camera.GetLocation();

  int w, h;
  glfwGetWindowSize(m_window, &w, &h);
  float aspect = w / h;
  glm::mat4 view = m_camera.GetViewMat();
  glm::mat4 projection = m_camera.GetProjectionMat(w, h);

  float jiggle_x = glm::cos(clicks) * 0.005;
  float jiggle_y = glm::sin(clicks) * 0.005;

  m_renderer->BeginBatchDraw(4);
  SpriteInstance static_sprite;
  static_sprite.position = glm::vec3(jiggle_x, jiggle_y, -clicks * 0.01);
  static_sprite.size = glm::vec2(0.6f, 0.4f);
  static_sprite.rotation = 0.0f;
  static_sprite.color = glm::vec4(0.5f, 1.0f, 0.5f, 1.0f);
  static_sprite.texture = test_texture;
  m_renderer->SubmitSprite(static_sprite);

  SpriteInstance clicker_sprite;
  clicker_sprite.position = glm::vec3(0.0f, 0.0f, -clicks * 0.01);
  if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
    clicker_sprite.color = glm::vec4(1.0f, 0.5f, 0.5f, 1.0f);
  } else {
    static_sprite.color = glm::vec4(0.5f, 1.0f, 0.5f, 1.0f);
  }
  clicker_sprite.size = glm::vec2(0.6f, 0.4f);
  clicker_sprite.rotation = 0.0f;
  clicker_sprite.texture = test_texture;
  m_renderer->SubmitSprite(clicker_sprite);

  // float follow_x = campos.x + ((xpos - w / 2) / w);
  // float follow_y = campos.y + ((1 - (ypos - h / 2)) / h);
  // SpriteInstance follow_sprite;
  // follow_sprite.color = glm::vec4(0.5f, 0.5f, 1.0f, 1.0f);
  // follow_sprite.position = glm::vec3(follow_x, follow_y, 0.0f);
  // // follow_sprite.position = glm::vec2(0.1, 0.1);
  // follow_sprite.rotation = 0.0f;
  // follow_sprite.size = glm::vec2(0.1f, 0.1f);
  // follow_sprite.texture = test_texture;
  // m_renderer->SubmitSprite(follow_sprite);

  m_renderer->RendBatch(view, projection);

  renderImgui(xpos, ypos);

  GLCall(glfwSwapBuffers(m_window));
}

void GardenEngine::renderImgui(double x, double y) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  bool boolle = true;

  if (boolle) {
    ImGuiIO io = ImGui::GetIO();

    ImGui::Begin("Settings"); // Create a window called "Hello, world!" and
                              // append into it.

    // ImGui::Text("This is some useful text.");               // Display some
    // text (you can use a format strings too) ImGui::Checkbox("Demo Window",
    // &boolle);        // Edit bools storing our window open/close state
    // ImGui::Checkbox("Another Window", &boolle);

    // ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float
    // using a slider from 0.0f to 1.0f ImGui::ColorEdit3("clear color",
    // (float*)&clear_color); // Edit 3 floats representing a color

    // if (ImGui::Button("Button"))                            // Buttons return
    // true when clicked (most widgets return true when edited/activated)
    // m_clickCounter->click();
    // ImGui::SameLine();
    ImGui::Text("Clicks = %d", m_clickCounter->GetClicks());

    if (ImGui::Button("Upgrade"))
      m_clickCounter->UpgradeClickValue();
    ImGui::SameLine();
    ImGui::Text("Click upgrade cost = %d",
                m_clickCounter->GetClickUpgradeValue());

    ImGui::Text("Mouse location: %.1f x %.1f", x, y);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / io.Framerate, io.Framerate);

    ImGui::End();
  }

  // ImGui::ShowDemoWindow(); // Show demo window! :)

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}
