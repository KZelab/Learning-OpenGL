# ACCA6021 Graphics and Games Programming Labs

| Lab | Title | File |
|-----|-------|------|
| 00 | Environment Setup | lab_00_environment_setup.html |
| 01 | Window and Context | lab_01_window_context.html |
| 02 | First Triangle | lab_02_first_triangle.html |
| 03 | Shaders | lab_03_shaders.html |
| 03B | The OpenGL Pipeline | lab_GP01_opengl_pipeline.html |
| 04 | Two Triangles | lab_04_two_triangles.html |
| 05 | Index Buffers and Uniforms | lab_05_index_buffers_uniforms.html |
| 06 | Textures | lab_06_textures.html |
| WS-REF | Matrix and Trig Reference Card | math_WSREF_reference_card.html |
| WS-M01 | Coordinate Spaces and the Pipeline | math_WS01_coordinate_spaces.html |
| WS-M02 | The Identity Matrix | math_WS02_identity_matrix.html |
| WS-M03 | Homogeneous Coordinates and Transformation Matrices | math_WS03_homogeneous_and_matrices.html |
| WS-M04 | Column-Major Convention and GLM | math_WS04_column_major_and_glm.html |
| 07 | Transformations | lab_07_transformations.html |
| WS-M05 | The Look-At Matrix | math_WS05_lookat_matrix.html |
| WS-M06 | Perspective Projection and NDC | math_WS06_perspective_and_ndc.html |
| 08 | Projections | lab_08_projections.html |
| 09 | GlCall Error System | lab_09_glcall_error_system.html |
| 10 | Vertex and Index Buffers | lab_10_vertex_index_buffers.html |
| 11 | Vertex Layout and Array | lab_11_vertex_layout_array.html |
| 12 | Shader Class | lab_12_shader_class.html |
| 13 | Texture Class | lab_13_texture_class.html |
| 14 | Renderer and Refactoring | lab_14_renderer_refactoring.html |
| 15 | Test Harness and C++ Architecture | lab_15_test_harness.html |
| 16 | ClearColour Test and ImGui | lab_16_clearcolour_test_and_imgui.html |
| 17 | RayCasting | lab_17_raycasting.html |
| 18 | Texture2D | lab_18_texture2d.html |
| 19 | Camera - WASD Movement and Mouse Look | lab_19_camera.html |
| 20 | Maze Layout and First-Person View | lab_20_maze_layout_first_person_view.html |
| 21 | AABB Player-Wall Collision | lab_21_aabb_player_wall_collision.html |
| 22 | Collectibles, Timer, and Win Condition | lab_22_collectibles_timer_win_condition.html |
| 23 | Exit Door, Polish, and Complete Game | lab_23_exit_door_polish_complete_game.html |

## Moodle upload instructions

1. In your Moodle course, add a File resource for each lab.
2. Upload the HTML file. Set Display to "In frame" or "Open" (not Force download).
3. Each file is self-contained - the solution is embedded and revealed on demand.
   Encourage students to attempt the lab before clicking "Show Solution".

## Moodle section order

Suggested grouping for Moodle course sections:

- **Section 1 - Getting Started:** Labs 00, 03B (pipeline overview), 01, 02, 03
- **Section 2 - Geometry and Data:** Labs 04, 05, 06
- **Section 3 - Math Foundation (before Lab 07):** WS-REF, WS-M01, WS-M02, WS-M03, WS-M04
- **Section 4 - Transformations and Projections:** Lab 07, WS-M05, WS-M06, Lab 08
- **Section 5 - Code Architecture:** Labs 09, 10, 11, 12, 13, 14
- **Section 6 - Test Framework and UI:** Labs 15, 16
- **Section 7 - Interactive Tests:** Labs 17, 18, 19
- **Section 8 - Maze Game Project:** Labs 20, 21, 22, 23


# exam topics 
Question One coverage — Lighting and shading

Local illumination model: ambient, diffuse, specular components and what each represents physically
Lambertian diffuse reflection and the N·L cosine term, including why it must be clamped to zero
The Phong Illumination model in full (ambient + diffuse + specular, including the specular exponent/shininess term)
Difference between Phong Illumination (the lighting model) and Phong Shading (the per-pixel interpolation technique), since students often conflate the two
Gouraud shading vs Flat shading vs per-pixel (Phong) shading, and the cost/quality trade-offs between them
Why interpolated vertex-lit shading loses specular highlights between vertices
Reading and manipulating a lighting equation term by term: being able to identify what happens if a term is missing, unclamped, or misapplied
Extending a single-light equation to a multi-light scene (summing per-light contributions, keeping ambient outside the sum)
Vector maths refresher: normals, light vectors, view vectors, dot products, unit vectors

Question Two coverage — Shadows

Shadow mapping pipeline: depth-from-light render pass, depth comparison in the main pass
Shadow map resolution and quantisation as the root cause of shadow acne
Depth bias / shadow bias as a mitigation, and the peter-panning trade-off from over-biasing
Percentage-Closer Filtering (PCF) and how multi-sample filtering softens shadow edges
Normal offset bias as a complement to depth bias
Variance Shadow Maps (VSM) as an alternative approach, including light bleeding as a weakness
Why real-time constraints favour cheap combined approaches (bias + PCF) over statistically elegant but costlier options (VSM)
Temporal shadow artefacts: shimmering/crawling edges caused by shadow map re-projection each frame
The link between camera/light movement and visible shadow instability

Worth explicitly contrasting this with the aliasing content from the previous paper (spatial vs temporal aliasing) if that's still being taught elsewhere in the module, since the underlying "sampling causes artefacts" concept is the same idea applied to a different context, and that transfer is worth surfacing to students directly.
Question Three coverage — Run-time performance

The game loop pattern generally, and why naive "update once per rendered frame" loops cause frame-rate-dependent simulation bugs
Fixed timestep vs variable timestep updates
The accumulator pattern for decoupling simulation rate from render rate
The "spiral of death" failure mode and why a maximum frame time clamp is needed
Interpolation/extrapolation between simulation steps for smooth rendering at a decoupled render rate
Draw call overhead and why batching/instancing matters at scale
GPU instancing for rendering large numbers of similar objects (particles) in a single draw call
Compute shaders / GPU-side simulation as an alternative to CPU-side particle updates
Spatial partitioning (grids, octrees) for reducing neighbour-comparison cost in CPU-side simulations
CPU-GPU data transfer and synchronisation costs, and why minimising buffer updates matters
Level of detail (LOD), frustum culling, and reduced update frequency as scene-management techniques
General profiling literacy: recognising that a frame-rate drop could originate in simulation, rendering, or data transfer, and that diagnosis should precede optimisation