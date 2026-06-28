import { PRISM_CORE_JS, PRISM_LANG_JS } from './prism-vendor.js';
import { KATEX_CSS }                   from './katex-vendor.js';
import { D3_JS }                       from './d3-vendor.js';
import type { LabSpec, LabOptions }    from './schema.js';

// ── Prism syntax-highlight theme ──────────────────────────────────────────────
export const PRISM_CSS =
  `code[class*=language-],pre[class*=language-]{color:#ccc;background:0 0;font-family:Consolas,Monaco,'Andale Mono','Ubuntu Mono',monospace;font-size:1em;text-align:left;white-space:pre;word-spacing:normal;word-break:normal;word-wrap:normal;line-height:1.5;-moz-tab-size:4;-o-tab-size:4;tab-size:4;-webkit-hyphens:none;-moz-hyphens:none;-ms-hyphens:none;hyphens:none}pre[class*=language-]{padding:1em;margin:.5em 0;overflow:auto}:not(pre)>code[class*=language-],pre[class*=language-]{background:#2d2d2d}:not(pre)>code[class*=language-]{padding:.1em;border-radius:.3em;white-space:normal}.token.block-comment,.token.cdata,.token.comment,.token.doctype,.token.prolog{color:#999}.token.punctuation{color:#ccc}.token.attr-name,.token.deleted,.token.namespace,.token.tag{color:#e2777a}.token.function-name{color:#6196cc}.token.boolean,.token.function,.token.number{color:#f08d49}.token.class-name,.token.constant,.token.property,.token.symbol{color:#f8c555}.token.atrule,.token.builtin,.token.important,.token.keyword,.token.selector{color:#cc99cd}.token.attr-value,.token.char,.token.regex,.token.string,.token.variable{color:#7ec699}.token.entity,.token.operator,.token.url{color:#67cdcc}.token.bold,.token.important{font-weight:700}.token.italic{font-style:italic}.token.entity{cursor:help}.token.inserted{color:green}`;

