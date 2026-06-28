import { buildAndWriteLab } from '../tools/lab-assembler/dist/assembler.js';

buildAndWriteLab({
  options: { solution: true, copy_protect: false },
  module_code: 'ACCA6021',
  module_title: 'Graphics and Games Programming',
  lab_number: 'M04',
  lab_title: 'Column-Major Convention and GLM',

  overview_html: `<p>This worksheet addresses the single most common source of matrix bugs in OpenGL code: the difference between how a matrix looks on paper and how GLM stores it in memory. You will read GLM matrix memory correctly, explain why multiplication is written M * v and not v * M, and write GLM function calls in the right order to produce a desired transform sequence.</p>`,

  objectives: [
    'By the end of this worksheet you will be able to state what column-major storage means and show how a translation matrix is laid out in GLM memory',
    'By the end of this worksheet you will be able to explain why matrix-vector multiplication is written M * v in GLM, not v * M',
    'By the end of this worksheet you will be able to identify a wrong GLM call order from its on-screen symptom and write the correct version',
    'By the end of this worksheet you will be able to write GLM function calls in the order that produces a specified sequence of transforms',
  ],

  concept_html: `<p>A student writes a translation matrix on paper with the offset in the top-right corner, looks at the raw memory from GLM, and sees the offset at a completely different position. They assume the matrix is transposed and start adding calls to <code>glm::transpose</code>. The object still moves in the wrong direction. This misunderstanding causes more assignment bugs in this module than any other single topic.</p>
<p><strong>Row-major</strong> storage places the first row of a matrix in memory first: elements [0..3] are row 0, [4..7] are row 1, and so on. <strong>Column-major</strong> storage (used by OpenGL and GLM) places the first column in memory first: elements [0..3] are column 0, [4..7] are column 1, and so on. The same mathematical matrix looks transposed when you read the raw memory as if it were row-major. The matrix itself is not transposed. Only the storage order differs. In GLM, the correct indexing is <code>mat[col][row]</code>, not <code>mat[row][col]</code>.</p>
<p>Multiplication order follows from the column-vector convention: the matrix goes on the left, the column vector on the right — <code>M * v</code>. GLM function calls <strong>left-multiply</strong> the accumulated matrix. Calling <code>model = glm::rotate(model, angle, axis)</code> gives <code>model = R * model</code>. A call written later in the source applies to the geometry <em>before</em> a call written earlier. Write GLM calls in the reverse of the mathematical order you want the transforms applied.</p>`,

  tasks: [
    {
      title: 'Task 1 - Row-Major vs Column-Major Side by Side',
      blocks: [
        {
          type: 'prose',
          html: `<p>Use the translation matrix T(3, 1, 0) throughout this task.</p>
<p><strong>How it looks on paper</strong> (row-major reading — each row displayed left to right):</p>
<pre>Math notation:
| 1  0  0  3 |
| 0  1  0  1 |
| 0  0  1  0 |
| 0  0  0  1 |</pre>
<p><strong>How GLM stores it in memory</strong> (column-major — first column occupies indices 0-3, second column 4-7, etc.):</p>
<pre>GLM memory (16 floats, column by column):
Index: [ 0][ 1][ 2][ 3]  [ 4][ 5][ 6][ 7]  [ 8][ 9][10][11]  [12][13][14][15]
Value:    1   0   0   0     0   1   0   0     0   0   1   0      3   1   0   1
         ---- col 0 ----   ---- col 1 ----   ---- col 2 ----   ---- col 3 ----</pre>
<p>Key observation: the translation values (3, 1, 0) appear at memory indices 12, 13, and 14. In column-major layout, column 3 (the rightmost column of the matrix as written on paper) is stored last in memory.</p>
<p>A common mistake: reading the raw memory row by row and concluding the matrix is transposed. It is not transposed. The storage order simply differs from the reading convention. If you access <code>mat[0][3]</code> in GLM you get the translation value 3 — because GLM uses <code>mat[col][row]</code> indexing, and column 0, row 3 is the bottom-left entry of the matrix, which is 0 for a translation matrix. The translation is at <code>mat[3][0]</code>, <code>mat[3][1]</code>, and <code>mat[3][2]</code> (column 3, rows 0-2).</p>`,
        },
        {
          type: 'fill_blanks',
          interactive: true,
          questions: [
            {
              question_html: 'In GLM, the correct indexing order is <code>mat[{{blank}}][row]</code>, not <code>mat[row][col]</code>.',
              answers: [['col', 'column']],
            },
            {
              question_html: 'For T(3, 1, 0), the translation value 3 is stored at GLM memory index <strong>{{blank}}</strong>.',
              answers: [['12']],
            },
            {
              question_html: 'The translation values in a 4x4 matrix occupy the <strong>{{blank}}</strong> column (0-indexed) in GLM memory.',
              answers: [['3', 'third', 'fourth']],
            },
            {
              question_html: 'In column-major storage, elements [0..3] in memory represent column <strong>{{blank}}</strong> of the matrix.',
              answers: [['0', 'zero']],
            },
          ],
        },
      ],
    },

    {
      title: 'Task 2 - Wrong Multiplication Order',
      blocks: [
        {
          type: 'prose',
          html: `<p>A student wants a cube to spin on its own axis while positioned at (5, 0, 0) in the scene. The desired behaviour: the cube rotates around its own local Y axis without moving from x = 5. The student writes the following code. Study the symptom, then answer the questions below.</p>
<p><strong>Observed symptom:</strong> instead of spinning in place at x = 5, the cube swings in a wide arc around the world origin, tracing a circle on the XZ plane.</p>`,
        },
        {
          type: 'code',
          lang: 'cpp',
          code_type: 'bug',
          filename: 'buggy render loop',
          content: String.raw`glm::mat4 model(1.0f);
model = glm::translate(model, glm::vec3(5.0f, 0.0f, 0.0f));  // line A
model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));  // line B`,
        },
        {
          type: 'short_answer',
          interactive: true,
          questions: [
            {
              question: 'Which line causes the wrong behaviour, and why does it produce the orbiting symptom instead of spinning in place?',
              model_answer: 'Line B causes the problem. GLM left-multiplies each call onto the accumulated matrix, so calling rotate after translate gives model = R * T. In mathematical terms, the rotation is applied AFTER the translation. The geometry is first moved to x=5 and then rotated around the world origin, which makes the cube orbit. The correct mathematical order for "spin first, then move" is T * R, which requires rotate to be called FIRST in code (so it is on the right in the accumulated product).',
            },
            {
              question: 'Write the corrected two-line GLM code that makes the cube spin on its own axis at position (5, 0, 0).',
              model_answer: `glm::mat4 model(1.0f);
model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));  // rotate first in code
model = glm::translate(model, glm::vec3(5.0f, 0.0f, 0.0f));       // translate second in code
// Mathematical result: model = T * R  (rotation applied to geometry first, then translation)`,
            },
          ],
        },
      ],
    },

    {
      title: 'Task 3 - Write the GLM Calls in the Right Order',
      blocks: [
        {
          type: 'prose',
          html: `<p>Desired transform (described in words): translate the object to (0, 2, 0), then rotate it 30 degrees around the Z axis. The desired mathematical matrix is T(0, 2, 0) * R_z(30).</p>
<ol>
  <li>Write the two GLM calls in the correct order to achieve T * R_z (remember: GLM calls are written in reverse of the mathematical application order).</li>
  <li>Trace the vertex v = (1, 0, 0, 1) through the transform by hand.
    <br>cos(30) = sqrt(3)/2 (approx 0.866), sin(30) = 0.5.
    <br>R_z(30) * (1, 0, 0, 1) = (cos30, sin30, 0, 1) = (0.866, 0.5, 0, 1)
    <br>Then apply T(0, 2, 0): add (0, 2, 0) to the position.
  </li>
</ol>`,
        },
        {
          type: 'short_answer',
          interactive: true,
          questions: [
            {
              question: 'Write the two GLM calls in the correct order to produce T(0, 2, 0) * R_z(30). State which call goes first and why.',
              model_answer: `glm::mat4 model(1.0f);
model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));  // first: rotation
model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));                      // second: translation
// rotate is called first because GLM left-multiplies, so the last call in math order
// must be the last call in code. R applied first means rotate call goes first in code.`,
            },
            {
              question: 'Trace v = (1, 0, 0, 1) through T(0,2,0) * R_z(30) step by step. What is the final position?',
              model_answer: `Step 1 - apply R_z(30) to (1, 0, 0, 1):
  R_z(30) * (1,0,0,1) = (cos30, sin30, 0, 1) = (sqrt(3)/2, 0.5, 0, 1) approx (0.866, 0.5, 0, 1)

Step 2 - apply T(0, 2, 0) to (0.866, 0.5, 0, 1):
  x stays 0.866, y becomes 0.5 + 2 = 2.5, z stays 0.

Final position: (sqrt(3)/2, 2.5, 0, 1) approx (0.866, 2.5, 0, 1)`,
            },
          ],
        },
      ],
    },
  ],

  solution_code: String.raw`WS04 - Column-Major Convention and GLM - Answer Key

Task 1 - Fill-in Blanks
  Correct indexing: mat[col][row]
  Memory index of value 3 (tx): 12
  Translation column index: 3 (the fourth column, 0-indexed)
  Elements [0..3] represent: column 0

Task 2 - Bug Analysis
  Buggy line: line B (rotate called AFTER translate)
  Reason: GLM left-multiplies, so result is R * T (rotation around world origin, not local axis)
  Symptom: cube orbits the origin instead of spinning in place

  Correct code:
    model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));   // first
    model = glm::translate(model, glm::vec3(5.0f, 0.0f, 0.0f));        // second
  Mathematical result: T * R (rotation applied first, translation second)

Task 3 - Correct GLM Order for T(0,2,0) * R_z(30)
  model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0,0,1));  // first call
  model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));         // second call

  Vertex trace for v = (1, 0, 0, 1):
    After R_z(30):     (sqrt(3)/2, 0.5, 0, 1) approx (0.866, 0.5, 0, 1)
    After T(0, 2, 0):  (sqrt(3)/2, 2.5, 0, 1) approx (0.866, 2.5, 0, 1)`,

  languages: ['cpp'],
}, 'labs/mcp/math_WS04_column_major_and_glm.html');
