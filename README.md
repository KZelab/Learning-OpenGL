# Branch: 08-abstraction



### Core Concepts Learned:
- **Class-Based Architecture**: Organizing code into logical, reusable components
- **Separation of Concerns**: Each class handles one specific responsibility
- **Encapsulation**: Hiding implementation details behind clean interfaces
- **Resource Management**: RAII (Resource Acquisition Is Initialization) pattern
- **Code Reusability**: Writing code that can be reused across different contexts
- **Maintainability**: Structuring code for easy modification and extension
- **Abstraction Layers**: Creating high-level interfaces over low-level OpenGL calls
- **Object Lifetime Management**: Proper construction and destruction of resources

### Design Patterns Introduced:
- **RAII Pattern**: Automatic resource management in constructors/destructors
- **Factory Pattern**: Static methods for creating common objects (Mesh::CreateCube)
- **Wrapper Pattern**: Encapsulating OpenGL state in C++ objects
- **Interface Segregation**: Small, focused class interfaces

### Architecture Components:
- **Shader Class**: Manages shader compilation, linking, and uniform setting
- **Mesh Class**: Handles vertex data, VAO/VBO/EBO management
- **Renderer Class**: Provides high-level rendering commands and OpenGL error checking
- **Resource Management**: Automatic cleanup via destructors

## Code Architecture

### File Structure
```
src/
├── CMakeHelloWorld.cpp    # Main application using abstracted classes
├── Shader.h/.cpp          # Shader compilation and uniform management
├── Mesh.h/.cpp            # Vertex data and geometry abstraction
├── Renderer.h/.cpp        # High-level rendering commands
├── VertexArray.h/.cpp     # VAO wrapper (from main branch)
├── VertexBuffer.h/.cpp    # VBO wrapper (from main branch)
├── IndexBuffer.h/.cpp     # EBO wrapper (from main branch)
└── VertexBufferLayout.h   # Vertex attribute layout helper

```

### Class Responsibility Overview

The abstracted architecture follows a clear separation of concerns:

**Application Layer (Main):**
- Coordinates overall rendering workflow
- Manages scene objects and transformations
- Handles user input and application lifecycle

**Renderer Class:**
- Provides high-level rendering commands
- Abstracts OpenGL draw calls and state management
- Handles error checking and debugging

**Shader Class:**
- Manages shader compilation and linking
- Handles uniform variable setting
- Provides clean interface for shader operations

**Mesh Class:**
- Encapsulates vertex and index data
- Manages VAO/VBO/EBO resources
- Provides factory methods for common geometries

### OpenGL State Management

**Before (Monolithic):**
- Manual glGenBuffers, glBindBuffer, glBufferData calls
- Manual glGenVertexArrays, glBindVertexArray management
- Manual glCreateShader, glCompileShader, glLinkProgram operations
- Manual cleanup with glDelete* functions

**After (Abstracted):**
- Automatic resource management via RAII pattern
- Clean interfaces: shader.Bind(), mesh.Bind()
- Exception-safe resource cleanup in destructors
- Centralised error checking and debugging support

## What's Different from Previous Branch (07-texturing)

### New Additions:
- **Shader Class**: Encapsulates shader compilation, linking, and uniform management
- **Mesh Class**: Manages vertex data, VAO/VBO/EBO creation and binding
- **Renderer Class**: Provides high-level rendering commands and error checking
- **RAII Resource Management**: Automatic OpenGL resource cleanup
- **Modular Architecture**: Separated concerns into focused classes
- **Error Checking Macros**: GlCall macro for debugging OpenGL errors
- **Factory Methods**: Static creation methods for common objects

### What Stayed the Same:
- Identical visual output (5 rotating textured cubes)
- Same vertex data layout and shader logic
- Same transformation matrices and camera setup
- Same texture creation and binding
- Same keyboard input handling

## Understanding the Code

### Shader Class Architecture

**Header (Shader.h):**
```cpp
class Shader {
private:
    std::string m_Filepath;
    unsigned int m_RendererID;
    std::unordered_map<std::string, int> m_uniformLocationCache;

public:
    Shader(const std::string& filepath);  // File-based constructor
    Shader(const char* vertexSrc, const char* fragmentSrc);  // String-based
    ~Shader();  // RAII cleanup

    void Bind() const;
    void Unbind() const;
    void setUniformMat4f(const std::string& name, const glm::mat4& matrix);
    // ... other uniform setters
};
```

