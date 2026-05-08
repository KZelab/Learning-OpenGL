# Branch: 02-first-triangle



### Core Concepts Learned:
- **Vertex Data**: The fundamental building blocks of 3D geometry
- **Vertex Buffer Objects (VBO)**: Storing vertex data in GPU memory
- **Vertex Array Objects (VAO)**: Configuring how OpenGL interprets vertex data
- **Shaders**: Small programs that run on the GPU (vertex and fragment shaders)
- **Graphics Pipeline**: Understanding the flow from vertices to pixels
- **Normalized Device Coordinates (NDC)**: The coordinate system OpenGL uses

### OpenGL Functions Introduced:
- `glGenBuffers()` / `glDeleteBuffers()` - Create/destroy buffer objects
- `glBindBuffer()` - Make a buffer active
- `glBufferData()` - Upload data to GPU memory
- `glGenVertexArrays()` / `glDeleteVertexArrays()` - Create/destroy VAOs
- `glBindVertexArray()` - Make a VAO active
- `glVertexAttribPointer()` - Define vertex attribute layout
- `glEnableVertexAttribArray()` - Enable vertex attributes
- `glCreateShader()` / `glDeleteShader()` - Create/destroy shader objects
- `glShaderSource()` - Attach source code to shader
- `glCompileShader()` - Compile shader code
- `glCreateProgram()` / `glDeleteProgram()` - Create/destroy shader programs
- `glAttachShader()` - Attach compiled shaders to program
- `glLinkProgram()` - Link shaders into complete program
- `glUseProgram()` - Activate shader program for rendering
- `glDrawArrays()` - Draw primitives from vertex data

### Technical Terms:
- **Vertex**: A point in 3D space with properties (position, colour, etc.)
- **Primitive**: Basic geometric shape (point, line, triangle)
- **Vertex Shader**: Processes individual vertices
- **Fragment Shader**: Determines pixel colours
- **Rasterization**: Converting vertices to pixels
- **Attribute**: Per-vertex data (like position or color)



### Program Flow
1. **Initialize GLFW & OpenGL** - Set up windowing and graphics context
2. **Define Vertex Data** - Create triangle vertex positions
3. **Create VBO** - Upload vertex data to GPU memory
4. **Create VAO** - Configure vertex attribute interpretation
5. **Create Shaders** - Write and compile vertex/fragment shaders
6. **Link Shader Program** - Combine shaders into executable program
7. **Render Loop** - Draw triangle every frame
8. **Cleanup** - Properly release GPU resources

### Understanding the Graphics Pipeline

```
Vertices → Vertex Shader → Primitive Assembly → Rasterization → Fragment Shader → Pixels
```

1. **Vertices**: Your 3 triangle points (-0.5,-0.5), (0.5,-0.5), (0.0,0.5)
2. **Vertex Shader**: Transforms each vertex position (identity transform here)
3. **Primitive Assembly**: Groups vertices into triangles
4. **Rasterization**: Determines which pixels are inside the triangle
5. **Fragment Shader**: Colours each pixel orange (1.0, 0.2, 1.0)


## Understanding the Code

### Vertex Data Explanation
```cpp
float vertices[] = {
    -0.5f, -0.5f,  0.0f,  // Bottom left
     0.5f, -0.5f,  0.0f,  // Bottom right
     0.0f,  0.5f,  0.0f   // Top center
};
```
- Coordinates are in Normalized Device Coordinates (NDC)
- NDC range from -1.0 to +1.0 for visible area
- Z=0.0 means the triangle is in the center depth-wise

### Shader Sources
**Vertex Shader:**
- Receives vertex position as input (`aPos`)
- Outputs final vertex position (`gl_Position`)
- Currently passes position through unchanged

**Fragment Shader:**
- Outputs the colour for each pixel
- Sets all pixels to orange (1.0, 0.5, 0.2, 1.0)
- RGBA format: Red=1.0, Green=0.5, Blue=0.2, Alpha=1.0

### Memory Layout
The VBO stores the vertex data [-0.5,-0.5,0] [0.5,-0.5,0] [0,0.5,0] in GPU memory. The VAO points to this data and configures OpenGL to interpret every 3 floats as 1 vertex position.

## Try These Experiments

### Beginner Challenges:
1. **Change triangle colour** - Modify the fragment shader RGB values:
   ```glsl
   FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);  // Red triangle
   FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);  // Green triangle
   ```

2. **Change triangle size** - Modify the vertex coordinates:
   ```cpp
   float vertices[] = {
       -0.8f, -0.8f,  0.0f,  // Bigger triangle
        0.8f, -0.8f,  0.0f,
        0.0f,  0.8f,  0.0f
   };
   ```

3. **Move the triangle** - Add offset to all X coordinates:
   ```cpp
   float offset = 0.3f;
   float vertices[] = {
       -0.5f + offset, -0.5f,  0.0f,
        0.5f + offset, -0.5f,  0.0f,
        0.0f + offset,  0.5f,  0.0f
   };
   ```

### Intermediate Challenges:
1. **Create a square using two triangles**:
   ```cpp
   float vertices[] = {
       // First triangle
       -0.5f, -0.5f, 0.0f,  // Bottom left
        0.5f, -0.5f, 0.0f,  // Bottom right
       -0.5f,  0.5f, 0.0f,  // Top left

       // Second triangle
        0.5f, -0.5f, 0.0f,  // Bottom right
        0.5f,  0.5f, 0.0f,  // Top right
       -0.5f,  0.5f, 0.0f   // Top left
   };
   // Change glDrawArrays(GL_TRIANGLES, 0, 6);
   ```

2. **Add colour as a vertex attribute**:
   ```cpp
   float vertices[] = {
       // Position        // Colour
       -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // Bottom left, red
        0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // Bottom right, green
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // Top center, blue
   };
   ```
   *Note: This requires updating the vertex shader and VAO configuration*

3. **Animate the triangle** - Add rotation in the vertex shader:
   ```cpp
   // Add uniform for time in fragment shader
   // Pass glfwGetTime() to shader
   // Apply rotation matrix to vertex positions
   ```

### Advanced Challenges:
1. **Add keyboard controls** - Use WASD to move the triangle
2. **Multiple triangles** - Draw several triangles with different colours
3. **Wireframe mode** - Use `glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)`



### Essential Reading:
- [LearnOpenGL - Hello Triangle](https://learnopengl.com/Getting-started/Hello-Triangle)
- [OpenGL Wiki - Vertex Specification](https://www.khronos.org/opengl/wiki/Vertex_Specification)
- [OpenGL Wiki - Shader](https://www.khronos.org/opengl/wiki/Shader)

### Video Tutorials:
- [The Cherno OpenGL Series - Episode 4: Vertex Buffers](https://www.youtube.com/watch?v=x0H--CL2tUI)
- [The Cherno OpenGL Series - Episode 5: Vertex Arrays](https://www.youtube.com/watch?v=Bcs4TufVmVk)

### Deep Dive Topics:
- OpenGL State Machine
- Graphics Pipeline Stages
- GPU Memory Management
- GLSL Language Specification
- Coordinate Systems in Computer Graphics

### Tools for Development:
- **RenderDoc**: Graphics debugging and profiling
- **OpenGL ES Emulator**: Testing on different GPU capabilities
- **Shader Playground**: Online GLSL shader testing



