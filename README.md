# Branch: 11-basic-geometry-factories


### Core Concepts Learned:
- **Geometry Factory Pattern**: Systematic approach to primitive generation
- **Vertex Data Management**: Organised handling of position, normals, colour, and UV data
- **Normal Calculation**: Proper surface normal generation for lighting
- **Index Buffer Optimisation**: Efficient vertex reuse through indexed rendering
- **Parametric Surface Generation**: Mathematical construction of spheres using spherical coordinates
- **Primitive Generation**: Mathematical construction of basic and advanced 3D shapes
- **Memory Layout**: Structured vertex attribute organisation with normals
- **Reusable Patterns**: Building blocks for complex geometry systems
- **Interactive Visualisation**: Real-time geometry parameter adjustment with lighting
- **Code Organisation**: Clean separation of geometry logic from rendering

### Test Demonstrations:
- **testClearColour**: Fundamental framebuffer clearing and colour management
- **testGeometryFactories**: Interactive primitive generation and rendering with lighting
- **Previous Tests**: testTexture2D and testRayCasting from branch 10

### Technical Features:
- **Triangle Factory**: Basic 3-vertex primitive with colour gradients
- **Quad Factory**: Optimised 4-vertex primitive using index buffers
- **Cube Factory**: Complex 8-vertex primitive with 12 triangles and proper face normals
- **Sphere Factory**: Parametric surface generation with configurable subdivision
- **Fullscreen Quad Factory**: NDC coordinates for post-processing effects
- **Vertex Management**: Structured Vertex class with position, normals, colour, and UV data
- **Index Optimisation**: Shared vertices to reduce memory usage
- **Interactive Controls**: Real-time geometry switching and parameter adjustment
- **Educational Interface**: Detailed information about each primitive type
- **Basic Lighting**: Directional lighting using vertex normals

## Code Architecture

### File Structure
```
src/
├── Main.cpp       # Test harness application with geometry demos
├── Mesh/
│   ├── Mesh.h/.cpp          # Enhanced mesh class for geometry rendering
│   └── GeometryFactory.h/.cpp # Static factory methods for primitive generation
├── Shader.h/.cpp             # Shader compilation and uniform management
├── Renderer.h/.cpp           # High-level rendering commands
├── VertexArray.h/.cpp        # VAO wrapper
├── VertexBuffer.h/.cpp       # VBO wrapper
├── IndexBuffer.h/.cpp        # EBO wrapper
├── VertexBufferLayout.h      # Vertex attribute layout helper
├── tests/
│   ├── Tests.h/.cpp          # Base test framework classes
│   ├── testClearColour.h/.cpp # Basic framebuffer clearing demonstration
│   ├── testGeometryFactories.h/.cpp # Primitive generation and rendering
│   ├── testTexture2D.h/.cpp  # 2D texture mapping (from branch 10)
│   └── testRayCasting.h/.cpp # Ray casting algorithms (from branch 10)
└── vendor/
    └── imgui/                # Dear ImGui library for interactive GUI

Dependencies:
├── OpenGL 3.3+              # Graphics API
├── GLM Library              # Vector and matrix mathematics
├── GLFW                     # Window management
├── GLEW                     # OpenGL extension loading
└── Dear ImGui               # Interactive parameter controls
```

### Geometry Factory System

**GeometryFactory Class - Static Factory Methods:**
```cpp
class GeometryFactory {
public:
    // Primary factory methods
    static Mesh* CreateTriangle();                    // 3 vertices, 1 triangle
    static Mesh* CreateQuad();                        // 4 vertices, 2 triangles
    static Mesh* CreateCube();                        // 8 vertices, 12 triangles
    static Mesh* CreateSphere(int sectors = 20, int stacks = 20);  // Parametric sphere
    static Mesh* CreateFullscreenQuad();              // NDC coordinates for post-processing

    // Vertex data generation
    static std::vector<Vertex> GenerateTriangleVertices();
    static std::vector<Vertex> GenerateQuadVertices();
    static std::vector<Vertex> GenerateCubeVertices();
    static std::vector<Vertex> GenerateSphereVertices(int sectors = 20, int stacks = 20);
    static std::vector<Vertex> GenerateFullscreenQuadVertices();

    // Index buffer generation
    static std::vector<unsigned int> GenerateTriangleIndices();
    static std::vector<unsigned int> GenerateQuadIndices();
    static std::vector<unsigned int> GenerateCubeIndices();
    static std::vector<unsigned int> GenerateSphereIndices(int sectors = 20, int stacks = 20);
    static std::vector<unsigned int> GenerateFullscreenQuadIndices();

private:
    // Utility methods
    static void AssignUVCoordinates(std::vector<Vertex>& vertices, int faceVertexCount);
    static void AssignColours(std::vector<Vertex>& vertices);
    static void CalculateNormals(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
};
```

