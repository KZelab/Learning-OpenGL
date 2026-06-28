import { buildAndWriteLab } from '../tools/lab-assembler/dist/assembler.js';

buildAndWriteLab({
  options: { solution: false, copy_protect: false },
  module_code: 'ACCA6021',
  module_title: 'Graphics and Games Programming',
  lab_number: 'REF',
  lab_title: 'Matrix and Trig Reference Card',

  overview_html: `<p>This card is designed to be printed and kept alongside your lab worksheet. It covers trig values at standard angles, the standard transformation matrices in column-major layout, and the GLM function calls that correspond to each matrix operation.</p>`,

  objectives: [
    'Use this card as a quick reference during any lab from Lab 07 (Transformations) onward',
    'All matrices are shown in the row-reading convention used on paper; GLM stores them column-major internally but the mathematical content is identical',
  ],

  concept_html: `<p>This reference card contains no exercises. Use it alongside worksheets WS-M02 through WS-M07 and the Transformations lab (Lab 07) onward. All matrices are written in the <strong>row-reading convention</strong> used on paper: each row is displayed left to right. GLM stores them in <strong>column-major order</strong> internally (first column occupies memory indices 0-3, second column 4-7, and so on), but the mathematical content is identical. You do not need to rearrange these matrices when writing GLM code. Refer to the Column-Major Convention lab (WS-M04) if you need a reminder of the storage layout.</p>`,

  tasks: [
    {
      title: 'Task 1 - Reference Tables',
      blocks: [
        {
          type: 'prose',
          html: `<h3>Section 1 - Trigonometric Values at Standard Angles</h3>
<table>
  <thead>
    <tr><th>Angle (deg)</th><th>Angle (rad)</th><th>sin</th><th>cos</th><th>tan</th></tr>
  </thead>
  <tbody>
    <tr><td>0</td><td>0</td><td>0</td><td>1</td><td>0</td></tr>
    <tr><td>30</td><td>&pi;/6</td><td>1/2</td><td>&radic;3/2 &approx; 0.866</td><td>1/&radic;3 &approx; 0.577</td></tr>
    <tr><td>45</td><td>&pi;/4</td><td>&radic;2/2 &approx; 0.707</td><td>&radic;2/2 &approx; 0.707</td><td>1</td></tr>
    <tr><td>60</td><td>&pi;/3</td><td>&radic;3/2 &approx; 0.866</td><td>1/2</td><td>&radic;3 &approx; 1.732</td></tr>
    <tr><td>90</td><td>&pi;/2</td><td>1</td><td>0</td><td>undefined</td></tr>
  </tbody>
</table>

<h3>Section 2 - Translation Matrix T(tx, ty, tz)</h3>
<p>Column-major layout (tx, ty, tz appear in column 3, GLM memory indices 12-14):</p>
<pre>| 1  0  0  tx |
| 0  1  0  ty |
| 0  0  1  tz |
| 0  0  0   1 |</pre>
<p>GLM: <code>glm::translate(mat, glm::vec3(tx, ty, tz))</code></p>

<h3>Section 3 - Rotation Matrices (angle t in radians, abbreviated)</h3>
<p><strong>R_x(t)</strong> - rotation around X axis:</p>
<pre>| 1    0       0    0 |
| 0  cos(t)  -sin(t) 0 |
| 0  sin(t)   cos(t) 0 |
| 0    0       0    1 |</pre>
<p>GLM: <code>glm::rotate(mat, t, glm::vec3(1,0,0))</code></p>

<p><strong>R_y(t)</strong> - rotation around Y axis (note sign swap: -sin at row 2, col 0):</p>
<pre>|  cos(t)  0  sin(t)  0 |
|    0     1    0     0 |
| -sin(t)  0  cos(t)  0 |
|    0     0    0     1 |</pre>
<p>GLM: <code>glm::rotate(mat, t, glm::vec3(0,1,0))</code></p>

<p><strong>R_z(t)</strong> - rotation around Z axis:</p>
<pre>| cos(t)  -sin(t)  0  0 |
| sin(t)   cos(t)  0  0 |
|   0        0     1  0 |
|   0        0     0  1 |</pre>
<p>GLM: <code>glm::rotate(mat, t, glm::vec3(0,0,1))</code></p>

<h3>Section 4 - Perspective Projection Matrix</h3>
<p>Parameters: f = 1/tan(fov/2), a = aspect ratio, n = near, z = far.</p>
<pre>| f/a   0       0            0         |
|  0    f       0            0         |
|  0    0   -(z+n)/(z-n)  -2zn/(z-n)  |
|  0    0      -1            0         |</pre>
<p>M[2][3] = -1 copies -z_view into w_clip, enabling the perspective divide. After dividing clip coords by w, NDC values land in [-1, 1] for visible geometry.</p>
<p>GLM: <code>glm::perspective(glm::radians(fov_degrees), aspect, near, far)</code></p>

<h3>Section 5 - GLM Function Reference</h3>
<table>
  <thead>
    <tr><th>Purpose</th><th>GLM Call</th></tr>
  </thead>
  <tbody>
    <tr><td>Identity matrix</td><td><code>glm::mat4(1.0f)</code></td></tr>
    <tr><td>Translation</td><td><code>glm::translate(mat, glm::vec3(tx, ty, tz))</code></td></tr>
    <tr><td>Rotation</td><td><code>glm::rotate(mat, glm::radians(degrees), axis)</code></td></tr>
    <tr><td>Uniform scale</td><td><code>glm::scale(mat, glm::vec3(s, s, s))</code></td></tr>
    <tr><td>Perspective projection</td><td><code>glm::perspective(glm::radians(fov), aspect, near, far)</code></td></tr>
    <tr><td>Orthographic projection</td><td><code>glm::ortho(left, right, bottom, top, near, far)</code></td></tr>
    <tr><td>Look-at / view matrix</td><td><code>glm::lookAt(eye, centre, worldUp)</code></td></tr>
    <tr><td>Normal matrix (C++)</td><td><code>glm::mat3(glm::transpose(glm::inverse(model)))</code></td></tr>
    <tr><td>Normal matrix (GLSL)</td><td><code>mat3(transpose(inverse(model)))</code></td></tr>
  </tbody>
</table>`,
        },
      ],
    },
  ],

  languages: [],
}, 'labs/mcp/math_WSREF_reference_card.html');
