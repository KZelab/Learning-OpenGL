import { buildAndWriteLab } from '../tools/lab-assembler/dist/assembler.js';

buildAndWriteLab({
  options: { solution: true, copy_protect: false },
  module_code:   'ACCA6014',
  module_title:  'Graphics Programming',
  lab_number:    '99',
  lab_title:     'v2 Block System Test',
  overview_html: '<p>This file tests every v2 block type: prose, code, fill_blanks, multiple_choice, and short_answer.</p>',
  objectives: [
    'Verify prose blocks render correctly',
    'Verify fill-in-the-blank interactive mode',
    'Verify multiple choice with immediate feedback',
    'Verify short answer with model answer reveal',
    'Verify static (non-interactive) block rendering',
  ],
  concept_html: '<p>Each task in a v2 lab is made up of an ordered array of <strong>content blocks</strong>. Blocks can be prose, code, fill-in-the-blank questions, multiple choice, or short answer. Each question block has an <code>interactive</code> flag (default <code>true</code>) that controls whether it renders as an interactive widget or a static print layout.</p>',
  tasks: [
    {
      title: 'Task 1 - Prose and Code Blocks',
      blocks: [
        {
          type: 'prose',
          html: '<p>This is a prose block. It renders raw HTML directly. Use it for explanations, instructions, or diagrams.</p><ol><li>Step one instruction.</li><li>Step two instruction.</li></ol>',
        },
        {
          type: 'code',
          lang: 'javascript',
          filename: 'src/example.js',
          code_type: 'skeleton',
          content: `function greet(name) {
  // TODO: return a greeting string
}

console.log(greet('World'));`,
        },
      ],
    },
    {
      title: 'Task 2 - Fill-in-the-Blank (Interactive)',
      blocks: [
        {
          type: 'prose',
          html: '<p>Complete the sentences below. Matching is case-insensitive.</p>',
        },
        {
          type: 'fill_blanks',
          interactive: true,
          questions: [
            {
              question_html: 'The <strong>{{blank}}</strong> coordinate system places the origin at the camera position.',
              answers: [['View', 'view', 'Camera', 'camera', 'Eye', 'eye']],
            },
            {
              question_html: 'NDC stands for <strong>{{blank}}</strong> Device Coordinates.',
              answers: [['Normalised', 'Normalized']],
            },
            {
              question_html: 'The {{blank}} matrix is used to project 3D points into 2D clip space.',
              answers: [['projection', 'Projection']],
            },
          ],
        },
      ],
    },
    {
      title: 'Task 3 - Multiple Choice (Interactive)',
      blocks: [
        {
          type: 'multiple_choice',
          interactive: true,
          questions: [
            {
              question: 'Which matrix transforms vertices from world space into camera space?',
              options: ['Model matrix', 'View matrix', 'Projection matrix', 'Identity matrix'],
              correct: 1,
              explanation: 'The view matrix (also called the camera or look-at matrix) transforms from world space to view space.',
            },
            {
              question: 'What is the purpose of the depth buffer?',
              options: [
                'To store per-pixel colour values',
                'To determine which fragments are behind others',
                'To hold texture coordinates',
                'To cache vertex positions',
              ],
              correct: 1,
            },
          ],
        },
      ],
    },
    {
      title: 'Task 4 - Short Answer (Interactive)',
      blocks: [
        {
          type: 'short_answer',
          interactive: true,
          questions: [
            {
              question: 'Explain why normal vectors must be transformed by the inverse-transpose of the model matrix rather than the model matrix itself.',
              model_answer: 'Normal vectors are perpendicular to a surface. If the model matrix applies non-uniform scaling, applying it directly to normals changes the direction — they are no longer perpendicular to the transformed surface. The inverse-transpose compensates for this by preserving the correct perpendicularity relationship.',
            },
          ],
        },
      ],
    },
    {
      title: 'Task 5 - Static (Non-Interactive) Blocks',
      blocks: [
        {
          type: 'prose',
          html: '<p>These blocks have <code>interactive: false</code> — they render as static print-ready layouts with no interactive widgets.</p>',
        },
        {
          type: 'fill_blanks',
          interactive: false,
          questions: [
            {
              question_html: 'The {{blank}} matrix moves vertices from model space to world space.',
              answers: [['model', 'Model']],
            },
          ],
        },
        {
          type: 'multiple_choice',
          interactive: false,
          questions: [
            {
              question: 'Which space comes immediately after clip space in the pipeline?',
              options: ['World space', 'NDC', 'Screen space', 'View space'],
              correct: 1,
            },
          ],
        },
        {
          type: 'short_answer',
          interactive: false,
          questions: [
            {
              question: 'Describe the difference between orthographic and perspective projection.',
              model_answer: 'Orthographic projection preserves parallel lines and scale regardless of depth; objects appear the same size at any distance. Perspective projection simulates depth by making distant objects appear smaller, creating a realistic sense of space.',
            },
          ],
        },
      ],
    },
    {
      title: 'Task 6 - Python and TypeScript Code',
      blocks: [
        {
          type: 'prose',
          html: '<p>Testing expanded language support.</p>',
        },
        {
          type: 'code',
          lang: 'python',
          code_type: 'reference',
          content: `import numpy as np

def look_at(eye, center, up):
    f = (center - eye) / np.linalg.norm(center - eye)
    s = np.cross(f, up) / np.linalg.norm(np.cross(f, up))
    u = np.cross(s, f)
    return np.array([
        [s[0], s[1], s[2], -np.dot(s, eye)],
        [u[0], u[1], u[2], -np.dot(u, eye)],
        [-f[0], -f[1], -f[2],  np.dot(f, eye)],
        [0, 0, 0, 1],
    ])`,
        },
        {
          type: 'code',
          lang: 'typescript',
          code_type: 'skeleton',
          content: `interface Vec3 { x: number; y: number; z: number; }

function normalize(v: Vec3): Vec3 {
  // TODO: compute the unit vector
}

function dot(a: Vec3, b: Vec3): number {
  // TODO: compute dot product
}`,
        },
      ],
    },
  ],
  solution_code: `// Full solution here
function greet(name) { return 'Hello, ' + name + '!'; }
function normalize(v) { const len = Math.sqrt(v.x*v.x+v.y*v.y+v.z*v.z); return {x:v.x/len,y:v.y/len,z:v.z/len}; }
function dot(a, b) { return a.x*b.x + a.y*b.y + a.z*b.z; }`,
  languages: ['javascript', 'typescript', 'python'],
}, 'labs/mcp/lab_99_v2_test.html');

console.log('Generated labs/mcp/lab_99_v2_test.html');
