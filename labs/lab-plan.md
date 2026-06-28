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
| 09 | GlCall Error System | Create `src/Renderer.h` with ASSERT and GlCall macros; implement glClearError and glLogCall so every OpenGL error is caught at the exact call site | `src/Renderer.h/.cpp` | Lab 08 |
| 10 | Vertex and Index Buffers | Wrap VBO and IBO in RAII classes; implement glGenBuffers, glBindBuffer, glBufferData, and glDeleteBuffers for each | `src/VertexBuffer.h/.cpp`, `src/IndexBuffer.h/.cpp` | Lab 09 |
| 11 | Vertex Layout and Array | VertexBufferLayout describes vertex format (stride, offset, Push templates); VAO records the layout with glVertexAttribPointer | `src/VertexBufferLayout.h`, `src/VertexArray.h/.cpp` | Lab 10 |
| 12 | Shader Class | Shader wraps the #shader parser, glCreateShader/glLinkProgram pipeline, and a uniform location cache | `src/Shader.h/.cpp` | Lab 11 |
| 13 | Texture Class | Texture wraps stb_image loading, glTexImage2D upload, and texture parameter setup | `src/Texture.h/.cpp` | Lab 12 |
| 14 | Renderer and Refactoring | Complete Renderer::Draw; refactor the Lab 08 monolithic main.cpp using all six abstraction classes; RAII eliminates the manual cleanup block | `src/Renderer.h/.cpp`, `src/main.cpp` | Lab 13 |
| 15 | Test Harness and C++ Architecture | A working test scene registered in the menu; student understands the factory pattern and can add their own scenes | `src/tests/Tests.h/.cpp` - polymorphism, factory pattern, variadic templates | Lab 14 |
| 15 | Test Template | A blank Test template as a starting point for all future tests | `src/tests/Tests.h` base class | Lab 15 |
| 16 | ClearColour Test and ImGui | First real test scene; ImGui colour picker controls the clear colour - introduces Dear ImGui at the point where it is first useful | `src/tests/testClearColour.h/.cpp`, ImGui widgets | Lab 16 |
| 17 | RayCasting | Mouse picking | `src/tests/testRayCasting.h/.cpp` as reference | Lab 17 |
| 18 | Texture2D | Texturing and MVP | `src/tests/testTexture2D.h/.cpp` | Lab 18 |
| 19 | Camera | WASD movement and mouse look using the Camera class | `src/utils/Camera.h/.cpp`, delta time | Lab 19 |
| 20 | Maze Layout and First-Person View | First-person 3D maze navigable with WASD+mouse; walls rendered from a 2D tile grid | `src/tests/maze/TestMaze.h/.cpp` (new) | Lab 20 |
| 21 | AABB Player-Wall Collision | Player cannot pass through walls; movement slides along surfaces | `src/tests/maze/TestMaze.h/.cpp` | Lab 21 |
| 22 | Collectibles, Timer, and Win Condition | Collect all coins before the timer expires; game has Playing/Won/Lost states | `src/tests/maze/TestMaze.h/.cpp` | Lab 22 |
| 23 | Exit Door, Polish, and Complete Game | Key unlocks exit door (RayCasting interaction); two levels; full HUD and state machine | `src/tests/maze/TestMaze.h/.cpp`, ray math from `testRayCasting.cpp` | Lab 23 |







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


Geometry factory 
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

---

## Math Foundation Worksheet Series - ACCA6021
Generated: 2026-06-27

Supplementary worksheets delivered alongside the code lab series.
Module: ACCA6021 - Graphics and Games Programming
Output folder: labs/mcp/ with filename prefix math_ (e.g. math_WS01_coordinate_spaces.html)
Purpose: build correct intuition and procedural confidence before and during the transformation-heavy labs. Not a maths module - no formal proofs. Emphasis on high-failure-risk concepts.

### Global generation rules (apply to every worksheet in this series)

TONE: Audience is second-year games/graphics students who know vector maths but have not written a
renderer before. Write at the level of a confident, approachable tutor. Intuition before mechanics,
mechanics before code. Never write "it can be shown that" or "proof is left as an exercise."

MATH NOTATION IN HTML: Do not use MathJax or any external math library.
- Inline scalars: plain text or <em> for variables, e.g. <em>theta</em>, <em>w</em>
- Inline vectors: bold name with components in parentheses, e.g. <strong>v</strong> = (x, y, z)
- Column vectors and matrices: render in <pre> blocks using plain ASCII box art (see examples)
- Superscripts: <sup> tag, e.g. M<sup>T</sup> for transpose, M<sup>-1</sup> for inverse
- Subscripts: <sub> tag, e.g. v<sub>x</sub>

Column vector example in HTML:
  <pre>| x |
| y |
| z |
| w |</pre>

4x4 matrix example in HTML:
  <pre>| 1  0  0  tx |