// ── UWTSD lab stylesheet ──────────────────────────────────────────────────────
export const LAB_CSS = `
    /* ---- Reset and base ---- */
    *, *::before, *::after { box-sizing: border-box; margin: 0; padding: 0; }

    :root {
      --uwtsd-navy:    #003366;
      --uwtsd-gold:    #C8A951;
      --uwtsd-light:   #F4F4F4;
      --uwtsd-white:   #FFFFFF;
      --uwtsd-text:    #1A1A1A;
      --uwtsd-muted:   #555555;
      --uwtsd-code-bg: #1E1E1E;
    }

    body {
      font-family: 'Segoe UI', Arial, sans-serif;
      font-size: 16px;
      line-height: 1.6;
      color: var(--uwtsd-text);
      background: var(--uwtsd-light);
    }

    /* ---- Header ---- */
    .lab-header {
      background: var(--uwtsd-navy);
      color: var(--uwtsd-white);
      padding: 1.25rem 2rem;
      display: flex;
      align-items: center;
      justify-content: space-between;
      gap: 1rem;
    }

    .lab-header img {
      height: 48px;
      filter: brightness(0) invert(1);
    }

    .lab-header-text { flex: 1; }

    .lab-header-text .module-code {
      font-size: 0.8rem;
      letter-spacing: 0.08em;
      text-transform: uppercase;
      color: var(--uwtsd-gold);
      font-weight: 600;
    }

    .lab-header-text h1 {
      font-size: 1.4rem;
      font-weight: 700;
      margin-top: 0.2rem;
    }

    /* ---- Main content ---- */
    main {
      max-width: 960px;
      margin: 2rem auto;
      padding: 0 1.5rem;
    }

    /* ---- Sections ---- */
    .lab-section {
      background: var(--uwtsd-white);
      border-radius: 6px;
      padding: 1.5rem 1.75rem;
      margin-bottom: 1.5rem;
      box-shadow: 0 1px 4px rgba(0,0,0,0.07);
    }

    .lab-section h2 {
      font-size: 1.1rem;
      font-weight: 700;
      color: var(--uwtsd-navy);
      margin-bottom: 1rem;
      text-transform: uppercase;
      letter-spacing: 0.04em;
    }

    .lab-section h3 {
      font-size: 1rem;
      font-weight: 600;
      color: var(--uwtsd-navy);
      margin: 1.25rem 0 0.5rem;
    }

    .lab-section p { margin-bottom: 0.75rem; }

    .lab-section ul,
    .lab-section ol {
      padding-left: 1.4rem;
      margin-bottom: 0.75rem;
    }

    .lab-section li { margin-bottom: 0.35rem; }

    code {
      font-family: 'Consolas', 'Courier New', monospace;
      font-size: 0.88em;
      background: #eef0f4;
      color: #c0392b;
      padding: 0.1em 0.35em;
      border-radius: 3px;
    }

    kbd {
      font-family: 'Consolas', 'Courier New', monospace;
      font-size: 0.85em;
      background: #f0f0f0;
      border: 1px solid #ccc;
      border-radius: 3px;
      padding: 0.1em 0.4em;
    }

    /* ---- Code blocks ---- */
    pre {
      background: var(--uwtsd-code-bg) !important;
      color: #cccccc;
      border-radius: 6px;
      padding: 1.1rem 1.25rem;
      overflow-x: auto;
      margin: 0.75rem 0 1rem;
      font-size: 0.88rem;
      line-height: 1.5;
    }

    pre code {
      background: none !important;
      color: inherit !important;
      padding: 0;
      font-size: inherit;
    }

    /* ---- Package table ---- */
    .pkg-table {
      width: 100%;
      border-collapse: collapse;
      font-size: 0.93rem;
      margin-top: 0.5rem;
    }

    .pkg-table th {
      background: var(--uwtsd-navy);
      color: var(--uwtsd-white);
      text-align: left;
      padding: 0.6rem 0.85rem;
      font-size: 0.8rem;
      text-transform: uppercase;
      letter-spacing: 0.05em;
    }

    .pkg-table td {
      padding: 0.7rem 0.85rem;
      vertical-align: top;
      border-bottom: 1px solid #e4e8f0;
    }

    .pkg-table td:first-child { white-space: nowrap; }
    .pkg-table td:last-child  { white-space: nowrap; }

    .pkg-table tr:last-child td { border-bottom: none; }

    .pkg-table tr:nth-child(even) td { background: #f8f9fc; }

    .pkg-table a { color: var(--uwtsd-navy); font-weight: 600; }
    .pkg-table a:hover { text-decoration: underline; }

    /* ---- Platform tabs ---- */
    .tab-bar {
      display: flex;
      gap: 0.25rem;
      margin-bottom: 1.25rem;
      border-bottom: 2px solid #dde3ed;
    }

    .tab-btn {
      padding: 0.5rem 1.1rem;
      font-size: 0.88rem;
      font-weight: 600;
      font-family: inherit;
      border: none;
      background: none;
      color: var(--uwtsd-muted);
      cursor: pointer;
      border-bottom: 3px solid transparent;
      margin-bottom: -2px;
      transition: color 0.15s, border-color 0.15s;
    }

    .tab-btn:hover { color: var(--uwtsd-navy); }

    .tab-btn.active {
      color: var(--uwtsd-navy);
      border-bottom-color: var(--uwtsd-gold);
    }

    .tab-panel { display: none; }
    .tab-panel.active { display: block; }

    /* ---- Task steps ---- */
    .task-block {
      border: 1px solid #dde3ed;
      border-radius: 6px;
      padding: 1rem 1.25rem;
      margin: 1rem 0;
      background: #f8f9fc;
    }

    .task-block h3 {
      font-size: 0.95rem;
      color: var(--uwtsd-navy);
      margin-bottom: 0.5rem;
    }

    /* ---- Code file header ---- */
    .code-file-header {
      background: #2d2d2d;
      color: #888;
      font-family: 'Consolas', 'Courier New', monospace;
      font-size: 0.78rem;
      padding: 0.3rem 1.25rem;
      border-radius: 6px 6px 0 0;
      letter-spacing: 0.03em;
    }

    .code-file-header + pre {
      border-radius: 0 0 6px 6px !important;
      margin-top: 0 !important;
    }

    /* ---- Solution reveal ---- */
    .solution-btn {
      display: inline-block;
      margin: 0.5rem 0 1rem;
      padding: 0.55rem 1.4rem;
      background: var(--uwtsd-navy);
      color: var(--uwtsd-white);
      border: none;
      border-radius: 4px;
      font-size: 0.93rem;
      font-weight: 600;
      font-family: inherit;
      cursor: pointer;
      transition: background 0.15s;
    }

    .solution-btn:hover { background: #004080; }

    .solution-panel { display: none; margin-top: 0.5rem; }
    .solution-panel.open { display: block; }

    .solution-canvas-wrap {
      overflow-x: auto;
      margin-bottom: 1rem;
    }
    .solution-canvas-wrap:last-child { margin-bottom: 0; }

    /* ---- Fill-in-the-blank ---- */
    .fill-blanks-group { margin: 0.5rem 0 0.25rem; }

    .blank-question {
      margin: 0.6rem 0;
      line-height: 2.2;
    }

    .blank-input {
      display: inline-block;
      border: none;
      border-bottom: 2px solid var(--uwtsd-navy);
      background: transparent;
      font-family: inherit;
      font-size: inherit;
      color: var(--uwtsd-text);
      padding: 0.05em 0.3em;
      width: 14em;
      outline: none;
      transition: border-color 0.2s, background 0.2s;
      vertical-align: baseline;
    }

    .blank-input:focus { border-bottom-color: var(--uwtsd-gold); }

    .blank-input.blank-correct {
      border-bottom-color: #27ae60;
      background: rgba(39,174,96,0.09);
      border-radius: 3px 3px 0 0;
    }

    .blank-input.blank-incorrect {
      border-bottom-color: #e74c3c;
      background: rgba(231,76,60,0.09);
      border-radius: 3px 3px 0 0;
    }

    .blank-feedback {
      display: block;
      font-size: 0.82rem;
      font-weight: 600;
      margin-top: 0.2rem;
      min-height: 1.2em;
    }

    .feedback-correct { color: #27ae60; }
    .feedback-incorrect { color: #c0392b; }

    .check-blanks-btn {
      display: inline-block;
      margin: 0.75rem 0 0.25rem;
      padding: 0.5rem 1.2rem;
      background: var(--uwtsd-gold);
      color: var(--uwtsd-navy);
      border: none;
      border-radius: 4px;
      font-size: 0.9rem;
      font-weight: 700;
      font-family: inherit;
      cursor: pointer;
      transition: background 0.15s;
    }

    .check-blanks-btn:hover { background: #b8952e; }

    /* ---- Multiple choice ---- */
    .mc-block { margin: 0.75rem 0 1rem; }

    .mc-item {
      border: 1px solid #dde3ed;
      border-radius: 6px;
      padding: 0.75rem 1rem;
      margin-bottom: 0.75rem;
      background: #fff;
    }

    .mc-question-text { font-weight: 600; margin-bottom: 0.5rem; }

    .mc-options { display: flex; flex-direction: column; gap: 0; margin: 0.4rem 0; }

    .mc-option {
      display: flex;
      align-items: flex-start;
      gap: 0.6rem;
      padding: 0.35rem 0.5rem;
      cursor: pointer;
      border-radius: 4px;
      transition: background 0.1s;
    }
    .mc-option:hover { background: #f0f4ff; }

    .mc-option input[type=radio] { margin-top: 0.2em; flex-shrink: 0; }

    .mc-option.mc-correct   { color: #27ae60; font-weight: 600; }
    .mc-option.mc-incorrect { color: #e74c3c; }

    .mc-explanation {
      display: none;
      font-size: 0.88rem;
      color: var(--uwtsd-muted);
      margin-top: 0.4rem;
      padding: 0.4rem 0.6rem;
      border-left: 3px solid var(--uwtsd-gold);
      background: #fffdf0;
    }
    .mc-explanation.visible { display: block; }

    .mc-feedback {
      font-size: 0.82rem;
      font-weight: 600;
      min-height: 1.2em;
      margin-top: 0.25rem;
    }

    .mc-item-static { margin-bottom: 0.75rem; }
    .mc-options-static { padding-left: 1.4rem; margin: 0.4rem 0; }

    /* ---- Short answer ---- */
    .sa-block { margin: 0.75rem 0 1rem; }

    .sa-item {
      border: 1px solid #dde3ed;
      border-radius: 6px;
      padding: 0.75rem 1rem;
      margin-bottom: 0.75rem;
      background: #fff;
    }

    .sa-question-text { font-weight: 600; margin-bottom: 0.5rem; }

    .sa-textarea {
      width: 100%;
      min-height: 6rem;
      padding: 0.5rem 0.75rem;
      font-family: inherit;
      font-size: 0.93rem;
      border: 1px solid #ccd0da;
      border-radius: 4px;
      resize: vertical;
      margin-bottom: 0.5rem;
      color: var(--uwtsd-text);
      background: #fafbfc;
      box-sizing: border-box;
    }
    .sa-textarea:focus { outline: none; border-color: var(--uwtsd-navy); }

    .sa-reveal-btn {
      display: inline-block;
      padding: 0.4rem 1rem;
      background: transparent;
      color: var(--uwtsd-navy);
      border: 1px solid var(--uwtsd-navy);
      border-radius: 4px;
      font-size: 0.88rem;
      font-weight: 600;
      font-family: inherit;
      cursor: pointer;
      transition: background 0.15s, color 0.15s;
    }
    .sa-reveal-btn:hover { background: var(--uwtsd-navy); color: white; }

    .sa-answer-panel {
      display: none;
      margin-top: 0.5rem;
      padding: 0.6rem 0.85rem;
      background: #f0f4f0;
      border-left: 3px solid #27ae60;
      border-radius: 0 4px 4px 0;
      font-size: 0.93rem;
    }
    .sa-answer-panel.visible { display: block; }

    .sa-item-static { margin-bottom: 0.75rem; }
    .sa-lines {
      border-bottom: 1px solid #aaa;
      min-height: 5rem;
      margin-top: 0.4rem;
    }

    /* ---- Math blocks (KaTeX pre-rendered) ---- */
    .math-block {
      margin: 1rem 0;
      overflow-x: auto;
      text-align: center;
    }
    .math-block .katex-display { margin: 0; }

    /* ---- Visualization blocks (D3) ---- */
    .viz-block {
      margin: 1rem 0;
      border: 1px solid #dde3ed;
      border-radius: 6px;
      overflow: hidden;
      background: #fff;
    }`;