**Enhanced Vertex Structure:**
```cpp
struct Vertex {
    float position[3];   // 3D position coordinates
    float normal[3];     // Surface normal for lighting calculations
    float colour[3];     // RGB colour values
    float texCoords[2];  // UV texture coordinates

    Vertex(float px, float py, float pz, float nx, float ny, float nz, float r, float g, float b, float u, float v)
        : position{px, py, pz}, normal{nx, ny, nz}, colour{r, g, b}, texCoords{u, v} {}

    // Convenience constructor for backward compatibility (assumes normal pointing up)
    Vertex(float px, float py, float pz, float r, float g, float b, float u, float v)
        : position{px, py, pz}, normal{0.0f, 0.0f, 1.0f}, colour{r, g, b}, texCoords{u, v} {}
};
```



## Understanding the Code

### Triangle Factory Implementation

**Core Triangle Generation:**
```cpp
std::vector<Vertex> GeometryFactory::GenerateTriangleVertices() {
    std::vector<Vertex> vertices = {
        // Position          Normal           Colour          UV
        Vertex( 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.5f, 1.0f),  // Top - Red
        Vertex(-0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f),  // Bottom-left - Green
        Vertex( 0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f)   // Bottom-right - Blue
    };
    return vertices;
}

std::vector<unsigned int> GeometryFactory::GenerateTriangleIndices() {
    return { 0, 1, 2 };  // Simple sequential indexing
}
```

**Key Learning Points:**
- **Simplest primitive**: Foundation for all 3D graphics
- **Sequential indexing**: No vertex sharing required
- **Colour gradients**: Each vertex has unique colour for visual distinction
- **Normal vectors**: All pointing in +Z direction for consistent lighting
- **UV mapping**: Proper texture coordinate assignment

### Quad Factory Implementation

**Optimized Quad with Index Buffer:**
```cpp
std::vector<Vertex> GeometryFactory::GenerateQuadVertices() {
    std::vector<Vertex> vertices = {
        Vertex(-0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f),  // Bottom-left - Red
        Vertex( 0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f),  // Bottom-right - Green
        Vertex( 0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f),  // Top-right - Blue
        Vertex(-0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f)   // Top-left - Yellow
    };
    return vertices;
}

std::vector<unsigned int> GeometryFactory::GenerateQuadIndices() {
    return { 0, 1, 2, 2, 3, 0 };  // Two triangles sharing vertices
}
```

**Key Learning Points:**
- **Vertex reuse**: 4 vertices create 2 triangles (6 indices)
- **Index optimization**: Demonstrates shared vertex efficiency
- **Triangle strips**: Understanding how quads decompose to triangles
- **Memory efficiency**: Reduced vertex data through intelligent indexing

### Cube Factory Implementation

**Complex 3D Primitive Construction:**
```cpp
std::vector<Vertex> GeometryFactory::GenerateCubeVertices() {
    std::vector<Vertex> vertices = {
        // Front face (Z = +0.5)
        Vertex(-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f),  // Front-bottom-left
        Vertex( 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f),  // Front-bottom-right
        Vertex( 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f),  // Front-top-right
        Vertex(-0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f),  // Front-top-left

        // Back face (Z = -0.5) - Similar pattern with different Z coordinates
        // ... 4 more vertices for the back face
    };
    return vertices;
}

std::vector<unsigned int> GeometryFactory::GenerateCubeIndices() {
    return {
        // Front face (2 triangles)
        0, 1, 2,  2, 3, 0,
        // Back face (2 triangles)
        4, 5, 6,  6, 7, 4,
        // Left face (2 triangles)
        7, 3, 0,  0, 4, 7,
        // Right face (2 triangles)
        1, 5, 6,  6, 2, 1,
        // Bottom face (2 triangles)
        4, 0, 1,  1, 5, 4,
        // Top face (2 triangles)
        3, 7, 6,  6, 2, 3
    };
}
```

**Key Learning Points:**
- **3D geometry construction**: Systematic approach to complex primitives
- **Face organisation**: Each cube face is composed of 2 triangles
- **Vertex sharing**: 8 vertices create 12 triangles (36 indices)
- **Coordinate systems**: Understanding 3D position mapping
- **Index management**: Complex indexing patterns for face construction

