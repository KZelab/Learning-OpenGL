/**
 * Camera.cpp
 *
 * Implementation of an FPS-style camera system with support for:
 * - First-person mouse look (yaw/pitch rotation)
 * - WASD keyboard movement along camera axes
 * - Orbit mode for rotating around a target point
 * - Smooth interpolation for cinematic transitions
 *
 * KEY MATH CONCEPTS:
 * ------------------
 * 1. Spherical to Cartesian coordinate conversion (for camera direction)
 * 2. Cross product for computing perpendicular vectors
 * 3. LookAt matrix construction
 * 4. Linear interpolation (lerp/mix) for smooth movement
 * 5. Delta time for frame-rate independent updates
 */

#include "Camera.h"
#include <iostream>

#include <imgui.h>
// =============================================================================
// CONSTRUCTOR
// =============================================================================
Camera::Camera(GLFWwindow* window, glm::vec3 startPosition, glm::vec3 startUp, float startYaw, float startPitch, float startFOV,
    bool invertPitch) :
    position(startPosition),
    worldUp(startUp),
    yaw(startYaw),
    pitch(startPitch),
    fov(startFOV),
    movementSpeed(5.0f),      // 5 units per second
    mouseSensitivity(0.1f),   // 0.1 degrees per pixel
    smoothFactor(0.1f),       // Interpolation rate
    m_window(window),
	m_invertPitch(invertPitch)
{
    // Calculate initial camera vectors from yaw/pitch
    UpdateCameraVectors();

    // Initialize smooth interpolation targets to current state
    targetPosition = position;
    targetFront = front;

    // Capture mouse cursor for FPS-style control
    // GLFW_CURSOR_DISABLED: hides cursor and provides unlimited mouse movement
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

// =============================================================================
// CAMERA VECTOR CALCULATION
// =============================================================================
/**
 * UpdateCameraVectors - Converts Euler angles to direction vectors
 *
 * SPHERICAL TO CARTESIAN CONVERSION:
 * We're essentially converting spherical coordinates (yaw, pitch) to a
 * Cartesian direction vector (x, y, z).
 *
 * In spherical coordinates:
 *   - yaw (θ) = azimuthal angle in XZ plane (rotation around Y)
 *   - pitch (φ) = elevation angle from XZ plane (rotation around X)
 *
 * The formulas derived from spherical coordinates:
 *   x = cos(pitch) × cos(yaw)
 *   y = sin(pitch)
 *   z = cos(pitch) × sin(yaw)
 *
 * VISUALIZATION:
 *
 *        +Y (up)
 *         |   /front (pitch > 0)
 *         |  /
 *         | / pitch angle
 *         |/_________ +X
 *        /\
 *       /  \ yaw angle
 *      /    \
 *    +Z      front projected on XZ plane
 *
 * The cos(pitch) term "squishes" the XZ components as we look up/down,
 * keeping the direction vector normalized.
 */
void Camera::UpdateCameraVectors()
{
    glm::vec3 newFront;

    // Convert degrees to radians and compute direction vector
    // Note: glm::radians() converts degrees to radians (degrees × π/180)
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    // Normalize to ensure unit length (should already be ~1, but floating point)
    front = glm::normalize(newFront);

    // ==========================================================================
    // COMPUTING RIGHT AND UP VECTORS USING CROSS PRODUCT
    // ==========================================================================
    /**
     * CROSS PRODUCT produces a vector perpendicular to two input vectors.
     * Direction follows the right-hand rule.
     *
     * RIGHT VECTOR:
     *   right = front × worldUp
     *
     *        worldUp
     *           |
     *           |
     *           +----→ right (perpendicular to both)
     *          /
     *         /
     *       front
     *
     * This gives us a horizontal vector pointing to the camera's right side.
     *
     * UP VECTOR:
     *   up = right × front
     *
     * This gives us the camera's local up direction, which may differ from
     * worldUp when looking up or down (the camera tilts).
     *
     * Together, front/right/up form an ORTHONORMAL BASIS:
     * - All vectors have length 1 (normal)
     * - All vectors are perpendicular to each other (ortho)
     * - This basis defines the camera's local coordinate system
     */
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}



// =============================================================================
// INPUT PROCESSING
// =============================================================================
/**
 * processInput - Main input handler called each frame
 *
 * Handles:
 * 1. Escape key toggle for detaching camera (enables GUI interaction)
 * 2. Keyboard movement (WASD)
 * 3. Mouse look (yaw/pitch rotation)
 *
 * INPUT DEBOUNCING:
 * The static 'escapePressed' variable prevents key repeat.
 * We only register a press on the transition from released to pressed,
 * not while the key is held down.
 */
void Camera::processInput(float deltaTime)
{
    // Debounce escape key - only trigger on initial press
    static bool escapePressed = false;

    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS && !escapePressed)
    {
        this->ToggleDetach();
        escapePressed = true;
    }
    else if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_RELEASE)
    {
        escapePressed = false;
    }

    // Only process camera movement when not detached (GUI mode)
    if (!detached)
    {
        this->ProcessKeyboard(deltaTime);
    }

    // ==========================================================================
    // MOUSE DELTA CALCULATION
    // ==========================================================================
    /**
     * We track mouse movement as DELTAS (change from last frame), not absolute position.
     *
     * RELATIVE MOUSE INPUT:
     *   offset = currentPosition - lastPosition
     *
     * Note: Y is inverted because screen coordinates have Y increasing downward,
     * but we want "mouse up" to pitch the camera up (positive pitch).
     *
     * static variables persist between function calls, maintaining lastX/lastY
     * across frames.
     */
    static double lastX = 400.0, lastY = 300.0;  // Initialize to screen center
    double xpos, ypos;
    glfwGetCursorPos(m_window, &xpos, &ypos);

    float xOffset = xpos - lastX;   // Positive = mouse moved right
    float yOffset = lastY - ypos;   // Positive = mouse moved up (inverted!)

    lastX = xpos;
    lastY = ypos;

    if (!detached)
    {
        this->ProcessMouse(xOffset, yOffset);
    }
}

