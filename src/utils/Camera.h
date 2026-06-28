/**
 * Camera.h
 *
 * Implements a first-person shooter (FPS) style camera with mouse look,
 * keyboard movement, and orbit capabilities.
 *
 * KEY CONCEPTS COVERED:
 * ---------------------
 * 1. View Matrix - Transforms world coordinates to camera (eye) space
 * 2. Euler Angles - Yaw and Pitch for camera orientation
 * 3. Camera Coordinate System - Front, Right, Up basis vectors
 * 4. Gimbal Lock - Why we clamp pitch angles
 * 5. Delta Time - Frame-rate independent movement
 * 6. Linear Interpolation - Smooth camera transitions
 */

#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
    /**
     * Camera Constructor
     *
     * @param window      - GLFW window for input handling
     * @param startPosition - Initial camera position in world space
     * @param startUp     - World up vector (typically Y-up: 0,1,0)
     * @param startYaw    - Initial horizontal rotation in degrees
     * @param startPitch  - Initial vertical rotation in degrees
     * @param startFOV    - Field of view in degrees (affects perspective projection)
     */
    Camera(GLFWwindow* window,
        glm::vec3 startPosition,
        glm::vec3 startUp,
        float startYaw,
        float startPitch,
        float startFOV,
        bool invertPitch = true);

    void processInput(float deltaTime);

    /**
     * Returns the View Matrix for rendering.
     *
     * THE VIEW MATRIX:
     * Transforms vertices from world space to camera (eye) space.
     * In eye space, the camera is at origin looking down -Z axis.
     *
     * The rendering pipeline transforms vertices through:
     *   Model -> World -> View -> Projection -> NDC -> Screen
     *           (Model)   (View)   (Projection)
     */
    glm::mat4 getViewMatrix();

    void Update(float deltaTime);
    float getFOV() const { return fov; }
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getFront() const { return front; }
    bool isDetached() const { return detached; }
    void cameraGUI();

    /**
     * GROUNDED (FPS) MODE:
     * When enabled, WASD movement is projected onto the XZ plane and the
     * camera is locked to a fixed eye height - looking up/down no longer
     * makes W/S fly into the sky or floor. Mouse look is unaffected.
     * Defaults to off, so existing free-fly tests behave exactly as before.
     */
    void setGrounded(bool grounded, float eyeHeight = 1.6f);

    // Teleports the camera (also syncs the smooth-interpolation target so the
    // camera doesn't lerp back to its old position when detached).
    void setPosition(const glm::vec3& pos);

    // Sets the look angles directly and rebuilds the basis vectors.
    void setYawPitch(float newYaw, float newPitch);

private:
    GLFWwindow* m_window;

    void ProcessKeyboard(float deltaTime);
    void ProcessMouse(float xOffset, float yOffset);
    void ToggleDetach();
    void Orbit(float angleX, float angleY);

    // ==========================================================================
    // CAMERA COORDINATE SYSTEM
    // ==========================================================================
    /**
     * The camera maintains its own local coordinate system (basis vectors):
     *
     *         up (Y-local)
     *          |
     *          |
     *          +------ right (X-local)
     *         /
     *        /
     *      front (Z-local, but inverted - camera looks down -Z in OpenGL)
     *
     * - position: Camera's location in world space
     * - front:    Direction camera is facing (normalized)
     * - up:       Camera's local up direction (for roll, not used here)
     * - right:    Camera's local right direction
     * - worldUp:  Reference up vector (usually Y-axis) for calculating right vector
     *
     * These vectors form an orthonormal basis that defines the camera's orientation.
     */
    glm::vec3 position, front, up, right, worldUp;

    // Target values for smooth interpolation when camera is detached
    glm::vec3 targetPosition;
    glm::vec3 targetFront;
    bool detached = false;
	
    // ==========================================================================
    // EULER ANGLES
    // ==========================================================================
    /**
     * EULER ANGLES represent rotation as a sequence of rotations around axes.
     *
     * For an FPS camera, we use two angles:
     *
     * YAW (horizontal rotation around Y-axis):
     *   - 0° = looking along +X axis
     *   - 90° = looking along +Z axis
     *   - 180° = looking along -X axis
     *   - 270° = looking along -Z axis
     *
     *        +Z
     *         |
     *    -X --+-- +X  (yaw rotates around this plane)
     *         |
     *        -Z
     *
     * PITCH (vertical rotation around X-axis):
     *   - 0° = looking at horizon
     *   - +90° = looking straight up
     *   - -90° = looking straight down
     *
     * Note: We don't use ROLL (rotation around Z/front axis) for FPS cameras,
     * as it would tilt the horizon and cause motion sickness.
     */
    float yaw, pitch;

    /**
     * FIELD OF VIEW (FOV):
     * The angular extent of the scene visible through the camera.
     * - Larger FOV = wider view, more visible but more distortion
     * - Smaller FOV = narrower view, telephoto lens effect
     * - Typical values: 45° (narrow) to 90° (wide), 60° is common
     *
     * FOV affects the projection matrix, not the view matrix.
     */
    float fov;

    /**
     * Movement parameters:
     * - movementSpeed: Units per second for WASD movement
     * - mouseSensitivity: Degrees per pixel of mouse movement
     * - smoothFactor: Interpolation speed for smooth transitions
     */
    float movementSpeed, mouseSensitivity, smoothFactor;

    /**
     * Recalculates front, right, and up vectors from current yaw/pitch.
     * Must be called whenever yaw or pitch changes.
     */
    void UpdateCameraVectors();

    bool m_invertPitch;

    // Grounded/FPS mode state (see setGrounded). Off by default so the camera
    // remains a free-fly camera for every test that doesn't opt in.
    bool m_Grounded = false;
    float m_EyeHeight = 1.6f;
};