| 0  1  0  ty |
| 0  0  1  tz |
| 0  0  0   1 |</pre>

SOLUTION FIELD: For math worksheets, solution_code holds a plain-text answer key, not compilable
source. Write clearly labelled numbered answers corresponding to skeleton blanks or conceptual
questions. Still rendered on a copy-resistant canvas behind the "Show Solution" button.

LANGUAGES FIELD: ["cpp"] if any GLM code appears. [] if no code.

CROSS-LAB REFERENCES: Always say "the Transformations lab (Lab 07)" not just "Lab 07".

NO-CODE WORKSHEETS: WS-M01 and WS-REF contain no code at all. Their tasks have code_type omitted
and no code field. steps_html contains only prose, tables, and pre-formatted math blocks.

| ID     | Title                                               | Deliver before                 | Tasks | Languages |
|--------|-----------------------------------------------------|--------------------------------|-------|-----------|
| WS-M01 | Coordinate Spaces and the Pipeline                  | Lab 07 (Transformations)       | 2     | none      |
| WS-M02 | The Identity Matrix                                 | Lab 07 (Transformations)       | 4     | cpp       |
| WS-M03 | Homogeneous Coordinates and Transformation Matrices | Lab 07 (Transformations)       | 3     | cpp       |
| WS-M04 | Column-Major Convention and GLM                     | Lab 07 (Transformations)       | 3     | cpp       |
| WS-M05 | The Look-At Matrix                                  | Lab 08/19 (Projections/Camera) | 3     | cpp       |
| WS-M06 | Perspective Projection and NDC                      | Lab 08 (Projections)           | 3     | cpp       |
| WS-M07 | Normal Vector Transformation                        | ACCB6014 lighting labs         | 4     | cpp       |
| WS-REF | Matrix and Trig Compendium                          | With WS-M03                    | 1     | none      |

---

### WS-M01 - Coordinate Spaces and the Pipeline - done

lab_number: "M01"
lab_title: "Coordinate Spaces and the Transformation Pipeline"
output_path: labs/mcp/math_WS01_coordinate_spaces.html

Outcome: Student can name all five coordinate spaces, describe what each represents geometrically,
and explain in one sentence why the pipeline separates modelling, camera, and projection concerns.

Student prior knowledge at this point:
- Completed Labs 00-06 (environment through textures)
- Has seen gl_Position = projection * view * model * position in a vertex shader
- Knows matrix multiplication mechanically but has not thought about what the matrices mean
- Has NOT derived any of these matrices yet

Concept section narrative (write in this order):
1. Problem: a 3D model defined at the origin needs to appear at a specific place on screen. Multiple
   steps are needed and they cannot be collapsed into one because modelling, camera, and projection
   serve different purposes and are controlled by different parts of the program.
2. Mechanism: the pipeline is a sequence of coordinate system changes. Each matrix moves vertices
   from one frame of reference to another. Name and describe all five spaces: Object Space (local
   to the mesh, origin at mesh centre by convention), World Space (shared scene frame, all objects
   coexist here), View Space (camera at origin looking down -Z, simplifies projection maths),
   Clip Space (after projection matrix, w carries depth, hardware clips here), NDC (after dividing
   by w, all axes in [-1,1], what the rasteriser uses).
3. API: in the vertex shader this is gl_Position = projection * view * model * position.
   Multiplication is right-to-left: model is applied first, projection last.

Tasks:

Task 1 (Reference) - "Tracing a Vertex Through the Pipeline"
  Trace the vertex p_obj = (1, 0, 0, 1) (right face of a unit cube) through all five spaces.
  Use these matrices (state them, do not derive them - derivation comes in WS-M03 through WS-M06):
  - Model M: translate by (3, 0, 0). Result: p_world = (4, 0, 0, 1).
  - View V: camera at (0,0,5) looking at origin. Result: p_view = (4, 0, -5, 1).
    Explain geometrically: the camera moving to z=5 is equivalent to the world shifting -5 on Z.
  - Projection P: 90 deg FoV, square viewport, near=1, far=10. Do NOT compute numerically here.
    Describe qualitatively: p_clip has w_clip = -z_view = 5. x and y are scaled by the focal
    length. z_clip encodes a nonlinear depth value.
  - NDC: divide clip vector by w_clip. x, y, z all land in [-1, 1] for visible geometry.
  For each step write a short paragraph explaining what changed geometrically.
  Include an ASCII pipeline diagram in a <pre> block:
    [Object Space] --model matrix--> [World Space] --view matrix--> [View Space]
        --projection matrix--> [Clip Space] --perspective divide--> [NDC]

