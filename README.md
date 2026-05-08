# Branch: 03-vertex-attributes


### Core Concepts Learned:
- **Multiple Vertex Attributes**: Each vertex can have multiple properties (position, colour, normals, etc.)
- **Interleaved Vertex Data**: Storing different attributes together in a single buffer
- **Vertex Attribute Pointers**: Telling OpenGL how to interpret different attributes
- **Shader Input/Output**: Communication between vertex and fragment shaders
- **Attribute Interpolation**: How OpenGL smoothly blends values across triangle surfaces
- **Vertex Attribute Locations**: Binding attributes to specific shader inputs

### OpenGL Functions Introduced:
- `glVertexAttribPointer()` with stride and offset parameters
- `glEnableVertexAttribArray()` for multiple attributes
- Multiple calls to configure different vertex attributes

### GLSL Features Introduced:
- **Input Attributes**: `layout (location = 1) in vec3 aColour`
- **Vertex Shader Outputs**: `out vec3 vertexColour`
- **Fragment Shader Inputs**: `in vec3 vertexColour`
- **Attribute Location Binding**: Matching locations between VAO and shader

### Technical Terms:
- **Interleaved Data**: Multiple attributes packed together per vertex
- **Stride**: Total bytes between consecutive vertices
- **Offset**: Bytes from vertex start to specific attribute
- **Varying Variables**: Data passed from vertex to fragment shader (interpolated)
- **Attribute Location**: Index number identifying vertex attributes
- **Rasterization Interpolation**: How values are smoothly blended across surfaces



### Vertex Data Layout
```
Vertex 0: [X, Y, Z, R, G, B] = [-0.5, -0.5, 0.0, 1.0, 0.0, 0.0]  // Red
Vertex 1: [X, Y, Z, R, G, B] = [ 0.5, -0.5, 0.0, 0.0, 1.0, 0.0]  // Green
Vertex 2: [X, Y, Z, R, G, B] = [ 0.0,  0.5, 0.0, 0.0, 0.0, 1.0]  // Blue

Memory Layout:
[X0,Y0,Z0,R0,G0,B0, X1,Y1,Z1,R1,G1,B1, X2,Y2,Z2,R2,G2,B2]
```

### Vertex Attribute Configuration
```cpp
// Attribute 0: Position (XYZ) - 3 floats, stride 6*sizeof(float), offset 0
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

// Attribute 1: Colour (RGB) - 3 floats, stride 6*sizeof(float), offset 3*sizeof(float)
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
```

## What's Different from Previous Branch (02-first-triangle)

### New Additions:
- **Interleaved vertex data** with position and colour per vertex
- **Multiple vertex attributes** configured in VAO
- **Enhanced vertex shader** with colour input and output
- **Updated fragment shader** using interpolated colours
- **Stride and offset calculations** for attribute pointers
- **Shader communication** between vertex and fragment stages

### What Stayed the Same:
- Basic window setup and OpenGL context
- VBO creation and data upload
- VAO creation (but with enhanced configuration)
- Render loop and drawing commands
- Resource cleanup

## Understanding the Code

### Interleaved Vertex Data Explained
Instead of separate arrays for position and colour, we store them together:
```cpp
float vertices[] = {
    // Position        // Colour
    -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // Vertex 0: Red
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  // Vertex 1: Green
     0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f   // Vertex 2: Blue
};
```

This is efficient because related data stays together in memory.

### Vertex Attribute Pointer Parameters
```cpp
glVertexAttribPointer(location, size, type, normalized, stride, offset)
```
- **location**: Which attribute (0 for position, 1 for colour)
- **size**: Components per attribute (3 for XYZ or RGB)
- **type**: Data type (GL_FLOAT)
- **normalized**: Whether to normalize data (GL_FALSE)
- **stride**: Bytes between consecutive vertices (6 * sizeof(float))
- **offset**: Bytes from vertex start to this attribute

