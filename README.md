# Branch: 05-transformations


### Core Concepts Learned:
- **Transformation Matrices**: Mathematical tools for moving, rotating, and scaling objects
- **Model-View-Projection (MVP)**: The standard 3D graphics pipeline
- **Coordinate Systems**: Local/Model, World, View, and Clip coordinates
- **Matrix Multiplication**: How transformations combine and order matters
- **Uniform Variables**: Passing data from CPU to GPU shaders
- **Interactive Controls**: Real-time input affecting 3D transformations
- **Perspective Projection**: Creating realistic 3D depth perception

### OpenGL Functions Introduced:
- `glGetUniformLocation()` - Find uniform variable locations in shaders
- `glUniformMatrix4fv()` - Upload matrix data to GPU
- `glfwGetKey()` - Check keyboard input state
- `glfwGetTime()` - Get elapsed time for animations
- `glm::perspective()` - Create perspective projection matrix
- `glm::lookAt()` - Create view/camera matrix
- `glm::translate()`, `glm::rotate()`, `glm::scale()` - Transformation functions

### GLM Library Functions:
- `glm::mat4()` - 4x4 matrix creation
- `glm::radians()` - Convert degrees to radians
- `glm::value_ptr()` - Get pointer to matrix data for OpenGL
- Matrix multiplication with `*` operator
- Vector creation with `glm::vec3()`

### Technical Terms:
- **MVP Matrix**: Combined transformation from model to screen coordinates
- **Model Matrix**: Local object space to world space transformation
- **View Matrix**: World space to camera/eye space transformation
- **Projection Matrix**: 3D to 2D screen projection transformation
- **Uniform**: Global shader variable set from CPU
- **Identity Matrix**: Neutral transformation (no change)
- **Homogeneous Coordinates**: 4D vectors for 3D transformations



### Transformation Pipeline
```
Local Coordinates → Model Matrix → World Coordinates
       ↓
View Matrix → Camera Coordinates → Projection Matrix → Clip Coordinates
       ↓
Perspective Division → Normalized Device Coordinates → Screen Coordinates
```

### Matrix Multiplication Order
The order of matrix multiplication is crucial:
```cpp
MVP = Projection * View * Model
```
This reads right-to-left: Model → View → Projection



## Understanding the Code

### The Graphics Pipeline Transformation

**1. Model Matrix (Local → World Space):**
```cpp
glm::mat4 model = glm::mat4(1.0f);  // Identity matrix (no transformation)
model = glm::rotate(model, currentTime, glm::vec3(0.0f, 0.0f, 1.0f));
```
Transforms vertices from the object's local coordinate system to world coordinates.

**2. View Matrix (World → Camera Space):**
```cpp
glm::mat4 view = glm::lookAt(
    glm::vec3(0.0f, 0.0f, 3.0f),   // Camera position (3 units back)
    glm::vec3(0.0f, 0.0f, 0.0f),   // Look at origin
    glm::vec3(0.0f, 1.0f, 0.0f)    // Up vector (Y-axis)
);
```
Simulates placing a camera in the 3D world.

**3. Projection Matrix (Camera → Screen Space):**
```cpp
glm::mat4 projection = glm::perspective(
    glm::radians(45.0f),    // 45-degree field of view
    960.0f / 540.0f,        // Aspect ratio (width/height)
    0.1f,                   // Near clipping plane
    100.0f                  // Far clipping plane
);
```
Creates perspective distortion - objects further away appear smaller.

### Matrix Uniform Upload

```cpp
// Get uniform location (do this once, outside render loop)
int mvpLocation = glGetUniformLocation(shaderProgram, "u_MVP");

// Upload matrix each frame (inside render loop)
glm::mat4 mvp = projection * view * model;
glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));
```

### Keyboard Controls Implementation

```cpp
if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    model = glm::translate(model, glm::vec3(0.0f, 0.001f, 0.0f));  // Small upward movement
}
```
Small incremental movements accumulate over frames for smooth motion.

## Try These Experiments

### Beginner Challenges:
1. **Change rotation axis** - Rotate around different axes:
   ```cpp
   // Rotate around X-axis (like a wheel)
   model = glm::rotate(model, currentTime, glm::vec3(1.0f, 0.0f, 0.0f));

   // Rotate around Y-axis (like a spinning top)
   model = glm::rotate(model, currentTime, glm::vec3(0.0f, 1.0f, 0.0f));
   ```

2. **Modify rotation speed**:
   ```cpp
   float rotationSpeed = 2.0f;  // 2x faster
   model = glm::rotate(model, currentTime * rotationSpeed, glm::vec3(0.0f, 0.0f, 1.0f));
   ```

3. **Change camera position**:
   ```cpp
   // Camera looking from the side
   glm::mat4 view = glm::lookAt(
       glm::vec3(3.0f, 0.0f, 0.0f),   // Camera to the right
       glm::vec3(0.0f, 0.0f, 0.0f),   // Still looking at origin
       glm::vec3(0.0f, 1.0f, 0.0f)    // Up vector unchanged
   );
   ```

### Intermediate Challenges:
1. **Add oscillating scale animation**:
   ```cpp
   float scaleAmount = 1.0f + 0.3f * sin(currentTime * 2.0f);  // Oscillate between 0.7 and 1.3
   model = glm::scale(model, glm::vec3(scaleAmount, scaleAmount, 1.0f));
   ```

2. **Create orbital motion**:
   ```cpp
   float radius = 1.5f;
   glm::vec3 orbitPosition = glm::vec3(
       radius * cos(currentTime),
       radius * sin(currentTime),
       0.0f
   );
   model = glm::translate(model, orbitPosition);
   ```

3. **Implement mouse look camera**:
   ```cpp
   // Get mouse position and convert to camera angles
   double mouseX, mouseY;
   glfwGetCursorPos(window, &mouseX, &mouseY);

   float yaw = (mouseX / 960.0f) * 2.0f * 3.14159f;     // Full circle
   float pitch = (mouseY / 540.0f) * 3.14159f - 1.57f;  // +/- 90 degrees

   glm::vec3 cameraPos = glm::vec3(
       3.0f * cos(pitch) * cos(yaw),
       3.0f * sin(pitch),
       3.0f * cos(pitch) * sin(yaw)
   );

   view = glm::lookAt(cameraPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
   ```

### Advanced Challenges:
1. **Multiple objects** - Render several squares with different transformations
2. **Hierarchical transformations** - Parent-child object relationships
3. **Smooth input handling** - Implement acceleration and deceleration
4. **Camera animation** - Automatic camera movement around the scene



### Essential Reading:
- [LearnOpenGL - Transformations](https://learnopengl.com/Getting-started/Transformations)
- [OpenGL Wiki - Uniform](https://www.khronos.org/opengl/wiki/Uniform_(GLSL))
- [GLM Documentation](https://glm.g-truc.net/0.9.9/index.html)

### Video Tutorials:
- [The Cherno OpenGL - Matrices](https://www.youtube.com/watch?v=LhQ85bkQpGk)
- [3Blue1Brown - Linear Transformations](https://www.youtube.com/watch?v=kYB8IZa5AuE)

### Deep Dive Topics:
- Linear algebra foundations for computer graphics
- Quaternions for rotation representation
- Matrix decomposition and inverse transformations
- Gimbal lock and rotation interpolation
- Camera control systems and movement patterns




### Debugging Matrix Values:
```cpp
// Print matrix for debugging
glm::mat4 mvp = projection * view * model;
for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
        std::cout << mvp[i][j] << " ";
    }
    std::cout << std::endl;
}
```