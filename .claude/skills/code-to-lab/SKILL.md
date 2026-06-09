---
name: code-to-lab
description: >
  Convert an OpenGL C++ project into a series of UWTSD-branded student lab
  worksheets, one per major system, delivered as standalone HTML files (with
  matching PDFs) ready to upload to Moodle as File resources. Use this skill
  whenever source code is present and the user wants a lab, worksheet,
  practical, or guided coding exercise. Trigger on: "turn this into a lab",
  "make a lab from this", "create a worksheet", "generate labs", "lab sheet",
  or any message combining code with words like lab, worksheet, practical, or
  tutorial. ALWAYS prefer over moodle-content when source code is present,
  regardless of whether Moodle is mentioned. When running inside Claude Code,
  this skill reads the project directory directly -- no pasting required.
---

# Code-to-Lab Skill

Converts a C++ OpenGL project into a series of **UWTSD-branded standalone lab
worksheets** -- one HTML file per major system, sequenced so each lab builds
incrementally on the previous one.

**Input:** project directory (Claude Code) or pasted code / GitHub URL (chat).
**Output:** `labs/lab_NN_title.html` per lab (solution embedded, hidden behind
a "Show Solution" button), plus `labs/README.md` as an ordered manifest.

HTML assembly is handled by the `assemble_lab` MCP tool (server:
`lab-assembler`). This skill's job is to produce structured content -- the MCP
tool owns all markup, CSS, JS, and branding.

Read `references/html-template.md` before generating content for any lab.

---

## Workflow

```
Project directory / pasted code / GitHub URL
        |
        v
Phase 1 -- COLLECT      Read source files from disk or input
        |
        v
Phase 2 -- ANALYSE      Detect systems, map dependencies, propose lab sequence
                        Confirm plan with user before proceeding
        |
        v
Phase 3 -- GENERATE     For each lab: build LabSpec, call assemble_lab, done
        |
        v
Phase 4 -- MANIFEST     Write labs/README.md
```

---

## Phase 1 -- Collect

**Before reading any source files, ask the user for:**

- **Module code** -- the short identifier printed on every lab (e.g. `ACCA6014`)
- **Module title** -- the full module name (e.g. `Graphics Programming`)

Store these as `MODULE_CODE` and `MODULE_TITLE`. They are substituted into
every lab header and PDF `@page` stamp.
Do not proceed to source collection until both are confirmed.

---

**Claude Code (primary):** list the project tree, then read relevant source
files directly. Focus on `.cpp`, `.h`, and `.glsl` files. Ignore build
artefacts, `.vs/`, `x64/`, `Debug/`, `Release/`, and third-party vendor dirs.

If the project has more than 40 source files, list top-level directories first
and ask which subsystems to focus on before reading further.

**Pasted code:** extract from fenced code blocks in the message.
**GitHub URL:** fetch the tree, then fetch individual source files.
**Mixed:** pasted version wins for any file that appears in both.

---

## Phase 2 -- Analyse and Sequence

**First, check for an existing project lab plan.** Look for `labs/lab-plan.md`
in the project directory. If it exists, read and display it, then ask:
"Use this existing plan, or re-analyse the project from scratch?"
If the user accepts it, skip straight to Phase 3 with that plan.

If no plan exists (or the user wants a fresh one), scan the source for systems
and their dependencies. See `references/lab-sequence.md` for the reference
OpenGL lab sequence. Adapt it to the actual project -- add, remove, or reorder
based on what is present.

After the user **confirms** the plan, write it to `labs/lab-plan.md` so it
persists across future invocations:

```markdown
# [MODULE_CODE] Lab Plan
Generated: YYYY-MM-DD

Lab 00  Environment Setup
        Outcome: Project compiles and a blank window opens

Lab 01  Window and Context
        Outcome: GLFW window with a clear colour renders at 60 fps
...
```

For each lab, determine the starter code type for each task:

| Type | Use when |
|------|----------|
| Skeleton | New concept -- function signatures with `// TODO:` stubs |
| Reference | Supporting or boilerplate code -- full, annotated source |
| Bug | Debugging task -- one deliberate, plausible fault; symptom described in Expected Result, fault not named |

Present the plan and wait for confirmation:

```
=== Lab Plan: [Module Code] OpenGL Labs ===

Lab 00  Environment Setup
        Outcome: Project compiles and a blank window opens

Lab 01  Window and Context
        Outcome: GLFW window with a clear colour renders at 60 fps

...

Confirm this plan, or suggest changes.
```

---

## Phase 3 -- Generate HTML