### Shader Communication Pipeline
1. **Vertex Shader Input**: Receives position and colour per vertex
2. **Vertex Shader Output**: Passes colour to next stage
3. **Rasterization**: OpenGL interpolates colours across triangle
4. **Fragment Shader Input**: Receives interpolated colour per pixel
5. **Fragment Shader Output**: Uses interpolated colour for final pixel

### Colour Interpolation
OpenGL automatically interpolates vertex colours across the triangle surface:
- **Red vertex** at bottom-left
- **Green vertex** at bottom-right
- **Blue vertex** at top-centre
- **Interpolated colours** everywhere else (yellow, cyan, magenta, etc.)

## Try These Experiments

### Beginner Challenges:
1. **Change vertex colours** - Try different RGB combinations:
   ```cpp
   // Make it a warm triangle (red, orange, yellow)
   -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // Red
    0.5f, -0.5f, 0.0f,  1.0f, 0.5f, 0.0f,  // Orange
    0.0f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f   // Yellow
   ```

2. **Create a monochrome triangle** - Use shades of the same colour:
   ```cpp
   // Different shades of blue
   -0.5f, -0.5f, 0.0f,  0.2f, 0.2f, 0.8f,  // Dark blue
    0.5f, -0.5f, 0.0f,  0.5f, 0.5f, 1.0f,  // Medium blue
    0.0f,  0.5f, 0.0f,  0.8f, 0.8f, 1.0f   // Light blue
   ```

3. **Experiment with alpha values** - Modify fragment shader:
   ```glsl
   FragColor = vec4(vertexColour, 0.5f);  // Semi-transparent
   ```

### Intermediate Challenges:
1. **Create a rainbow square** using two triangles:
   ```cpp
   float vertices[] = {
       // First triangle
       -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // Bottom-left: Red
        0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // Bottom-right: Green
       -0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  // Top-left: Blue

       // Second triangle
        0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // Bottom-right: Green
        0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f,  // Top-right: Yellow
       -0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // Top-left: Blue
   };
   // Change to: glDrawArrays(GL_TRIANGLES, 0, 6);
   ```

2. **Add a brightness attribute**:
   ```cpp
   float vertices[] = {
       // Position        // Colour        // Brightness
       -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // Bright red
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f,  // Dim green
        0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.8f   // Medium blue
   };
   ```
   *Note: Requires updating shaders and VAO configuration*

3. **Create pulsating colours** - Add time uniform:
   ```glsl
   // In fragment shader
   uniform float uTime;
   void main() {
       vec3 pulsatingColour = vertexColour * (0.5 + 0.5 * sin(uTime));
       FragColor = vec4(pulsatingColour, 1.0);
   }
   ```

### Advanced Challenges:
1. **HSV colour space** - Convert RGB to HSV in fragment shader
2. **Vertex colour animation** - Modify colours in vertex shader based on time
3. **Multiple triangles** with different colour schemes
4. **Index buffer usage** - Eliminate duplicate vertices in square example


## Resources for Deeper Learning

### Essential Reading:
- [LearnOpenGL - Shaders](https://learnopengl.com/Getting-started/Shaders)
- [OpenGL Wiki - Vertex Attribute](https://www.khronos.org/opengl/wiki/Vertex_Attribute)
- [GLSL Specification - Input/Output](https://registry.khronos.org/OpenGL/specs/gl/GLSLangSpec.4.60.html)

### Video Tutorials:
- [The Cherno OpenGL - Vertex Attributes](https://www.youtube.com/watch?v=x0H--CL2tUI)
- [OpenGL Tutorial - Multiple Vertex Attributes](https://www.youtube.com/watch?v=bI4FHmLHK8Q)

### Deep Dive Topics:
- Vertex Attribute Array Object (VAO) state management
- Memory layout optimization for GPU performance
- GLSL varying interpolation qualifiers (smooth, flat, noperspective)
- Vertex pulling vs vertex pushing architectures

### Tools for Development:
- **RenderDoc**: Inspect vertex attribute data
- **OpenGL Debugger**: Verify VAO configuration
- **GPU Memory Profilers**: Analyse vertex data layout efficiency

