#pragma once

#include "CGameObject.h"
#include "CWindow.h"

class KeyboardMovmentController
{
public:
	struct KeyMappings
	{
        int moveLeft        = GLFW_KEY_A;
        int moveRight       = GLFW_KEY_D;
        int moveForward     = GLFW_KEY_W;
        int moveBackward    = GLFW_KEY_S;
        int moveUp          = GLFW_KEY_E;
        int moveDown        = GLFW_KEY_Q;
        int lookLeft        = GLFW_KEY_LEFT;
        int lookRight       = GLFW_KEY_RIGHT;
        int lookUp          = GLFW_KEY_UP;
        int lookDown        = GLFW_KEY_DOWN;
        int sprint          = GLFW_KEY_LEFT_CONTROL;
	};

    void moveInPLaneXZ(GLFWwindow* pWindow, float deltatime, GameObject& gameObject);

    KeyMappings keys{};
    float m_moveSpeed{ 3.0f };
    int m_speedMultyplier{ 1 };
    float m_turnSpeed{ 1.5f };
};