### Sphere Factory Implementation

**Parametric Surface Generation:**
```cpp
std::vector<Vertex> GeometryFactory::GenerateSphereVertices(int sectors, int stacks) {
    std::vector<Vertex> vertices;
    float radius = 0.5f;
    const float PI = 3.14159265359f;

    for (int stack = 0; stack <= stacks; ++stack) {
        float phi = PI * (float(stack) / stacks); // from 0 to π
        float y = cos(phi);
        float sinPhi = sin(phi);

        for (int sector = 0; sector <= sectors; ++sector) {
            float theta = 2.0f * PI * (float(sector) / sectors); // from 0 to 2π
            float x = sinPhi * cos(theta);
            float z = sinPhi * sin(theta);

            // Position
            float posX = radius * x;
            float posY = radius * y;
            float posZ = radius * z;

            // Normal (for unit sphere, normal = normalised position)
            float normX = x, normY = y, normZ = z;

            // Colour based on position for visual variety
            float r = (x + 1.0f) * 0.5f;
            float g = (y + 1.0f) * 0.5f;
            float b = (z + 1.0f) * 0.5f;

            // UV coordinates
            float u = float(sector) / sectors;
            float v = float(stack) / stacks;

            vertices.emplace_back(posX, posY, posZ, normX, normY, normZ, r, g, b, u, v);
        }
    }
    return vertices;
}
```

**Key Learning Points:**
- **Parametric equations**: Mathematical sphere generation using spherical coordinates
- **Perfect normals**: Each vertex normal points outward from sphere center
- **UV mapping**: Proper texture coordinate calculation for spherical surfaces
- **Subdivision control**: sectors/stacks parameters control mesh resolution
- **Colour variation**: Position-based colouring for visual surface understanding

### Fullscreen Quad Factory Implementation

**NDC Coordinate Generation:**
```cpp
std::vector<Vertex> GeometryFactory::GenerateFullscreenQuadVertices() {
    std::vector<Vertex> vertices = {
        // NDC coordinates for fullscreen rendering
        Vertex(-1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f),  // Bottom-left
        Vertex( 1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f),  // Bottom-right
        Vertex(-1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f),  // Top-left
        Vertex( 1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f)   // Top-right
    };
    return vertices;
}

std::vector<unsigned int> GeometryFactory::GenerateFullscreenQuadIndices() {
    return { 0, 1, 2, 1, 3, 2 };
}
```

**Key Learning Points:**
- **NDC coordinates**: Range from -1 to 1 covers entire screen space
- **Post-processing foundation**: Essential for effects like blur, bloom, tone mapping
- **No transformation needed**: Bypasses MVP matrix calculations
- **Perfect UV mapping**: 0-1 range matches framebuffer coordinates
- **Efficient rendering**: Minimal vertex count for maximum screen coverage

### Interactive Test Implementation

**Real-time Geometry Switching:**
```cpp
void testGeometryFactories::UpdateMesh() {
    if (m_CurrentMesh) {
        delete m_CurrentMesh;
        m_CurrentMesh = nullptr;
    }

    switch (m_CurrentGeometry) {
        case TRIANGLE:
            m_CurrentMesh = GeometryFactory::CreateTriangle();
            break;
        case QUAD:
            m_CurrentMesh = GeometryFactory::CreateQuad();
            break;
        case CUBE:
            m_CurrentMesh = GeometryFactory::CreateCube();
            break;
        case SPHERE:
            m_CurrentMesh = GeometryFactory::CreateSphere(20, 20);
            break;
        case FULLSCREEN_QUAD:
            m_CurrentMesh = GeometryFactory::CreateFullscreenQuad();
            break;
    }
}
```

