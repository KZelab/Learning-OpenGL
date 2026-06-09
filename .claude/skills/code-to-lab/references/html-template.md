# HTML Template Reference

HTML assembly is handled by the `assemble_lab` MCP tool (server: `lab-assembler`).
You do **not** write HTML directly. Populate a `LabSpec` object and call the tool.

The tool owns: UWTSD brand CSS, Prism tomorrow theme, solution reveal button,
`switchTab()` and `toggleSolution()` JS, print stylesheet.

---

## LabSpec schema

```typescript
interface LabSpec {
  // Identity
  module_code:   string;   // "ACCA6014"
  module_title:  string;   // "Graphics Programming"
  lab_number:    string;   // "03"
  lab_title:     string;   // "Shaders"

  // Core sections (all required)
  overview_html: string;   // HTML body, one or more <p> tags
  objectives:    string[]; // plain-text strings (tool wraps in <li>)
  concept_html:  string;   // HTML body, prose paragraphs

  // Optional sections
  prerequisites?: string[];           // plain-text strings
  packages?: {                        // setup labs only
    name: string;
    description: string;
    docs_url: string;
  }[];

  // Tasks (at least one required)
  tasks: {
    title:      string;   // "Task 1 - Create the VAO"
    steps_html: string;   // full HTML body: ol, p, inline pre/code, platform tabs, expected-result div
    code?: {
      lang:    "cpp" | "glsl" | "bash" | "json";
      content: string;    // raw source -- tool HTML-escapes this
    };
    code_type?: "skeleton" | "reference" | "bug";
  }[];

  // Solution (strongly recommended -- omit only if truly no worked answer exists)
  solution_code?: string;  // complete merged source, raw -- tool HTML-escapes and hides behind button

  // Prism component scripts to load
  languages: string[];     // e.g. ["cpp", "glsl"]
}
```

### output_path (extra field for the tool call)

Pass `output_path` alongside the spec -- it is not part of the spec itself:

```
output_path: "labs/lab_03_shaders.html"
```

The tool creates the directory if needed and returns `{ path, bytes }`.

---

## steps_html patterns

### Simple numbered steps

```html
<ol>
  <li>Open <code>src/Renderer.cpp</code> and locate <code>Renderer::init()</code>.</li>
  <li>...</li>
</ol>
```

### Inline code snippet inside a step

HTML-escape `<`, `>`, `&` inside `<pre><code>`:

```html
<ol>
  <li>
    Add the following near the top of <code>main.cpp</code>:
    <pre><code class="language-cpp">#include &lt;GL/glew.h&gt;
#include &lt;GLFW/glfw3.h&gt;</code></pre>
  </li>
</ol>
```

### Platform tabs

Use when Windows and Mac/Linux steps differ. Include the full tab structure
inside `steps_html`:

```html
<div class="tab-bar">
  <button class="tab-btn active" onclick="switchTab(event, 'tab-windows')">Windows</button>
  <button class="tab-btn"        onclick="switchTab(event, 'tab-unix')">Mac / Linux</button>
</div>
<div id="tab-windows" class="tab-panel active">
  <div class="task-block">
    <h3>Task 1 - Install on Windows</h3>
    <ol><li>...</li></ol>
  </div>
</div>
<div id="tab-unix" class="tab-panel">
  <div class="task-block">
    <h3>Task 1 - Install on Mac / Linux</h3>
    <ol><li>...</li></ol>
  </div>
</div>
```

### Expected result

Append inside `steps_html` after the `<ol>`:

```html
<div class="expected-result">
  <strong>Expected result:</strong> A blank window opens at 800x600 with a grey background.
</div>
```

### Extension challenge

Append inside `steps_html` at the end of the last task:

```html
<div class="extension">
  <strong>Extension:</strong> Add a second triangle using a second VAO and VBO.
</div>
```

---

## Concept section rules

Write in this order:
1. The problem -- what breaks or is impossible without this concept
2. The mechanism -- how it works, with enough detail to implement it
3. The API -- the specific calls, types, or keywords the student will use

Use `<strong>` on the first occurrence of every technical term.
Use `<code>` for all identifiers, function names, types, and API calls.
Avoid bullet points; write prose.

---

## Cross-lab references

Always use the lab name alongside its number:
"In the Environment Setup lab (Lab 00)" not "In Lab 00".

---

## Punctuation rules

- No em dashes (`--`, `&mdash;`) or semicolons in prose. Use a hyphen-minus or split into two sentences.
- No arrow glyphs (`->`, `&rarr;`). Use `->`.
- Titles and headings use hyphen-minus as separator: `Lab 03 - Shaders`, `Task 1 - Create the VAO`.

---

## Prism language keys

| Language | key   |
|----------|-------|
| C++      | `cpp`  |
| GLSL     | `glsl` |
| Bash     | `bash` |
| JSON     | `json` |

Include only the languages actually used in the lab.
