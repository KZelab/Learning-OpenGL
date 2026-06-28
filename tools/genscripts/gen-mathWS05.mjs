import { buildAndWriteLab } from '../tools/lab-assembler/dist/assembler.js';

buildAndWriteLab({
  options: { solution: true, copy_protect: false },
  module_code: 'ACCA6021',
  module_title: 'Graphics and Games Programming',
  lab_number: 'M05',
  lab_title: 'The Look-At Matrix',

  overview_html: `<p>This worksheet derives the view matrix from first principles. You will compute the three camera basis vectors from an eye position and a look-at point using cross products, assemble the look-at matrix, and verify the results numerically. Understanding what <code>glm::lookAt</code> computes explains why the view matrix appears to move the world rather than the camera, and why its translation column is negated. This worksheet builds on the Homogeneous Coordinates lab (WS-M03) and the Column-Major Convention lab (WS-M04).</p>`,

  objectives: [
    'By the end of this worksheet you will be able to derive the three camera basis vectors (forward, right, up) from eye, centre, and worldUp using cross products',
    'By the end of this worksheet you will be able to assemble a look-at matrix from those basis vectors and verify it numerically',
    'By the end of this worksheet you will be able to explain why the translation column of the view matrix is negated',
    'By the end of this worksheet you will be able to identify and explain the gimbal singularity that occurs when the forward vector is parallel to worldUp',
  ],

  concept_html: `<p>The camera is positioned at a point in the scene and aimed at a target. The vertex shader needs all scene vertices expressed relative to the camera. Students often think the view matrix moves the camera to where it belongs, but it actually does the opposite: it moves the entire world so the camera ends up at the origin looking down -Z. The vertices do not move in world space. The coordinate frame shifts.</p>
<p>Three <strong>basis vectors</strong> describe the camera's orientation. GLM computes them as follows. The <strong>forward</strong> vector f is the normalised direction from eye to centre: f = normalise(centre - eye). The <strong>right</strong> vector s is perpendicular to both forward and worldUp: s = normalise(cross(f, worldUp)). The <strong>corrected up</strong> vector u is then recomputed as u = cross(s, f) to ensure perfect orthogonality. The view matrix combines a rotation block (rows built from s, u, -f) with a negated translation that shifts the world so the camera sits at the origin. The translation values are negated because the world moves opposite to the camera's position.</p>
<p>The function <code>glm::lookAt(eye, centre, worldUp)</code> computes this matrix. Its three arguments map directly to the three inputs of the derivation. One critical edge case: if the camera is aimed straight up or straight down, f and worldUp are parallel. Their cross product is the zero vector and normalising it is undefined. This <strong>gimbal singularity</strong> must be avoided in camera code.</p>`,

  tasks: [
    {
      title: 'Task 1 - Deriving a Look-At Matrix Step by Step',
      blocks: [
        {
          type: 'prose',
          html: `<p>Use: eye = (0, 0, 5), centre = (0, 0, 0), worldUp = (0, 1, 0). These values produce clean integer results.</p>
<p><strong>Step 1 - Forward vector f</strong></p>
<pre>f = normalise(centre - eye)
  = normalise((0,0,0) - (0,0,5))
  = normalise((0, 0, -5))
  = (0, 0, -1)</pre>

<p><strong>Step 2 - Right vector s</strong></p>
<pre>s = normalise(cross(f, worldUp))
  = normalise(cross((0,0,-1), (0,1,0)))</pre>
<p>Cross product expansion (u x v = (uy*vz - uz*vy, uz*vx - ux*vz, ux*vy - uy*vx)):</p>
<pre>cross((0,0,-1), (0,1,0)):
  x = (0)(0)  - (-1)(1) =  0 + 1 = 1
  y = (-1)(0) - (0)(0)  =  0 - 0 = 0
  z = (0)(1)  - (0)(0)  =  0 - 0 = 0
Result: (1, 0, 0)  -- already unit length, so s = (1, 0, 0)</pre>

<p><strong>Step 3 - Corrected up vector u</strong></p>
<pre>u = cross(s, f)
  = cross((1,0,0), (0,0,-1)):
  x = (0)(-1) - (0)(0)  = 0
  y = (0)(0)  - (1)(-1) = 1
  z = (1)(0)  - (0)(0)  = 0
Result: u = (0, 1, 0)</pre>

<p><strong>Step 4 - Assemble the view matrix</strong></p>
<pre>The look-at matrix has the form:
|  s.x   s.y   s.z   -dot(s, eye) |
|  u.x   u.y   u.z   -dot(u, eye) |
| -f.x  -f.y  -f.z    dot(f, eye) |
|   0     0     0         1       |

Compute the translation column:
  -dot(s, eye) = -(1*0 + 0*0 + 0*5) = 0
  -dot(u, eye) = -(0*0 + 1*0 + 0*5) = 0
   dot(f, eye) = (0*0 + 0*0 + (-1)*5) = -5

Final look-at matrix:
| 1  0   0   0 |
| 0  1   0   0 |
| 0  0   1  -5 |
| 0  0   0   1 |</pre>
<p>Interpretation: this is a pure translation of -5 on Z. The camera is at z = 5, so the world must shift by -5 to bring the camera to the origin. Because the camera has no rotation relative to the world axes, the rotation block of the matrix is the identity. The -5 in the translation column is exactly -dot(f, eye) = -(-1 * 5) ... wait, that gives dot(f, eye) = -5, and the matrix places this at row 2, column 3 directly.</p>`,
        },
      ],
    },

    {
      title: 'Task 2 - Derive a Different Configuration',
      blocks: [
        {
          type: 'prose',
          html: `<p>Use: eye = (4, 0, 0), centre = (0, 0, 0), worldUp = (0, 1, 0). The camera is to the right of the scene, looking left toward the origin. Follow the same four steps and fill in the blanks.</p>`,
        },
        {
          type: 'fill_blanks',
          interactive: true,
          questions: [
            {
              question_html: 'Step 1: f = normalise(centre - eye) = normalise((0,0,0) - (4,0,0)) = normalise((-4,0,0)) = (<strong>{{blank}}</strong>, 0, 0)',
              answers: [['-1']],
            },
            {
              question_html: 'Step 2: cross(f, worldUp) = cross((-1,0,0), (0,1,0)). x component = (0)(0) - (0)(1) = <strong>{{blank}}</strong>',
              answers: [['0']],
            },
            {
              question_html: 'Step 2 continued: y component = (0)(0) - (-1)(0) = <strong>{{blank}}</strong>',
              answers: [['0']],
            },
            {
              question_html: 'Step 2 continued: z component = (-1)(1) - (0)(0) = <strong>{{blank}}</strong>',
              answers: [['-1']],
            },
            {
              question_html: 'So s = normalise((0, 0, -1)) = (0, 0, <strong>{{blank}}</strong>)',
              answers: [['-1']],
            },
            {
              question_html: 'Step 3: u = cross(s, f) = cross((0,0,-1), (-1,0,0)). x component = (0)(0) - (-1)(0) = <strong>{{blank}}</strong>',
              answers: [['0']],
            },
            {
              question_html: 'Step 3 continued: y component = (-1)(-1) - (0)(0) = <strong>{{blank}}</strong>',
              answers: [['1']],
            },
            {
              question_html: 'Step 3 continued: z component = (0)(0) - (0)(-1) = <strong>{{blank}}</strong>. So u = (0, 1, 0).',
              answers: [['0']],
            },
            {
              question_html: 'Step 4: -dot(s, eye) = -(0*4 + 0*0 + (-1)*0) = <strong>{{blank}}</strong>',
              answers: [['0']],
            },
            {
              question_html: 'Step 4: -dot(u, eye) = -(0*4 + 1*0 + 0*0) = <strong>{{blank}}</strong>',
              answers: [['0']],
            },
            {
              question_html: 'Step 4: dot(f, eye) = (-1)(4) + (0)(0) + (0)(0) = <strong>{{blank}}</strong>',
              answers: [['-4']],
            },
          ],
        },
        {
          type: 'prose',
          html: `<p>The completed view matrix for eye = (4, 0, 0), centre = (0, 0, 0), worldUp = (0, 1, 0):</p>
<pre>|  0   0  -1   0 |
|  0   1   0   0 |
| -1   0   0  -4 |
|  0   0   0   1 |</pre>
<p>The right vector s = (0, 0, -1): the camera looks left (-X), so its right side points into the screen (-Z in world space). The translation -4 at row 2, column 3 shifts the world -4 on X to bring the camera (at x = 4) to the origin.</p>`,
        },
      ],
    },

    {
      title: 'Task 3 - Conceptual Questions',
      blocks: [
        {
          type: 'short_answer',
          interactive: true,
          questions: [
            {
              question: 'The camera is pointing straight up. worldUp = (0,1,0) and (centre - eye) points in the same direction (0,1,0). What goes wrong in Step 2, and what does this failure mean physically?',
              model_answer: 'In Step 2, f = (0,1,0) and worldUp = (0,1,0). The cross product of two parallel vectors is the zero vector. Normalising the zero vector is undefined (division by zero). Physically, the camera is aiming straight up and there is no unique horizontal direction for "right" — the camera could be rotated any amount around the vertical and the view would look the same. This is a gimbal singularity. Camera code must avoid aiming the camera directly along the worldUp direction.',
            },
            {
              question: 'Why is the translation column of the look-at matrix negated?',
              model_answer: 'The view matrix moves the world, not the camera. If the camera is at position e in world space, the world must shift by -e so the camera ends up at the origin. The translation values in the view matrix are -dot(s, eye), -dot(u, eye), and dot(f, eye) (with a sign flip because f is used negated in the matrix row). These values encode the negated camera position projected onto each basis axis, producing exactly the world-shift needed to bring the camera to the origin.',
            },
            {
              question: 'What does the right vector s represent, and why is the cross product the correct tool for computing it?',
              model_answer: 'The right vector s points in the direction to the camera\'s right. It must be perpendicular to both the forward direction f and the worldUp vector. The cross product of two vectors produces a vector that is perpendicular to both of them — this is exactly the constraint needed. No other common operation gives this perpendicularity property directly.',
            },
          ],
        },
      ],
    },
  ],

  solution_code: String.raw`WS05 - The Look-At Matrix - Answer Key

Task 2 - Fill-in Blanks (eye=(4,0,0), centre=(0,0,0), worldUp=(0,1,0))

  Step 1: f = (-1, 0, 0)
  Step 2: cross(f, worldUp) = cross((-1,0,0),(0,1,0))
          x = (0)(0) - (0)(1) = 0
          y = (0)(0) - (-1)(0) = 0
          z = (-1)(1) - (0)(0) = -1
          s = normalise((0,0,-1)) = (0, 0, -1)
  Step 3: u = cross(s, f) = cross((0,0,-1),(-1,0,0))
          x = (0)(0) - (-1)(0) = 0
          y = (-1)(-1) - (0)(0) = 1
          z = (0)(0) - (0)(-1) = 0
          u = (0, 1, 0)
  Step 4: -dot(s, eye) = -(0*4+0*0+(-1)*0) = 0
          -dot(u, eye) = -(0*4+1*0+0*0)    = 0
           dot(f, eye) = (-1*4+0+0)         = -4

  Final view matrix:
  |  0   0  -1   0 |
  |  0   1   0   0 |
  | -1   0   0  -4 |
  |  0   0   0   1 |

Task 3 - Short Answer Summaries
  Q1 (singularity): f parallel to worldUp -> cross product = zero vector -> s undefined.
     Camera aims along worldUp; no unique right direction. Gimbal lock.
  Q2 (negation): view matrix moves the world by -eye, not the camera. Negation brings camera to origin.
  Q3 (cross product): s must be perpendicular to f and worldUp; cross product gives exactly this.`,

  languages: ['cpp'],
}, 'labs/mcp/math_WS05_lookat_matrix.html');