Task 2 (Skeleton) - "Label the Pipeline"
  Present the ASCII pipeline diagram with five space names replaced by blanks and three matrix
  arrow labels replaced by blanks. Then provide five sentence-starters, one per space:
    "Object Space is the coordinate system where ___."
    "World Space is the shared frame where ___."
    "View Space places the ___ at the origin, looking down ___."
    "Clip Space exists after the ___ matrix. The w component encodes ___."
    "NDC is produced by ___. All visible geometry fits within ___."
  Solution (answer key for solution_code):
    Object Space: local to the mesh; origin is at the mesh's own centre by convention.
    World Space: all objects coexist here after their model matrix is applied.
    View Space: camera sits at the origin looking down -Z; simplifies the projection maths.
    Clip Space: produced by the projection matrix; w encodes depth; hardware clips here.
    NDC: produced by dividing clip coords by w; all visible geometry in the cube [-1,1]^3.

Generation notes:
- No code in this worksheet. steps_html uses only prose, <pre> blocks, and ordered lists.
- Do not introduce the perspective divide as a matrix - mention it only as the step after clip space.
- The diagram in Task 1 should appear at the top of steps_html before the numbered steps.

---

### WS-M02 - The Identity Matrix
lab_number: "M02"
lab_title: "The Identity Matrix"
output_path: labs/mcp/math_WS02_identity_matrix.html
(see gen-mathWS07.mjs for full spec)

---

### WS-M03 - Homogeneous Coordinates and Transformation Matrices

lab_number: "M03"
lab_title: "Homogeneous Coordinates and Transformation Matrices"
output_path: labs/mcp/math_WS03_homogeneous_and_matrices.html

Outcome: Student can explain why a 4x4 matrix is required for translation, construct a translation
matrix and a Y-axis rotation matrix in column-major layout, and trace a vertex through a composed
transformation by hand.

Student prior knowledge at this point:
- Has read WS-M01 (knows the five spaces conceptually)
- Can multiply a 3x3 matrix by a 3-vector by hand
- Has not yet written or read a 4x4 matrix

Concept section narrative:
1. Problem: a 3x3 matrix can only encode linear transformations (rotation, scale, shear). It always
   maps the origin to itself. Translation moves the origin, so it cannot be expressed as a 3x3
   multiply. Without a unified representation, combining translate + rotate requires two separate
   operations, making composition and inversion awkward.
2. Mechanism: add a fourth component w to every vector. Set w=1 for points, w=0 for directions.
   The 4x4 translation matrix T(tx, ty, tz) stores the offset in column 3. When multiplied by a
   point (w=1) the offset is added; when multiplied by a direction (w=0) the offset is ignored.
   Rotation matrices are lifted to 4x4 by placing the 3x3 block top-left and 1 bottom-right.
3. API: glm::translate(mat4(1.0f), vec3(tx, ty, tz)) and glm::rotate(mat4(1.0f), angle_radians, axis).

Tasks:

Task 1 (Reference) - "Why 3x3 Cannot Translate"
  Use concrete numbers tx=4, ty=5, tz=6 throughout.
  Show that no fixed 3x3 matrix can add the offset to every possible input vector.
  Argument: the y-component of M*v = m10*vx + m11*vy + m12*vz. No choice of m10,m11,m12 can
  always produce vy+5 for all inputs. Conclude: translation is not a linear operation in 3D.
  Then show the 4x4 solution T(4,5,6) applied to (1,2,3,1) to get (5,7,9,1).
  Then apply T to the direction (0,1,0,0) to get (0,1,0,0) unchanged. Explain why.
  Solution: completed matrix multiplications. Results: (5,7,9,1) and (0,1,0,0).

Task 2 (Reference) - "The Y-Axis Rotation Matrix"
  Write and annotate R_y(theta) with entries labelled:
  <pre>|  cos(t)  0  sin(t)  0 |
|    0     1    0     0 |
| -sin(t)  0  cos(t)  0 |
|    0     0    0     1 |</pre>
  Use theta=90 deg (cos=0, sin=1) to verify two concrete cases:
  - R_y(90) * (1,0,0,1) = (0,0,-1,1): the X-axis tip swings to -Z. Explain via the right-hand rule.
  - R_y(90) * (0,0,1,1) = (1,0,0,1): the Z-axis tip swings to +X. Explain geometrically.
  Explain the sign of the -sin(t) entry at row 2, col 0: rotating X toward Z gives a negative
  contribution to the Z output when going through 0-to-90, which is what -sin encodes.
  Note: this task is a reference. No blanks. solution_code holds a clean copy of the matrix and
  the two verification multiplications laid out in full.

Task 3 (Skeleton) - "Compose and Apply by Hand"
  Goal: start at p = (0,0,0,1). Translate by (2,0,0) then rotate 90 deg around Y.
  Composition must be R * T (column-major right-to-left: T applied first, then R).
  Provide both matrices fully filled in. Students fill in 16 blanks for C = R_y(90) * T(2,0,0):
  T(2,0,0):
  <pre>| 1  0  0  2 |