For each confirmed lab, call the `assemble_lab` MCP tool (server:
`lab-assembler`) with a populated `LabSpec`. The tool writes
`labs/lab_NN_short_title.html` directly to disk and returns
`{ path, bytes }`. Do **not** write HTML manually.

Read `references/html-template.md` in full before populating any LabSpec.

### Lab sections

**Core sections** (present in every lab):

1. **Overview** -- one paragraph; what this lab builds and how it fits the larger project
2. **Learning Objectives** -- 3 to 5 items written as "By the end of this lab you will be able to..."
3. **Concept** -- 150-250 words of prose; problem, mechanism, API; see Concept rules below
4. **Tasks** -- one or more task objects; each references an exact file and function

**Optional sections** (include only when appropriate):

- **Prerequisites** -- a short list of what must already be working; omit for Lab 00
- **Packages** -- a table with Package / Description / Docs columns; use for
  environment and setup labs where the student installs dependencies

**Optional pattern inside Tasks:**

- **Platform tabs** -- when setup steps differ by OS, include the `.tab-bar` +
  `.tab-panel` HTML inside the task's `steps_html`. See `references/html-template.md`
  for the switchTab pattern.

**Solution section** -- always include `solution_code` (the complete merged
source for the lab). The MCP tool embeds it with a "Show Solution" button;
students can reveal it when they are ready.

### Concept section rules

Write in this order:
1. The problem -- what breaks or is impossible without this concept
2. The mechanism -- how it works, with enough detail to implement it
3. The API -- the specific calls, types, or keywords the student will use

Use `<strong>` on the first occurrence of every technical term.
Use `<code>` for all identifiers, function names, types, and API calls.
Avoid bullet points here; write prose.

### Cross-lab references

When prose refers to another lab, always use its name alongside the number.
Never write "In Lab 00" or "see Lab 03" on its own. Write
"In the Environment Setup lab (Lab 00)" or "building on the Shaders lab (Lab 03)".

### Punctuation rules

**Em dashes and semicolons are banned everywhere** -- titles, headings, labels,
body text, code comments. No exceptions. The character `--`, the entity
`&mdash;`, and `;` must never appear in prose. Use a plain hyphen-minus (`-`)
as a separator in titles and headings. In body prose, split into two sentences
with a full stop rather than reaching for a dash or semicolon.

Never use arrow glyphs (`->`, `&rarr;`, `-->`). Use the plain ASCII sequence
`->` instead.

### Task rules

- Number every instruction with `<ol>`
- Name the exact file and function to edit
- State the observable success condition explicitly
- Use `<code>` for all identifiers
- For Skeleton tasks: include the full skeleton in the task's `code.content`
  with `// TODO:` comments; set `code_type` to `"skeleton"`
- For Reference tasks: include the complete annotated source; set `code_type`
  to `"reference"`
- For Bug tasks: include the faulty code; describe the symptom only; set
  `code_type` to `"bug"`

### Code block rules (steps_html)

When a task has inline code snippets within `<li>` elements (e.g. setup labs),
include them directly inside `steps_html` as `<pre><code class="language-X">`.
**HTML-escape all code content** inside these blocks (`<` becomes `&lt;`,
`>` becomes `&gt;`, `&` becomes `&amp;`).

For a trailing code block at the end of a task (the main skeleton or reference),
use the `code` field instead -- the MCP tool handles escaping.

---

## Phase 3 solution_code field

The `solution_code` field is a **single continuous source file** showing the
complete output from top to bottom, all tasks merged together.

- Start from the top of the file (includes, then `main()` or the class).
- Work through every task in order.
- Add one brief inline comment per logical step.
- For Reference tasks, include the code in the same continuous flow.
- For Bug tasks, show the corrected code with a comment marking the fix.

The MCP tool places this behind a "Show Solution" button. Students see the
same instructions they already have, scroll past them, and click to reveal
when ready.

---

## Phase 4 -- Manifest

Write `labs/README.md` after all files are generated:

```markdown
# [Module Code] OpenGL Labs

| Lab | Title | File |
|-----|-------|------|
| 00 | Environment Setup | lab_00_environment_setup.html |
| 01 | Window and Context | lab_01_window_context.html |
...

## Moodle upload instructions

1. In your Moodle course, add a File resource for each lab.
2. Upload the HTML file. Set Display to "In frame" or "Open" (not Force download).
3. Each file is self-contained -- the solution is embedded and revealed on demand.
   Encourage students to attempt the lab before clicking "Show Solution".
```

---
