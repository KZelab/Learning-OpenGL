import { buildAndWriteLab } from '../lab-assembler/dist/assembler.js';

const pipelineViz = `
  const w = 760;
  const h = 275;

  const stages = [
    { lines: ['Application'], color: '#5a6268', sub: 'CPU',
      api: ['glBufferData()', 'glVertexAttribPointer()', 'glDrawArrays()'] },
    { lines: ['Vertex', 'Shader'], color: '#003366', sub: 'programmable',
      api: ['layout(location=N) in', 'gl_Position', 'out varyings'] },
    { lines: ['Primitive', 'Assembly'], color: '#5a6268', sub: 'fixed',
      api: ['GL_TRIANGLES mode', 'frustum clipping'] },
    { lines: ['Rasterization'], color: '#5a6268', sub: 'fixed',
      api: ['fragment generation', 'varying interpolation'] },
    { lines: ['Fragment', 'Shader'], color: '#003366', sub: 'programmable',
      api: ['in varyings', 'out vec4 fragColour', 'gl_FragCoord'] },
    { lines: ['Output', 'Merger'], color: '#5a6268', sub: 'fixed',
      api: ['GL_DEPTH_TEST', 'glDepthFunc()', 'glBlendFunc()'] },
  ];
  const arrowLabels = ['vertex attrs', 'clip-space', 'triangles', 'fragments', 'colour'];

  const n = stages.length;
  const arrowW = 44;
  const pad = 10;
  const boxW = Math.floor((w - 2*pad - (n-1)*arrowW) / n);
  const boxH = 52;
  const boxY = 38;
  const midY = boxY + boxH / 2;

  const svg = d3.select(container).append('svg')
    .attr('viewBox', \`0 0 \${w} \${h}\`)
    .attr('preserveAspectRatio', 'xMidYMid meet')
    .style('width', '100%').style('height', 'auto').style('display', 'block')
    .style('font-family', "'Segoe UI', Arial, sans-serif");

  svg.append('defs').append('marker')
    .attr('id', 'arr-gp01')
    .attr('markerWidth', 7).attr('markerHeight', 5)
    .attr('refX', 7).attr('refY', 2.5).attr('orient', 'auto')
    .append('polygon').attr('points', '0 0, 7 2.5, 0 5').attr('fill', '#888');

  stages.forEach((s, i) => {
    const x = pad + i * (boxW + arrowW);
    const cx = x + boxW / 2;

    svg.append('rect')
      .attr('x', x).attr('y', boxY).attr('width', boxW).attr('height', boxH)
      .attr('rx', 4).attr('fill', s.color);

    const lh = 13;
    const ly = midY - (s.lines.length - 1) * lh / 2;
    s.lines.forEach((line, li) =>
      svg.append('text')
        .attr('x', cx).attr('y', ly + li * lh)
        .attr('text-anchor', 'middle').attr('dominant-baseline', 'middle')
        .attr('fill', '#fff').attr('font-size', 10).attr('font-weight', '600')
        .text(line)
    );

    svg.append('text')
      .attr('x', cx).attr('y', boxY + boxH + 11)
      .attr('text-anchor', 'middle').attr('fill', '#888')
      .attr('font-size', 8.5).attr('font-style', 'italic')
      .text(s.sub);

    s.api.forEach((line, li) =>
      svg.append('text')
        .attr('x', cx).attr('y', boxY + boxH + 25 + li * 14)
        .attr('text-anchor', 'middle').attr('fill', '#444')
        .attr('font-size', 7).attr('font-family', "'Consolas','Courier New',monospace")
        .text(line)
    );

    if (i < n - 1) {
      const ax = x + boxW;
      svg.append('line')
        .attr('x1', ax).attr('y1', midY).attr('x2', ax + arrowW).attr('y2', midY)
        .attr('stroke', '#888').attr('stroke-width', 1.5)
        .attr('marker-end', 'url(#arr-gp01)');
      svg.append('text')
        .attr('x', ax + arrowW / 2).attr('y', boxY - 6)
        .attr('text-anchor', 'middle').attr('fill', '#555').attr('font-size', 8.5)
        .text(arrowLabels[i]);
    }
  });

  const legY = h - 12;
  [['#003366', 'Programmable (GLSL)'], ['#5a6268', 'Fixed-function']].forEach(([c, l], i) => {
    const lx = pad + i * 180;
    svg.append('rect').attr('x', lx).attr('y', legY - 8).attr('width', 12).attr('height', 10).attr('fill', c).attr('rx', 2);
    svg.append('text').attr('x', lx + 16).attr('y', legY).attr('font-size', 10).attr('fill', '#555').text(l);
  });
`;