| 0  1  0  0 |
| 0  0  1  0 |
| 0  0  0  1 |</pre>
  R_y(90):
  <pre>| 0  0  1  0 |
| 0  1  0  0 |
|-1  0  0  0 |
| 0  0  0  1 |</pre>
  Answer - C = R_y(90) * T(2,0,0):
  <pre>| 0  0  1  0 |
| 0  1  0  0 |
|-1  0  0 -2 |
| 0  0  0  1 |</pre>
  C * (0,0,0,1) = (0, 0, -2, 1). Interpretation: origin moves to (2,0,0) then rotates 90 around Y,
  landing at (0,0,-2). Show the GLM equivalent at the end for comparison only:
  model = glm::rotate(mat4(1.0f), glm::radians(90.0f), vec3(0,1,0));
  model = glm::translate(model, vec3(2,0,0));
  Note: rotate is called FIRST in code because GLM left-multiplies each call.

Generation notes:
- Write all matrix arithmetic step by step in <pre> blocks. Show each row dot-product.
- Do not use GLM in Tasks 1 or 2. Show the GLM equivalent only at the very end of Task 3.
- Skeleton blanks in steps_html: use underscores ___ for each blank.

---

### WS-M04 - Column-Major Convention and GLM

lab_number: "M04"
lab_title: "Column-Major Convention and GLM"
output_path: labs/mcp/math_WS04_column_major_and_glm.html

Outcome: Student can state what column-major storage means, read GLM matrix memory correctly,
explain why multiplication is written M * v and not v * M, and write GLM calls in the right order.

Student prior knowledge at this point:
- Has read WS-M03 (knows translation and rotation matrices, knows composition)
- Has seen glm::translate and glm::rotate in lab starter code
- Has NOT been told that GLM stores matrices differently from how they appear on paper
- This is the highest-confusion topic in the module. Take extra care to be explicit.

Concept section narrative:
1. Problem: a student writes a translation matrix on paper, looks at GLM memory, and thinks it is
   transposed. They pass the matrix to a shader and the object moves in the wrong direction. This
   single misunderstanding causes more assignment bugs than any other topic in the module.
2. Mechanism: row-major puts the first row in memory first. Column-major (OpenGL and GLM) puts the
   first COLUMN in memory first. The same mathematical matrix looks transposed when you read the
   raw memory. GLM mat4 stores column 0 at elements [0..3], column 1 at [4..7], and so on.
   mat[col][row] is the correct indexing order. mat[row][col] gives the wrong entry.
   Multiplication order follows: the matrix goes on the left, the column vector on the right: M * v.
   Writing v * M gives the wrong result.
3. API: GLM calls LEFT-multiply the accumulated matrix. Model = glm::rotate(model, a, axis) gives
   model = R * model. A call written later in the source applies BEFORE the geometry in math terms.
   Write GLM calls in REVERSE order from how you want transforms applied.

Tasks:

Task 1 (Reference) - "Row-Major vs Column-Major Side by Side"
  Use T(3, 1, 0) throughout. Show the matrix as written on paper:
  <pre>Math notation (row-major reading):
| 1  0  0  3 |
| 0  1  0  1 |
| 0  0  1  0 |
| 0  0  0  1 |</pre>
  Then show how GLM stores it in memory as a flat float array (column by column):
  <pre>GLM memory (column-major, 16 floats):
Index: [ 0][ 1][ 2][ 3]  [ 4][ 5][ 6][ 7]  [ 8][ 9][10][11]  [12][13][14][15]
Value:    1   0   0   0     0   1   0   0     0   0   1   0      3   1   0   1
         ---- col 0 ----   ---- col 1 ----   ---- col 2 ----   ---- col 3 ----</pre>
  Key observation: the translation values (3, 1, 0) appear at indices 12-14 - the last column.
  A common mistake: reading the raw memory row by row and thinking the matrix is transposed. It is
  not transposed. The storage order just differs from the reading order on paper.

Task 2 (Bug) - "Wrong Multiplication Order"
  Scenario: student wants a cube to rotate on its own axis while positioned at (5, 0, 0).
  Desired transform: T * R (translate after rotate, i.e. spin first then move).
  Student writes the calls in the "logical" order - translate first in code, rotate second:
  model = glm::translate(model, glm::vec3(5.0f, 0.0f, 0.0f));
  model = glm::rotate(model, angle, glm::vec3(0, 1, 0));
  This produces R * T (rotate applied AFTER translate in math terms - the object orbits the origin).
  Symptom to describe in the task (do NOT name the bug): "the cube swings in a wide arc around the
  world origin instead of spinning on the spot at x=5."
  Students must: (a) identify which line causes the wrong behaviour and explain why, (b) write the fix.
  Correct fix (rotate call first in code, translate second):
  model = glm::rotate(model, angle, glm::vec3(0, 1, 0));
  model = glm::translate(model, glm::vec3(5.0f, 0.0f, 0.0f));
  Explanation: rotate first gives model = R. Translate second gives model = T * R. Vertex path:
  R applied first (spins in local space), then T (moved to world position). Correct.

