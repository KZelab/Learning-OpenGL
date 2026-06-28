import { buildAndWriteLab } from '../tools/lab-assembler/dist/assembler.js';

buildAndWriteLab({
  options: { solution: true, copy_protect: false },
  module_code: 'ACCA6021',
  module_title: 'Graphics and Games Programming',
  lab_number: 'M02',
  lab_title: 'The Identity Matrix',

  overview_html: `<p>This worksheet explores the <strong>identity matrix</strong>, the cornerstone of every linear transform in OpenGL. You will construct a 4x4 identity matrix, verify its multiplicative property by hand calculation, and recognise how GLM uses it as the default starting point for model, view, and projection matrices.</p>`,

  objectives: [
    'By the end of this worksheet you will be able to construct a 4x4 identity matrix from memory',
    'By the end of this worksheet you will be able to verify the multiplicative identity property I * M = M by hand',
    'By the end of this worksheet you will be able to explain why glm::mat4(1.0f) produces an identity matrix in C++',
    'By the end of this worksheet you will be able to describe why the identity matrix is the correct starting point for transform chains in OpenGL',
  ],

  concept_html: `<p>Without a neutral element for matrix multiplication, there is no safe starting point for building transform chains. When you construct a model matrix in OpenGL, you combine rotations, translations, and scales by multiplying matrices together. You need a matrix that contributes nothing on its own. In ordinary arithmetic, the number <strong>1</strong> plays this role. The product of 1 and x always equals x, regardless of x. Matrices have an exact equivalent.</p>
<p>The <strong>identity matrix</strong>, written as <strong>I</strong>, is a square matrix with the value 1 on every entry of the <strong>main diagonal</strong> (running from the top-left entry to the bottom-right) and 0 everywhere else. Its defining property is that multiplying any conformable matrix M by I from either side leaves M completely unchanged. This relationship, written I * M = M * I = M, makes I the <strong>multiplicative identity</strong> for matrices, exactly as 1 is the multiplicative identity for ordinary numbers.</p>
<p>In GLM, the C++ maths library used alongside OpenGL, you create a 4x4 identity matrix by writing <code>glm::mat4(1.0f)</code>. The single float argument fills the four diagonal entries with that value and sets all other entries to zero. Writing <code>glm::mat4(0.0f)</code> instead produces an all-zero matrix, which destroys any vector multiplied by it. The convention of starting every transform with <code>glm::mat4(1.0f)</code> and then applying <code>glm::translate</code>, <code>glm::rotate</code>, and <code>glm::scale</code> on top of it follows directly from this identity property.</p>`,

  tasks: [
    {
      title: 'Task 1 - The Structure of the Identity Matrix',
      blocks: [
        {
          type: 'prose',
          html: `<p>Study the 4x4 identity matrix below, then answer the questions that follow.</p>
<pre><code>| 1  0  0  0 |
| 0  1  0  0 |
| 0  0  1  0 |
| 0  0  0  1 |</code></pre>
<p>The <strong>main diagonal</strong> runs from the top-left entry (row 1, column 1) to the bottom-right entry (row 4, column 4). Every entry on this diagonal is 1. Every other entry is 0.</p>`,
        },
        {
          type: 'fill_blanks',
          interactive: true,
          questions: [
            {
              question_html: 'The value placed on the main diagonal of an identity matrix is <strong>{{blank}}</strong>.',
              answers: [['1', 'one']],
            },
            {
              question_html: 'Every entry that is not on the main diagonal is <strong>{{blank}}</strong>.',
              answers: [['0', 'zero']],
            },
            {
              question_html: 'A 4x4 identity matrix has <strong>{{blank}}</strong> non-zero entries.',
              answers: [['4', 'four']],
            },
            {
              question_html: 'The identity matrix is only defined for <strong>{{blank}}</strong> matrices.',
              answers: [['square']],
            },
          ],
        },
        {
          type: 'multiple_choice',
          interactive: true,
          questions: [
            {
              question: 'Which of the following correctly describes the identity matrix?',
              options: [
                'A matrix with 1s everywhere',
                'A matrix with 1s on the main diagonal and 0s elsewhere',
                'A matrix with 0s on the main diagonal and 1s elsewhere',
                'A matrix with alternating 1s and 0s on each row',
              ],
              correct: 1,
              explanation: 'The identity matrix has exactly 1 on each diagonal entry and 0 for all other entries. Multiplying any matrix by it leaves that matrix unchanged.',
            },
          ],
        },
      ],
    },

    {
      title: 'Task 2 - The Multiplicative Identity Property',
      blocks: [
        {
          type: 'prose',
          html: `<p>The <strong>multiplicative identity property</strong> states that for any matrix M of appropriate size, multiplying by the identity matrix I leaves M unchanged. This is written as I * M = M * I = M.</p>
<p>Work through the example below. Let M be a 2x2 matrix and I be the 2x2 identity:</p>
<pre><code>    | 1  0 |         | 3  7 |
I = |      |    M =  |      |
    | 0  1 |         | 2  5 |</code></pre>
<p>To compute I * M, multiply each row of I by each column of M and sum the products:</p>
<pre><code>Row 1, Col 1:  (1)(3) + (0)(2) = ?
Row 1, Col 2:  (1)(7) + (0)(5) = ?
Row 2, Col 1:  (0)(3) + (1)(2) = ?
Row 2, Col 2:  (0)(7) + (1)(5) = ?</code></pre>
<p>Fill in the four results below.</p>`,
        },
        {
          type: 'fill_blanks',
          interactive: true,
          questions: [
            {
              question_html: 'Row 1, Col 1: (1)(3) + (0)(2) = <strong>{{blank}}</strong>',
              answers: [['3']],
            },
            {
              question_html: 'Row 1, Col 2: (1)(7) + (0)(5) = <strong>{{blank}}</strong>',
              answers: [['7']],
            },
            {
              question_html: 'Row 2, Col 1: (0)(3) + (1)(2) = <strong>{{blank}}</strong>',
              answers: [['2']],
            },
            {
              question_html: 'Row 2, Col 2: (0)(7) + (1)(5) = <strong>{{blank}}</strong>',
              answers: [['5']],
            },
          ],
        },
        {
          type: 'short_answer',
          interactive: true,
          questions: [
            {
              question: 'The result above equals M exactly. Explain why multiplying by the identity matrix always returns the original matrix. What feature of I causes each entry of M to pass through unchanged?',
              model_answer: 'The identity matrix has 1s on the diagonal and 0s everywhere else. When computing each entry of the result, only one term in the dot product is non-zero: the term where the diagonal 1 aligns with the corresponding entry of M. All other terms are zeroed out by the off-diagonal 0s. This means each entry of M is selected exactly once and passes through to the result without modification.',
            },
          ],
        },
      ],
    },

    {
      title: 'Task 3 - The Identity Matrix in GLM',
      blocks: [
        {
          type: 'prose',
          html: `<p>GLM stores matrices in <strong>column-major</strong> order, matching the layout OpenGL expects on the GPU. When you call <code>glm::mat4(1.0f)</code>, the constructor sets each of the four diagonal entries to <code>1.0f</code> and all other entries to <code>0.0f</code>, producing the 4x4 identity matrix.</p>
<p>The code below initialises a model matrix to the identity and then applies a translation:</p>`,
        },
        {
          type: 'code',
          lang: 'cpp',
          code_type: 'reference',
          filename: 'typical transform setup',
          content: String.raw`glm::mat4 model(1.0f);   // identity - neutral starting point
model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));  // shift right by 2 units`,
        },
        {
          type: 'fill_blanks',
          interactive: true,
          questions: [
            {
              question_html: 'To create a 4x4 identity matrix in GLM, write <code>glm::mat4({{blank}})</code>.',
              answers: [['1.0f', '1.0', '1']],
            },
            {
              question_html: 'The float argument to the constructor sets the value of all <strong>{{blank}}</strong> entries.',
              answers: [['diagonal']],
            },
            {
              question_html: 'Writing <code>glm::mat4(0.0f)</code> produces an all-<strong>{{blank}}</strong> matrix.',
              answers: [['zero', '0']],
            },
          ],
        },
        {
          type: 'multiple_choice',
          interactive: true,
          questions: [
            {
              question: 'Why is glm::mat4(0.0f) almost never a correct starting point for a model matrix?',
              options: [
                '0.0f is not a valid float value in C++',
                'An all-zero matrix maps every vertex to the origin, destroying all geometry',
                'GLM does not accept 0.0f as a constructor argument for mat4',
                'The identity matrix requires 1.0f on all 16 entries, not just the diagonal',
              ],
              correct: 1,
              explanation: 'Multiplying any vector by an all-zero matrix yields the zero vector. Every vertex in the mesh would collapse to the origin and nothing would appear on screen. The identity is the correct neutral starting point.',
            },
          ],
        },
      ],
    },

    {
      title: 'Task 4 - Identity in the Transform Pipeline',
      blocks: [
        {
          type: 'prose',
          html: `<p>Every model matrix in a typical OpenGL scene begins as an identity matrix. Transforms are then layered on top using GLM functions that multiply the current matrix by a new transform matrix. Starting from the identity ensures no unexpected rotation, scale, or translation is already present when the first transform is applied.</p>
<p>Study the typical per-object matrix setup below:</p>`,
        },
        {
          type: 'code',
          lang: 'cpp',
          code_type: 'reference',
          filename: 'render loop',
          content: String.raw`glm::mat4 model(1.0f);                                                      // reset to identity each frame
model = glm::translate(model, position);                                        // apply translation
model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f)); // apply rotation
model = glm::scale(model, scale);                                               // apply scale
shader.setMat4("model", model);                                                 // send to GPU`,
        },
        {
          type: 'short_answer',
          interactive: true,
          questions: [
            {
              question: 'A student reuses the same model matrix variable across multiple draw calls without resetting it to identity each frame. Describe what will happen to the rendered objects after several frames, and state the correct fix.',
              model_answer: `Each frame, the new transforms are multiplied on top of the previous frame's accumulated matrix rather than replacing it. After several frames the object will have been translated, rotated, and scaled many times over, causing it to drift rapidly off-screen or appear wildly distorted. The fix is to reset the matrix to glm::mat4(1.0f) at the start of each draw call so each frame applies a fresh set of transforms to a clean identity base.`,
            },
          ],
        },
        {
          type: 'multiple_choice',
          interactive: true,
          questions: [
            {
              question: 'Which line correctly resets a model matrix to the identity at the start of a render loop?',
              options: [
                'glm::mat4 model;',
                'model = glm::mat4(0.0f);',
                'model = glm::mat4(1.0f);',
                'model = glm::mat4();',
              ],
              correct: 2,
              explanation: 'glm::mat4(1.0f) explicitly sets the diagonal to 1.0f and all other entries to 0.0f, producing the identity. A default-constructed glm::mat4 without arguments leaves memory uninitialised in some build configurations. glm::mat4(0.0f) produces an all-zero matrix.',
            },
          ],
        },
      ],
    },
  ],

  solution_code: String.raw`// WS07 - The Identity Matrix - Answer Key
//
// Task 1 - Structure
//   Main diagonal value:          1
//   Off-diagonal value:           0
//   Non-zero entries in 4x4 I:   4
//   Matrix type:                  square
//   Correct description: "1s on the main diagonal and 0s elsewhere"
//
// Task 2 - Multiplicative Property
//   Row 1, Col 1: (1)(3) + (0)(2) = 3
//   Row 1, Col 2: (1)(7) + (0)(5) = 7
//   Row 2, Col 1: (0)(3) + (1)(2) = 2
//   Row 2, Col 2: (0)(7) + (1)(5) = 5
//   Result: [[3, 7], [2, 5]] = M  (M is unchanged, confirming I * M = M)
//
//   Why it works: the diagonal 1s each select exactly one entry of M.
//   The off-diagonal 0s eliminate all other contributions.
//   Every entry of M passes through to the result without change.
//
// Task 3 - GLM
//   Constructor call:      glm::mat4(1.0f)
//   Entries set by arg:    diagonal
//   mat4(0.0f) produces:   zero matrix
//   Why 0.0f is wrong:     all-zero matrix maps every vertex to the origin
//
// Task 4 - Transform Pipeline
//   Correct reset line:    model = glm::mat4(1.0f);
//
//   Reusing without reset: transforms accumulate each frame.
//   After N frames the object has been moved/rotated/scaled N times
//   and will have drifted far from its intended position.
//   Fix: reset to glm::mat4(1.0f) at the start of each draw call.
//
// Full usage example:
glm::mat4 model(1.0f);
model = glm::translate(model, position);
model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
model = glm::scale(model, scale);
shader.setMat4("model", model);`,

  languages: ['cpp'],
}, 'labs/mcp/math_WS02_identity_matrix.html');
