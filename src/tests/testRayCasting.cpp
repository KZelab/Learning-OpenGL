#include "testRayCasting.h"
#include "../Renderer.h"
#include "../vendor/imgui/imgui.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>


namespace test {
    /**
     * Constructor - Initializes the ray casting test scene.
     *
     * Sets up:
     * - Camera position and orientation
     * - Test objects (spheres and cubes) in the scene
     * - Vertex buffers for rendering geometry
     * - Shader program for rendering
     */
    testRayCasting::testRayCasting(GLFWwindow* window)
        : m_window(window), selectedObjectIndex(-1) {

        // Initialize first-person camera
        cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);  // Camera starts 3 units back
        cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);    // Looking down negative Z-axis
        cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);        // Y-axis is up
        cameraSpeed = 0.5f;

        // Create test objects in the scene
        // Format: {position, size, name, type}
        // For spheres: size = radius
        // For cubes: size = half-extent (distance from center to face)
        objects.push_back({ glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, "Sphere 1", ObjectType::Sphere });
        objects.push_back({ glm::vec3(2.0f, 0.0f, -3.0f), 1.5f, "Sphere 2", ObjectType::Sphere });
        objects.push_back({ glm::vec3(-2.0f, 1.0f, -2.0f), 1.0f, "Cube 1", ObjectType::Cube });

        // Setup vertex buffers for rendering
        SetupBuffers();     // Sphere geometry
        SetupCubeBuffers(); // Cube geometry

        // Load shader program for rendering objects
        m_Shader = std::make_unique<Shader>("res/Shaders/BasicMVP.shader");
    }

    /**
     * Sets up OpenGL buffers for sphere rendering.
     *
     * Creates:
     * - Vertex Array Object (VAO) to store vertex attribute configuration
     * - Vertex Buffer Object (VBO) containing vertex positions and normals
     * - Index Buffer Object (IBO) defining triangle connectivity
     *
     * Enables depth testing for proper 3D rendering.
     */
    void testRayCasting::SetupBuffers() {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        // Generate unit sphere with radius=1.0, 20 longitude divisions, 20 latitude divisions
        GenerateSphereData(vertices, indices, 1.0f, 20, 20);

        // Create and bind Vertex Array Object
        m_VAO = std::make_unique<VertexArray>();

        // Upload vertex data to GPU
        m_VBO = std::make_unique<VertexBuffer>(vertices.data(), vertices.size() * sizeof(float));

        // Define vertex attribute layout: Position (3 floats) + Normal (3 floats)
        VertexBufferLayout layout;
        layout.Push<float>(3); // Position (x, y, z)
        layout.Push<float>(3); // Normal (nx, ny, nz)
        m_VAO->AddBuffer(*m_VBO, layout);

        // Upload index data to GPU
        m_IBO = std::make_unique<IndexBuffer>(indices.data(), indices.size());

        // Enable depth testing for proper 3D occlusion
        glEnable(GL_DEPTH_TEST);
    }
    /**
     * Generates sphere geometry using parametric equations.
     *
     * This function creates a UV sphere using spherical coordinates:
     * - Theta (θ): Latitude angle from 0 to π (top to bottom)
     * - Phi (φ): Longitude angle from 0 to 2π (around the equator)
     *
     * Parametric sphere equations:
     * x = cos(φ) * sin(θ)
     * y = cos(θ)
     * z = sin(φ) * sin(θ)
     */
    void testRayCasting::GenerateSphereData(std::vector<float>& vertices, std::vector<unsigned int>& indices, float radius, unsigned int longitudeDiv, unsigned int latitudeDiv) {
        // Generate vertices by iterating over latitude (vertical) divisions
        for (unsigned int lat = 0; lat <= latitudeDiv; ++lat) {
            // Calculate latitude angle (0 at north pole, π at south pole)
            float theta = lat * glm::pi<float>() / latitudeDiv;
            float sinTheta = sin(theta);
            float cosTheta = cos(theta);

            // Generate vertices along longitude (horizontal) divisions
            for (unsigned int lon = 0; lon <= longitudeDiv; ++lon) {
                // Calculate longitude angle (0 to 2π around the sphere)
                float phi = lon * 2.0f * glm::pi<float>() / longitudeDiv;
                float sinPhi = sin(phi);
                float cosPhi = cos(phi);

                // Convert spherical coordinates to Cartesian coordinates
                float x = cosPhi * sinTheta;
                float y = cosTheta;
                float z = sinPhi * sinTheta;

                // Store vertex position (scaled by radius)
                vertices.push_back(radius * x);
                vertices.push_back(radius * y);
                vertices.push_back(radius * z);

                // Store vertex normal (unit sphere normal equals position for a sphere centered at origin)
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);
            }
        }

        // Generate triangle indices by connecting adjacent vertices
        // Each quad is split into two triangles
        for (unsigned int lat = 0; lat < latitudeDiv; ++lat) {
            for (unsigned int lon = 0; lon < longitudeDiv; ++lon) {
                // Calculate vertex indices for the current quad
                unsigned int first = (lat * (longitudeDiv + 1)) + lon;
                unsigned int second = first + longitudeDiv + 1;

                // First triangle of the quad (counter-clockwise winding)
                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first + 1);

                // Second triangle of the quad (counter-clockwise winding)
                indices.push_back(second);
                indices.push_back(second + 1);
                indices.push_back(first + 1);
            }
        }
    }

    /**
     * Update loop - handles input and performs ray casting for object selection.
     *
     * Ray Casting Algorithm:
     * 1. Get the current mouse position on screen
     * 2. Convert screen coordinates to a 3D ray in world space
     * 3. Test the ray against each sphere object for intersection
     * 4. Select the first intersected object
     */
    void testRayCasting::Update(float deltaTime) {
        ProcessInput();

        // Get current mouse cursor position in screen coordinates
        double mouseX, mouseY;
        glfwGetCursorPos(m_window, &mouseX, &mouseY);

        // Convert mouse position to a ray in world space
        glm::vec3 rayDirection = CalculateRayDirection((float)mouseX, (float)mouseY);
        glm::vec3 rayOrigin = cameraPosition;

        // Perform ray casting: test ray against all objects in the scene
        selectedObjectIndex = -1;
        for (size_t i = 0; i < objects.size(); ++i) {
            bool hit = false;
            if (objects[i].type == ObjectType::Sphere) {
                hit = RayIntersectsSphere(rayOrigin, rayDirection, objects[i].position, objects[i].size);
            }
            else {
                hit = RayIntersectsCube(rayOrigin, rayDirection, objects[i].position, objects[i].size);
            }

            if (hit) {
                selectedObjectIndex = static_cast<int>(i);
                break;
            }
        }
    }

    /**
     * Render loop - Draws all objects in the scene.
     *
     * Rendering Pipeline:
     * 1. Clear framebuffer
     * 2. Calculate view and projection matrices
     * 3. For each object:
     *    a. Calculate model matrix (translation + scale)
     *    b. Set color based on selection state
     *    c. Draw appropriate geometry (sphere or cube)
     *
     * Selected objects are rendered in green, unselected in red.
     */
    void testRayCasting::Render() {
        Renderer renderer;
        renderer.Clear();

        m_Shader->Bind();

        // Calculate view matrix using camera position and orientation
        // lookAt(eye, center, up) creates a view matrix for a camera looking from 'eye' towards 'center'
        viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);

        // Calculate perspective projection matrix
        // perspective(fov, aspect, near, far)
        projectionMatrix = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        // Upload matrices to shader
        m_Shader->setUniformMat4f("view", viewMatrix);
        m_Shader->setUniformMat4f("projection", projectionMatrix);

        // Render each object in the scene
        for (size_t i = 0; i < objects.size(); ++i) {
            // Model matrix: Translate to object position
            glm::mat4 model = glm::translate(glm::mat4(1.0f), objects[i].position);

            // Scale based on object size (radius for sphere, half-extent for cube)
            model = glm::scale(model, glm::vec3(objects[i].size));
            m_Shader->setUniformMat4f("model", model);

            // Visual feedback: Green if selected, red otherwise
            glm::vec3 color = (selectedObjectIndex == static_cast<int>(i))
                ? glm::vec3(0.0f, 1.0f, 0.0f)  // Green
                : glm::vec3(1.0f, 0.0f, 0.0f); // Red
            m_Shader->setUniform4f("u_Colour", color.r, color.g, color.b, 1.0f);

            // Draw the appropriate geometry based on object type
            if (objects[i].type == ObjectType::Sphere) {
                renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
            }
            else {
                renderer.Draw(*m_CubeVAO, *m_CubeIBO, *m_Shader);
            }
        }
    }

    /**
     * Renders ImGui debug interface.
     *
     * Displays:
     * - Camera position and movement speed control
     * - Selected object information (name, position, size, type)
     * - Selection state
     */
    void testRayCasting::RenderGUI() {
        ImGui::Text("Camera Position: (%.1f, %.1f, %.1f)", cameraPosition.x, cameraPosition.y, cameraPosition.z);
        ImGui::SliderFloat("Camera Speed", &cameraSpeed, 0.1f, 10.0f);

        // Display information about the currently selected object
        if (selectedObjectIndex != -1) {
            const Object& selectedObject = objects[selectedObjectIndex];
            ImGui::Text("Selected Object: %s", selectedObject.name.c_str());
            ImGui::Text("Position: (%.1f, %.1f, %.1f)",
                selectedObject.position.x, selectedObject.position.y, selectedObject.position.z);

            // BUG FIX: Changed from selectedObject.radius to selectedObject.size
            // The Object struct uses 'size' for both spheres (radius) and cubes (half-extent)
            if (selectedObject.type == ObjectType::Sphere) {
                ImGui::Text("Radius: %.1f", selectedObject.size);
            }
            else {
                ImGui::Text("Half-Extent: %.1f", selectedObject.size);
            }

            ImGui::Text("Type: %s",
                selectedObject.type == ObjectType::Sphere ? "Sphere" : "Cube");
        }
        else {
            ImGui::Text("No object selected.");
        }
    }
    /**
     * Processes keyboard input for first-person camera movement.
     *
     * Controls:
     * - W: Move forward (along camera front direction)
     * - S: Move backward (opposite camera front direction)
     * - A: Strafe left (perpendicular to camera front)
     * - D: Strafe right (perpendicular to camera front)
     *
     * Movement is scaled by cameraSpeed and deltaTime for frame-rate independence.
     */
    void testRayCasting::ProcessInput() {
        // Forward/Backward movement
        if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPosition += cameraSpeed * cameraFront;
        if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPosition -= cameraSpeed * cameraFront;

        // Strafe left/right using camera right vector
        // Right vector = cross(front, up) gives perpendicular direction
        if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }

    /**
     * Converts 2D screen coordinates to a 3D ray direction in world space.
     *
     * Ray Calculation Process:
     * Screen Space -> NDC (Normalized Device Coordinates) -> Clip Space -> Eye Space -> World Space
     *
     * 1. Screen Space: Mouse position in pixels (0,0) at top-left
     * 2. NDC: Normalized coordinates in range [-1, 1] for both x and y
     * 3. Clip Space: Homogeneous coordinates before perspective divide
     * 4. Eye Space: Camera-relative coordinates
     * 5. World Space: Global 3D coordinates
     *
     * The resulting ray originates from the camera position and points through
     * the pixel location on the virtual screen/viewport.
     */
    glm::vec3 testRayCasting::CalculateRayDirection(float mouseX, float mouseY) {
        // Get framebuffer dimensions
        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);

        // Step 1: Convert screen coordinates to Normalized Device Coordinates (NDC)
        // NDC range: x and y in [-1, 1], with (0,0) at screen center
        float x = (2.0f * mouseX) / width - 1.0f;
        float y = 1.0f - (2.0f * mouseY) / height; // Invert Y-axis (screen Y is top-to-bottom, NDC Y is bottom-to-top)

        // Step 2: Create ray in clip space
        // z = -1.0 points into the screen (near plane in OpenGL's right-handed coordinate system)
        // w = 1.0 for proper homogeneous coordinates
        glm::vec4 rayClip(x, y, -1.0f, 1.0f);

        // Step 3: Transform from clip space to eye/camera space
        // Inverse projection matrix undoes the perspective projection
        glm::vec4 rayEye = glm::inverse(projectionMatrix) * rayClip;

        // We only need the direction, so set z = -1 (forward) and w = 0 (direction vector, not a point)
        rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

        // Step 4: Transform from eye space to world space
        // Inverse view matrix converts from camera-relative to world coordinates
        glm::vec3 rayWorld = glm::vec3(glm::inverse(viewMatrix) * rayEye);

        // Step 5: Normalize to get unit direction vector
        return glm::normalize(rayWorld);
    }

    /**
     * Ray-Sphere Intersection Test using analytical solution.
     *
     * Mathematical Background:
     * A ray is defined as: P(t) = rayOrigin + t * rayDirection, where t >= 0
     * A sphere is defined as: ||P - sphereCenter||² = sphereRadius²
     *
     * Substituting the ray equation into the sphere equation gives a quadratic:
     * ||rayOrigin + t * rayDirection - sphereCenter||² = sphereRadius²
     *
     * Expanding and rearranging:
     * a*t² + b*t + c = 0, where:
     * - a = dot(rayDirection, rayDirection)  [typically = 1 if rayDirection is normalized]
     * - b = 2 * dot(rayOrigin - sphereCenter, rayDirection)
     * - c = dot(rayOrigin - sphereCenter, rayOrigin - sphereCenter) - sphereRadius²
     *
     * The discriminant (b² - 4ac) determines intersection:
     * - discriminant < 0: No intersection (ray misses sphere)
     * - discriminant = 0: One intersection (ray is tangent to sphere)
     * - discriminant > 0: Two intersections (ray enters and exits sphere)
     *
     * Additionally, we check that t >= 0 to ensure intersections are in front of the ray origin.
     */
    bool testRayCasting::RayIntersectsSphere(const glm::vec3& rayOrigin, const glm::vec3& rayDirection,
        const glm::vec3& sphereCenter, float sphereRadius) {
        // Vector from sphere center to ray origin
        glm::vec3 oc = rayOrigin - sphereCenter;

        // Quadratic equation coefficients
        // Note: rayDirection should be normalized, so a ≈ 1
        float a = glm::dot(rayDirection, rayDirection);
        float b = 2.0f * glm::dot(oc, rayDirection);
        float c = glm::dot(oc, oc) - sphereRadius * sphereRadius;

        // Calculate discriminant to determine if intersection exists
        float discriminant = b * b - 4.0f * a * c;

        // No intersection if discriminant is negative
        if (discriminant < 0.0f) {
            return false;
        }

        // Calculate the nearest intersection point parameter t
        // Using the quadratic formula: t = (-b ± sqrt(discriminant)) / (2a)
        // We want the smaller positive t (nearest intersection in front of ray)
        float sqrtDiscriminant = sqrt(discriminant);
        float t1 = (-b - sqrtDiscriminant) / (2.0f * a); // Near intersection
        float t2 = (-b + sqrtDiscriminant) / (2.0f * a); // Far intersection

        // Check if at least one intersection is in front of the ray (t >= 0)
        return (t1 >= 0.0f || t2 >= 0.0f);
    }

    /**
     * Generates cube geometry data.
     *
     * Creates a cube centered at the origin with each face at distance 'size' from the center.
     *
     * Cube Structure:
     * - 8 vertices (corners of the cube)
     * - 12 triangles (2 per face, 6 faces total)
     * - Each vertex has position (x,y,z) and normal (nx,ny,nz)
     *
     * Vertex Layout (size = s):
     *     7-------6
     *    /|      /|
     *   3-------2 |
     *   | |     | |
     *   | 4-----|-5
     *   |/      |/
     *   0-------1
     *
     * @param vertices Output vector for vertex data (interleaved position + normal)
     * @param indices Output vector for triangle indices
     * @param size Half-extent of the cube (distance from center to face)
     */
    void testRayCasting::GenerateCubeData(std::vector<float>& vertices, std::vector<unsigned int>& indices, float size) {
        float s = size; // Half-extent of the cube

        // Define 8 corner vertices with positions and approximate normals
        // Format: x, y, z, nx, ny, nz (6 floats per vertex)
        // Note: Normals are simplified corner normals (not face normals)
        float cubeVerts[] = {
            // Position          // Normal (approximate)
            -s, -s,  s,  -1.0f, -1.0f,  1.0f, // 0: Front-bottom-left
             s, -s,  s,   1.0f, -1.0f,  1.0f, // 1: Front-bottom-right
             s,  s,  s,   1.0f,  1.0f,  1.0f, // 2: Front-top-right
            -s,  s,  s,  -1.0f,  1.0f,  1.0f, // 3: Front-top-left
            -s, -s, -s,  -1.0f, -1.0f, -1.0f, // 4: Back-bottom-left
             s, -s, -s,   1.0f, -1.0f, -1.0f, // 5: Back-bottom-right
             s,  s, -s,   1.0f,  1.0f, -1.0f, // 6: Back-top-right
            -s,  s, -s,  -1.0f,  1.0f, -1.0f  // 7: Back-top-left
        };
        vertices.insert(vertices.end(), std::begin(cubeVerts), std::end(cubeVerts));

        // Define triangle indices for 6 faces (2 triangles per face, counter-clockwise winding)
        unsigned int cubeIndices[] = {
            0, 1, 2, 2, 3, 0, // Front face (+Z)
            1, 5, 6, 6, 2, 1, // Right face (+X)
            5, 4, 7, 7, 6, 5, // Back face (-Z)
            4, 0, 3, 3, 7, 4, // Left face (-X)
            3, 2, 6, 6, 7, 3, // Top face (+Y)
            4, 5, 1, 1, 0, 4  // Bottom face (-Y)
        };
        indices.insert(indices.end(), std::begin(cubeIndices), std::end(cubeIndices));
    }

    /**
     * Sets up OpenGL buffers for cube rendering.
     *
     * Similar to SetupBuffers() but for cube geometry.
     * Creates VAO, VBO, and IBO for rendering cubes with unit size.
     */
    void testRayCasting::SetupCubeBuffers() {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        // Generate unit cube with half-extent = 1.0
        GenerateCubeData(vertices, indices, 1.0f);

        // Create Vertex Array Object
        m_CubeVAO = std::make_unique<VertexArray>();

        // Upload vertex data (positions + normals)
        m_CubeVBO = std::make_unique<VertexBuffer>(vertices.data(), vertices.size() * sizeof(float));

        // Define vertex layout: Position (3 floats) + Normal (3 floats)
        VertexBufferLayout layout;
        layout.Push<float>(3); // Position
        layout.Push<float>(3); // Normal
        m_CubeVAO->AddBuffer(*m_CubeVBO, layout);

        // Upload index data
        m_CubeIBO = std::make_unique<IndexBuffer>(indices.data(), indices.size());
    }
    /**
     * Ray-AABB (Axis-Aligned Bounding Box) Intersection Test using the Slab Method.
     *
     * Algorithm Overview:
     * An AABB can be thought of as the intersection of three pairs of parallel planes
     * (slabs) aligned with the coordinate axes. A ray intersects the box if and only if
     * it intersects all three slabs simultaneously.
     *
     * Mathematical Background:
     * For each axis (x, y, z), we calculate where the ray intersects the two planes:
     * - t_min: parameter where ray enters the slab
     * - t_max: parameter where ray exits the slab
     *
     * The ray equation: P(t) = rayOrigin + t * rayDirection
     *
     * For the X-axis slab bounded by [boxMin.x, boxMax.x]:
     *   rayOrigin.x + t * rayDirection.x = boxMin.x  =>  t = (boxMin.x - rayOrigin.x) / rayDirection.x
     *   rayOrigin.x + t * rayDirection.x = boxMax.x  =>  t = (boxMax.x - rayOrigin.x) / rayDirection.x
     *
     * Similar calculations for Y and Z axes.
     *
     * Intersection Logic:
     * 1. Calculate entry/exit t values for each axis pair
     * 2. tNear = maximum of all entry t values (last slab entered)
     * 3. tFar = minimum of all exit t values (first slab exited)
     * 4. Intersection exists if tNear <= tFar and tFar > 0
     *
     * Edge Cases Handled:
     * - Ray parallel to axis (division by ~0): min/max handles this correctly
     * - Ray pointing away from box: tFar < 0 check
     * - Ray origin inside box: tNear < 0, but tFar > 0
     *
     * @param rayOrigin Starting point of the ray
     * @param rayDirection Direction vector of the ray (should be normalized)
     * @param boxCenter Center position of the AABB
     * @param boxSize Half-extent of the box (distance from center to face)
     * @return true if ray intersects the box, false otherwise
     */
    bool testRayCasting::RayIntersectsCube(const glm::vec3& rayOrigin, const glm::vec3& rayDirection,
        const glm::vec3& boxCenter, float boxSize) {
        // Calculate AABB bounds from center and half-extent
        glm::vec3 boxMin = boxCenter - glm::vec3(boxSize);
        glm::vec3 boxMax = boxCenter + glm::vec3(boxSize);

        // Calculate t parameters where ray intersects each pair of planes
        // tMin: t values for intersecting min planes (x_min, y_min, z_min)
        // tMax: t values for intersecting max planes (x_max, y_max, z_max)
        glm::vec3 tMin = (boxMin - rayOrigin) / rayDirection;
        glm::vec3 tMax = (boxMax - rayOrigin) / rayDirection;

        // Handle ray direction sign: ensure t1 has entry times, t2 has exit times
        // If rayDirection component is negative, tMin and tMax are swapped
        glm::vec3 t1 = glm::min(tMin, tMax); // Entry times for each axis
        glm::vec3 t2 = glm::max(tMin, tMax); // Exit times for each axis

        // Find the overall entry and exit points
        // tNear: Latest entry point (maximum of all entry times)
        // tFar: Earliest exit point (minimum of all exit times)
        float tNear = glm::max(glm::max(t1.x, t1.y), t1.z);
        float tFar = glm::min(glm::min(t2.x, t2.y), t2.z);

        // Intersection conditions:
        // 1. tNear <= tFar: Ray enters before it exits (overlapping slabs)
        // 2. tFar > 0: Intersection is in front of ray origin (not behind)
        return tNear <= tFar && tFar > 0.0f;
    }
}