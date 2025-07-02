#include <iostream>  
#include <string>
#include <print>
#include <chrono>
#include <thread>

#include "Engine.hpp"

GardenEngine::GardenEngine(std::string name, int win_width, int win_height)
{
    std::println("Creating Window and GardenEngine...");
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    int monitor_count;
    GLFWmonitor** _monitors = glfwGetMonitors(&monitor_count);
    if (!monitor_count) {
        std::println("GLFWmonitor found monitors to be null");
        throw std::runtime_error("GLFWMonitor found monitors to be null");
    }
    std::println("GLFWmonitors has found {} monitors", monitor_count);
    for (int i = 0; i < monitor_count; i++) {
        const GLFWvidmode* mode = glfwGetVideoMode(_monitors[i]);
        std::cout << "Monitor " << i << ", " << _monitors[i] << ", has resolution " << mode->width << "x" << mode->height << std::endl;
    }

    //// set fullscreen
    // GLFWmonitor* pMonitor = glfwGetPrimaryMonitor();
    // std::cout << "Using monitor: " << pMonitor << std::endl;
    // window = glfwCreateWindow(win_width, win_height, name.c_str(), pMonitor, NULL);

    // set windowed
    m_window = glfwCreateWindow(win_width, win_height, name.c_str(), NULL, NULL);
    glfwSwapInterval(0);

    if (!m_window)
    {
        std::println("Failed to create GLFW window");
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(m_window);
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::println("Failed to initialize OpenGL context");
        throw std::runtime_error("Failed to initialize OpenGL context through gladLoadGLLoader");
    }
    GLCall(glViewport(0, 0, win_width, win_height));


    std::println("Window and GardenEngine Created!");
}

GardenEngine::~GardenEngine()
{
    delete m_grubs;
    m_grubs = nullptr;
}


int GardenEngine::Start(float fps){
    std::println("GardenEngine Warming Up....");

    m_grubs = new GrubWrangler();

    // Render loop!!!!!
    std::println("GardenEngine finished warming up! Starting Rendering...");
    glClearColor(0.1, 0.6f, 0.2f, 1.0f);

    int frame_time_limit_ms = (int)((1 / fps) * 1000);

    while (!glfwWindowShouldClose(m_window)) {
        
        // start
        auto start_time = std::chrono::high_resolution_clock::now();

        processInput();
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        m_grubs->Render();

        GLCall(glfwSwapBuffers(m_window));
        GLCall(glfwPollEvents());


        //finish and fps limit
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = end_time - start_time;
        int ms = (int)std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        //std::println("Time elapsed to generate frame: {}", ms);
        int waittime = frame_time_limit_ms - ms;
        if (waittime < 0) std::println("WARNING, cannot keep up with set FPS. FPS currently at {}", 1000 / ms);
        else std::this_thread::sleep_for(std::chrono::milliseconds(waittime));
    }

    
    glfwTerminate();
    std::println("GardenEngine Exited Successfully!");
    return 0;
}

void GardenEngine::processInput() {
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_window, true);

    if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(m_window, &xpos, &ypos);
        
        int w, h;
        glfwGetWindowSize(m_window, &w, &h);

        double nxpos = xpos / w;
        double nypos = 1 - (ypos / h);

        //std::println("left click detected at x: {}-{}, y: {}-{}", nxpos, xpos, nypos, ypos);

        m_grubs->AddBug(1, nxpos - 0.5f, nypos - 0.5f);
    }

    //if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) {
    //    locy += 0.001f;
    //}
    //if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) {
    //    locy -= 0.001f;
    //}
    //if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) {
    //    locx -= 0.001f;
    //}
    //if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) {
    //    locx += 0.001f;
    //}
    //if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    //    approach = true;
    //}

    //if (approach) {
    //    //locz -= 0.0001f;
    //    if (red < 1.0f) red += 0.00001;
    //    if (other > 0.0f) other -= 0.00001;
    //}


}





void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
