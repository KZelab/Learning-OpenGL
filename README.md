# Branch: 06-depth-testing



### Core Concepts Learned:
- **Depth Buffer (Z-Buffer)**: Hardware-accelerated depth testing for hidden surface removal
- **Depth Testing Function**: How the GPU determines which fragments are visible
- **3D Cube Geometry**: Creating proper 3D objects with 8 vertices and 12 triangles
- **Indexed Vertex Data**: Efficient geometry storage using Element Buffer Objects
- **Backface Culling**: Performance optimisation by discarding non-visible faces
- **Winding Order**: Counter-clockwise vertex ordering for front-facing triangles
- **Multiple Object Rendering**: Drawing several objects with individual transformations
- **Z-Fighting**: Understanding and preventing depth buffer precision issues

### OpenGL Functions Introduced:
- `glEnable(GL_DEPTH_TEST)` - Enable depth buffer testing
- `glDepthFunc(GL_LESS)` - Set depth comparison function
- `glClear(GL_DEPTH_BUFFER_BIT)` - Clear depth buffer each frame
- `glEnable(GL_CULL_FACE)` - Enable backface culling
- `glCullFace(GL_BACK)` - Specify which faces to cull
- `glFrontFace(GL_CCW)` - Set counter-clockwise as front-facing

### Technical Terms:
- **Depth Buffer**: Per-pixel storage of depth values for visibility determination
- **Z-Buffer Algorithm**: Classic hidden surface removal technique
- **Depth Test**: Per-fragment comparison to determine visibility
- **Backface Culling**: Discarding triangles facing away from the camera
- **Winding Order**: Clockwise/counter-clockwise vertex arrangement
- **Element Buffer Object (EBO)**: OpenGL buffer storing vertex indices
- **Index Buffer**: Alternative name for Element Buffer Object
- **Near/Far Plane**: Depth range boundaries in perspective projection
- **Z-Fighting**: Visual artifacts from insufficient depth buffer precision



### Depth Testing Pipeline
```
Fragment Generation → Depth Test → Colour Test → Frame Buffer
                            ↓
                    Compare fragment depth
                    with depth buffer value
                            ↓
                    Pass: Update colour and depth
                    Fail: Discard fragment
```

### 3D Cube Structure
```
8 vertices × 6 faces = 12 triangles = 36 indices
Each vertex: Position (XYZ) + Colour (RGB) = 6 floats
Total vertex data: 8 × 6 = 48 floats
Index array: 36 unsigned integers
```


## Understanding the Code

### Depth Testing Setup

**1. Enable Depth Testing:**
```cpp
glEnable(GL_DEPTH_TEST);        // Enable depth buffer testing
glDepthFunc(GL_LESS);           // Fragments pass if closer to camera
```

**2. Clear Depth Buffer Each Frame:**
```cpp
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear both colour and depth
```

### 3D Cube Vertex Data

**Cube Vertices (8 unique points):**
```cpp
float vertices[] = {
    // Position        // Colour
    // X      Y     Z     R     G     B
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  // 0: Front-bottom-left - Red
     0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  // 1: Front-bottom-right - Green
     0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  // 2: Front-top-right - Blue
    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  // 3: Front-top-left - Yellow

    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,  // 4: Back-bottom-left - Magenta
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  // 5: Back-bottom-right - Cyan
     0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.0f,  // 6: Back-top-right - Orange
    -0.5f,  0.5f, -0.5f,  0.5f, 0.5f, 0.5f   // 7: Back-top-left - Grey
};
```

**Cube Indices (12 triangles, counter-clockwise winding):**
```cpp
unsigned int indices[] = {
    // Front face (Z = +0.5)
    0, 1, 2,  2, 3, 0,
    // Back face (Z = -0.5)
    4, 5, 6,  6, 7, 4,
    // Left face (X = -0.5)
    7, 3, 0,  0, 4, 7,
    // Right face (X = +0.5)
    1, 5, 6,  6, 2, 1,
    // Bottom face (Y = -0.5)
    4, 0, 1,  1, 5, 4,
    // Top face (Y = +0.5)
    3, 7, 6,  6, 2, 3
};
```

### Backface Culling Implementation

```cpp
glEnable(GL_CULL_FACE);         // Enable culling
glCullFace(GL_BACK);            // Cull back-facing triangles
glFrontFace(GL_CCW);            // Counter-clockwise = front face
```

### Multiple Object Rendering

