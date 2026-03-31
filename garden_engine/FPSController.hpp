#pragma once

#include <GLFW/glfw3.h>

#include "Camera.hpp"
#include "Surface.hpp"

class FPSController
{
private:
    Camera &m_camera;

    float m_player_radius = 0.3f;
    float m_speed = 5.0f;
    double m_mouse_sensitivity = 0.05;
    double m_lastMouseX = 0.0;
    double m_lastMouseY = 0.0;

    bool m_left_click_before = false;

public:
    int KeyForward = GLFW_KEY_W;
    int KeyBack = GLFW_KEY_S;
    int KeyLeft = GLFW_KEY_A;
    int KeyRight = GLFW_KEY_D;
    int KeyJump = GLFW_KEY_SPACE;
    int KeyCrouch = GLFW_KEY_C;

public:
    FPSController(Camera &camera);
    ~FPSController();
    void Init(GLFWwindow &window);
    void HandleInput(GLFWwindow &window, float delta);
    void ResolveCollisions(const std::vector<Surface> &surfaces);
};

FPSController::FPSController(Camera &camera) : m_camera(camera)
{
}

FPSController::~FPSController()
{
}

inline void FPSController::Init(GLFWwindow &window)
{
    double x, y;
    glfwGetCursorPos(&window, &x, &y);
    m_lastMouseX = x;
    m_lastMouseY = y;
}

inline void FPSController::HandleInput(GLFWwindow &window, float delta)
{
    float speed = m_speed;
    if (glfwGetKey(&window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(&window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
    {
        speed *= 5;
    }
    auto flat_forward = m_camera.GetForward() * glm::vec3(1, 0, 1);
    auto flat_right = m_camera.GetRight() * glm::vec3(1, 0, 1);

    if (glfwGetKey(&window, KeyForward) == GLFW_PRESS)
        m_camera.ShiftCamera(flat_forward * speed * delta);
    if (glfwGetKey(&window, KeyBack) == GLFW_PRESS)
        m_camera.ShiftCamera(-flat_forward * speed * delta);
    if (glfwGetKey(&window, KeyLeft) == GLFW_PRESS)
        m_camera.ShiftCamera(-flat_right * speed * delta);
    if (glfwGetKey(&window, KeyRight) == GLFW_PRESS)
        m_camera.ShiftCamera(flat_right * speed * delta);
    if (glfwGetKey(&window, KeyJump) == GLFW_PRESS)
        m_camera.ShiftCamera(glm::vec3(0, 1, 0) * speed * delta);
    if (glfwGetKey(&window, KeyCrouch) == GLFW_PRESS)
        m_camera.ShiftCamera(glm::vec3(0, -1, 0) * speed * delta);

    // bool left_click = false;
    // if (!m_left_click_before &&
    //     glfwGetMouseButton(&window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    // { left_click = true; }

    // if (glfwGetMouseButton(&window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) m_left_click_before = true;
    // else m_left_click_before = false;

    double mouseX, mouseY;
    glfwGetCursorPos(&window, &mouseX, &mouseY);

    double deltaX = (mouseX - m_lastMouseX) * m_mouse_sensitivity;
    double deltaY = (mouseY - m_lastMouseY) * -m_mouse_sensitivity;

    m_camera.ShiftRotation(deltaX, deltaY);

    m_lastMouseX = mouseX;
    m_lastMouseY = mouseY;
}

inline void FPSController::ResolveCollisions(const std::vector<Surface> &surfaces)
{
    glm::vec3 campos = m_camera.GetLocation();

    for (auto surface : surfaces)
    {
        if (surface.type != SurfaceType::Wall)
            continue;

        glm::vec3 normal = glm::normalize(glm::vec3(surface.rotation[2]));
        glm::vec3 to_player = campos - surface.position;
        float dist = glm::dot(to_player, normal);

        if (dist <= -m_player_radius || dist >= m_player_radius)
            continue;

        glm::mat4 invRot = glm::inverse(surface.rotation);
        glm::vec3 localPos = glm::vec3(invRot * glm::vec4(to_player, 0.0f));
        float halfW = surface.size.x * 0.5f;
        float halfH = surface.size.y * 0.5f;

        if (localPos.x < -halfW || localPos.x > halfW)
            continue;
        if (localPos.y < -halfH || localPos.y > halfH)
            continue;

        float penetration = m_player_radius - glm::abs(dist);
        campos += glm::sign(dist) * normal * penetration;
    }

    m_camera.SetCamera(campos);
}
