import { mkdirSync, writeFileSync } from 'fs';
import { dirname, resolve } from 'path';
import { PRISM_CSS, LAB_CSS, printStyle, LAB_JS, prismScriptTags } from './template.js';

export interface LabPackage {
  name: string;
  description: string;
  docs_url: string;
}

export interface LabTask {
  title: string;
  steps_html: string;
  code?: { lang: 'cpp' | 'glsl' | 'bash' | 'json'; content: string };
  code_type?: 'skeleton' | 'reference' | 'bug';
}

export interface LabSpec {
  module_code: string;
  module_title: string;
  lab_number: string;
  lab_title: string;
  overview_html: string;
  objectives: string[];
  concept_html: string;
  prerequisites?: string[];
  packages?: LabPackage[];
  tasks: LabTask[];
  solution_code?: string;
  languages: string[];
}

function escapeHtml(str: string): string {
  return str
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;');
}

function buildLabHtml(spec: LabSpec): string {
  const labTitle = `Lab ${spec.lab_number} - ${spec.lab_title}`;

  const objectiveItems = spec.objectives
    .map(o => `      <li>${o}</li>`)
    .join('\n');

  const prerequisitesSection = spec.prerequisites && spec.prerequisites.length > 0
    ? `\n  <section class="lab-section">
    <h2>Prerequisites</h2>
    <ul>
${spec.prerequisites.map(p => `      <li>${p}</li>`).join('\n')}
    </ul>
  </section>`
    : '';

  const packagesSection = spec.packages && spec.packages.length > 0
    ? `\n  <section class="lab-section">
    <h2>Packages</h2>
    <table class="pkg-table">
      <thead>
        <tr><th>Package</th><th>Description</th><th>Docs</th></tr>
      </thead>
      <tbody>
${spec.packages.map(p => `        <tr>
          <td><strong>${p.name}</strong></td>
          <td>${p.description}</td>
          <td><a href="${p.docs_url}" target="_blank">${p.docs_url.replace(/^https?:\/\//, '')}</a></td>
        </tr>`).join('\n')}
      </tbody>
    </table>
  </section>`
    : '';

  const taskBlocks = spec.tasks.map(task => {
    let trailingCode = '';
    if (task.code) {
      const comment = task.code_type === 'reference' ? `// file: ... -- provided for reference\n` : '';
      trailingCode = `\n      <pre><code class="language-${task.code.lang}">${escapeHtml(comment + task.code.content)}</code></pre>`;
    }
    return `    <div class="task-block">
      <h3>${task.title}</h3>
      ${task.steps_html}${trailingCode}
    </div>`;
  }).join('\n\n');

  const solutionSection = spec.solution_code
    ? `\n  <section class="lab-section worked-solution">
    <h2>Solution</h2>
    <p>Try the tasks above before revealing the solution.</p>
    <button class="solution-reveal-btn" onclick="toggleSolution(this)">Show Solution</button>
    <div class="solution-content" style="display:none">
      <div class="task-block">
        <pre><code class="language-cpp">${escapeHtml(spec.solution_code)}</code></pre>
      </div>
    </div>
  </section>`
    : '';

  return `<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>${labTitle} | ${spec.module_code}</title>

  <style>
    ${PRISM_CSS}
  </style>

  <style>${LAB_CSS}${printStyle(spec.module_code)}
  </style>
</head>
<body>

<header class="lab-header">
  <img src="https://www.uwtsd.ac.uk/themes/custom/rutherford/images/logo.svg"
       alt="University of Wales Trinity Saint David">
  <div class="lab-header-text">
    <div class="module-code">${spec.module_code} - ${spec.module_title}</div>
    <h1>${labTitle}</h1>
  </div>
</header>

<main>

  <section class="lab-section">
    <h2>Overview</h2>
    ${spec.overview_html}
  </section>

  <section class="lab-section">
    <h2>Learning Objectives</h2>
    <p>By the end of this lab you will be able to:</p>
    <ul>
${objectiveItems}
    </ul>
  </section>
${prerequisitesSection}
  <section class="lab-section">
    <h2>Concept</h2>
    ${spec.concept_html}
  </section>
${packagesSection}
  <section class="lab-section">
    <h2>Tasks</h2>

${taskBlocks}

  </section>
${solutionSection}
</main>

<script>${LAB_JS}
</script>

${prismScriptTags(spec.languages)}

</body>
</html>`;
}

export function buildAndWriteLab(
  spec: LabSpec,
  outputPath: string,
): { path: string; bytes: number } {
  const abs = resolve(process.cwd(), outputPath);
  mkdirSync(dirname(abs), { recursive: true });
  const html = buildLabHtml(spec);
  writeFileSync(abs, html, 'utf8');
  return { path: outputPath, bytes: Buffer.byteLength(html, 'utf8') };
}