export function printStyle(moduleCode: string): string {
  return `

    /* ---- Print stylesheet ---- */
    @media print {
      .lab-header { background: white; color: black; border-bottom: 2px solid #003366; }
      .lab-header img { filter: none; }
      .lab-header-text .module-code { color: #003366; }
      .lab-header-text .lab-meta { color: #555; }
      body { background: white; font-size: 13px; }
      main { max-width: 100%; margin: 0; padding: 0 1rem; }
      .lab-section { box-shadow: none; border: 1px solid #ccc; break-inside: avoid; }
      pre { white-space: pre-wrap; word-break: break-word; }
      a { color: inherit; text-decoration: none; }
      .solution-btn { display: none; }
      .solution-panel { display: block !important; }
      .check-blanks-btn { display: none; }
      .blank-input { border-bottom: 1px solid #555; min-width: 8em; background: transparent !important; }
      .blank-feedback { display: none; }
      .mc-option { break-inside: avoid; }
      .mc-option input[type=radio] { display: none; }
      .sa-reveal-btn { display: none; }
      .sa-textarea { border: 1px solid #999; background: transparent !important; min-height: 5rem; }
      .sa-answer-panel { display: block !important; border-left: 2px solid #555; background: #f9f9f9; }
      @page {
        margin: 1.8cm 2cm;
        @top-right { content: "${moduleCode}"; font-size: 9pt; color: #555; }
        @bottom-center { content: counter(page) " of " counter(pages); font-size: 9pt; }
      }
    }`;
}

