export const PRISM_CSS =
  `code[class*=language-],pre[class*=language-]{color:#ccc;background:0 0;font-family:Consolas,Monaco,'Andale Mono','Ubuntu Mono',monospace;font-size:1em;text-align:left;white-space:pre;word-spacing:normal;word-break:normal;word-wrap:normal;line-height:1.5;-moz-tab-size:4;-o-tab-size:4;tab-size:4;-webkit-hyphens:none;-moz-hyphens:none;-ms-hyphens:none;hyphens:none}pre[class*=language-]{padding:1em;margin:.5em 0;overflow:auto}:not(pre)>code[class*=language-],pre[class*=language-]{background:#2d2d2d}:not(pre)>code[class*=language-]{padding:.1em;border-radius:.3em;white-space:normal}.token.block-comment,.token.cdata,.token.comment,.token.doctype,.token.prolog{color:#999}.token.punctuation{color:#ccc}.token.attr-name,.token.deleted,.token.namespace,.token.tag{color:#e2777a}.token.function-name{color:#6196cc}.token.boolean,.token.function,.token.number{color:#f08d49}.token.class-name,.token.constant,.token.property,.token.symbol{color:#f8c555}.token.atrule,.token.builtin,.token.important,.token.keyword,.token.selector{color:#cc99cd}.token.attr-value,.token.char,.token.regex,.token.string,.token.variable{color:#7ec699}.token.entity,.token.operator,.token.url{color:#67cdcc}.token.bold,.token.important{font-weight:700}.token.italic{font-style:italic}.token.entity{cursor:help}.token.inserted{color:green}`;

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

    /* ---- Expected result ---- */
    .expected-result {
      background: #eaf4ea;
      border-radius: 6px;
      padding: 0.85rem 1rem;
      margin-top: 0.5rem;
      font-size: 0.95rem;
    }

    /* ---- Extension challenge ---- */
    .extension {
      background: #fef9ec;
      border-radius: 6px;
      padding: 0.85rem 1rem;
      margin-top: 0.5rem;
    }

    .extension strong { color: #7a5c00; }

    /* ---- Solution reveal ---- */
    .solution-reveal-btn {
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

    .solution-reveal-btn:hover { background: #004080; }`;

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
      .solution-reveal-btn { display: none; }
      .solution-content { display: block !important; }
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

  function toggleSolution(btn) {
    const content = btn.nextElementSibling;
    const hidden = content.style.display === 'none';
    content.style.display = hidden ? 'block' : 'none';
    btn.textContent = hidden ? 'Hide Solution' : 'Show Solution';
  }`;

const PRISM_COMPONENTS: Record<string, string> = {
  cpp:  'prism-cpp.min.js',
  glsl: 'prism-glsl.min.js',
  bash: 'prism-bash.min.js',
  json: 'prism-json.min.js',
};

export function prismScriptTags(languages: string[]): string {
  const base = `<script src="https://cdn.jsdelivr.net/npm/prismjs@1.29.0/prism.min.js"></script>`;
  const components = languages
    .filter(l => PRISM_COMPONENTS[l])
    .map(l => `<script src="https://cdn.jsdelivr.net/npm/prismjs@1.29.0/components/${PRISM_COMPONENTS[l]}"></script>`)
    .join('\n');
  return components ? `${base}\n${components}` : base;
}
