import { buildAndWriteLab } from '../tools/lab-assembler/dist/assembler.js';

buildAndWriteLab({
  options: { solution: true, copy_protect: false },
  module_code: 'ACCA6021',
  module_title: 'Graphics and Games Programming',
  lab_number: 'M07',
  lab_title: 'Normal Vector Transformation',

  overview_html: `<p>This worksheet explains why surface normals cannot be transformed with the model matrix when a non-uniform scale is present, derives the correct normal matrix, and shows when the shortcut of using the model matrix directly is safe. These concepts underpin every Phong and PBR lighting calculation in ACCB6014.</p>`,

  objectives: [
    'By the end of this worksheet you will be able to demonstrate with a concrete example why applying the model matrix directly to a normal vector breaks perpendicularity under non-uniform scale',
    'By the end of this worksheet you will be able to state the correct normal matrix formula and explain intuitively why it works',
    'By the end of this worksheet you will be able to compute the normal matrix for a given model matrix and write the C++ and GLSL code to use it',
    'By the end of this worksheet you will be able to list the conditions under which the model matrix can be used directly as the normal matrix',
  ],

  concept_html: `<p>When a model is scaled non-uniformly (differently along each axis), applying the same model matrix M to its surface normals rotates them to point in the wrong direction. The dot product between the wrong normal and the light direction gives the wrong angle. Lighting becomes visibly incorrect: surfaces that should appear brightly lit appear dark, or vice versa. This artefact is immediately noticeable in Phong shading and fatal in PBR.</p>
<p>A <strong>normal</strong> is not an ordinary direction vector. It encodes a perpendicularity constraint: for every surface tangent vector t, the normal n must satisfy n dot t = 0. When tangents are transformed by M, normals must be transformed by a different matrix N so that the constraint is preserved. The correct matrix is N = (M<sup>-1</sup>)<sup>T</sup>, the <strong>inverse-transpose</strong> of M. For orthonormal matrices (pure rotations), (M<sup>-1</sup>)<sup>T</sup> = M, so using the model matrix directly is safe. For non-uniform scales it is not.</p>
<p>In C++, compute the normal matrix on the CPU and pass it as a uniform: <code>glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)))</code>. In the GLSL vertex shader, apply it with <code>vec3 worldNormal = normalize(normalMatrix * aNormal)</code>. Always use the 3x3 block — normals are directions and directions ignore the translation column.</p>`,

  tasks: [
    {
      title: 'Task 1 - The Failure Case',
      blocks: [
        {
          type: 'prose',
          html: `<p>Setup: surface tangent <strong>t</strong> = (1, 1, 0) and normal <strong>n</strong> = (-1, 1, 0). These are perpendicular.</p>
<p><strong>Verification:</strong></p>
<pre>n dot t = (-1)(1) + (1)(1) + (0)(0) = -1 + 1 + 0 = 0  ✓  perpendicular</pre>
<p>Apply non-uniform scale M = scale(2, 1, 1):</p>
<pre>M:
| 2  0  0  0 |
| 0  1  0  0 |
| 0  0  1  0 |
| 0  0  0  1 |</pre>
<p><strong>Transform the tangent:</strong></p>
<pre>Mt = M * (1, 1, 0, 0) = (2, 1, 0, 0)   (only the 3x3 block matters for directions)</pre>
<p><strong>Naively transform the normal with the same M:</strong></p>
<pre>Mn = M * (-1, 1, 0, 0) = (-2, 1, 0, 0)</pre>
<p><strong>Check perpendicularity:</strong></p>
<pre>Mt dot Mn = (2)(-2) + (1)(1) + (0)(0) = -4 + 1 = -3  ✗  NOT perpendicular</pre>
<p>The naively-transformed normal no longer satisfies the perpendicularity constraint. Lighting computed with this normal will be wrong.</p>
<p><strong>Compute the correct normal using the inverse-transpose:</strong></p>
<pre>M^-1 = scale(1/2, 1, 1):
| 1/2  0  0 |
|  0   1  0 |
|  0   0  1 |

(M^-1)^T = transpose of a diagonal matrix = the same diagonal matrix:
| 1/2  0  0 |
|  0   1  0 |
|  0   0  1 |

Apply (M^-1)^T to n = (-1, 1, 0):
correct_n = (1/2 * -1,  1 * 1,  1 * 0) = (-1/2, 1, 0)</pre>
<p><strong>Check perpendicularity with the correct normal:</strong></p>
<pre>Mt dot correct_n = (2)(-1/2) + (1)(1) + (0)(0) = -1 + 1 = 0  ✓  perpendicular</pre>
<p>The correct normal should be renormalised before use in lighting calculations: normalise(-1/2, 1, 0) = (-0.447, 0.894, 0).</p>`,
        },
        {
          type: 'fill_blanks',
          interactive: true,
          questions: [
            {
              question_html: 'The original n dot t = <strong>{{blank}}</strong>, confirming they are perpendicular.',
              answers: [['0', 'zero']],
            },
            {
              question_html: 'After naive transform: Mt dot Mn = <strong>{{blank}}</strong>, confirming perpendicularity is broken.',
              answers: [['-3']],
            },
            {
              question_html: 'The correct normal matrix is written as (M<sup><strong>{{blank}}</strong></sup>)<sup>T</sup>.',
              answers: [['-1']],
            },
            {
              question_html: 'After applying the correct normal matrix: Mt dot correct_n = <strong>{{blank}}</strong>.',
              answers: [['0', 'zero']],
            },
          ],
        },
      ],
    },

    {
      title: 'Task 2 - Why the Inverse-Transpose Works',
      blocks: [
        {
          type: 'prose',
          html: `<p>The following is an intuitive argument, not a formal proof. The goal is to understand <em>why</em> the formula N = (M<sup>-1</sup>)<sup>T</sup> preserves the perpendicularity constraint.</p>
<p>We need a matrix N such that (Mt) dot (Nn) = 0 whenever t dot n = 0.</p>
<p>Rewrite the dot product as a matrix product: (Mt)<sup>T</sup>(Nn) = 0, which expands to t<sup>T</sup> M<sup>T</sup> N n = 0.</p>
<p>For this to hold whenever t<sup>T</sup> n = 0, we need M<sup>T</sup> N = I (the identity). Solving for N:</p>
<pre>M^T * N = I
N = (M^T)^-1 = (M^-1)^T</pre>
<p>In plain language: the normal matrix is the matrix that undoes what M does to the perpendicularity relationship. The result N = transpose(inverse(M)) is exactly the operation needed.</p>
<p><strong>When does this simplify?</strong></p>
<p>If M is a pure rotation: M<sup>-1</sup> = M<sup>T</sup>, so N = (M<sup>T</sup>)<sup>T</sup> = M. Using the model matrix directly is safe for pure rotations.</p>
<p>If M is a uniform scale S*I: N = (1/S)*I, which only changes the magnitude of n. Renormalising the normal after applying M fixes this, so the model matrix (or its 3x3 block) is also safe after renormalisation.</p>`,
        },
        {
          type: 'multiple_choice',
          interactive: true,
          questions: [
            {
              question: 'For a pure rotation matrix R, what is the inverse-transpose (R^-1)^T?',
              options: [
                'The zero matrix',
                'R itself (the rotation matrix unchanged)',
                'The negative of R',
                'R transposed twice, which is undefined',
              ],
              correct: 1,
              explanation: 'For a rotation matrix, R^-1 = R^T. Therefore (R^-1)^T = (R^T)^T = R. The normal matrix equals the model matrix for pure rotations, so the shortcut is safe.',
            },
          ],
        },
      ],
    },

    {
      title: 'Task 3 - Compute the Normal Matrix',
      blocks: [
        {
          type: 'prose',
          html: `<p><strong>Matrix A:</strong> M = scale(3, 3, 3) — a uniform scale.</p>
<ol>
  <li>Compute (M<sup>-1</sup>)<sup>T</sup> for this matrix.</li>
  <li>Is the shortcut (using M directly as the normal matrix) safe here?</li>
</ol>
<p><strong>Matrix B:</strong> M = scale(2, 1, 1) composed with an arbitrary rotation R. Do not compute the product by hand.</p>
<ol>
  <li>Is the shortcut safe for matrix B?</li>
  <li>Write the C++ GLM code to compute the correct normal matrix for model matrix B.</li>
  <li>Write the GLSL vertex shader line that applies the normal matrix to the vertex normal.</li>
</ol>`,
        },
        {
          type: 'short_answer',
          interactive: true,
          questions: [
            {
              question: 'Matrix A = scale(3,3,3). Compute (M^-1)^T. Is the model matrix shortcut safe? Explain.',
              model_answer: `M^-1 = scale(1/3, 1/3, 1/3).
(M^-1)^T = scale(1/3, 1/3, 1/3)  (diagonal matrix: transpose equals itself).

The shortcut IS safe here. Uniform scale only changes the magnitude of the normal, not its direction. After renormalising (which lighting code always does), the result is identical to using the inverse-transpose. Using M or (M^-1)^T both give correctly oriented normals after normalisation.`,
            },
            {
              question: 'Matrix B = scale(2,1,1) * R. Is the shortcut safe? Write the C++ GLM code for the correct normal matrix and the GLSL line to apply it.',
              model_answer: `The shortcut is NOT safe. M contains a non-uniform scale (2 along X, 1 along Y and Z). Non-uniform scale distorts normal directions in a way that renormalisation cannot fix.

C++ (compute on CPU, pass as uniform):
  glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));

GLSL vertex shader:
  vec3 worldNormal = normalize(normalMatrix * aNormal);

Note: computing inverse() in the vertex shader itself is expensive and should be avoided. Always compute the normal matrix on the CPU and pass it as a uniform.`,
            },
          ],
        },
      ],
    },

    {
      title: 'Task 4 - When Is the Shortcut Safe?',
      blocks: [
        {
          type: 'short_answer',
          interactive: true,
          questions: [
            {
              question: 'List two conditions under which you can use the model matrix (or its 3x3 block) directly as the normal matrix, without computing the inverse-transpose. Explain why each condition is sufficient.',
              model_answer: `1. The model matrix contains only rotations (and optionally a uniform scale).
   Reason: for pure rotations, M^-1 = M^T, so (M^-1)^T = M. The normal matrix equals the model matrix. A uniform scale factor cancels out after normalisation, so it does not affect the final result.

2. The model matrix is the identity.
   Reason: trivially safe. The identity transforms nothing. I^-1 = I and I^T = I, so (I^-1)^T = I = the model matrix.

In any other case (non-uniform scale, shear), the full inverse-transpose must be computed.`,
            },
          ],
        },
      ],
    },
  ],

  solution_code: String.raw`WS07 - Normal Vector Transformation - Answer Key

Task 1 - The Failure Case
  Original: n dot t = 0  (perpendicular)
  After naive M transform: Mt dot Mn = -3  (NOT perpendicular)
  Correct normal matrix: (M^-1)^T
  After correct transform: Mt dot correct_n = 0  (perpendicular restored)
  Correct normal = (-1/2, 1, 0), renormalised = (-0.447, 0.894, 0)

Task 2 - Multiple Choice
  For rotation R: (R^-1)^T = R   (answer B)
  Reason: R^-1 = R^T for orthonormal matrices, so (R^-1)^T = R

Task 3 - Compute Normal Matrix
  Matrix A = scale(3,3,3):  (M^-1)^T = scale(1/3,1/3,1/3)
    Shortcut: SAFE (uniform scale; direction unchanged after renormalisation)
  Matrix B = scale(2,1,1)*R:
    Shortcut: NOT SAFE (non-uniform scale distorts direction)
    C++ code:   glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
    GLSL code:  vec3 worldNormal = normalize(normalMatrix * aNormal);

Task 4 - When Is the Shortcut Safe?
  Condition 1: model matrix contains only rotations (optionally uniform scale).
    Why: (R^-1)^T = R; uniform scale cancels after normalise.
  Condition 2: model matrix is the identity.
    Why: I^-1 = I and I^T = I, so (I^-1)^T = I = model.`,

  languages: ['cpp'],
}, 'labs/mcp/math_WS07_normal_transform.html');
