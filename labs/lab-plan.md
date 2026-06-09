# UWTSD OpenGL Lab Plan
Module: ACCA6021 - Graphics and Games Programming

---

| # | Title | Outcome | Source System | Depends on |
|---|-------|---------|---------------|------------|
| 00 | Environment Setup | Project compiles; blank console window opens | vcpkg, VS, GLFW, GLEW, GLM, ImGui | — |
| 01 | Window and Context | 800×600 GLFW window with clear colour at 60 fps | GLFW window creation, main loop | Lab 00 |
| 02 | First Triangle | White triangle rendered in the centre of the window | VAO, VBO, `glDrawArrays` | Lab 01 |
| 03 | Shaders | Triangle renders in a GLSL-defined colour | Vertex/fragment shaders, custom `.shader` format | Lab 02 |
| 04 | Two Triangles | Quad built from two explicit triangles; 6 vertices, no index buffer | VAO, VBO, `glDrawArrays` | Lab 03 |
| 05 | Index Buffers and Uniforms | Quad rendered via IBO with 4 vertices; colour driven by a `uniform vec4` set from C++ | `glDrawElements`, `GL_ELEMENT_ARRAY_BUFFER`, `glGetUniformLocation`, `glUniform4f` | Lab 04 |
|    | _Note: lecture covers shader/uniform theory before this lab. Lab is the practical follow-up._ | | | |
| 06 | Textures | Textured quad displays a loaded image | stb_image, UV coords, `sampler2D` uniform | Lab 05 |
| 07 | Transformations | Textured quad rotates continuously using GLM matrices | GLM translate/rotate/scale, `glUniformMatrix4fv`, MVP | Lab 06 |
| 08 | Projections | Orthographic and perspective cameras toggled at runtime | GLM ortho/perspective, console or key toggle | Lab 07 |
|    | Add Projections, camera controls   Ortho vs Perspective - use console input to toggle it? | | | |
|    | So far we had been working exclusively in `src/main.cpp`, but as the codebase grows, 
we will start to split it into multiple files and classes to improve organization and maintainability. 
The next few labs will focus on refactoring and abstracting our code | | | |
| 07 | Abstracting |Take the existing codebase and abstract each buffer out into their own files with detailed explanations (Renderer, IBO, VBO, VAO, Texture, Shader) |   | Lab 08 |
| 08 |  Index Buffers | `src/IndexBuffer.h` | Lab 07 |
| 09 | Shaders | Shaders load from `.shader` files; uniforms set from C++ with debugging output (e.g., compile errors) | `src/Shader.h/.cpp`, `#shader vertex/fragment` parser | Lab 03 |
| 10 | Textures | `src/Texture.h` Textured quad displays a loaded image  and a default texture to prevent crashes  | stb_image, UV coords | Lab 04 |
| 11 | ImGui Integration | Preparing for the Test Harness menu | | Lab 10 |
| 12 | Test Harness & C++ Architecture | A working test scene registered in the menu; student understands the factory pattern and can add their own scenes | `src/tests/Tests.h/.cpp` — polymorphism, factory pattern, variadic templates, perfect forwarding, `std::apply` | Lab 08 |
| 12 | Test template | A blank Test template as a starting point for all future tests 
| 13 | Camera | WASD movement and mouse look using the Camera class | `src/utils/Camera.h/.cpp`, delta time | Lab 06 |


Abstract into separate classes (Renderer, IBO, VBO, VAO, Texture, Shader)
Build a test harness and implement dear imgui  
ClearColour test 
RayCasting │ Mouse picking  
Texture2D  │ Texturing + MVP 



Module: ACCB6014 - GPU Programming
At this point, the first module will conclude and the second module will begin (ACCB6014), building on the foundation laid in the first half of the course. 
The following labs will introduce more advanced graphics techniques and concepts:

