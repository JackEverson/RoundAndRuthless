#include <iostream>  
#include <string>

#include "Engine.hpp"
#include "Renderer.hpp"
#include "IndexBuffer.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "VertexArray.hpp"
#include "VertexBuffer.hpp"


GardenEngine::GardenEngine(std::string name, int win_width, int win_height)
{
    std::cout << "Creating Window and GardenEngine..." << std::endl;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    int monitor_count;
    GLFWmonitor** _monitors = glfwGetMonitors(&monitor_count);
    if (!monitor_count) {
        std::cout << "GLFWmonitor found monitors to be null" << std::endl;
        throw std::runtime_error("GLFWMonitor found monitors to be null");
    }
    std::cout << "GLFWmonitors has found " << monitor_count << " monitors" << std::endl;
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

    if (!m_window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(m_window);
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        throw std::runtime_error("Failed to initialize OpenGL context through gladLoadGLLoader");
    }
    GLCall(glViewport(0, 0, win_width, win_height));

    std::cout << "Window and GardenEngine Created!" << std::endl;
}

GardenEngine::~GardenEngine()
{

}


int GardenEngine::Start(){
    std::cout << "GardenEngine Warming Up...." << std::endl;

    // some hard coded stuff
    const float vertices[] = {
 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,  // top right
 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
-0.5f,  0.5f, 0.0f, 0.0f, 1.0f// top left 
    };

    unsigned int indices[] = {
    0, 1, 3,
    1, 2, 3,
    };

    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    Renderer renderer;
    VertexArray va;
    VertexBuffer vb(vertices, 4 * 5 * sizeof(float));

    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(2);
    va.AddBuffer(vb, layout);

    IndexBuffer ib(indices, 6);

    // Shader shader ("res/shaders/basic.shader");
    Shader shader("res/shaders/basictexture.shader");
    shader.Bind();

    // Texture texture("res/textures/container.jpg");
    // Texture texture("res/textures/container2.png");
    Texture texture("res/textures/sushi.png");
    texture.Bind();
    
    // Render loop!!!!!
    std::cout << "GardenEngine finished warming up! Starting Rendering..." << std::endl;

    while (!glfwWindowShouldClose(m_window)) {
        processInput();

        renderer.Clear(red, 0.2f, 0.2f, 0.2f);
        shader.SetUniform3f("aColor", 0.0f, 0.0f, 0.0f);
        shader.SetUniform3f("aLoc", locx, locy, locz);
        shader.SetUniform1i("atexCoord", 0);

        renderer.Draw(va, ib, shader);


        GLCall(glfwSwapBuffers(m_window));
        GLCall(glfwPollEvents());


        glClear(GL_COLOR_BUFFER_BIT);
    }

    
    glfwTerminate();
    std::cout << "GardenEngine Exited Successfully!" << std::endl;
    return 0;
}

void GardenEngine::processInput() {
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_window, true);

    if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(m_window, &xpos, &ypos);

        std::cout << "left click detected at x: " << xpos << ", y: " << ypos << std::endl;
    }

    if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) {
        locy += 0.001f;
    }
    if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) {
        locy -= 0.001f;
    }
    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) {
        locx -= 0.001f;
    }
    if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) {
        locx += 0.001f;
    }
    if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        approach = true;
    }

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