**Educational GUI Features:**
```cpp
void testGeometryFactories::RenderGUI() {
    // Geometry type selection
    const char* geometryTypes[] = { "Triangle", "Quad", "Cube", "Sphere", "Fullscreen Quad" };
    if (ImGui::Combo("Geometry Type", &currentItem, geometryTypes, 5)) {
        SwitchGeometry(static_cast<GeometryType>(currentItem));
    }

    // Rendering controls
    ImGui::Checkbox("Wireframe", &m_ShowWireframe);
    ImGui::SliderFloat("Rotation Speed", &m_RotationSpeed, 0.0f, 200.0f);
    ImGui::SliderFloat("Scale", &m_Scale, 0.1f, 3.0f);

    // Educational information
    ImGui::Text("Index Count: %u", m_CurrentMesh->GetIndexCount());

    switch (m_CurrentGeometry) {
        case TRIANGLE:
            ImGui::BulletText("3 vertices, 1 triangle (3 indices)");
            ImGui::BulletText("Fundamental primitive in 3D graphics");
            break;
        case QUAD:
            ImGui::BulletText("4 vertices, 2 triangles (6 indices)");
            ImGui::BulletText("Demonstrates index buffer optimisation");
            break;
        case CUBE:
            ImGui::BulletText("8 vertices, 12 triangles (36 indices)");
            ImGui::BulletText("Shows 3D geometry construction");
            break;
        case SPHERE:
            ImGui::BulletText("Parametric surface generation");
            ImGui::BulletText("Perfect normals for lighting calculations");
            break;
        case FULLSCREEN_QUAD:
            ImGui::BulletText("NDC coordinates (-1 to 1)");
            ImGui::BulletText("Used for post-processing effects");
            break;
    }
}
```



## Learning Experiments

### Basic Geometry Modifications:
1. **Triangle Variations**:
   ```cpp
   // Experiment with different triangle shapes
   std::vector<Vertex> vertices = {
       Vertex( 0.0f,  0.8f, 0.0f,  1.0f, 0.0f, 0.0f,  0.5f, 1.0f),  // Taller triangle
       Vertex(-0.8f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f),  // Wider base
       Vertex( 0.8f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f)
   };
   ```

2. **Quad Tessellation**:
   ```cpp
   // Create subdivided quad with more triangles
   static Mesh* CreateSubdividedQuad(int subdivisions);
   // Generate (subdivisions+1)² vertices
   // Create subdivisions² quads, each made of 2 triangles
   ```

3. **Colour Pattern Experiments**:
   ```cpp
   // Try different colour assignment patterns
   void GeometryFactory::AssignColors(std::vector<Vertex>& vertices) {
       for (size_t i = 0; i < vertices.size(); ++i) {
           // Rainbow pattern
           float hue = (float)i / vertices.size() * 360.0f;
           // Convert HSV to RGB for smooth color transitions
       }
   }
   ```

### Advanced Geometry Challenges:
1. **Add New Primitives**:
   ```cpp
   // Extend GeometryFactory with new shapes
   static Mesh* CreatePyramid();        // 5 vertices, 8 triangles
   static Mesh* CreateOctahedron();     // 6 vertices, 8 triangles
   static Mesh* CreateIcosahedron();    // 12 vertices, 20 triangles
   ```

2. **Parametric Generation**:
   ```cpp
   // Add parameters to primitive generation
   static Mesh* CreateQuad(float width, float height);
   static Mesh* CreateCube(float width, float height, float depth);
   static Mesh* CreateTriangle(float base, float height);
   ```

3. **UV Coordinate Experiments**:
   ```cpp
   // Modify UV mapping for different texture effects
   void AssignUVCoordinates(std::vector<Vertex>& vertices, UVMode mode) {
       switch (mode) {
           case UV_STANDARD:  // 0-1 mapping
           case UV_REPEAT:    // 0-2 mapping (texture repeats)
           case UV_CENTERED:  // -0.5 to 0.5 mapping
       }
   }
   ```

### Interactive Enhancements:
1. **Animation Systems**:
   ```cpp
   // Add vertex animation to testGeometryFactories
   void UpdateVertexPositions(float time) {
       // Animate vertex positions for morphing effects
       // Try sine waves, rotation, scaling animations
   }
   ```

2. **Multi-geometry Rendering**:
   ```cpp
   // Render multiple primitives simultaneously
   std::vector<std::unique_ptr<Mesh>> m_Meshes;
   std::vector<GeometryType> m_Types;
   // Display all primitive types in a grid layout
   ```

3. **Performance Monitoring**:
   ```cpp
   // Add performance metrics to GUI
   ImGui::Text("Vertices: %d", vertexCount);
   ImGui::Text("Triangles: %d", triangleCount);
   ImGui::Text("Draw Calls: %d", drawCallCount);
   ImGui::Text("GPU Memory: %.2f MB", memoryUsage);
   ```

## Implementation Details

### Memory Management
The GeometryFactory system emphasizes proper memory management:

```cpp
// Factory returns raw pointers for direct OpenGL resource management
Mesh* GeometryFactory::CreateTriangle() {
    std::vector<Vertex> vertices = GenerateTriangleVertices();
    std::vector<unsigned int> indices = GenerateTriangleIndices();
    return new Mesh(vertices, indices);  // Caller responsible for deletion
}

// Test class properly manages mesh lifecycle
testGeometryFactories::~testGeometryFactories() {
    delete m_CurrentMesh;  // Clean resource cleanup
    delete m_Shader;
    delete m_Renderer;
}
```