buildAndWriteLab({
  options: { solution: true, copy_protect: false },
  module_code:  'ACCA6021',
  module_title: 'Graphics and Games Programming',
  lab_number:   'GP01',
  lab_title:    'The OpenGL Rendering Pipeline',

  overview_html: `<p>This worksheet traces the journey a vertex takes from CPU memory to a pixel colour
  on screen. You will learn the name and purpose of each pipeline stage, which stages you can program
  in GLSL, which OpenGL API calls belong to which stage, and how to identify which stage is responsible
  when something goes wrong.</p>`,

  objectives: [
    'Name each stage of the OpenGL rendering pipeline and describe its input and output',
    'Distinguish programmable (GLSL) stages from fixed-function stages',
    'Match OpenGL API calls and GLSL features to the stage that owns them',
    'Trace vertex data from application memory through to a pixel colour in the framebuffer',
    'Identify which pipeline stage is responsible for common rendering artefacts',
  ],

  concept_html: `<p>Every draw call passes through six pipeline stages before a pixel colour reaches
  the framebuffer. Without knowing which stage owns which piece of code, debugging becomes guesswork.
  A triangle that disappears might have a broken <code>glVertexAttribPointer</code> call (Application
  stage), a wrong MVP matrix (Vertex Shader), or a depth test failure (Output Merger). Each requires
  a different fix.</p>

  <p>The <strong>Application</strong> stage runs on the CPU. Your code uploads vertex data into a
  <strong>Vertex Buffer Object (VBO)</strong> with <code>glBufferData</code>, describes its layout
  with <code>glVertexAttribPointer</code>, and issues a draw call with <code>glDrawArrays</code>.
  The <strong>Vertex Shader</strong> is the first programmable stage: the GPU runs your GLSL once
  per vertex. It receives per-vertex inputs via <code>layout(location = N) in</code>, writes
  clip-space position to <code>gl_Position</code>, and passes <strong>varyings</strong> to later
  stages via <code>out</code> variables. Fixed-function <strong>Primitive Assembly</strong> then
  groups the transformed vertices into triangles and clips geometry outside the view frustum.</p>

  <p><strong>Rasterization</strong> determines which pixels each triangle covers, emits one
  <strong>fragment</strong> per covered pixel, and linearly interpolates all varyings across the
  triangle surface. The <strong>Fragment Shader</strong> is the second programmable stage: it
  receives those interpolated values as <code>in</code> variables and writes a colour to an
  <code>out vec4</code>. The built-in <code>gl_FragCoord</code> gives each fragment its
  window-space position. Finally, the <strong>Output Merger</strong> applies the depth test and
  blending through state calls such as <code>glEnable(GL_DEPTH_TEST)</code> and
  <code>glBlendFunc</code>. These fixed-function stages cannot be replaced with GLSL — only
  configured through API calls.</p>`,

  tasks: [
    // ── Task 1 ────────────────────────────────────────────────────────────────
    {
      title: 'Task 1 - Pipeline Overview',
      blocks: [
        {
          type: 'prose',
          html: `<p>The diagram below maps each pipeline stage to the OpenGL calls or GLSL features
          that implement it. Navy boxes are the two programmable stages; grey boxes are fixed-function.
          Study it carefully before completing the blanks below.</p>`,
        },
        {
          type: 'visualization',
          id:     'pipeline-diagram',
          script: pipelineViz,
        },
        {
          type: 'fill_blanks',
          interactive: true,
          questions: [
            {
              question_html: 'The first programmable stage is the <strong>{{blank}}</strong> Shader.',
              answers: [['Vertex', 'vertex']],
            },
            {
              question_html: 'Primitive <strong>{{blank}}</strong> groups the vertices produced by the vertex shader into geometric shapes such as triangles.',
              answers: [['Assembly', 'assembly']],
            },
            {
              question_html: 'The <strong>{{blank}}</strong> stage walks each triangle and emits one fragment per covered pixel.',
              answers: [['Rasterization', 'rasterization', 'Rasteriser', 'Rasterizer', 'rasteriser', 'rasterizer']],
            },
            {
              question_html: 'Vertex shader <code>out</code> variables are called <strong>{{blank}}</strong>. The rasterizer interpolates them across the triangle before passing them to the fragment shader.',
              answers: [['varyings', 'varying', 'Varyings']],
            },
            {
              question_html: 'The depth test runs in the <strong>{{blank}}</strong> stage, after the fragment shader has written a colour.',
              answers: [['Output Merger', 'output merger', 'per-fragment', 'per-fragment tests', 'output']],
            },
          ],
        },
      ],
    },

    // ── Task 2 ────────────────────────────────────────────────────────────────
    {
      title: 'Task 2 - Tracing a Vertex from CPU to Pixel',
      blocks: [
        {
          type: 'prose',
          html: `<p>Consider a textured triangle. One vertex has position <code>(0.5, 0.5, 0.0)</code>
          and texture coordinate <code>(1.0, 1.0)</code>. The following steps trace this vertex
          through every pipeline stage from CPU memory to a sampled colour on screen.</p>
          <ol>
            <li><strong>Application stage.</strong> You upload the vertex data into a VBO and call:
              <pre style="margin:0.5rem 0"><code class="language-cpp">glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));</code></pre>
              Location 0 holds the 3-component position and location 1 holds the 2-component texture
              coordinate.</li>
            <li><strong>Vertex shader.</strong> The shader receives the vertex via
              <code>layout(location=0) in vec3 aPos</code> and
              <code>layout(location=1) in vec2 aTexCoord</code>. It transforms the position and
              declares <code>out vec2 TexCoord</code> to pass the texture coordinate to the
              rasterizer. The MVP formula below shows the clip-space result written to
              <code>gl_Position</code>.</li>
          </ol>`,
        },
        {
          type: 'math',
          display: true,
          latex: String.raw`\texttt{gl\_Position} = \textbf{projection} \cdot \textbf{view} \cdot \textbf{model} \cdot \texttt{vec4(aPos,\;1.0)}`,
        },
        {
          type: 'prose',
          html: `<ol start="3">
            <li><strong>Primitive assembly and clipping.</strong> Three transformed vertices are
              grouped into a triangle. Fixed-function hardware clips any geometry whose clip-space
              coordinates fall outside the view frustum and discards it before rasterization.</li>
            <li><strong>Rasterization.</strong> The GPU determines which pixels the triangle covers
              and emits one fragment per covered pixel. The texture coordinate
              <code>(1.0, 1.0)</code> at this corner and the coordinates at the other two corners
              are linearly interpolated. A fragment at the centre receives a
              <code>TexCoord</code> roughly midway between all three corner values.</li>
            <li><strong>Fragment shader.</strong> The fragment receives the interpolated
              <code>in vec2 TexCoord</code>. The shader samples the bound texture with
              <code>texture(uTexture, TexCoord)</code> and writes the result to
              <code>out vec4 fragColour</code>.</li>
            <li><strong>Output merger.</strong> The depth value is compared to the value already
              stored in the depth buffer at that pixel. If this fragment is closer, it passes the
              depth test and its colour is written to the framebuffer. Otherwise it is discarded.</li>
          </ol>`,
        },
        {
          type: 'multiple_choice',
          interactive: true,
          questions: [
            {
              question: 'A vertex attribute declared as `layout(location=2) in vec2 aTexCoord` in the vertex shader corresponds to which glVertexAttribPointer call?',
              options: ['glVertexAttribPointer(0, ...)', 'glVertexAttribPointer(1, ...)', 'glVertexAttribPointer(2, ...)', 'glVertexAttribPointer(3, ...)'],
              correct: 2,
              explanation: 'The layout location qualifier (2) maps directly to the first argument (index) of glVertexAttribPointer. Location 2 corresponds to index 2.',
            },
            {
              question: 'Which value does the rasterizer interpolate across a triangle?',
              options: ['Uniform variables', 'gl_Position components', 'Vertex shader `out` varyings', 'Raw vertex data from the VBO'],
              correct: 2,
              explanation: 'Uniforms are constant across the draw call. gl_Position is consumed by the rasterizer to determine coverage. Raw VBO data goes through the vertex shader first. Only `out` variables from the vertex shader are linearly interpolated by the rasterizer.',
            },
            {
              question: 'A vertex shader declares `out vec3 vColour`. Where does this value go after the vertex shader finishes?',
              options: [
                'It is discarded — only gl_Position leaves the vertex shader.',
                'It is passed unchanged to the fragment shader as a per-vertex constant.',
                'It is linearly interpolated by the Rasterization stage and delivered per-fragment.',
                'It is written back into the VBO for the next draw call.',
              ],
              correct: 2,
              explanation: 'out variables from the vertex shader are varyings. The Rasterization stage interpolates them across the triangle. The fragment shader receives a per-fragment value, not the raw per-vertex value.',
            },
            {
              question: "At which stage is gl_FragCoord (the fragment's window-space pixel coordinates) available?",
              options: ['Vertex Shader', 'Primitive Assembly', 'Fragment Shader', 'Application (CPU)'],
              correct: 2,
              explanation: 'gl_FragCoord is a built-in input variable in the fragment shader. It contains the window-space coordinates of the fragment being processed, computed by the rasterizer.',
            },
            {
              question: 'The MVP formula in GLSL is written as `projection * view * model * vec4(aPos, 1.0)`. In what order are the matrices actually applied to the vertex?',
              options: [
                'projection first, then view, then model',
                'model first, then view, then projection',
                'All three are applied simultaneously by the GPU',
                'The order depends on which uniform was set most recently',
              ],
              correct: 1,
              explanation: 'Matrix-vector multiplication applies right to left. vec4(aPos, 1.0) is on the far right, so model is applied first (world space), then view (camera space), then projection (clip space). The left-to-right reading order in GLSL is the reverse of the mathematical application order.',
            },
          ],
        },
      ],
    },

    // ── Task 3 ────────────────────────────────────────────────────────────────
    {
      title: 'Task 3 - Programmable vs Fixed-Function Stages',
      blocks: [
        {
          type: 'prose',
          html: `<p>Two stages accept custom GLSL. The remaining four are fixed-function — their
          behaviour is configured through OpenGL state calls but cannot be replaced with shader code.
          Understanding this split explains why some bugs require a shader change and others require
          a state call.</p>`,
        },
        {
          type: 'fill_blanks',
          interactive: true,
          questions: [
            {
              question_html: 'You can write GLSL for the <strong>{{blank}}</strong> shader and the <strong>{{blank}}</strong> shader.',
              answers: [['Vertex', 'vertex'], ['Fragment', 'fragment']],
            },
            {
              question_html: 'Rasterization is a <strong>{{blank}}</strong>-function stage. You control it through state calls such as <code>glPolygonMode</code>, not by writing GLSL.',
              answers: [['fixed', 'Fixed']],
            },
            {
              question_html: 'The depth buffer comparison runs in the <strong>{{blank}}</strong> stage.',
              answers: [['Output Merger', 'output merger', 'per-fragment', 'per-fragment tests', 'output']],
            },
          ],
        },
        {
          type: 'multiple_choice',
          interactive: true,
          questions: [
            {
              question: 'Which OpenGL call configures the Output Merger stage?',
              options: [
                'glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, offset)',
                'glDrawArrays(GL_TRIANGLES, 0, 3)',
                'glEnable(GL_DEPTH_TEST)',
                'glUseProgram(shaderProgram)',
              ],
              correct: 2,
              explanation: 'glEnable(GL_DEPTH_TEST) activates the depth test, which runs at the Output Merger stage. glVertexAttribPointer configures vertex input. glDrawArrays issues the draw call from the Application stage. glUseProgram selects the shader program.',
            },
            {
              question: 'After the vertex shader runs, which stage groups the transformed vertices into triangles and clips geometry outside the view frustum?',
              options: [
                'Rasterization',
                'Fragment Shader',
                'Output Merger',
                'Primitive Assembly',
              ],
              correct: 3,
              explanation: 'Primitive Assembly is a fixed-function stage that follows the vertex shader. It groups vertices into the primitive type specified in glDrawArrays and clips any triangle whose clip-space coordinates fall outside the view volume before rasterization begins.',
            },
            {
              question: 'A scene has two overlapping triangles. Without glEnable(GL_DEPTH_TEST) the front triangle sometimes appears behind the back one depending on draw order. Which stage is responsible, and what additional call must be made each frame to fix it?',
              options: [
                'Rasterization — call glPolygonMode(GL_FRONT_AND_BACK, GL_FILL) each frame',
                'Output Merger — call glClear(GL_DEPTH_BUFFER_BIT) each frame',
                'Fragment Shader — write gl_FragDepth manually each frame',
                'Primitive Assembly — call glFrontFace(GL_CCW) each frame',
              ],
              correct: 1,
              explanation: 'The depth test runs at the Output Merger stage and requires glEnable(GL_DEPTH_TEST). You must also clear the depth buffer at the start of each frame with glClear(GL_DEPTH_BUFFER_BIT), otherwise stale depth values from the previous frame cause fragments to be incorrectly discarded.',
            },
          ],
        },
        {
          type: 'short_answer',
          interactive: true,
          questions: [
            {
              question: 'Explain what happens to geometry that falls outside the view frustum after the vertex shader has run. Which stage handles this, and can your GLSL code control it?',
              model_answer: 'After the vertex shader, geometry is in clip space. A fixed-function clipping stage (part of Primitive Assembly) discards or trims any triangle whose clip-space coordinates fall outside [-w, w] on any axis. Your GLSL does not control this clipping directly. You can influence what is clipped by adjusting the near and far planes in the projection matrix, which changes the shape of the frustum and therefore what lies inside it.',
            },
            {
              question: 'A fragment shader writes a fully opaque red colour (1.0, 0.0, 0.0, 1.0) to fragColour, but the pixel on screen appears black. Name two pipeline stages that could be responsible and give one plausible cause for each.',
              model_answer: '(1) Output Merger — depth test: a closer object already wrote a black colour and a depth value to the framebuffer at that pixel. The red fragment is behind it, fails the depth test, and is discarded. The black colour from the earlier draw remains.\n\n(2) Output Merger — blending: alpha blending is enabled with blend factors that zero out the output. For example, if GL_ZERO is used as the source blend factor, the red colour is multiplied by zero before writing, producing black. The fragment is not discarded but its colour is mathematically eliminated.',
            },
            {
              question: 'The depth test and alpha blending both run at the Output Merger stage, but they interact in a way that can produce incorrect results. Explain why drawing transparent objects (alpha less than 1.0) with depth writes enabled can cause artefacts, and describe the standard workaround.',
              model_answer: 'The depth test writes the closest fragment\'s depth value to the depth buffer when that fragment is drawn. If a transparent object is drawn first, its depth value is written even though it is partially see-through. A second opaque object drawn behind it then fails the depth test and is discarded, even though it should be visible through the transparent surface.\n\nThe standard workaround is to draw all opaque objects first (with depth test and depth writes on), then draw transparent objects in back-to-front order with depth writes disabled (glDepthMask(GL_FALSE)). This allows transparent objects to be blended correctly without their depth values blocking objects drawn later in the same frame.',
            },
          ],
        },
      ],
    },
  ],

  solution_code: `ANSWER KEY — The OpenGL Rendering Pipeline (ACCA6021 GP01)

Task 1 — Fill in the Blanks
1. Vertex           (first programmable stage is the Vertex Shader)
2. Assembly         (Primitive Assembly)
3. Rasterization    (also accepted: Rasteriser, Rasterizer)
4. varyings         (also accepted: varying)
5. Output Merger    (also accepted: per-fragment, per-fragment tests)

Task 2 — Multiple Choice
Q1: C — glVertexAttribPointer(2, ...)
    The layout location value (2) matches the first argument of glVertexAttribPointer.

Q2: C — Vertex shader out varyings
    Uniforms are constant across the draw call.
    gl_Position is consumed by the rasterizer to determine coverage, not interpolated.
    Raw VBO data does not bypass the vertex shader.
    Only out variables from the vertex shader are linearly interpolated.

Q3: C — Linearly interpolated by the Rasterization stage and delivered per-fragment
    The fragment shader receives an already-interpolated value, not the raw per-vertex value.

Q4: C — Fragment Shader
    gl_FragCoord is a built-in input available only in the fragment shader.

Q5: B — model first, then view, then projection
    Matrix multiplication applies right to left. model is closest to the vertex on the right.

Task 3 — Fill in the Blanks
1. Vertex / Fragment    (either order accepted)
2. fixed
3. Output Merger        (also accepted: per-fragment, per-fragment tests)

Task 3 — Multiple Choice
Q1: C — glEnable(GL_DEPTH_TEST)
    This configures the Output Merger stage.

Q2: D — Primitive Assembly
    Fixed-function stage that groups vertices into primitives and clips outside the frustum.

Q3: B — Output Merger — glClear(GL_DEPTH_BUFFER_BIT) each frame
    Depth test lives at the Output Merger. The depth buffer must be cleared each frame.

Task 3 — Short Answer Q1 (Clipping)
Fixed-function clipping in Primitive Assembly discards or trims triangles whose clip-space
coordinates fall outside [-w, w]. GLSL cannot control this directly; adjust near/far in the
projection matrix to change what falls inside the frustum.

Task 3 — Short Answer Q2 (Black pixel)
(1) Depth test failure at Output Merger — a closer object with a black colour already wrote to
    that pixel's depth buffer entry; the red fragment fails the test and is discarded.
(2) Blend state at Output Merger — GL_ZERO as the source blend factor multiplies the red colour
    by zero before writing, producing black even though the fragment is not discarded.

Task 3 — Short Answer Q3 (Transparency artefacts)
A transparent object drawn first writes its depth value, blocking opaque objects drawn behind it.
Fix: draw opaques first (depth writes on), then transparents back-to-front with
glDepthMask(GL_FALSE) so they blend without poisoning the depth buffer.`,

  languages: ['cpp', 'glsl'],
}, 'labs/acca6021/lab_GP01_opengl_pipeline.html');