**Key Features:**
- **Uniform Caching**: Avoids expensive glGetUniformLocation calls
- **Multiple Constructors**: Support for file-based and string-based shaders
- **RAII**: Automatic glDeleteProgram in destructor
- **Error Handling**: Comprehensive shader compilation error reporting

### Mesh Class Architecture

**Vertex Structure:**
```cpp
struct Vertex {
    float position[3];   // XYZ coordinates
    float colour[3];     // RGB colour values
    float texCoords[2];  // UV texture coordinates
};
```

**Mesh Management:**
```cpp
class Mesh {
private:
    unsigned int m_VAO, m_VBO, m_EBO;
    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;

public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();  // RAII cleanup

    void Bind() const;
    static Mesh* CreateCube();  // Factory method
};
```

**Advantages:**
- **Type Safety**: Structured vertex data instead of raw float arrays
- **Memory Management**: Automatic VAO/VBO/EBO cleanup
- **Factory Pattern**: Easy creation of common geometries
- **Reusability**: Same mesh can be drawn multiple times

### Renderer Class Architecture

**Error Checking Macros:**
```cpp
#define ASSERT(x) if (!(x)) __debugbreak();

#define GlCall(x) glClearError();\
    x;\
    ASSERT(glLogCall(#x, __FILE__, __LINE__))
```

**Rendering Interface:**
```cpp
class Renderer {
public:
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
    void Clear() const;
    void ClearColour_Black() const;
    void ClearColour_White() const;
};
```

**Benefits:**
- **Error Detection**: Automatic OpenGL error checking and reporting
- **High-Level Interface**: Simple Draw() calls instead of manual OpenGL
- **Debugging Support**: File and line number reporting for errors
- **State Management**: Centralized rendering state control

### Refactored Main Loop

**Before (Monolithic):**
```cpp
// Manual OpenGL calls scattered throughout main()
glGenBuffers(1, &VBO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
glUseProgram(shaderProgram);
glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(cubeMvp));
glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
```

**After (Abstracted):**
```cpp
// Clean object-oriented interface
Mesh* cubeMesh = Mesh::CreateCube();
Shader shader(vertexShader, fragmentShader);
Renderer renderer;

// In render loop:
shader.Bind();
cubeMesh->Bind();
shader.setUniformMat4f("u_MVP", cubeMvp);
glDrawElements(GL_TRIANGLES, cubeMesh->GetIndexCount(), GL_UNSIGNED_INT, 0);
```

## Try These Experiments

### Beginner Challenges:
1. **Add new uniform setters**:
   ```cpp
   // In Shader class, add:
   void setUniform3f(const std::string& name, const glm::vec3& vector) {
       GlCall(glUniform3fv(getUniformLocation(name), 1, &vector[0]));
   }
   ```

2. **Create different mesh types**:
   ```cpp
   // Add to Mesh class:
   static Mesh* CreateTriangle();
   static Mesh* CreateQuad();
   static Mesh* CreateSphere(int subdivisions);
   ```

3. **Add renderer state methods**:
   ```cpp
   // In Renderer class:
   void EnableWireframe() const;
   void SetViewport(int x, int y, int width, int height) const;
   ```

### Intermediate Challenges:
1. **Implement shader hot-reloading**:
   ```cpp
   class Shader {
       void ReloadFromFile();  // Re-read and recompile shader file
       bool CheckFileModified();  // Check if file changed on disk
   };
   ```

2. **Add mesh transformation support**:
   ```cpp
   class Mesh {
       void Transform(const glm::mat4& matrix);  // Apply transformation to vertices
       void Scale(float factor);
       void Translate(const glm::vec3& offset);
   };
   ```

3. **Create material system**:
   ```cpp
   struct Material {
       glm::vec3 ambient, diffuse, specular;
       float shininess;
       unsigned int diffuseTexture, normalTexture;
   };
   ```

### Advanced Challenges:
1. **Implement render batching**:
   - Group similar objects to reduce draw calls
   - Sort by shader, then by texture, then by mesh
   - Use instanced rendering for identical objects

