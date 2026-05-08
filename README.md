# Branch: 09-test-harness

> **Installing ImGui:** `vcpkg install imgui[glfw-binding,opengl3-binding]`



### Core Concepts Learned:
- **Interactive Testing**: Building GUI-based tools for experimenting with graphics concepts
- **Test Framework Architecture**: Modular system for registering and running different tests
- **ImGui Integration**: Adding immediate mode GUI to OpenGL applications
- **Concept Demonstration**: Using interactive tests to validate and showcase functionality
- **Modular Test Design**: Creating focused tests that demonstrate specific features
- **Real-Time Parameter Adjustment**: Using sliders and controls to modify rendering in real-time
- **Educational Tool Development**: Building applications that teach through interaction
- **Graphics Debugging**: Using visual tests to identify and solve rendering issues

### Test Framework Components:
- **Test Base Class**: Abstract interface for all test implementations
- **Test Menu System**: Registration and navigation between different tests
- **ImGui Controls**: Interactive widgets for real-time parameter adjustment
- **Shader Testing**: Interactive demonstration of shader compilation and uniform handling
- **Mesh Testing**: Visual validation of geometry creation and factory methods
- **Renderer Testing**: OpenGL state management and rendering pipeline validation

### Technical Implementation:
- **Test Registration**: Template-based system for adding new tests
- **Resource Management**: Proper cleanup of test-specific OpenGL resources
- **GUI Layout**: Organised ImGui interfaces for each test type
- **Real-Time Updates**: Delta time-based animations and parameter changes

## Code Architecture

### File Structure
```
src/
├── Main.cpp                  # Test harness main application with ImGui integration
├── Shader.h/.cpp             # Shader compilation and uniform management
├── Mesh.h/.cpp               # Vertex data and geometry abstraction
├── Renderer.h/.cpp           # High-level rendering commands
├── VertexArray.h/.cpp        # VAO wrapper (from main branch)
├── VertexBuffer.h/.cpp       # VBO wrapper (from main branch)
├── IndexBuffer.h/.cpp        # EBO wrapper (from main branch)
├── VertexBufferLayout.h      # Vertex attribute layout helper
├── tests/
│   ├── Tests.h/.cpp          # Base test framework classes
│   ├── TestShader.h/.cpp     # Interactive shader feature demonstration
│   ├── TestMesh.h/.cpp       # Interactive mesh feature demonstration
│   ├── TestRenderer.h/.cpp   # Interactive renderer feature demonstration
│   ├── TestLightingShader.h/.cpp  # Advanced lighting demonstrations (from main)
│   └── testEffects.h/.cpp    # Visual effects demonstrations (from main)
└── vendor/
    └── imgui/                # Dear ImGui library for interactive GUI
        ├── imgui.h/.cpp
        ├── imgui_impl_glfw.h/.cpp
        ├── imgui_impl_opengl3.h/.cpp
        └── ... (additional ImGui files)

Dependencies:
├── OpenGL 3.3+              # Graphics API
├── GLM Library              # Mathematics
├── GLFW                     # Window management
├── GLEW                     # OpenGL extension loading
└── Dear ImGui               # Immediate mode GUI
```

### Test Framework Architecture

The test harness follows a modular, extensible design:

**Main Application (Main.cpp):**
- Initialises GLFW, GLEW, and ImGui systems
- Creates and manages the test menu system
- Provides main rendering loop with delta time calculation
- Handles test navigation and GUI integration

**Test Base Classes (Tests.h/cpp):**
- Abstract Tests class defining common interface (Update, Render, RenderGUI)
- TestMenu class managing test registration and selection
- Template-based registration system for type-safe test creation

**Individual Test Classes:**
- TestShader: Demonstrates shader compilation, uniform setting, and multiple shader programs
- TestMesh: Shows mesh creation, factory methods, and geometry management
- TestRenderer: Validates OpenGL state management and rendering pipeline

**ImGui Integration:**
- Real-time parameter adjustment through sliders, colour pickers, and checkboxes
- Interactive test selection via button-based menu system
- Organised GUI panels with feature descriptions and controls

### Test Registration System

**Registration Process:**
```cpp
// In main application
TestMenu->RegisterTest<TestShader>("Shader Abstraction", window);
TestMenu->RegisterTest<TestMesh>("Mesh Abstraction", window);
TestMenu->RegisterTest<TestRenderer>("Renderer Abstraction", window);
```

**Template-Based Creation:**
- Type-safe test instantiation with constructor parameters
- Automatic memory management and cleanup
- Extensible design for adding new test types



## Understanding the Code

### Test Framework Implementation

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