Task 3 (Skeleton) - "Write the GLM Calls"
  Goal (described in words): "Translate the object to (0, 2, 0), then rotate it 30 degrees around Z."
  (Desired matrix: T * R_z(30). GLM call order: rotate first, then translate.)
  Students: (a) write the two GLM calls in the correct order, (b) trace vertex (1,0,0,1) by hand.
  Hand trace:
    After R_z(30): (cos30, sin30, 0, 1) = (sqrt(3)/2, 0.5, 0, 1) approx (0.866, 0.5, 0, 1)
    After T(0,2,0): (sqrt(3)/2, 2.5, 0, 1)
  Solution: correct GLM calls (rotate first, translate second), intermediate vectors, final position.

Generation notes:
- Task 2 is a Bug task. The faulty code goes in the code field. Describe only the symptom in steps_html.
- Use glm::radians() in all code. Never pass raw degree values to glm::rotate.
- In Task 3 step b write sqrt(3)/2 as "sqrt(3)/2 (approx 0.866)" so students can verify numerically.

---

### WS-M05 - The Look-At Matrix

lab_number: "M05"
lab_title: "The Look-At Matrix"
output_path: labs/mcp/math_WS05_lookat_matrix.html

Outcome: Student can derive the three camera basis vectors from eye, centre, and world-up using
cross products, explain why the view matrix is the inverse of the camera's world transform, and
explain why the translation is negated in the final matrix.

Student prior knowledge at this point:
- Has read WS-M03 and WS-M04
- Knows dot and cross product mechanically
- Has called glm::lookAt in Lab 08 or Lab 19 starter code without knowing what it computes
- Has NOT derived basis vectors or inverse transforms

Concept section narrative:
1. Problem: the camera is positioned and aimed at something in the scene. The vertex shader needs
   all vertices expressed relative to the camera. Students often think the view matrix "moves the
   camera" but it actually does the opposite: it moves the entire world so the camera ends up at
   the origin looking down -Z.
2. Mechanism: three basis vectors describe the camera's orientation - right, up, and forward. They
   are derived from the eye position, the look-at point (centre), and world-up using cross products.
   The view matrix combines a rotation (aligning world axes to camera axes) with a translation
   (moving the world so the camera sits at the origin). The translation values are negated because
   the world moves opposite to the camera's position.
3. API: glm::lookAt(eye, centre, worldUp) computes this matrix. The three arguments map directly to
   the three inputs of the derivation.

Tasks:

Task 1 (Reference) - "Deriving a Look-At Matrix Step by Step"
  Use: eye = (0,0,5), centre = (0,0,0), worldUp = (0,1,0). Gives clean integer results.
  Show every step using GLM's convention (f = normalize(centre - eye)):
  1. f = normalize(centre - eye) = normalize(0,0,-5) = (0, 0, -1)
  2. s (right) = normalize(cross(f, worldUp)) = normalize(cross((0,0,-1),(0,1,0)))
       cross: (0*0-(-1)*1, (-1)*0-0*0, 0*1-0*0) = (1, 0, 0). Already unit length.
  3. u (corrected up) = cross(s, f) = cross((1,0,0),(0,0,-1))
       = (0*(-1)-0*0, 0*0-1*(-1), 1*0-0*0) = (0, 1, 0)
  Final matrix (write in standard row notation then show the numeric result side by side):
  <pre>|  s.x   s.y   s.z   -dot(s,eye) |   | 1  0  0   0 |
|  u.x   u.y   u.z   -dot(u,eye) | = | 0  1  0   0 |
| -f.x  -f.y  -f.z    dot(f,eye) |   | 0  0  1  -5 |
|   0     0     0     1          |   | 0  0  0   1 |</pre>
  Interpret: a pure translation of -5 on Z. The camera is at z=5 so the world shifts -5 on Z.
  Show the negated translation arithmetic: -dot(s, eye) = -(1*0+0*0+0*5) = 0 for right.
  dot(f, eye) = (0*0+0*0+(-1)*5) = -5 appears at row 2 col 3 of the final matrix.

Task 2 (Skeleton) - "Derive a Different Configuration"
  Use: eye = (4,0,0), centre = (0,0,0), worldUp = (0,1,0). Camera to the right, looking left.
  Provide the step labels with blanks for each intermediate vector and matrix entry.
  Worked answer for solution_code:
    f = normalize((0,0,0)-(4,0,0)) = (-1, 0, 0)
    s = normalize(cross((-1,0,0),(0,1,0))) = normalize((0,0,-1)) = (0, 0, -1)
    u = cross((0,0,-1),(-1,0,0)) = (0, 1, 0)
  View matrix:
  <pre>| 0  0 -1   0 |