// =============================================================================
// VIEW MATRIX GENERATION
// =============================================================================
/**
 * getViewMatrix - Constructs the View matrix using glm::lookAt
 *
 * THE LOOKAT MATRIX:
 * Transforms world coordinates into camera (eye/view) space where:
 * - Camera is at the origin (0, 0, 0)
 * - Camera looks down the -Z axis
 * - +Y is up, +X is right
 *
 * glm::lookAt(eye, target, up) constructs this matrix from:
 * - eye:    Camera position in world space
 * - target: Point the camera is looking at (position + front)
 * - up:     Which direction is "up" for the camera
 *
 * MATRIX CONSTRUCTION (conceptually):
 * The lookAt matrix is composed of:
 * 1. Translation to move world so camera is at origin
 * 2. Rotation to align camera axes with world axes
 *
 *     | right.x    right.y    right.z    -dot(right, eye) |
 * V = | up.x       up.y       up.z       -dot(up, eye)    |
 *     | -front.x   -front.y   -front.z   dot(front, eye)  |
 *     | 0          0          0          1                |
 *
 * The negative front is used because OpenGL cameras look down -Z.
 */
glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(position, position + front, up);
}

// =============================================================================
// SMOOTH CAMERA UPDATE
// =============================================================================
/**
 * Update - Handles smooth interpolation when camera is detached
 *
 * LINEAR INTERPOLATION (LERP):
 * Smoothly blends between current and target values over time.
 *
 * Formula: result = current + (target - current) × t
 * Or equivalently: result = current × (1-t) + target × t
 *
 * glm::mix(a, b, t) implements this: returns a when t=0, b when t=1
 *
 * EXPONENTIAL SMOOTHING:
 * Using (smoothFactor × deltaTime) as t creates exponential decay toward target:
 * - Each frame moves a percentage of the remaining distance
 * - Creates smooth, natural-feeling deceleration
 * - Larger smoothFactor = faster response
 * - Movement is frame-rate independent due to deltaTime
 *
 * This is commonly used for:
 * - Camera following (games)
 * - Smooth UI transitions
 * - Audio parameter smoothing
 */
void Camera::Update(float deltaTime)
{
    if (detached)
    {
        // Smoothly interpolate position and orientation toward targets
        position = glm::mix(position, targetPosition, smoothFactor * deltaTime);
        front = glm::mix(front, targetFront, smoothFactor * deltaTime);
    }
}

// =============================================================================
// KEYBOARD MOVEMENT
// =============================================================================
/**
 * ProcessKeyboard - WASD movement along camera axes
 *
 * FPS-STYLE MOVEMENT:
 * Movement is relative to camera orientation, not world axes:
 * - W/S: Move along front vector (forward/backward)
 * - A/D: Move along right vector (strafe left/right)
 *
 * DELTA TIME AND VELOCITY:
 * velocity = speed × deltaTime
 *
 * deltaTime is the time elapsed since the last frame (in seconds).
 * This makes movement FRAME-RATE INDEPENDENT:
 *
 * At 60 FPS: deltaTime ≈ 0.0167s, movement = 5.0 × 0.0167 = 0.083 units/frame
 * At 30 FPS: deltaTime ≈ 0.0333s, movement = 5.0 × 0.0333 = 0.167 units/frame
 *
 * In both cases, after 1 second: 60 × 0.083 = 30 × 0.167 = 5 units
 * The character moves the same distance regardless of frame rate!
 *
 * Without deltaTime, higher FPS would mean faster movement.
 */
void Camera::ProcessKeyboard(float deltaTime)
{
    if (detached) return;

    // Calculate frame-rate independent velocity
    float velocity = movementSpeed * deltaTime;

    // Movement along camera axes (not world axes)
    if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) position += front * velocity;  // Forward
    if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) position -= front * velocity;  // Backward
    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) position -= right * velocity;  // Strafe left
    if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) position += right * velocity;  // Strafe right
}

