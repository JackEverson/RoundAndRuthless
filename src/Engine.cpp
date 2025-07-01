#include <iostream>  
#include <string>
#include <print>


#include "Engine.hpp"
#include "bugs/GrubWrangler.hpp"



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
    //glfwSwapInterval(1);

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

}


int GardenEngine::Start(){
    std::println("GardenEngine Warming Up....");

    GrubWrangler grubs;
    /*grubs.AddBug(1);*/

    GrubWrangler grubs2;


    // Render loop!!!!!
    std::println("GardenEngine finished warming up! Starting Rendering...");

    while (!glfwWindowShouldClose(m_window)) {
        processInput();

        glClearColor(0.1, 0.4f, 0.2f, 1.0f);
        GLCall(glClear(GL_COLOR_BUFFER_BIT));


        grubs.Render();

        GLCall(glfwSwapBuffers(m_window));
        GLCall(glfwPollEvents());


        glClear(GL_COLOR_BUFFER_BIT);
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

        std::println("left click detected at x: {}, y: {}", xpos, ypos);
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