| 0  1  0   0 |
| 1  0  0  -4 |
| 0  0  0   1 |</pre>
  Note: dot(f, eye) = (-1*4) = -4 at row 2 col 3. The right vector is (0,0,-1) because the camera
  looks left (-X), so its right side points into the screen (-Z in world space).

Task 3 (Conceptual) - "Three Short-Answer Questions"
  Q1: "The camera is pointing straight up. worldUp = (0,1,0) and centre - eye = (0,1,0). What
      goes wrong in step 2, and what does this failure mean physically?"
      Answer: f and worldUp are parallel. Their cross product is the zero vector. Normalising
      zero is undefined. The camera has no unique right direction. This is a gimbal singularity.
  Q2: "Why is the translation column negated in the look-at matrix?"
      Answer: the view matrix moves the world, not the camera. If the camera is at position e,
      the world must shift by -e so the camera ends up at the origin.
  Q3: "What does the right vector represent and why is a cross product the right tool for it?"
      Answer: right is the direction pointing to the camera's right side. It must be perpendicular
      to both forward and worldUp. The cross product of two vectors is perpendicular to both,
      which is exactly the constraint.

Generation notes:
- In Task 1, write the cross product expansion in full - show all six products. Do not skip steps.
- Task 2: provide the formula template for each step with ___ blanks in steps_html.
- Task 3 is conceptual only. No code field. Questions in steps_html, answers in solution_code.

---

### WS-M06 - Perspective Projection and NDC

lab_number: "M06"
lab_title: "Perspective Projection and NDC"
output_path: labs/mcp/math_WS06_perspective_and_ndc.html

Outcome: Student can explain what each frustum parameter controls geometrically, carry a view-space
point through the projection matrix by hand, perform the perspective divide, and explain why a large
near/far ratio degrades depth precision.

Student prior knowledge at this point:
- Has read WS-M01 (knows clip space and NDC conceptually)
- Has called glm::perspective in Lab 08 without knowing what the matrix contains
- Knows what a frustum looks like from lectures but has not done the maths

Concept section narrative:
1. Problem: how does a 3D point become a 2D pixel coordinate? Scaling alone does not explain why
   far objects appear smaller. Something must divide by depth. The projection matrix sets up that
   division but does not perform it - the hardware divides as a separate step. Understanding the
   two-step process (matrix then divide) is key to understanding the w component and depth precision.
2. Mechanism: the projection matrix encodes the frustum shape. Multiplying a view-space point by it
   produces a clip-space vector where w = -z_view. The hardware then divides x, y, z by w to get
   NDC. Because w carries depth, dividing by it causes far objects to shrink more than near ones -
   that is perspective. NDC depth is NOT linear between near and far. Precision concentrates near
   the near plane. A small near/far ratio gives reasonable precision throughout. A large ratio
   compresses nearly all precision into a thin band close to the camera, causing z-fighting on
   distant geometry.
3. API: glm::perspective(glm::radians(fov), aspect, near, far). Parameters map to matrix entries
   derived below.

Tasks:

Task 1 (Reference) - "The Projection Matrix and Its Parameters"
  Parameters: FoV=90 deg, aspect=1.0, near=1.0, far=10.0. (FoV/2 = 45 deg, tan(45) = 1.)
  State the key entries and explain each in plain language (do not derive from scratch):
    f = 1/tan(FoV/2) = 1
    M[0][0] = f/aspect = 1    scales x by focal length and aspect ratio
    M[1][1] = f = 1           scales y by focal length
    M[2][2] = -(far+near)/(far-near) = -11/9 approx -1.222    maps z into [-1,1] range
    M[3][2] = -(2*far*near)/(far-near) = -20/9 approx -2.222  sets the scale of the depth mapping
    M[2][3] = -1              copies -z_view into w_clip (enables the perspective divide)
  Write the full matrix:
  <pre>| 1   0      0       0   |
| 0   1      0       0   |
| 0   0   -11/9  -20/9  |
| 0   0     -1      0   |</pre>
  Explain M[2][3] = -1 in depth: this causes w_clip = -z_view. Because z_view is negative for
  visible geometry (OpenGL looks down -Z), w_clip is positive. After dividing by w_clip, NDC z
  maps the near plane to -1 and the far plane to +1.

Task 2 (Skeleton) - "Carry a Point to NDC by Hand"
  Use the projection matrix from Task 1 (FoV=90, aspect=1, near=1, far=10).
  Starting point: v = (2, 1, -3, 1) in view space. (z=-3 is between near=1 and far=10, visible.)
  Students fill in blanks at each step:
    x_clip = 1*2 = ___                      answer: 2
    y_clip = 1*1 = ___                      answer: 1
    z_clip = (-11/9)*(-3) + (-20/9)*1 = 33/9 - 20/9 = 13/9 approx ___ answer: 1.444
    w_clip = (-1)*(-3) = ___                answer: 3
  Perspective divide (divide each component by w_clip = 3):
    NDC_x = 2/3 approx ___                  answer: 0.667
    NDC_y = 1/3 approx ___                  answer: 0.333
    NDC_z = (13/9)/3 = 13/27 approx ___    answer: 0.481
  Final question: are all three NDC values in [-1, 1]? Is the point visible? Which axis is
  closest to a clip boundary? (NDC_z = 0.481, closer to the far plane than to the near plane.)

