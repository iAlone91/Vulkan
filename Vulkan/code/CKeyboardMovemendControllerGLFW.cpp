#include "CKeyboardMovemendControllerGLFW.h"

#include <limits>

void KeyboardMovmentController::moveInPLaneXZ(GLFWwindow* pWindow, float deltatime ,GameObject& gameObject)
{
	if (glfwGetKey(pWindow, keys.sprint) == GLFW_PRESS) { m_speedMultyplier = 5; } else if (glfwGetKey(pWindow, keys.sprint) == GLFW_RELEASE) { m_speedMultyplier = 1; }

	glm::vec3 rotate(0);
	if (glfwGetKey(pWindow, keys.lookRight) == GLFW_PRESS) { rotate.y += 1.0f; }
	if (glfwGetKey(pWindow, keys.lookLeft) == GLFW_PRESS) { rotate.y -= 1.0f; }
	if (glfwGetKey(pWindow, keys.lookUp) == GLFW_PRESS) { rotate.x += 1.0f; }
	if (glfwGetKey(pWindow, keys.lookDown) == GLFW_PRESS) { rotate.x -= 1.0f; }

	if( glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon() )
		gameObject.transform.rotation += m_turnSpeed * deltatime * glm::normalize(rotate);

	gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
	gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

	float yaw = gameObject.transform.rotation.y;
	const glm::vec3 forwardDir(sin(yaw), 0.0f, cos(yaw));
	const glm::vec3 rightDir(forwardDir.z, 0.0f, -forwardDir.x);
	const glm::vec3 upDir(0.0f, -1.0f, 0.0f);

	glm::vec3 moveDir(0);
	if (glfwGetKey(pWindow, keys.moveForward) == GLFW_PRESS) { moveDir += forwardDir; }
	if (glfwGetKey(pWindow, keys.moveBackward) == GLFW_PRESS) { moveDir -= forwardDir; }
	if (glfwGetKey(pWindow, keys.moveRight) == GLFW_PRESS) { moveDir += rightDir; }
	if (glfwGetKey(pWindow, keys.moveLeft) == GLFW_PRESS) { moveDir -= rightDir; }
	if (glfwGetKey(pWindow, keys.moveUp) == GLFW_PRESS) { moveDir += upDir; }
	if (glfwGetKey(pWindow, keys.moveDown) == GLFW_PRESS) { moveDir -= upDir; }

	if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
		gameObject.transform.translation += (m_moveSpeed * m_speedMultyplier) * deltatime * glm::normalize(moveDir);
}
