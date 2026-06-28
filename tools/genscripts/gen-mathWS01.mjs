import { buildAndWriteLab } from '../tools/lab-assembler/dist/assembler.js';

buildAndWriteLab({
  options: { solution: true, copy_protect: false },
  module_code: 'ACCA6021',
  module_title: 'Graphics and Games Programming',
  lab_number: 'M01',
  lab_title: 'Coordinate Spaces and the Transformation Pipeline',

  overview_html: `<p>This worksheet traces a single vertex through the five coordinate spaces that every OpenGL pipeline passes through. You will identify what each space represents geometrically, label the matrices responsible for each transition, and connect the sequence to the vertex shader expression <code>gl_Position = projection * view * model * position</code>.</p>`,

  objectives: [
    'By the end of this worksheet you will be able to name all five coordinate spaces in the OpenGL pipeline and describe what each one represents geometrically',
    'By the end of this worksheet you will be able to state which matrix is responsible for each transition between coordinate spaces',
    'By the end of this worksheet you will be able to trace a vertex from object space to NDC and describe the geometric change at each step',
    'By the end of this worksheet you will be able to explain why the transforms in gl_Position = projection * view * model * position are applied right to left',
  ],

  concept_html: `<p>A 3D model is defined in its own local space, but it needs to appear at a specific location on a screen showing a scene that may contain many other objects. Getting there requires more than one step. Modelling, camera placement, and screen mapping are controlled by different parts of the program and can change independently: the object may move, the camera may orbit, and the viewport may be resized without affecting the others. Collapsing all of this into a single operation would make these concerns inseparable. The pipeline keeps them distinct.</p>
<p>The pipeline is a sequence of coordinate system changes. Each stage transforms vertices from one frame of reference to the next using a matrix multiplication. <strong>Object Space</strong> (also called local space) is the native coordinate system of the mesh, with the origin at the mesh centre by convention. The <strong>model matrix</strong> moves vertices into <strong>World Space</strong>, the shared frame where all objects in the scene coexist. The <strong>view matrix</strong> shifts everything so the camera sits at the origin looking down the negative Z axis, producing <strong>View Space</strong>. The <strong>projection matrix</strong> encodes the camera frustum and yields <strong>Clip Space</strong>, where the hardware performs clipping and the w component carries depth information. Dividing each clip-space vector by its w component produces <strong>NDC</strong> (Normalised Device Coordinates), where all visible geometry fits within the cube [-1, 1] on every axis.</p>
<p>In the vertex shader this entire pipeline is expressed in one line: <code>gl_Position = projection * view * model * position</code>. Matrix multiplication is applied right to left, so <code>model</code> acts on the vertex first, then <code>view</code>, then <code>projection</code>. The result assigned to <code>gl_Position</code> is a clip-space vector. The hardware performs the perspective divide and viewport mapping automatically after the shader runs.</p>`,

  tasks: [
    {
      title: 'Task 1 - Tracing a Vertex Through the Pipeline',
      blocks: [
        {
          type: 'prose',
          html: `<p>The diagram below shows the five coordinate spaces and the matrices that connect them. The worked example traces the vertex <strong>p</strong> = (1, 0, 0, 1) - the tip of the positive X axis in object space - through each transformation stage.</p>
<pre>
  [ Object Space ]
        |
   model matrix
        |
        v
  [ World Space  ]
        |
   view matrix
        |
        v
  [ View Space   ]
        |
 projection matrix
        |
        v
  [ Clip Space   ]
        |
   divide by w
        |
        v
  [     NDC      ]
</pre>
<ol>
  <li>
    <strong>Object to World Space</strong> - model matrix: translate by (3, 0, 0)
    <pre>p_world = (1+3, 0+0, 0+0, 1) = (4, 0, 0, 1)</pre>
    <p>The vertex now lives 4 units along X in the shared scene frame. Every other object in the scene is also expressed in World Space after its own model matrix is applied.</p>
  </li>
  <li>
    <strong>World to View Space</strong> - view matrix: camera at (0, 0, 5) looking at the origin
    <pre>p_view = (4, 0, -5, 1)</pre>
    <p>The view matrix moves the entire world so the camera ends up at the origin looking down -Z. A camera at z = 5 is equivalent to shifting the world by -5 on Z. The x and y coordinates are unchanged here because the camera has no rotation in this example.</p>
  </li>
  <li>
    <strong>View to Clip Space</strong> - projection matrix: 90 deg FoV, aspect = 1.0, near = 1, far = 10
    <pre>w_clip = -z_view = -(-5) = 5
x_clip = f * x_view = 1 * 4 = 4   (f = 1/tan(45 deg) = 1 for 90 deg FoV)
y_clip = f * y_view = 1 * 0 = 0</pre>
    <p>The projection matrix scales x and y by the focal length and encodes depth into z_clip. The critical result is w_clip = 5. The z_clip value encodes a nonlinear depth that will map to the range [-1, 1] after the next step. The full derivation of the projection matrix is covered in the Perspective Projection worksheet (WS-M06).</p>
  </li>
  <li>
    <strong>Clip Space to NDC</strong> - perspective divide: divide by w_clip = 5
    <pre>NDC_x = x_clip / w_clip = 4 / 5 = 0.8
NDC_y = y_clip / w_clip = 0 / 5 = 0.0</pre>
    <p>Dividing by w compresses all visible geometry into the cube [-1, 1] on every axis. NDC_x = 0.8 means the vertex is 80% of the way to the right edge of the screen. The rasteriser uses NDC coordinates to compute the final pixel position.</p>
  </li>
</ol>`,
        },
      ],
    },

    {
      title: 'Task 2 - Label the Pipeline',
      blocks: [
        {
          type: 'prose',
          html: `<p>The diagram below has all five space names and all three matrix labels removed. Use the questions below to identify each blank, then complete the five sentence starters.</p>
<pre>
  [   ???  (A)  ]
        |
    ??? (1) matrix
        |
        v
  [   ???  (B)  ]
        |
    ??? (2) matrix
        |
        v
  [   ???  (C)  ]
        |
    ??? (3) matrix
        |
        v
  [   ???  (D)  ]
        |
    divide by w
        |
        v
  [   ???  (E)  ]
</pre>`,
        },
        {
          type: 'fill_blanks',
          interactive: true,
          questions: [
            {
              question_html: '<strong>(A)</strong> - The mesh\'s native coordinate system, origin at the mesh centre: <strong>{{blank}}</strong> Space.',
              answers: [['Object', 'object', 'Local', 'local']],
            },
            {
              question_html: '<strong>(B)</strong> - The shared frame where all objects in the scene coexist: <strong>{{blank}}</strong> Space.',
              answers: [['World', 'world']],
            },
            {
              question_html: '<strong>(C)</strong> - The camera sits at the origin looking down -Z: <strong>{{blank}}</strong> Space.',
              answers: [['View', 'view', 'Camera', 'camera', 'Eye', 'eye']],
            },
            {
              question_html: '<strong>(D)</strong> - Produced by the projection matrix; w encodes depth; hardware clips here: <strong>{{blank}}</strong> Space.',
              answers: [['Clip', 'clip']],
            },
            {
              question_html: '<strong>(E)</strong> - All visible geometry fits within [-1, 1] on every axis: <strong>{{blank}}</strong>.',
              answers: [['NDC', 'ndc', 'Normalised Device Coordinates', 'Normalized Device Coordinates']],
            },
            {
              question_html: '<strong>(1)</strong> - The matrix that moves vertices from Object Space into the shared scene: the <strong>{{blank}}</strong> matrix.',
              answers: [['model', 'Model']],
            },
            {
              question_html: '<strong>(2)</strong> - The matrix that positions the world relative to the camera: the <strong>{{blank}}</strong> matrix.',
              answers: [['view', 'View']],
            },
            {
              question_html: '<strong>(3)</strong> - The matrix that encodes the camera frustum and produces Clip Space: the <strong>{{blank}}</strong> matrix.',
              answers: [['projection', 'Projection']],
            },
          ],
        },
        {
          type: 'short_answer',
          interactive: true,
          questions: [
            {
              question: 'Complete: "Object Space is the coordinate system where ___."',
              model_answer: 'Object Space is the coordinate system where vertices are defined relative to the mesh\'s own origin. It is local to each individual mesh. The origin is placed at the mesh centre by convention, so a vertex at (1, 0, 0) is 1 unit to the right of the mesh centre.',
            },
            {
              question: 'Complete: "World Space is the shared frame where ___."',
              model_answer: 'World Space is the shared frame where all objects in the scene coexist after their individual model matrices have been applied. A vertex at (4, 0, 0) in world space is 4 units along the global X axis, regardless of which mesh it belongs to.',
            },
            {
              question: 'Complete: "View Space places the ___ at the origin, looking down ___."',
              model_answer: 'View Space places the camera at the origin, looking down the negative Z axis. This is achieved by the view matrix, which moves the entire world so the camera ends up at the origin. The -Z convention simplifies the projection maths.',
            },
            {
              question: 'Complete: "Clip Space exists after the ___ matrix. The w component encodes ___."',
              model_answer: 'Clip Space exists after the projection matrix. The w component encodes the original view-space depth (-z_view). The hardware uses w to perform the perspective divide and clips any geometry outside the view frustum in this space.',
            },
            {
              question: 'Complete: "NDC is produced by ___. All visible geometry fits within ___."',
              model_answer: 'NDC is produced by dividing each clip-space vector by its w component (the perspective divide). All visible geometry fits within the cube [-1, 1] on every axis. The rasteriser uses NDC coordinates to map vertices to pixel positions on screen.',
            },
          ],
        },
      ],
    },
  ],

  solution_code: String.raw`WS01 - Coordinate Spaces and the Transformation Pipeline - Answer Key

Task 2 - Fill-in Blanks
  (A) Object Space   - the mesh's native local coordinate system
  (B) World Space    - the shared scene frame
  (C) View Space     - camera at origin looking down -Z
  (D) Clip Space     - produced by projection matrix; w carries depth
  (E) NDC            - Normalised Device Coordinates; all visible geometry in [-1,1]^3

  (1) model matrix       - Object Space -> World Space
  (2) view matrix        - World Space  -> View Space
  (3) projection matrix  - View Space   -> Clip Space

Task 2 - Sentence Completions
  Object Space:  vertices are defined relative to the mesh's own origin (local space; origin at mesh centre).
  World Space:   all objects in the scene coexist here after their model matrices are applied.
  View Space:    the camera sits at the origin looking down -Z; the view matrix achieves this.
  Clip Space:    after the projection matrix; w encodes -z_view (depth); hardware clips here.
  NDC:           produced by dividing clip coords by w; all visible geometry fits in [-1,1]^3.`,

  languages: [],
}, 'labs/mcp/math_WS01_coordinate_spaces.html');