Task 3 (Conceptual) - "Depth Precision"
  Q1: "Two scenes use the same projection except one has near=1, far=10 and the other has
      near=0.01, far=10000. A wall at z=-9 and a wall at z=-9.01 must be distinguishable in
      a 24-bit depth buffer. In which scene is this more likely to fail, and why?"
      Answer: the scene with near=0.01, far=10000. The NDC depth mapping is nonlinear and
      concentrates almost all precision near the near plane. At z=-9 and z=-9.01 the NDC
      difference is smaller than one depth buffer increment, so both walls round to the same
      integer value (z-fighting). With near=1 the precision is spread over a 1-to-10 range
      and the two walls resolve cleanly.
  Q2: "Without changing FoV or the scene geometry, what single parameter change most effectively
      reduces z-fighting on distant objects?"
      Answer: move the near plane further from the camera (increase near). A larger near value
      spreads precision more evenly over the visible range.

Generation notes:
- In Task 1 write -11/9 and -20/9 as both fractions and decimal approximations.
- In Task 2 show the row dot-product template in steps_html with blank spaces before answers.
- Task 3 is conceptual only. No code field.

---

### WS-M07 - Normal Vector Transformation

lab_number: "M07"
lab_title: "Normal Vector Transformation"
output_path: labs/mcp/math_WS07_normal_transform.html

Outcome: Student can explain why normals cannot be transformed with the model matrix when a
non-uniform scale is present, compute the normal matrix for a given model matrix, state when the
shortcut (using the model matrix directly) is safe, and write the correct GLM and GLSL code.

Student prior knowledge at this point:
- Has completed ACCA6021 code labs and started ACCB6014 lighting labs (Phong shading)
- Has probably written normalMatrix = mat3(transpose(inverse(model))) without understanding why
- Knows dot product geometrically (cos of angle between vectors)

Concept section narrative:
1. Problem: when a model is non-uniformly scaled, applying the same model matrix to surface normals
   makes them point in the wrong direction. The dot product between the wrong normal and the light
   direction gives the wrong angle. Lighting becomes incorrect and the artefact is immediately visible.
2. Mechanism: a normal is not an ordinary direction vector. It encodes a perpendicularity constraint:
   n.t = 0, where t is a surface tangent. When tangents are transformed by M, normals must be
   transformed by (M^-1)^T so the constraint is preserved. For orthonormal matrices (pure rotations),
   (M^-1)^T = M, so using the model matrix directly is safe. For non-uniform scales it is not.
3. API: compute the normal matrix on the CPU: glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model))).
   Pass it as a uniform and apply in the vertex shader: vec3 worldNormal = normalMatrix * normal.
   Always use the 3x3 block (directions ignore the translation column, but the issue is the
   scale, not the translation, so the full 4x4 is never needed for normals).

Tasks:

Task 1 (Reference) - "The Failure Case"
  Concrete setup: surface tangent t = (1,1,0) (unnormalized). Normal n = (-1,1,0) (perpendicular).
  Verify: n.t = (-1)(1)+(1)(1)+(0)(0) = 0. Correct.
  Apply non-uniform scale M = scale(2,1,1):
    Transformed tangent: Mt = (2,1,0).
    Naive normal transform: Mn = (-2,1,0).
    Check perpendicularity: dot((2,1,0),(-2,1,0)) = -4+1 = -3. NOT zero. Normal is wrong.
  Compute the correct normal using (M^-1)^T:
    M^-1 = scale(1/2,1,1). (M^-1)^T = scale(1/2,1,1) (diagonal, so transpose equals itself).
    Correct normal: (M^-1)^T * n = (-1/2, 1, 0).
    Check: dot((2,1,0),(-1/2,1,0)) = -1+1 = 0. Perpendicularity preserved.
  Note: the correct normal should be renormalized before use in lighting.

Task 2 (Reference) - "Why the Transpose-Inverse Works"
  Frame the argument intuitively, not as a formal proof:
  We need a matrix N such that (Mt).(Nn) = 0 whenever t.n = 0.
  Rewrite: (Mt)^T (Nn) = 0, which is t^T M^T N n = 0.
  We need M^T N = I, so N = (M^T)^-1 = (M^-1)^T.
  State in plain language: the normal matrix is the matrix that undoes what M does to the
  perpendicularity relationship. Result: N = transpose(inverse(M)).
  When does this simplify? If M is a rotation, M^-1 = M^T, so N = (M^T)^T = M. Safe to skip.
  For uniform scale S*I: N = (1/S)*I, which just changes the magnitude. Renormalising the normal
  fixes this, so using the model matrix directly is also safe after renormalisation.

