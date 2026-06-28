import { mkdirSync, writeFileSync } from 'fs';
import { dirname, resolve } from 'path';
import type { LabSpec, LabOptions, ContentBlock } from './schema.js';
import { renderProseBlock }                                           from './blocks/prose.js';
import { renderCodeBlock, escapeHtml }                               from './blocks/code.js';
import { renderFillBlanks, renderMultipleChoice, renderShortAnswer } from './blocks/questions.js';
import { renderMathBlock }                                           from './blocks/math.js';
import { renderVizBlock }                                            from './blocks/viz.js';
import { buildPage }                                                 from './template.js';

export type { LabSpec, LabOptions };

// ── Helpers ───────────────────────────────────────────────────────────────────
function resolveOptions(opt?: LabOptions): Required<LabOptions> {
  return {
    solution:       opt?.solution       ?? true,
    copy_protect:   opt?.copy_protect   ?? true,
    print_friendly: opt?.print_friendly ?? false,
  };
}

function hasBlockType(spec: LabSpec, type: string): boolean {
  return spec.tasks.some(t => t.blocks.some(b => b.type === type));
}

// ── Block dispatch ────────────────────────────────────────────────────────────
let _mcCounter = 0;

function renderBlock(block: ContentBlock): string {
  const interactive = 'interactive' in block ? (block.interactive ?? true) : true;
  switch (block.type) {
    case 'prose':           return renderProseBlock(block);
    case 'code':            return renderCodeBlock(block);
    case 'fill_blanks':     return renderFillBlanks(block.questions, interactive);
    case 'multiple_choice': return renderMultipleChoice(block.questions, interactive, ++_mcCounter);
    case 'short_answer':    return renderShortAnswer(block.questions, interactive);
    case 'math':            return renderMathBlock(block);
    case 'visualization':   return renderVizBlock(block);
  }
}

// ── Section builders ──────────────────────────────────────────────────────────
function buildPackagesHtml(spec: LabSpec): string {
  if (!spec.packages || spec.packages.length === 0) return '';
  return `\n  <section class="lab-section">
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
  </section>`;
}

function buildTasksHtml(spec: LabSpec): string {
  return spec.tasks.map(task => {
    const content = task.blocks.map(b => renderBlock(b)).join('\n\n');
    return `    <div class="task-block">
      <h3>${task.title}</h3>
      ${content}
    </div>`;
  }).join('\n\n');
}

function solutionCanvas(content: string, filename?: string): string {
  const fnAttr = filename ? ` data-filename="${filename}"` : '';
  return `<div class="solution-canvas-wrap"><canvas data-code="${escapeHtml(content)}"${fnAttr} style="display:block;border-radius:6px"></canvas></div>`;
}

function buildSolutionHtml(spec: LabSpec, opts: Required<LabOptions>): string {
  if (!opts.solution) return '';
  const preamble = `\n  <section class="lab-section worked-solution">
    <h2>Solution</h2>
    <p>Try the tasks above before revealing the solution.</p>
    <button class="solution-btn" onclick="toggleSolution(this)">Show Solution</button>
    <div class="solution-panel">`;
  const close = `\n    </div>\n  </section>`;

  if (spec.solution_files && spec.solution_files.length > 0) {
    const canvases = spec.solution_files
      .map(f => `      ${solutionCanvas(f.content, f.filename)}`)
      .join('\n');
    return `${preamble}\n${canvases}${close}`;
  }
  if (spec.solution_code) {
    return `${preamble}\n      ${solutionCanvas(spec.solution_code)}${close}`;
  }
  return '';
}

// ── HTML assembly ─────────────────────────────────────────────────────────────
function buildLabHtml(spec: LabSpec): string {
  _mcCounter = 0;
  const opts      = resolveOptions(spec.options);
  const useKaTeX  = hasBlockType(spec, 'math');
  const useD3     = hasBlockType(spec, 'visualization');

  return buildPage({
    spec,
    opts,
    objectivesHtml: spec.objectives.map(o => `      <li>${o}</li>`).join('\n'),
    packagesHtml:   buildPackagesHtml(spec),
    tasksHtml:      buildTasksHtml(spec),
    solutionHtml:   buildSolutionHtml(spec, opts),
    useKaTeX,
    useD3,
  });
}

// ── Public API ────────────────────────────────────────────────────────────────
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