export const LAB_JS = `
  function switchTab(e, id) {
    const section = e.target.closest('.lab-section');
    section.querySelectorAll('.tab-btn').forEach(b => b.classList.remove('active'));
    section.querySelectorAll('.tab-panel').forEach(p => p.classList.remove('active'));
    e.target.classList.add('active');
    section.querySelector('#' + id).classList.add('active');
  }

  function renderSolutionCanvas(canvas, code, filename) {
    var ctx = canvas.getContext('2d');
    var dpr = window.devicePixelRatio || 1;
    var lineHeight = 19;
    var headerHeight = filename ? 28 : 0;
    var padX = 20;
    var padY = 14;
    var codeFont = '13px Consolas, "Courier New", monospace';
    ctx.font = codeFont;
    var lines = code.split('\\n').map(function(l) { return l.replace(/\\r$/, ''); });
    while (lines.length && lines[lines.length - 1] === '') lines.pop();
    var maxW = 0;
    for (var i = 0; i < lines.length; i++) {
      var w = ctx.measureText(lines[i]).width;
      if (w > maxW) maxW = w;
    }
    var containerW = canvas.parentElement ? (canvas.parentElement.clientWidth || 0) : 0;
    var cssW = Math.max(maxW + padX * 2, containerW || 400);
    var cssH = lines.length * lineHeight + padY * 2 + headerHeight;
    canvas.width = Math.round(cssW * dpr);
    canvas.height = Math.round(cssH * dpr);
    canvas.style.width = cssW + 'px';
    canvas.style.height = cssH + 'px';
    ctx.scale(dpr, dpr);
    if (filename) {
      ctx.fillStyle = '#2d2d2d';
      ctx.fillRect(0, 0, cssW, headerHeight);
      ctx.font = '11px Consolas, "Courier New", monospace';
      ctx.fillStyle = '#888888';
      ctx.textBaseline = 'middle';
      ctx.fillText(filename, padX, headerHeight / 2);
      ctx.fillStyle = '#444444';
      ctx.fillRect(0, headerHeight - 1, cssW, 1);
    }
    ctx.fillStyle = '#1E1E1E';
    ctx.fillRect(0, headerHeight, cssW, cssH - headerHeight);
    ctx.font = codeFont;
    ctx.fillStyle = '#cccccc';
    ctx.textBaseline = 'top';
    for (var i = 0; i < lines.length; i++) {
      ctx.fillText(lines[i], padX, headerHeight + padY + i * lineHeight);
    }
  }

  function toggleSolution(btn) {
    var panel = btn.nextElementSibling;
    var open = panel.classList.toggle('open');
    btn.textContent = open ? 'Hide Solution' : 'Show Solution';
    if (open) {
      panel.querySelectorAll('canvas[data-code]').forEach(function(c) {
        if (!c.dataset.rendered) {
          renderSolutionCanvas(c, c.dataset.code, c.dataset.filename || '');
          c.dataset.rendered = '1';
        }
      });
    }
  }

  function checkBlanks(btn) {
    var task = btn.closest('.task-block');
    task.querySelectorAll('.blank-question').forEach(function(q) {
      var answers = JSON.parse(q.dataset.answers);
      var inputs = q.querySelectorAll('.blank-input');
      var allCorrect = true;
      inputs.forEach(function(input, i) {
        var expected = [].concat(answers[i]).map(function(s) { return s.toLowerCase().trim(); });
        var val = input.value.toLowerCase().trim();
        var ok = expected.indexOf(val) !== -1;
        input.classList.toggle('blank-correct', ok);
        input.classList.toggle('blank-incorrect', !ok && val !== '');
        if (!ok) allCorrect = false;
      });
      var fb = q.querySelector('.blank-feedback');
      fb.textContent = allCorrect ? 'Correct!' : 'Check your answer.';
      fb.className = 'blank-feedback ' + (allCorrect ? 'feedback-correct' : 'feedback-incorrect');
    });
  }

  function revealMCAnswer(radio) {
    var item    = radio.closest('.mc-item');
    var correct = parseInt(item.dataset.correct, 10);
    var radios  = Array.from(item.querySelectorAll('input[type=radio]'));
    var chosen  = radios.indexOf(radio);
    item.querySelectorAll('.mc-option').forEach(function(opt, i) {
      opt.classList.toggle('mc-correct',   i === correct);
      opt.classList.toggle('mc-incorrect', i === chosen && i !== correct);
    });
    var fb = item.querySelector('.mc-feedback');
    if (fb) {
      fb.textContent = chosen === correct ? 'Correct!' : 'Incorrect.';
      fb.className   = 'mc-feedback ' + (chosen === correct ? 'feedback-correct' : 'feedback-incorrect');
    }
    var exp = item.querySelector('.mc-explanation');
    if (exp) exp.classList.add('visible');
    item.querySelectorAll('input[type=radio]').forEach(function(r) { r.disabled = true; });
  }

  function toggleModelAnswer(btn) {
    var panel = btn.nextElementSibling;
    var open  = panel.classList.toggle('visible');
    btn.textContent = open ? 'Hide Model Answer' : 'Show Model Answer';
  }`;