| # | Title | Outcome | Source System | Depends on |
|---|-------|---------|---------------|------------|
| 1 | Lighting (Phong) | Lit scene with point light; specular highlight visible | `src/tests/TestLightingShader`, normal buffer | Lab 09 |
| 1 | Lighting (Gourand) 
| 1 | Lighting (Flat) 
| 1 | Lighting (Blinn Phong) 


| 3 | Framebuffers & Post-Processing | Scene renders off-screen; greyscale effect applied | `src/Framebuffer.h/.cpp`, `src/tests/testEffects` | Lab 11 |
| 4 | Shadow Mapping | Directional light casts a shadow onto the floor | `src/tests/TestShadowMapping`, depth FBO | Lab 12 |
| 5 | Model Loading (Assimp) | Textured backpack.obj renders with all materials | `src/Mesh/Model.h/.cpp`, `src/Mesh/ModelMesh.h/.cpp` | Lab 08 |
| 6 | Skybox | Procedural gradient skybox surrounds the scene | `src/Skybox.h/.cpp`, `res/Shaders/Skybox.shader` | Lab 14 |
| 7 | PBR Shading | Sphere grid shows full metallic/roughness space | `src/tests/TestPBR.h/.cpp`, Cook-Torrance BRDF | Lab 10 |
| 8 | Normal Mapping & TBN | Backpack model shows surface detail under moving light | `src/tests/TestPBRNormalMap.h/.cpp`, TBN matrix | Lab 16 |
| 9 | MSAA & Multisampling | Geometry edges smoothed; toggle-able sample count | `src/MSAAFramebuffer.h/.cpp`, `src/tests/TestMSAA` | Lab 12 |
| 10 | CPU Particle System | Particle fountain emits, fades, and respects gravity | `src/tests/TestParticleSystem.h/.cpp` | Lab 11 |
| 11 | Compute Shaders & GPU Particles | Millions of particles updated entirely on the GPU | `src/ComputeShader.h/.cpp`, `src/tests/TestGPUParticles` | Lab 19 |



Camera │ FPS controls (introduces the  mesh/geometry factory / utils camera)
Effects │ Post-processing 
ProceduralArt │ Shader art (5 patterns) 
LightingShader │ Phong/Flat/Gouraud/Blinn-Phong comparison 
MultipleLightSources │ Point/Dir/Spot lights
PBR │ Cook-Torrance BRDF 
ParticleSystem │ CPU particles | GPU Particles (Introduces computeshader)
Shadow Mapping (Introduces Framebuffer)
HighDensityMesh │ Model loading + perf 
Batching │ Performance

 
1. Skybox / Cubemap — Environment rendering using cubemap textures. Sets up for IBL (image-based lighting) which is the natural PBR companion.  https://opengameart.org/content/miramar-skybox
2. Instanced Rendering — so far we have batching for identical geometry; instancing is the GPU-native next step. Great performance comparison against existing batch test.  
3. Stencil Buffer — Outlining selected objects, mirrors, or portals. Pairs well with ray casting test (select an object, outline it).  
4. Framebuffer / Render-to-Texture — the effects test hints at this, but a dedicated FBO demo (e.g., security camera / minimap) would solidify the concept.  
5. Transparency & Alpha Blending — Order-dependent rendering, depth sorting. The particle system does some of this but a dedicated test with overlapping translucent  
objects would be more educational.
6. perlin noise procedural world generation https://www.reddit.com/r/opengl/comments/1rm7sm0/my_procedural_opengl_terrain_in_4k/
7. perlin noise water generation
8. Tessellation shaders  https://www.youtube.com/watch?v=7ypCmEHV0pU https://learnopengl.com/Guest-Articles/2021/Tessellation/Tessellation
1. Voxel based world generation
Wave function collapse algorithm for procedural generation 
fluid simulation (SPH or grid-based)
1. GPU-based culling (frustum, occlusion) for high-density scenes
1. Skeletal animation with Assimp bone weights and animation data
1. Animation blending and state machines