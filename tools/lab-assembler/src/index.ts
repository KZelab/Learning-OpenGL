import { Server } from '@modelcontextprotocol/sdk/server/index.js';
import { StdioServerTransport } from '@modelcontextprotocol/sdk/server/stdio.js';
import {
  CallToolRequestSchema,
  ListToolsRequestSchema,
} from '@modelcontextprotocol/sdk/types.js';
import { ArgsSchema } from './schema.js';
import { buildAndWriteLab } from './assembler.js';
import type { LabSpec } from './schema.js';

// ── MCP tool JSON schema ──────────────────────────────────────────────────────
const TOOL_INPUT_SCHEMA = {
  type: 'object',
  properties: {
    options: {
      type: 'object',
      description: 'Document-level options (all optional; defaults shown)',
      properties: {
        solution:       { type: 'boolean', description: 'Include solution section (default true)' },
        copy_protect:   { type: 'boolean', description: 'Render solution on canvas to resist copy-paste (default true)' },
        print_friendly: { type: 'boolean', description: 'Add extra print CSS for page breaks (default false)' },
      },
    },
    module_code:    { type: 'string', description: 'Module code, e.g. ACCA6021' },
    module_title:   { type: 'string', description: 'Full module name, e.g. Graphics Programming' },
    lab_number:     { type: 'string', description: 'Zero-padded two-digit lab number, e.g. 03' },
    lab_title:      { type: 'string', description: 'Short lab title, e.g. Shaders' },
    overview_html:  { type: 'string', description: 'HTML for the Overview section body (one or more <p> tags)' },
    objectives:     { type: 'array', items: { type: 'string' }, description: 'Learning objective strings (plain text)' },
    concept_html:   { type: 'string', description: 'HTML for the Concept section body (prose paragraphs)' },
    packages: {
      type: 'array',
      items: {
        type: 'object',
        properties: {
          name:        { type: 'string' },
          description: { type: 'string' },
          docs_url:    { type: 'string' },
        },
        required: ['name', 'description', 'docs_url'],
      },
      description: 'Optional packages table rows (for setup labs)',
    },
    tasks: {
      type: 'array',
      items: {
        type: 'object',
        properties: {
          title:  { type: 'string', description: 'Task heading, e.g. Task 1 - Create the VAO' },
          blocks: {
            type: 'array',
            description: 'Ordered content blocks for this task',
            items: {
              type: 'object',
              properties: {
                type: {
                  type: 'string',
                  enum: ['prose', 'code', 'fill_blanks', 'multiple_choice', 'short_answer', 'math', 'visualization'],
                  description: 'Block type',
                },
                // prose
                html: { type: 'string', description: '(prose) HTML body' },
                // code
                lang:      { type: 'string', enum: ['cpp', 'glsl', 'bash', 'json', 'javascript', 'typescript', 'python', 'java', 'sql', 'html', 'css'], description: '(code) language' },
                content:   { type: 'string', description: '(code) raw source; server HTML-escapes this' },
                filename:  { type: 'string', description: '(code) optional filename shown as title bar' },
                code_type: { type: 'string', enum: ['skeleton', 'reference', 'bug'], description: '(code) starter type' },
                // question blocks
                interactive: { type: 'boolean', description: '(fill_blanks|multiple_choice|short_answer) render as interactive widget (default true)' },
                questions:   { type: 'array', description: '(fill_blanks|multiple_choice|short_answer) array of question objects' },
                // math
                latex:   { type: 'string', description: '(math) LaTeX source string, e.g. "\\\\frac{1}{2}mv^2"' },
                display: { type: 'boolean', description: '(math) block-level equation (default true); false for inline' },
                // visualization
                id:     { type: 'string', description: '(visualization) unique id for the container div, e.g. "clip-space-viz"' },
                height: { type: 'number', description: '(visualization) container height in pixels (default 400)' },
                script: { type: 'string', description: '(visualization) D3 JavaScript; `d3` and `container` (the div) are pre-bound' },
              },
              required: ['type'],
            },
          },
        },
        required: ['title', 'blocks'],
      },
    },
    solution_code:  { type: 'string', description: 'Single-file solution source (raw). Use solution_files for multi-file.' },
    solution_files: {
      type: 'array',
      items: {
        type: 'object',
        properties: {
          filename: { type: 'string', description: 'Filename shown as title bar in canvas, e.g. src/main.cpp' },
          content:  { type: 'string', description: 'Raw source for this file' },
        },
        required: ['content'],
      },
      description: 'Multi-file solution; each entry renders as a separate copy-resistant canvas',
    },
    languages:   { type: 'array', items: { type: 'string' }, description: 'Prism language keys used: cpp, glsl, bash, json, javascript, typescript, python, java, sql, html, css' },
    output_path: { type: 'string', description: 'Output path relative to project root, e.g. labs/acca6021/lab_03_shaders.html' },
  },
  required: [
    'module_code', 'module_title', 'lab_number', 'lab_title',
    'overview_html', 'objectives', 'concept_html',
    'tasks', 'languages', 'output_path',
  ],
} as const;

// ── Server ────────────────────────────────────────────────────────────────────
const server = new Server(
  { name: 'lab-assembler', version: '2.1.0' },
  { capabilities: { tools: {} } },
);

server.setRequestHandler(ListToolsRequestSchema, async () => ({
  tools: [{
    name: 'assemble_lab',
    description:
      'Assembles a complete UWTSD-branded lab HTML file from structured content and writes it to disk. ' +
      'Handles all CSS, JS, Prism syntax highlighting (inlined, no CDN), KaTeX math (pre-rendered server-side, no CDN), ' +
      'D3.js visualizations (inlined, no CDN), UWTSD branding, solution reveal, ' +
      'fill-in-the-blank, multiple choice, and short-answer blocks. ' +
      'Returns the output path and byte count; the HTML never enters the conversation context.',
    inputSchema: TOOL_INPUT_SCHEMA,
  }],
}));

server.setRequestHandler(CallToolRequestSchema, async (request) => {
  if (request.params.name !== 'assemble_lab') {
    throw new Error(`Unknown tool: ${request.params.name}`);
  }

  const args = ArgsSchema.parse(request.params.arguments);
  const { output_path, ...spec } = args;
  const result = buildAndWriteLab(spec as LabSpec, output_path);

  return {
    content: [{
      type: 'text',
      text: JSON.stringify(result),
    }],
  };
});

const transport = new StdioServerTransport();
await server.connect(transport);
