import { buildAndWriteLab } from '../tools/lab-assembler/dist/assembler.js';

buildAndWriteLab({
  options: { solution: true, copy_protect: false },
  module_code: 'ACCA6021',
  module_title: 'Graphics and Games Programming',
  lab_number: 'M06',
  lab_title: 'Perspective Projection and NDC',

  overview_html: `<p>This worksheet demystifies the projection matrix. You will learn what each frustum parameter controls geometrically, carry a view-space point through the projection matrix by hand, perform the perspective divide, and explain why a large near/far ratio degrades depth precision in a 24-bit depth buffer.</p>`,

  objectives: [
    'By the end of this worksheet you will be able to state what each parameter of glm::perspective controls geometrically',
    'By the end of this worksheet you will be able to carry a view-space point through the projection matrix and perspective divide by hand to obtain NDC coordinates',
    'By the end of this worksheet you will be able to explain why w_clip = -z_view enables perspective and produces the size-with-distance effect',
    'By the end of this worksheet you will be able to explain why a small near/far ratio gives better depth precision than a large one',
  ],

  concept_html: `<p>How does a 3D point become a 2D pixel coordinate with objects appearing smaller in the distance? Scaling alone cannot explain the effect. Something must divide by depth. The <strong>projection matrix</strong> sets up that division but does not perform it. The hardware divides as a separate step after the vertex shader finishes. Understanding the two-step process — matrix then divide — is the key to understanding the w component and depth buffer precision.</p>
<p>The projection matrix encodes the <strong>frustum</strong> shape defined by field of view, aspect ratio, near plane, and far plane. Multiplying a view-space point by it produces a clip-space vector where <strong>w_clip = -z_view</strong>. The hardware then divides x, y, z by w to get NDC. Because w carries depth, dividing by it causes far objects to have a larger divisor and thus appear smaller — that is perspective. NDC depth is not linear between near and far. Precision concentrates near the near plane, compressing rapidly toward the far plane.</p>
<p>In GLM, the projection matrix is created with <code>glm::perspective(glm::radians(fov), aspect, near, far)</code>. A small near/far ratio spreads depth precision evenly. A very large ratio (such as near = 0.01, far = 10000) compresses nearly all precision into a thin band close to the camera, causing z-fighting on distant geometry even with a 24-bit depth buffer.</p>`,

  tasks: [
    {
      title: 'Task 1 - The Projection Matrix and Its Parameters',
      blocks: [
        {
          type: 'prose',
          html: `<p>Using FoV = 90 degrees, aspect = 1.0, near = 1.0, far = 10.0. At 90 degrees, FoV/2 = 45 degrees and tan(45) = 1, so the focal length f = 1/tan(FoV/2) = 1. These values produce clean numbers.</p>
<p>The key entries of the projection matrix and what they mean:</p>
<pre>f = 1 / tan(FoV/2) = 1 / tan(45 deg) = 1

M[0][0] = f / aspect = 1 / 1.0 = 1      scales x by focal length, corrects for aspect ratio
M[1][1] = f = 1                           scales y by focal length
M[2][2] = -(far+near)/(far-near)         maps z_view into a depth value
         = -(10+1)/(10-1) = -11/9 approx -1.222
M[3][2] = -(2*far*near)/(far-near)       scale of the depth mapping
         = -(2*10*1)/(10-1) = -20/9 approx -2.222
M[2][3] = -1                             copies -z_view into w_clip (enables perspective divide)</pre>
<p>Full matrix (column-major display):</p>
<pre>| 1    0       0          0    |
| 0    1       0          0    |
| 0    0   -11/9  approx -1.222|
| 0    0      -1          0    |

Wait - column 3 holds the translation-equivalent terms. In standard form:
| 1   0      0       0    |
| 0   1      0       0    |
| 0   0  -11/9   -20/9    |
| 0   0     -1       0    |</pre>
<p>The entry M[2][3] = -1 is the most important. It causes w_clip = -z_view. For visible geometry in OpenGL, z_view is negative (the camera looks down -Z), so w_clip = -z_view is positive. After dividing by w_clip, NDC values land in [-1, 1] for geometry within the frustum.</p>`,
        },
        {
          type: 'fill_blanks',
          interactive: true,
          questions: [
            {
              question_html: 'For FoV = 90 degrees, tan(FoV/2) = tan(45 deg) = <strong>{{blank}}</strong>, so f = 1.',
              answers: [['1', 'one']],
            },
            {
              question_html: 'The matrix entry M[2][3] = <strong>{{blank}}</strong> causes w_clip = -z_view.',
              answers: [['-1']],
            },
            {
              question_html: 'For visible geometry, z_view is <strong>{{blank}}</strong> (sign), so w_clip is positive.',
              answers: [['negative', 'neg']],
            },
            {
              question_html: 'With near=1 and far=10: M[2][2] = -(far+near)/(far-near) = -(10+1)/(10-1) = <strong>{{blank}}</strong>/9',
              answers: [['-11']],
            },
          ],
        },
      ],
    },

    {
      title: 'Task 2 - Carry a Point to NDC by Hand',
      blocks: [
        {
          type: 'prose',
          html: `<p>Use the projection matrix from Task 1 (FoV=90, aspect=1, near=1, far=10). Starting view-space point: <strong>v</strong> = (2, 1, -3, 1). z_view = -3 is between near=1 and far=10, so this point is visible.</p>
<p>Clip-space components are computed as matrix rows dotted with v = (2, 1, -3, 1):</p>
<pre>x_clip = (1)(2) + (0)(1) + (0)(-3) + (0)(1)
y_clip = (0)(2) + (1)(1) + (0)(-3) + (0)(1)
z_clip = (0)(2) + (0)(1) + (-11/9)(-3) + (-20/9)(1)
w_clip = (0)(2) + (0)(1) + (-1)(-3) + (0)(1)</pre>
<p>Fill in the results, then divide by w_clip to get NDC.</p>`,
        },
        {
          type: 'fill_blanks',
          interactive: true,
          questions: [
            {
              question_html: 'x_clip = (1)(2) = <strong>{{blank}}</strong>',
              answers: [['2']],
            },
            {
              question_html: 'y_clip = (1)(1) = <strong>{{blank}}</strong>',
              answers: [['1']],
            },
            {
              question_html: 'z_clip = (-11/9)(-3) + (-20/9)(1) = 33/9 - 20/9 = 13/9 approx <strong>{{blank}}</strong> (to 3 decimal places)',
              answers: [['1.444', '1.44', '13/9']],
            },
            {
              question_html: 'w_clip = (-1)(-3) = <strong>{{blank}}</strong>',
              answers: [['3']],
            },
            {
              question_html: 'NDC_x = x_clip / w_clip = 2 / 3 approx <strong>{{blank}}</strong> (to 3 decimal places)',
              answers: [['0.667', '2/3']],
            },
            {
              question_html: 'NDC_y = y_clip / w_clip = 1 / 3 approx <strong>{{blank}}</strong> (to 3 decimal places)',
              answers: [['0.333', '1/3']],
            },
            {
              question_html: 'NDC_z = z_clip / w_clip = (13/9) / 3 = 13/27 approx <strong>{{blank}}</strong> (to 3 decimal places)',
              answers: [['0.481', '13/27']],
            },
            {
              question_html: 'Are all three NDC values in the range [-1, 1]? Is the point <strong>{{blank}}</strong>?',
              answers: [['visible', 'yes']],
            },
          ],
        },
      ],
    },

    {
      title: 'Task 3 - Depth Precision',
      blocks: [
        {
          type: 'short_answer',
          interactive: true,
          questions: [
            {
              question: 'Two scenes use the same FoV and geometry but different near/far planes: Scene A has near=1, far=10. Scene B has near=0.01, far=10000. A wall at z=-9 and another at z=-9.01 must be distinguishable in a 24-bit depth buffer. In which scene is this more likely to fail, and why?',
              model_answer: 'Scene B is more likely to fail. The NDC depth mapping is nonlinear: precision concentrates very close to the near plane and compresses rapidly toward the far plane. With near=0.01 and far=10000, almost all of the 24-bit depth range is consumed by geometry very close to the camera. By the time we reach z=-9, both walls round to the same integer depth buffer value, causing z-fighting. In Scene A, near=1 and far=10 means the entire depth range covers only 9 units. The walls at z=-9 and z=-9.01 are separated by a 0.01-unit gap near the far end of a small range, giving much better resolution.',
            },
            {
              question: 'Without changing FoV or the scene geometry, what single parameter change most effectively reduces z-fighting on distant objects?',
              model_answer: 'Move the near plane further from the camera (increase near). A larger near value reduces the near/far ratio and spreads depth precision more evenly across the visible range. Increasing far makes precision worse. Decreasing near also makes it worse. Increasing near is the most effective single-parameter fix for depth precision on distant objects.',
            },
          ],
        },
      ],
    },
  ],

  solution_code: String.raw`WS06 - Perspective Projection and NDC - Answer Key

Task 1 - Fill-in Blanks
  tan(45 deg) = 1       so f = 1
  M[2][3] = -1          causes w_clip = -z_view
  z_view sign: negative (camera looks down -Z; near=-1 not near=+1)
  M[2][2] numerator: -11 (so M[2][2] = -11/9)

Task 2 - Carry v=(2,1,-3,1) to NDC (FoV=90, aspect=1, near=1, far=10)
  x_clip = 2
  y_clip = 1
  z_clip = 33/9 - 20/9 = 13/9 approx 1.444
  w_clip = 3

  Perspective divide (divide by w_clip = 3):
  NDC_x = 2/3  approx 0.667
  NDC_y = 1/3  approx 0.333
  NDC_z = 13/27 approx 0.481

  All three values are in [-1, 1]: the point is VISIBLE.
  NDC_z = 0.481 means the point is about halfway between near and far in NDC depth
  (closer to the far plane due to the nonlinear depth mapping).

Task 3 - Depth Precision
  Q1: Scene B (near=0.01, far=10000) fails. Huge ratio crushes precision near far plane.
      At z=-9 both walls map to the same depth buffer value. Z-fighting.
  Q2: Increase near. Larger near reduces near/far ratio; spreads precision more evenly.`,

  languages: ['cpp'],
}, 'labs/mcp/math_WS06_perspective_and_ndc.html');