// =============================================================================
// MOUSE LOOK
// =============================================================================
/**
 * ProcessMouse - Converts mouse movement to camera rotation
 *
 * SENSITIVITY SCALING:
 * Raw pixel offsets are too large for direct use as angles.
 * mouseSensitivity (e.g., 0.1) converts pixels to degrees:
 *   10 pixels of movement = 1 degree of rotation
 *
 * AXIS MAPPING:
 * - Horizontal mouse movement (xOffset) → Yaw (rotate left/right)
 * - Vertical mouse movement (yOffset) → Pitch (look up/down)
 *
 * Note: yOffset is SUBTRACTED from pitch because screen Y is inverted
 * (already handled in processInput, but the sign here reinforces the intent).
 *
 * GIMBAL LOCK AND PITCH CLAMPING:
 * We clamp pitch to ±89° (not ±90°) to prevent issues:
 *
 * At exactly ±90° pitch (looking straight up/down):
 * - The front vector becomes parallel to worldUp
 * - cross(front, worldUp) = 0 (undefined/zero right vector)
 * - The camera orientation becomes undefined
 *
 * This is related to GIMBAL LOCK - a loss of one degree of freedom
 * when two rotation axes align. Clamping to 89° keeps us just shy
 * of this singularity while still allowing near-vertical views.
 *
 * Alternative solutions include:
 * - Quaternions (no gimbal lock, but more complex)
 * - Different rotation orders
 * - Accepting the limitation (most FPS games do this)
 */
void Camera::ProcessMouse(float xOffset, float yOffset)
{
    if (detached) return;

    // Scale mouse movement to rotation angles
    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    // Apply rotation
    yaw += xOffset;     // Horizontal mouse → horizontal rotation
    pitch += m_invertPitch ? yOffset : -yOffset;

    // Clamp pitch to prevent gimbal lock and camera flipping
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    // Recalculate direction vectors with new angles
    UpdateCameraVectors();
}

// =============================================================================
// DETACH TOGGLE
// =============================================================================
/**
 * ToggleDetach - Switches between FPS mode and GUI mode
 *
 * CURSOR MODES:
 * - GLFW_CURSOR_DISABLED: Hides cursor, enables unlimited mouse movement
 *   Used for FPS-style camera control where mouse rotates view
 *
 * - GLFW_CURSOR_NORMAL: Shows cursor, normal mouse behavior
 *   Used when user needs to interact with GUI elements
 *
 * When detaching, we store the current position/orientation as targets
 * so the camera smoothly returns to this state when re-attached.
 */
void Camera::ToggleDetach()
{
    detached = !detached;

    std::cout << (detached ? "Camera is detached for GUI use. \n" : "Camera reattached. \n");

    if (detached)
    {
        // Enable cursor for GUI interaction
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        // Store current state as target for smooth return
        targetPosition = position;
        targetFront = front;
    }
    else
    {
        // Hide cursor for FPS-style movement
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

// =============================================================================
// ORBIT CAMERA
// =============================================================================
/**
 * Orbit - Rotates camera around a target point (arcball-style)
 *
 * ORBIT CAMERA CONCEPT:
 * Instead of rotating in place (FPS), the camera moves on a sphere
 * centered at targetPosition, always facing the target.
 *
 * Think of it like a satellite orbiting Earth:
 * - The satellite (camera) maintains a fixed distance (radius)
 * - It can move around the sphere's surface
 * - It always points toward the center (target)
 *
 * SPHERICAL COORDINATES FOR ORBIT:
 * The camera position is computed from:
 * - radius: Distance from target (constant during orbit)
 * - yaw: Angle around the vertical axis (longitude)
 * - pitch: Angle above/below the horizontal plane (latitude)
 *
 * POSITION CALCULATION:
 *   x = target.x + radius × cos(yaw) × cos(pitch)
 *   y = target.y + radius × sin(pitch)
 *   z = target.z + radius × sin(yaw) × cos(pitch)
 *
 * This is the same spherical-to-Cartesian conversion used in UpdateCameraVectors,
 * but offset from targetPosition instead of origin.
 *
 * VISUALIZATION (top-down view, Y up):
 *
 *            +Z
 *             |
 *        .----|----.
 *       /     |     \
 *      |   (target)  |    radius
 *      |      +------|------→ +X
 *       \           /
 *        '---------'
 *           camera moves along this circle as yaw changes
 *
 * USE CASES:
 * - 3D model viewers
 * - Scene editors
 * - Third-person cameras in games
 * - CAD software
 */
void Camera::Orbit(float angleX, float angleY)
{
    // Calculate current distance from target (sphere radius)
    float radius = glm::length(position - targetPosition);

    // Update angles based on input
    yaw += angleX * mouseSensitivity;
    pitch += angleY * mouseSensitivity;

    // Clamp pitch to prevent flipping over poles (same gimbal lock issue)
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    // Convert spherical coordinates to Cartesian, offset from target
    position.x = targetPosition.x + radius * cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    position.y = targetPosition.y + radius * sin(glm::radians(pitch));
    position.z = targetPosition.z + radius * sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    // Update direction vectors to face the new position
    UpdateCameraVectors();
}
void Camera::cameraGUI()
{
    ImGui::Text("Camera Position: (%.1f, %.1f, %.1f)", position.x, position.y, position.z);
    ImGui::Text("Camera Detached: %s", isDetached() ? "yes (ESC to reattach)" : "no");
}