### Vertex Layout Optimization
The Vertex structure is designed for optimal GPU performance:

```cpp
struct Vertex {
    float position[3];   // 12 bytes, aligned to 4-byte boundary
    float colour[3];     // 12 bytes, aligned to 4-byte boundary
    float texCoords[2];  // 8 bytes, aligned to 4-byte boundary
    // Total: 32 bytes per vertex (cache-friendly size)
};
```

### Index Buffer Efficiency
Different primitives demonstrate various levels of vertex reuse:

```cpp
// Triangle: No vertex sharing (efficiency ratio: 1.0)
// 3 vertices, 3 indices = 100% unique vertices

// Quad: Moderate vertex sharing (efficiency ratio: 0.67)
// 4 vertices, 6 indices = 67% unique vertices

// Cube: High vertex sharing (efficiency ratio: 0.22)
// 8 vertices, 36 indices = 22% unique vertices
```


## Resources for Deeper Learning

### Geometry and Mathematics:
- [Real-Time Rendering by Möller, Haines & Hoffman](https://www.realtimerendering.com/) - Chapter 12: Acceleration Algorithms
- [3D Math Primer for Graphics and Game Development](https://gamemath.com/) - Geometry representation
- [Mathematics for 3D Game Programming and Computer Graphics](https://www.amazon.com/Mathematics-Programming-Computer-Graphics-Third/dp/1435458869)

### OpenGL and Vertex Data:
- [LearnOpenGL - Hello Triangle](https://learnopengl.com/Getting-started/Hello-Triangle) - Fundamental vertex concepts
- [OpenGL Programming Guide](https://www.opengl.org/documentation/books/) - Chapter 2: State Management and Drawing
- [OpenGL Vertex Specification](https://www.khronos.org/opengl/wiki/Vertex_Specification)

### Design Patterns and Code Organization:
- [Game Programming Patterns by Robert Nystrom](http://gameprogrammingpatterns.com/) - Factory and Object Pool patterns
- [Effective C++ by Scott Meyers](https://www.aristeia.com/books.html) - Resource management
- [Clean Code by Robert Martin](https://www.amazon.com/Clean-Code-Handbook-Software-Craftsmanship/dp/0132350884)

### Interactive Learning:
- [GeoGebra 3D](https://www.geogebra.org/3d) - Visualize 3D geometry concepts
- [Desmos 3D Calculator](https://www.desmos.com/3d) - Explore parametric equations
- [Shadertoy](https://www.shadertoy.com/) - Experiment with procedural geometry

## Debug Tips

### Debugging Vertex Data:
```cpp
// Print vertex information for validation
void PrintVertices(const std::vector<Vertex>& vertices) {
    for (size_t i = 0; i < vertices.size(); ++i) {
        std::cout << "Vertex " << i << ": ";
        std::cout << "Pos(" << vertices[i].position[0] << ", "
                  << vertices[i].position[1] << ", " << vertices[i].position[2] << ") ";
        std::cout << "Color(" << vertices[i].colour[0] << ", "
                  << vertices[i].colour[1] << ", " << vertices[i].colour[2] << ") ";
        std::cout << "UV(" << vertices[i].texCoords[0] << ", "
                  << vertices[i].texCoords[1] << ")" << std::endl;
    }
}
```

### Debugging Index Buffers:
```cpp
// Validate index buffer correctness
void ValidateIndices(const std::vector<unsigned int>& indices, size_t vertexCount) {
    for (size_t i = 0; i < indices.size(); ++i) {
        if (indices[i] >= vertexCount) {
            std::cout << "Invalid index at position " << i << ": " << indices[i]
                      << " (vertex count: " << vertexCount << ")" << std::endl;
        }
    }

    // Check for proper triangle winding
    for (size_t i = 0; i < indices.size(); i += 3) {
        std::cout << "Triangle " << (i/3) << ": "
                  << indices[i] << ", " << indices[i+1] << ", " << indices[i+2] << std::endl;
    }
}
```

### OpenGL State Debugging:
```cpp
// Check OpenGL errors after geometry operations
void CheckGLError(const std::string& operation) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "OpenGL Error in " << operation << ": " << error << std::endl;
    }
}

// Usage
CheckGLError("Vertex Buffer Creation");
CheckGLError("Index Buffer Creation");
CheckGLError("Vertex Array Setup");
```
