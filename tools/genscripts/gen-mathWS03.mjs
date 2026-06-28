import { buildAndWriteLab } from '../tools/lab-assembler/dist/assembler.js';

buildAndWriteLab({
  options: { solution: true, copy_protect: false },
  module_code: 'ACCA6021',
  module_title: 'Graphics and Games Programming',
  lab_number: 'M03',
  lab_title: 'Homogeneous Coordinates and Transformation Matrices',

  overview_html: `<p>This worksheet explains why a 4x4 matrix is required to represent translation alongside rotation and scale. You will construct a translation matrix and a Y-axis rotation matrix in column-major layout, then compose them and trace a vertex through the combined transform by hand. The values used here connect directly to the Transformations lab (Lab 07).</p>`,

  objectives: [
    'By the end of this worksheet you will be able to explain why a 3x3 matrix cannot represent translation and why a 4x4 matrix is needed',
    'By the end of this worksheet you will be able to construct a 4x4 translation matrix T(tx, ty, tz) and apply it to a homogeneous point and direction vector',
    'By the end of this worksheet you will be able to write out the Y-axis rotation matrix R_y(theta) and verify it on concrete vectors',
    'By the end of this worksheet you will be able to compose two transforms by matrix multiplication and trace a vertex through the result',
  ],

  concept_html: `<p>A 3x3 matrix can only encode <strong>linear transformations</strong>: operations that map the origin to itself. Rotation, scale, and shear all satisfy this constraint. <strong>Translation</strong> does not. Moving every point by a fixed offset means the origin maps to a non-zero position, which no matrix whose columns span from the origin can achieve. Without a unified representation that includes translation, combining rotate-then-translate requires two separate operations, making composition and inversion awkward.</p>
<p>The solution is to add a fourth component <strong>w</strong> to every vector, producing <strong>homogeneous coordinates</strong>. A <strong>point</strong> uses w = 1 and a <strong>direction vector</strong> uses w = 0. The 4x4 translation matrix T(tx, ty, tz) stores the offset in its fourth column. When it multiplies a point (w = 1) the offset is added to x, y, z. When it multiplies a direction (w = 0) the offset is ignored, which is geometrically correct because directions have no position. Rotation matrices are extended to 4x4 by placing the 3x3 block in the top-left corner and filling the rest with identity values.</p>
<p>Transforms are combined by multiplying their matrices together. The resulting <strong>composite matrix</strong> applies both transforms in a single multiplication. GLM functions such as <code>glm::translate</code> and <code>glm::rotate</code> construct these 4x4 matrices internally. Understanding the matrices they produce is what makes the order of function calls in code predictable rather than guesswork.</p>`,

  tasks: [
    {
      title: 'Task 1 - Why 3x3 Cannot Translate',
      blocks: [
        {
          type: 'prose',
          html: `<p>This task shows concretely that no 3x3 matrix can add a fixed offset to every input vector, then demonstrates the 4x4 solution.</p>
<p>Suppose we want to translate every point by (tx, ty, tz) = (4, 5, 6). A 3x3 matrix M must satisfy M * v = v + (4, 5, 6) for every possible v. Consider the y component of the result:</p>
<pre>y_out = m10 * vx + m11 * vy + m12 * vz</pre>
<p>For this to equal vy + 5 for every input, we would need m11 = 1 and m10 * vx + m12 * vz = 5 for all vx and vz simultaneously. There are no constants m10 and m12 that satisfy this for every possible vx and vz. Translation is not a linear operation in 3D.</p>
<p>The 4x4 solution: add a fourth component w = 1 to every point. The translation matrix is:</p>
<pre>T(4, 5, 6):
| 1  0  0  4 |
| 0  1  0  5 |
| 0  0  1  6 |
| 0  0  0  1 |</pre>
<p>Apply T(4, 5, 6) to the point p = (1, 2, 3, 1):</p>
<pre>| 1  0  0  4 |   | 1 |   | 1*1 + 0*2 + 0*3 + 4*1 |   | 5 |
| 0  1  0  5 | * | 2 | = | 0*1 + 1*2 + 0*3 + 5*1 | = | 7 |
| 0  0  1  6 |   | 3 |   | 0*1 + 0*2 + 1*3 + 6*1 |   | 9 |
| 0  0  0  1 |   | 1 |   | 0*1 + 0*2 + 0*3 + 1*1 |   | 1 |</pre>
<p>Result: p_translated = (5, 7, 9, 1). The offset (4, 5, 6) was added correctly.</p>
<p>Now apply the same T(4, 5, 6) to the direction vector d = (0, 1, 0, 0):</p>
<pre>| 1  0  0  4 |   | 0 |   | 1*0 + 0*1 + 0*0 + 4*0 |   | 0 |
| 0  1  0  5 | * | 1 | = | 0*0 + 1*1 + 0*0 + 5*0 | = | 1 |
| 0  0  1  6 |   | 0 |   | 0*0 + 0*1 + 1*0 + 6*0 |   | 0 |
| 0  0  0  1 |   | 0 |   | 0*0 + 0*1 + 0*0 + 1*0 |   | 0 |</pre>
<p>Result: d_translated = (0, 1, 0, 0). The direction is unchanged. The w = 0 zeroed out the entire fourth column of the matrix, so translation has no effect on direction vectors. This is geometrically correct: directions represent orientation, not position.</p>`,
        },
      ],
    },

    {
      title: 'Task 2 - The Y-Axis Rotation Matrix',
      blocks: [
        {
          type: 'prose',
          html: `<p>The Y-axis rotation matrix R_y(t) rotates points around the world Y axis by angle t (in radians). Note the sign swap on the sin terms compared to the X and Z rotation matrices: the -sin(t) appears at row 2, column 0 (bottom-left of the 3x3 block) rather than at row 0, column 2.</p>
<pre>R_y(t):
| cos(t)   0   sin(t)   0 |
|    0     1      0     0 |
| -sin(t)  0   cos(t)   0 |
|    0     0      0     1 |</pre>
<p>At theta = 90 degrees: cos(90) = 0, sin(90) = 1. R_y(90) becomes:</p>
<pre>R_y(90):
|  0   0   1   0 |
|  0   1   0   0 |
| -1   0   0   0 |
|  0   0   0   1 |</pre>
<p><strong>Verification 1:</strong> apply R_y(90) to the point (1, 0, 0, 1) (tip of the X axis).</p>
<pre>|  0   0   1   0 |   | 1 |   | 0 |
|  0   1   0   0 | * | 0 | = | 0 |
| -1   0   0   0 |   | 0 |   |-1 |
|  0   0   0   1 |   | 1 |   | 1 |</pre>
<p>Result: (0, 0, -1, 1). Rotating the X-axis tip 90 degrees around Y swings it to -Z. This follows the right-hand rule: curl the fingers of the right hand from X toward Z; the thumb points up (+Y); a positive rotation swings X toward -Z.</p>
<p><strong>Verification 2:</strong> apply R_y(90) to the point (0, 0, 1, 1) (tip of the Z axis).</p>
<pre>|  0   0   1   0 |   | 0 |   | 1 |
|  0   1   0   0 | * | 0 | = | 0 |
| -1   0   0   0 |   | 1 |   | 0 |
|  0   0   0   1 |   | 1 |   | 1 |</pre>
<p>Result: (1, 0, 0, 1). The Z-axis tip swings to +X. Both results are consistent: a 90-degree Y rotation takes X to -Z and Z to +X, cycling the horizontal axes as expected.</p>
<p>The sign of the -sin(t) entry at row 2, column 0 encodes this direction. When rotating from X toward -Z (a positive angle around Y), the X component contributes negatively to the new Z component. This is exactly what -sin(t) at position [2][0] produces.</p>`,
        },
      ],
    },

    {
      title: 'Task 3 - Compose and Apply by Hand',
      blocks: [
        {
          type: 'prose',
          html: `<p>Goal: start at p = (0, 0, 0, 1) (the world origin). First translate by (2, 0, 0), then rotate 90 degrees around Y. The final position should be (0, 0, -2, 1).</p>
<p>In column-major convention, the transform applied first goes on the right in the multiplication. To translate first and then rotate: C = R_y(90) * T(2, 0, 0).</p>
<p>The two matrices:</p>
<pre>T(2, 0, 0):             R_y(90):
| 1  0  0  2 |          |  0  0  1  0 |
| 0  1  0  0 |          |  0  1  0  0 |
| 0  0  1  0 |          | -1  0  0  0 |
| 0  0  0  1 |          |  0  0  0  1 |</pre>
<p>Compute C = R_y(90) * T(2, 0, 0) by multiplying row by column. Each entry C[row][col] is the dot product of row <em>row</em> of R_y(90) with column <em>col</em> of T(2, 0, 0). Fill in the 16 entries below.</p>`,
        },
        {
          type: 'fill_blanks',
          interactive: true,
          questions: [
            {
              question_html: 'C[0][0]: row 0 of R_y dot col 0 of T = (0)(1) + (0)(0) + (1)(-1) + (0)(0) ... wait, use row of R_y = (0,0,1,0) and col of T = (1,0,0,0): (0)(1)+(0)(0)+(1)(0)+(0)(0) = <strong>{{blank}}</strong>',
              answers: [['0']],
            },
            {
              question_html: 'C[0][1]: row 0 of R_y = (0,0,1,0), col 1 of T = (0,1,0,0): (0)(0)+(0)(1)+(1)(0)+(0)(0) = <strong>{{blank}}</strong>',
              answers: [['0']],
            },
            {
              question_html: 'C[0][2]: row 0 of R_y = (0,0,1,0), col 2 of T = (0,0,1,0): (0)(0)+(0)(0)+(1)(1)+(0)(0) = <strong>{{blank}}</strong>',
              answers: [['1']],
            },
            {
              question_html: 'C[0][3]: row 0 of R_y = (0,0,1,0), col 3 of T = (2,0,0,1): (0)(2)+(0)(0)+(1)(0)+(0)(1) = <strong>{{blank}}</strong>',
              answers: [['0']],
            },
            {
              question_html: 'C[1][0]: row 1 of R_y = (0,1,0,0), col 0 of T = (1,0,0,0): = <strong>{{blank}}</strong>',
              answers: [['0']],
            },
            {
              question_html: 'C[1][1]: row 1 of R_y = (0,1,0,0), col 1 of T = (0,1,0,0): = <strong>{{blank}}</strong>',
              answers: [['1']],
            },
            {
              question_html: 'C[1][2]: row 1 of R_y = (0,1,0,0), col 2 of T = (0,0,1,0): = <strong>{{blank}}</strong>',
              answers: [['0']],
            },
            {
              question_html: 'C[1][3]: row 1 of R_y = (0,1,0,0), col 3 of T = (2,0,0,1): = <strong>{{blank}}</strong>',
              answers: [['0']],
            },
            {
              question_html: 'C[2][0]: row 2 of R_y = (-1,0,0,0), col 0 of T = (1,0,0,0): (-1)(1)+(0)(0)+(0)(0)+(0)(0) = <strong>{{blank}}</strong>',
              answers: [['-1']],
            },
            {
              question_html: 'C[2][1]: row 2 of R_y = (-1,0,0,0), col 1 of T = (0,1,0,0): = <strong>{{blank}}</strong>',
              answers: [['0']],
            },
            {
              question_html: 'C[2][2]: row 2 of R_y = (-1,0,0,0), col 2 of T = (0,0,1,0): = <strong>{{blank}}</strong>',
              answers: [['0']],
            },
            {
              question_html: 'C[2][3]: row 2 of R_y = (-1,0,0,0), col 3 of T = (2,0,0,1): (-1)(2)+(0)(0)+(0)(0)+(0)(1) = <strong>{{blank}}</strong>',
              answers: [['-2']],
            },
            {
              question_html: 'C[3][0]: row 3 of R_y = (0,0,0,1), col 0 of T = (1,0,0,0): = <strong>{{blank}}</strong>',
              answers: [['0']],
            },
            {
              question_html: 'C[3][1] = <strong>{{blank}}</strong>',
              answers: [['0']],
            },
            {
              question_html: 'C[3][2] = <strong>{{blank}}</strong>',
              answers: [['0']],
            },
            {
              question_html: 'C[3][3]: row 3 of R_y = (0,0,0,1), col 3 of T = (2,0,0,1): (0)(2)+(0)(0)+(0)(0)+(1)(1) = <strong>{{blank}}</strong>',
              answers: [['1']],
            },
          ],
        },
        {
          type: 'prose',
          html: `<p>The composite matrix C = R_y(90) * T(2, 0, 0) should be:</p>
<pre>C:
|  0   0   1   0 |
|  0   1   0   0 |
| -1   0   0  -2 |
|  0   0   0   1 |</pre>
<p>Apply C to the starting point p = (0, 0, 0, 1):</p>
<pre>|  0   0   1   0 |   | 0 |   | 0 |
|  0   1   0   0 | * | 0 | = | 0 |
| -1   0   0  -2 |   | 0 |   |-2 |
|  0   0   0   1 |   | 1 |   | 1 |</pre>
<p>Result: (0, 0, -2, 1). Interpretation: the origin first moved to (2, 0, 0) by the translation, then a 90-degree Y rotation swung that point to (0, 0, -2). This matches the expected result.</p>
<p>The GLM equivalent (note: GLM calls are written in reverse order because each call left-multiplies the accumulated matrix):</p>`,
        },
        {
          type: 'code',
          lang: 'cpp',
          code_type: 'reference',
          filename: 'GLM equivalent',
          content: String.raw`// GLM applies right to left: rotate is called FIRST in code, translate SECOND
// This gives model = T(2,0,0) * I then model = R_y(90) * model = R_y(90) * T(2,0,0)
glm::mat4 model(1.0f);
model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));`,
        },
      ],
    },
  ],

  solution_code: String.raw`WS03 - Homogeneous Coordinates and Transformation Matrices - Answer Key

Task 3 - Composite Matrix C = R_y(90) * T(2,0,0)
  C[0][0] = 0    C[0][1] = 0    C[0][2] = 1    C[0][3] =  0
  C[1][0] = 0    C[1][1] = 1    C[1][2] = 0    C[1][3] =  0
  C[2][0] = -1   C[2][1] = 0    C[2][2] = 0    C[2][3] = -2
  C[3][0] = 0    C[3][1] = 0    C[3][2] = 0    C[3][3] =  1

Full matrix:
|  0   0   1   0 |
|  0   1   0   0 |
| -1   0   0  -2 |
|  0   0   0   1 |

Vertex trace: C * (0,0,0,1) = (0, 0, -2, 1)
Interpretation: origin -> translated to (2,0,0) -> rotated 90deg around Y -> lands at (0,0,-2)

Key answers from Tasks 1 and 2:
Task 1:  T(4,5,6) * (1,2,3,1) = (5,7,9,1)    T(4,5,6) * (0,1,0,0) = (0,1,0,0)
Task 2:  R_y(90) * (1,0,0,1) = (0,0,-1,1)     R_y(90) * (0,0,1,1) = (1,0,0,1)`,

  languages: ['cpp'],
}, 'labs/mcp/math_WS03_homogeneous_and_matrices.html');