Task 3 (Skeleton) - "Compute the Normal Matrix"
  Matrix A: M = scale(3,3,3).
    Students compute (M^-1)^T = scale(1/3,1/3,1/3). Then answer: is the shortcut safe?
    Answer: yes - uniform scale. The orientation is preserved. Renormalise after applying M.
  Matrix B: M = scale(2,1,1) composed with a rotation (do not compute the product by hand).
    a) Is the shortcut safe? Answer: No - M contains a non-uniform scale.
    b) Write the C++ code to compute the correct normal matrix:
       glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
    c) Write the GLSL vertex shader line that applies it:
       vec3 worldNormal = normalize(normalMatrix * aNormal);

Task 4 (Conceptual) - "When Is the Shortcut Safe?"
  Question: "List two conditions under which you can use the model matrix (or its 3x3 block)
  directly as the normal matrix. Explain why each condition is sufficient."
  Answer:
  1. Model matrix contains only rotations (and optionally uniform scale). Rotations are orthonormal
     so M^-T = M. Uniform scale factors cancel after normalisation.
  2. Model matrix is the identity. Trivially safe - no transform changes any direction.

Generation notes:
- Task 1 carries the most learning. Give it the most space. Show every dot product in full.
- In Task 2 use M<sup>T</sup> and M<sup>-1</sup> HTML notation throughout.
- In Task 3 include BOTH the C++ (GLM) and GLSL versions in the solution.
- Remind students in the concept section that computing inverse() per frame in GLSL is expensive.
  Compute the normal matrix on the CPU and pass it as a uniform.

---

### WS-REF - Matrix and Trig Compendium

lab_number: "REF"
lab_title: "Matrix and Trig Reference Card"
output_path: labs/mcp/math_WSREF_reference_card.html

Outcome: A single-page reference card kept open alongside any lab from Lab 07 onward.
No exercises. No solution button. Print-friendly.

Content (single Reference task, five subsections):

Section 1 - Trig Values at Standard Angles
  HTML <table> with columns: Angle (deg), Angle (rad), sin, cos, tan.
  0:   0,      0,         1,          0
  30:  pi/6,   1/2,       sqrt(3)/2,  1/sqrt(3)
  45:  pi/4,   sqrt(2)/2, sqrt(2)/2,  1
  60:  pi/3,   sqrt(3)/2, 1/2,        sqrt(3)
  90:  pi/2,   1,         0,          undefined

Section 2 - Translation Matrix T(tx, ty, tz) in Column-Major Layout
  <pre>| 1  0  0  tx |
| 0  1  0  ty |
| 0  0  1  tz |
| 0  0  0   1 |</pre>
  Caption: tx, ty, tz appear in column 3 (indices 12-14 in GLM memory).

Section 3 - Rotation Matrices in Column-Major Layout (angles in radians, abbreviated t)
  R_x(t), R_y(t), R_z(t) each in a <pre> block.
  R_y reminder: note the sign swap on the sin terms compared to R_x and R_z.

Section 4 - Perspective Projection Matrix
  Parameters: f = 1/tan(fov/2), a = aspect, n = near, z = far.
  <pre>| f/a   0      0           0        |
|  0    f      0           0        |
|  0    0  -(z+n)/(z-n)  -2zn/(z-n)|
|  0    0     -1           0        |</pre>
  Caption: M[2][3] = -1 copies -z_view into w_clip (enables the perspective divide).

Section 5 - GLM Function Table
  HTML <table> with columns: Purpose, GLM Call.
  Rows:
    Identity matrix:      glm::mat4(1.0f)
    Translation:          glm::translate(mat, glm::vec3(tx, ty, tz))
    Rotation:             glm::rotate(mat, glm::radians(degrees), axis)
    Uniform scale:        glm::scale(mat, glm::vec3(s, s, s))
    Perspective:          glm::perspective(glm::radians(fov), aspect, near, far)
    Orthographic:         glm::ortho(left, right, bottom, top, near, far)
    Look-At / View:       glm::lookAt(eye, centre, worldUp)
    Normal matrix (C++):  glm::mat3(glm::transpose(glm::inverse(model)))
    Normal matrix (GLSL): mat3(transpose(inverse(model)))

Generation notes:
- Wrap all content in a single task titled "Task 1 - Reference Tables" with code_type omitted.
- Do not include solution_code or solution_files - nothing to reveal.
- The overview_html should say: "This card is designed to be printed and kept alongside your lab
  worksheet. It covers trig values, the standard transformation matrices in column-major layout,
  and the GLM function calls that correspond to each."
- languages: []