```cpp
glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f),   // Center cube
    glm::vec3( 2.0f,  0.0f, -1.0f),   // Right cube (further back)
    glm::vec3(-1.5f, -0.5f, -2.0f),   // Left-bottom cube (furthest back)
    glm::vec3( 0.5f,  1.5f, 0.5f),    // Right-top cube (closer)
    glm::vec3(-1.0f,  0.8f, 1.0f)     // Left-top cube (closest)
};

for (unsigned int i = 0; i < 5; i++) {
    // Individual model matrix per cube
    glm::mat4 cubeModel = glm::mat4(1.0f);
    cubeModel = glm::translate(cubeModel, cubePositions[i]);

    // Different rotation speed and axis for each cube
    float angle = currentTime * (1.0f + i * 0.3f);
    cubeModel = glm::rotate(cubeModel, angle, glm::vec3(1.0f + i * 0.2f, 0.3f + i * 0.1f, 0.5f));

    glm::mat4 cubeMvp = projection * view * cubeModel;
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(cubeMvp));
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}
```

## Try These Experiments

### Beginner Challenges:
1. **Disable depth testing** to see Z-fighting:
   ```cpp
   // Comment out this line temporarily
   // glEnable(GL_DEPTH_TEST);
   ```
   Notice how cubes flicker and render incorrectly.

2. **Change depth function**:
   ```cpp
   glDepthFunc(GL_GREATER);  // Further objects pass the test (inverted)
   ```

3. **Disable backface culling**:
   ```cpp
   // Comment out culling setup
   // glEnable(GL_CULL_FACE);
   ```
   See the performance impact and interior faces.

### Intermediate Challenges:
1. **Add more cubes in a grid pattern**:
   ```cpp
   for (int x = -2; x <= 2; x++) {
       for (int z = -2; z <= 2; z++) {
           glm::vec3 position(x * 2.0f, 0.0f, z * 2.0f);
           // Create model matrix and render cube
       }
   }
   ```

2. **Create depth-dependent colouring**:
   ```cpp
   // In vertex shader, pass depth to fragment shader
   float depthColor = (gl_Position.z + 1.0) * 0.5;  // Normalize depth
   vertexColour = mix(aColour, vec3(depthColor), 0.3);  // Blend with depth
   ```

3. **Implement wireframe mode toggle**:
   ```cpp
   if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
       glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Wireframe
   } else {
       glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // Solid
   }
   ```

### Advanced Challenges:
1. **Implement depth buffer visualisation**:
   - Create a separate shader that renders depth values as grayscale
   - Use second render pass to display depth buffer contents

2. **Add transparent objects**:
   - Disable depth writing for transparent objects
   - Sort transparent objects back-to-front for correct blending

3. **Create shadow mapping**:
   - Render scene from light's perspective to depth texture
   - Use depth texture for shadow calculations in main render



### Essential Reading:
- [LearnOpenGL - Depth Testing](https://learnopengl.com/Advanced-OpenGL/Depth-testing)
- [OpenGL Wiki - Depth Buffer](https://www.khronos.org/opengl/wiki/Depth_Buffer)
- [Real-Time Rendering - Z-Buffer Algorithm](http://www.realtimerendering.com/)

### Video Tutorials:
- [The Cherno OpenGL - Depth Testing](https://www.youtube.com/watch?v=poHDHe1Kjno)
- [3D Graphics Fundamentals - Hidden Surface Removal](https://www.youtube.com/results?search_query=z+buffer+algorithm)

### Deep Dive Topics:
- Z-buffer algorithm implementation and optimisation
- Early-Z testing and hierarchical Z-buffers
- Depth buffer precision and floating-point formats
- Shadow mapping and depth-based techniques
- Depth peeling for order-independent transparency
- Reverse Z-buffer for improved precision

### Tools for Development:
- **RenderDoc**: Visualise depth buffer contents and depth testing
- **Graphics Debugger**: Step through depth test operations
- **GPU Profiler**: Analyse depth testing performance impact

## Debug Tips

### Checking Depth Test State:
```cpp
GLboolean depthTestEnabled;
glGetBooleanv(GL_DEPTH_TEST, &depthTestEnabled);
if (!depthTestEnabled) {
    std::cout << "Warning: Depth testing is disabled!" << std::endl;
}
```

### Debugging Index Buffer:
```cpp
// Print indices to verify winding order
for (int i = 0; i < 36; i += 3) {
    std::cout << "Triangle " << i/3 << ": "
              << indices[i] << ", " << indices[i+1] << ", " << indices[i+2] << std::endl;
}
```

### Visualising Depth Values:
```cpp
// In fragment shader - output depth as colour
float depth = gl_FragCoord.z;
FragColor = vec4(depth, depth, depth, 1.0);  // Grayscale depth
```

### Testing Face Culling:
```cpp
// Temporarily disable culling to see all faces
glDisable(GL_CULL_FACE);
// Or switch to front face culling to see backfaces only
glCullFace(GL_FRONT);
```

