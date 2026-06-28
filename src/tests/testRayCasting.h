#pragma once
#include "Tests.h"
#include "../VertexBuffer.h"
#include "../VertexBufferLayout.h"
#include "../IndexBuffer.h"
#include "../VertexArray.h"
#include "../Shader.h"
#include "gl/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include <string>
#include <memory>

namespace test {
    /**
     * Ray Casting Test Class
     *
     * Demonstrates 3D ray casting for mouse-based object selection.
     * This test creates multiple objects (spheres and cubes) in 3D space and uses
     * ray casting to detect which object the mouse cursor is hovering over.
     *
     * Key Features:
     * - First-person camera controls (WASD movement)
     * - Mouse-based ray casting for object selection
     * - Analytical ray-sphere intersection testing (quadratic equation method)
     * - Ray-AABB intersection testing for cubes (slab method)
     * - Visual feedback (selected objects turn green, unselected are red)
     * - ImGui debug interface showing selection info
     *
     * Algorithms Demonstrated:
     * 1. Screen-to-World Ray Calculation (mouse picking)
     * 2. Ray-Sphere Intersection (analytical geometry)
     * 3. Ray-AABB Intersection (slab method for axis-aligned boxes)
     */
    class testRayCasting : public Tests {
    public:
        testRayCasting(GLFWwindow* window);
        void Update(float deltaTime) override;
        void Render() override;
        void RenderGUI() override;

    private:
        GLFWwindow* m_window;

        // Camera properties
        glm::vec3 cameraPosition;
        glm::vec3 cameraFront;
        glm::vec3 cameraUp;
        float cameraSpeed;

        // Transformation matrices
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;

        /**
         * Enum defining the types of objects that can be ray-casted.
         */
        enum class ObjectType { Sphere, Cube };

        /**
         * Structure representing a ray-castable object in the scene.
         *
         * The 'size' field has different meanings based on object type:
         * - For Sphere: size = radius
         * - For Cube: size = half-extent (distance from center to face)
         */
        struct Object {
            glm::vec3 position;   // World-space position of object center
            float size;           // Size parameter (radius or half-extent)
            std::string name;     // Display name for GUI
            ObjectType type;      // Object geometry type
        };

        // ... inside private members ...
        std::unique_ptr<VertexArray> m_CubeVAO;
        std::unique_ptr<VertexBuffer> m_CubeVBO;
        std::unique_ptr<IndexBuffer> m_CubeIBO;

        std::vector<Object> objects;
        int selectedObjectIndex; // Index of the currently selected object (-1 if none)

        // OpenGL rendering resources
        std::unique_ptr<VertexArray> m_VAO;
        std::unique_ptr<VertexBuffer> m_VBO;
        std::unique_ptr<IndexBuffer> m_IBO;
        std::unique_ptr<Shader> m_Shader;

        /**
         * Generates procedural sphere geometry using parametric equations.
         * Creates vertices with positions and normals, and triangle indices.
         */
        void GenerateSphereData(std::vector<float>& vertices, std::vector<unsigned int>& indices,
            float radius, unsigned int longitudeDiv, unsigned int latitudeDiv);

        /**
         * Processes keyboard input for camera movement (WASD controls).
         */
        void ProcessInput();

        /**
         * Initializes vertex buffers, index buffers, and vertex arrays for rendering.
         */
        void SetupBuffers();

        /**
         * Converts 2D screen coordinates to a 3D ray direction in world space.
         * This is the core of the ray casting implementation.
         */
        glm::vec3 CalculateRayDirection(float mouseX, float mouseY);

        /**
         * Tests if a ray intersects with a sphere using analytical geometry.
         * Returns true if intersection exists in front of the ray origin.
         */
        bool RayIntersectsSphere(const glm::vec3& rayOrigin, const glm::vec3& rayDirection,
            const glm::vec3& sphereCenter, float sphereRadius);


        /**
         * Generates cube geometry with positions and normals.
         * Creates 8 vertices and 12 triangles (2 per face).
         */
        void GenerateCubeData(std::vector<float>& vertices, std::vector<unsigned int>& indices, float size);

        /**
         * Sets up OpenGL buffers (VAO, VBO, IBO) for cube rendering.
         */
        void SetupCubeBuffers();

        /**
         * Ray-AABB intersection test using the slab method.
         * Tests if a ray intersects an axis-aligned bounding box (cube).
         * Returns true if intersection exists in front of the ray.
         */
        bool RayIntersectsCube(const glm::vec3& rayOrigin, const glm::vec3& rayDirection,
            const glm::vec3& boxCenter, float boxSize);
    };
}