export function prismInlineScripts(languages: string[]): string {
  const components = languages
    .filter(l => PRISM_LANG_JS[l])
    .map(l => PRISM_LANG_JS[l])
    .join('\n');
  return `<script>${PRISM_CORE_JS}\n${components}<\/script>`;
}

// ── Page assembly ─────────────────────────────────────────────────────────────
export interface PageParts {
  spec:           LabSpec;
  opts:           Required<LabOptions>;
  objectivesHtml: string;
  packagesHtml:   string;
  tasksHtml:      string;
  solutionHtml:   string;
  useKaTeX:       boolean;
  useD3:          boolean;
}

export function buildPage(parts: PageParts): string {
  const { spec, opts, objectivesHtml, packagesHtml, tasksHtml, solutionHtml, useKaTeX, useD3 } = parts;
  const labTitle    = `Lab ${spec.lab_number} - ${spec.lab_title}`;
  const katexStyle  = useKaTeX ? `\n  <style>${KATEX_CSS}</style>` : '';
  const d3HeadScript = useD3   ? `\n  <script>${D3_JS}<\/script>` : '';

  return `<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>${labTitle} | ${spec.module_code}</title>
  <link rel="icon" type="image/svg+xml" href="https://www.uwtsd.ac.uk/themes/custom/rutherford/images/logo.svg">

  <style>
    ${PRISM_CSS}
  </style>

  <style>${LAB_CSS}${printStyle(spec.module_code)}
  </style>${katexStyle}${d3HeadScript}
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
${objectivesHtml}
    </ul>
  </section>

  <section class="lab-section">
    <h2>Concept</h2>
    ${spec.concept_html}
  </section>
${packagesHtml}
  <section class="lab-section">
    <h2>Tasks</h2>

${tasksHtml}

  </section>
${solutionHtml}
</main>

<script>${LAB_JS}
<\/script>

${prismInlineScripts(spec.languages)}

</body>
</html>`;
}
