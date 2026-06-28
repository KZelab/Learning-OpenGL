import { buildAndWriteLab } from '../lab-assembler/dist/assembler.js';

// D3 pipeline diagram — runs in the browser as an IIFE with d3 and container pre-bound
const vizScript = `
  container.style.overflowX = 'auto';
  const w = Math.max(container.clientWidth || 0, 700);
  const h = 185;

  const stages = [
    { lines: ['Application'],           color: '#5a6268', sub: 'CPU' },
    { lines: ['Vertex', 'Shader'],      color: '#003366', sub: 'programmable' },
    { lines: ['Primitive','Assembly'],  color: '#5a6268', sub: 'fixed' },
    { lines: ['Rasterization'],         color: '#5a6268', sub: 'fixed' },
    { lines: ['Fragment','Shader'],     color: '#003366', sub: 'programmable' },
    { lines: ['Output','Merger'],       color: '#5a6268', sub: 'fixed' },
  ];
  const arrowLabels = ['VBO / uniforms', 'gl_Position', 'triangles', 'fragments', 'out_Color'];

  const n = stages.length;
  const arrowW = 50;
  const pad = 14;
  const boxW = Math.floor((w - 2*pad - (n-1)*arrowW) / n);
  const boxH = 60;
  const boxY = 48;
  const midY = boxY + boxH / 2;

  const svg = d3.select(container).append('svg')
    .attr('width', w).attr('height', h)
    .style('font-family', "'Segoe UI', Arial, sans-serif");

  svg.append('defs').append('marker')
    .attr('id', 'pipe-arrow').attr('markerWidth', 7).attr('markerHeight', 5)
    .attr('refX', 7).attr('refY', 2.5).attr('orient', 'auto')
    .append('polygon').attr('points', '0 0, 7 2.5, 0 5').attr('fill', '#888');

  stages.forEach((s, i) => {
    const x = pad + i * (boxW + arrowW);

    svg.append('rect')
      .attr('x', x).attr('y', boxY)
      .attr('width', boxW).attr('height', boxH)
      .attr('rx', 4).attr('fill', s.color);

    const lh = 14;
    const ly = midY - (s.lines.length - 1) * lh / 2;
    s.lines.forEach((line, li) =>
      svg.append('text')
        .attr('x', x + boxW / 2).attr('y', ly + li * lh)
        .attr('text-anchor', 'middle').attr('dominant-baseline', 'middle')
        .attr('fill', '#fff').attr('font-size', 10.5).attr('font-weight', '600')
        .text(line)
    );

    svg.append('text')
      .attr('x', x + boxW / 2).attr('y', boxY + boxH + 12)
      .attr('text-anchor', 'middle').attr('fill', '#777')
      .attr('font-size', 9).attr('font-style', 'italic')
      .text(s.sub);

    if (i < n - 1) {
      const ax = x + boxW;
      svg.append('line')
        .attr('x1', ax).attr('y1', midY).attr('x2', ax + arrowW).attr('y2', midY)
        .attr('stroke', '#888').attr('stroke-width', 1.5)
        .attr('marker-end', 'url(#pipe-arrow)');
      svg.append('text')
        .attr('x', ax + arrowW / 2).attr('y', boxY - 7)
        .attr('text-anchor', 'middle').attr('fill', '#555').attr('font-size', 8.5)
        .text(arrowLabels[i]);
    }
  });

  // Legend
  const legY = 163;
  [['#003366', 'Programmable (GLSL)'], ['#5a6268', 'Fixed-function']].forEach(([c, l], i) => {
    const lx = pad + i * 170;
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
  on screen. You will learn the name and purpose of each stage in the OpenGL rendering pipeline,
  which stages you can program in GLSL, and how to identify which stage is responsible when
  something goes wrong.</p>`,

  objectives: [
    'Name each stage of the OpenGL rendering pipeline and describe its input and output',
    'Distinguish programmable stages from fixed-function stages',
    'Trace vertex data from application memory through to a pixel colour in the framebuffer',
    'Explain what data passes between each pair of adjacent stages',
    'Identify which pipeline stage is responsible for common rendering artefacts',
  ],

  concept_html: `<p>When you call <code>glDrawArrays</code>, a white triangle appears on screen. But
  between that one API call and the final pixel, the GPU executes a sequence of distinct stages.
  Most pipeline bugs come from not knowing which stage transformed, discarded, or interpolated your data.</p>

  <p>The pipeline has seven stages. The <strong>Application</strong> stage runs on the CPU. Your code
  uploads vertex data into a <strong>Vertex Buffer Object (VBO)</strong>, describes its layout with
  <code>glVertexAttribPointer</code>, and supplies uniform values. The <strong>Vertex Shader</strong>
  is the first programmable stage: the GPU runs your GLSL once per vertex and you must write a value
  to <code>gl_Position</code> in clip space. <strong>Primitive Assembly</strong> groups the output
  vertices into triangles. The <strong>Rasterization</strong> stage walks each triangle, determines
  which pixels it covers, and emits one <strong>fragment</strong> per covered pixel. It also linearly
  interpolates any <code>out</code> variables declared in the vertex shader across the triangle
  surface. The <strong>Fragment Shader</strong> is the second programmable stage: it receives those
  interpolated values and must write a colour to <code>out_Color</code>. Finally, the
  <strong>Output Merger</strong> (also called per-fragment tests) applies the depth test, stencil
  test, and blending before writing surviving fragments into the framebuffer.</p>

  <p>Two stages accept GLSL code: the vertex shader and the fragment shader. The remaining stages
  are <strong>fixed-function</strong> — their behaviour is configured through OpenGL state calls
  such as <code>glEnable</code>, <code>glDepthFunc</code>, and <code>glBlendFunc</code>, but you
  cannot replace their logic with custom code. Understanding this split explains why some bugs
  require a shader change and others require a state call.</p>`,

  tasks: [
    // ── Task 1 ────────────────────────────────────────────────────────────────
    {
      title: 'Task 1 - Pipeline Overview',
      blocks: [
        {
          type: 'prose',
          html: `<p>The diagram below shows the six main stages of the OpenGL rendering pipeline.
          Navy boxes are programmable in GLSL. Study the diagram, then complete the blanks.</p>`,
        },
        {
          type: 'visualization',
          id:     'pipeline-diagram',
          height: 185,
          script: vizScript,
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
          and texture coordinate <code>(1.0, 1.0)</code>. Work through each stage below to trace
          this vertex from CPU memory to a sampled colour on screen.</p>
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
          latex: String.raw`\text{gl\_Position} = \mathbf{P} \cdot \mathbf{V} \cdot \mathbf{M} \cdot \mathbf{v}_{\text{local}}`,
        },
        {
          type: 'prose',
          html: `<ol start="3">
            <li><strong>Primitive assembly and clipping.</strong> Three transformed vertices are
              grouped into a triangle. Fixed-function hardware clips any geometry whose clip-space
              coordinates fall outside the view frustum and discards it before it reaches the
              rasterizer.</li>
            <li><strong>Rasterization.</strong> The GPU determines which pixels the triangle covers
              and emits one fragment per covered pixel. The texture coordinate
              <code>(1.0, 1.0)</code> at this corner and the coordinates at the other two corners
              are linearly interpolated. A fragment at the centre receives a
              <code>TexCoord</code> roughly midway between all three corner values.</li>
            <li><strong>Fragment shader.</strong> The fragment receives the interpolated
              <code>in vec2 TexCoord</code>. The shader samples the bound texture with
              <code>texture(uTexture, TexCoord)</code> and writes the result to
              <code>out vec4 out_Color</code>.</li>
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
              question: "At which stage is gl_FragCoord (the fragment's window-space pixel coordinates) available?",
              options: ['Vertex Shader', 'Primitive Assembly', 'Fragment Shader', 'Application (CPU)'],
              correct: 2,
              explanation: 'gl_FragCoord is a built-in input variable in the fragment shader. It contains the window-space coordinates of the fragment being processed, computed by the rasterizer.',
            },
          ],
        },
      ],
    },

    // ── Task 3 ────────────────────────────────────────────────────────────────
    {
      title: 'Task 3 - Programmable vs Fixed-Function',
      blocks: [
        {
          type: 'prose',
          html: `<p>Two stages accept custom GLSL. The others are configured through state calls but
          cannot be replaced. Complete the blanks and answer the questions below.</p>`,
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
          type: 'short_answer',
          interactive: true,
          questions: [
            {
              question: 'Explain what happens to geometry that falls outside the view frustum after the vertex shader has run. Which stage handles this, and can your GLSL code control it?',
              model_answer: 'After the vertex shader, geometry is in clip space. A fixed-function clipping stage (part of primitive assembly) discards or trims any triangle whose clip-space coordinates fall outside [-w, w] on any axis. Your GLSL does not control this clipping directly. You can influence what is clipped by adjusting the near and far planes in the projection matrix, which changes the shape of the frustum and therefore what lies inside it.',
            },
            {
              question: 'A fragment shader writes a fully opaque red colour (1.0, 0.0, 0.0, 1.0) to out_Color, but the pixel on screen appears black. Name two pipeline stages that could be responsible and give one plausible cause for each.',
              model_answer: '(1) Output Merger - depth test: a closer object already wrote a black colour and a depth value to the framebuffer at that pixel. The red fragment is behind it, fails the depth test, and is discarded. The black colour from the earlier draw remains.\n\n(2) Output Merger - blending: alpha blending is enabled with blend factors that zero out the output. For example, if GL_ZERO is used as the destination blend factor and an additive equation is set, the red colour could be blended away. The fragment is not discarded but its colour is mathematically reduced to black.',
            },
          ],
        },
      ],
    },
  ],

  solution_code: `ANSWER KEY - The OpenGL Rendering Pipeline (ACCA6021 GP01)

Task 1 - Fill in the Blanks
1. Vertex           (first programmable stage is the Vertex Shader)
2. Assembly         (Primitive Assembly)
3. Rasterization    (also accepted: Rasteriser, Rasterizer)
4. varyings         (also accepted: varying)
5. Output Merger    (also accepted: per-fragment, per-fragment tests)

Task 2 - Multiple Choice
Q1: C - glVertexAttribPointer(2, ...)
    The layout location value (2) matches the first argument of glVertexAttribPointer.
    Location 2 maps to index 2.

Q2: C - Vertex shader out varyings
    Uniforms are constant across the draw call.
    gl_Position is consumed by the rasterizer to determine coverage, not interpolated.
    Raw VBO data does not bypass the vertex shader.
    Only out variables from the vertex shader are linearly interpolated by the rasterizer.

Q3: C - Fragment Shader
    gl_FragCoord is a built-in input available only in the fragment shader.
    It is computed by the rasterizer and contains window-space pixel coordinates.

Task 3 - Fill in the Blanks
1. Vertex / Fragment    (either order accepted)
2. fixed
3. Output Merger        (also accepted: per-fragment, per-fragment tests)

Task 3 - Short Answer Q1 (Clipping)
After the vertex shader, geometry is in clip space. Fixed-function clipping (part of primitive
assembly) discards or trims any triangle whose clip-space coordinates fall outside [-w, w] on any
axis. Your GLSL does not control this. You can influence it indirectly by adjusting near and far
in the projection matrix, which changes the frustum shape and therefore what gets clipped.

Task 3 - Short Answer Q2 (Black pixel causes)
(1) Depth test failure at the Output Merger: a closer object already wrote a black colour and a
depth value at that pixel. The red fragment fails the depth test and is discarded. The black
colour from the earlier object remains in the framebuffer.

(2) Blend state at the Output Merger: alpha blending is enabled with blend factors that zero the
output. For example, GL_ZERO as the source factor multiplies the red colour by zero, producing
black before writing. The fragment is not discarded but its colour is blended away.`,

  languages: ['cpp', 'glsl'],
}, 'labs/acca6021/math_WS_GP01_graphics_pipeline.html');