2. **Add memory profiling**:
   - Track OpenGL memory usage
   - Monitor vertex buffer and texture memory
   - Implement resource pooling

3. **Create scene graph**:
   - Hierarchical object relationships
   - Automatic transformation inheritance
   - Efficient frustum culling

## Common Issues & Solutions

### "Linker errors with new classes"
**Build Configuration Problems:**
- **Missing source files**: Ensure all .cpp files are in Visual Studio project
- **Include path issues**: Check header file paths and dependencies
- **Circular includes**: Use forward declarations in headers
- **Static library linking**: Verify OpenGL libraries are linked correctly

### "Runtime crashes in destructors"
**Resource Management Issues:**
- **Double deletion**: Ensure resources aren't deleted multiple times
- **Use after destruction**: Don't use objects after they're destroyed
- **OpenGL context**: Ensure OpenGL context exists when destructors run
- **Exception safety**: Use RAII consistently throughout codebase

### "Performance regression with abstraction"
**Overhead Concerns:**
- **Virtual function calls**: Keep hot paths non-virtual where possible
- **Excessive state changes**: Batch similar rendering operations
- **Memory allocations**: Pre-allocate containers and reuse objects
- **Debug vs Release**: Ensure compiler optimizations are enabled

### "Debugging abstracted OpenGL calls"
**Error Tracking Issues:**
- **GlCall macro**: Use consistently for all OpenGL calls
- **Error context**: Check which specific call failed
- **State validation**: Verify OpenGL state before operations
- **Resource tracking**: Monitor resource creation and deletion

## What's Next?

In the next branch (`09-test-harness`), we'll:
- **Implement testing framework** from the main branch architecture
- **Add unit tests** for our abstracted classes
- **Create integration tests** for rendering pipeline
- **Set up automated testing** for graphics code validation
- **Learn testing patterns** specific to graphics programming
- **Establish quality assurance** practices for continued development

## Resources for Deeper Learning

### Essential Reading:
- [Effective C++ by Scott Meyers](https://www.amazon.com/Effective-Specific-Improve-Programs-Designs/dp/0321334876)
- [Clean Code by Robert Martin](https://www.amazon.com/Clean-Code-Handbook-Software-Craftsmanship/dp/0132350884)
- [Game Engine Architecture by Jason Gregory](https://www.gameenginebook.com/)

### Video Tutorials:
- [The Cherno C++ Series](https://www.youtube.com/playlist?list=PLlrATfBNZ98dudnM48yfGUldqGD0S4FFb)
- [OpenGL Abstraction Patterns](https://www.youtube.com/results?search_query=opengl+abstraction+patterns)

### Deep Dive Topics:
- RAII and modern C++ resource management
- Design patterns in game engine architecture
- OpenGL debugging and profiling techniques
- Memory management in graphics applications
- Component-based entity systems
- Render graph architecture

### Tools for Development:
- **Visual Studio Debugger**: Step through abstracted object lifetimes
- **RenderDoc**: Analyze abstracted rendering calls
- **Application Verifier**: Detect resource leaks and errors

## Debug Tips

### Checking Object Lifetime:
```cpp
// Add logging to constructors/destructors
Shader::Shader(const std::string& filepath) {
    std::cout << "Creating shader: " << filepath << std::endl;
    // ... construction code
}

Shader::~Shader() {
    std::cout << "Destroying shader: " << m_Filepath << std::endl;
    GlCall(glDeleteProgram(m_RendererID));
}
```

### Validating OpenGL State:
```cpp
// Add state validation methods
void Renderer::ValidateState() const {
    GLint currentProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    std::cout << "Current shader program: " << currentProgram << std::endl;
}
```

### Memory Leak Detection:
```cpp
// Track resource creation/deletion
static int shaderCount = 0;

Shader::Shader(...) {
    ++shaderCount;
    std::cout << "Shader count: " << shaderCount << std::endl;
}

Shader::~Shader() {
    --shaderCount;
    std::cout << "Shader count: " << shaderCount << std::endl;
}
```

### Common Abstraction Mistakes:
- Creating objects without proper OpenGL context
- Forgetting to call Bind() before using objects
- Not handling OpenGL errors consistently
- Mixing raw OpenGL calls with abstracted classes
- Improper resource cleanup ordering

---

