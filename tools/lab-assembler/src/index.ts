import { Server } from '@modelcontextprotocol/sdk/server/index.js';
import { StdioServerTransport } from '@modelcontextprotocol/sdk/server/stdio.js';
import {
  CallToolRequestSchema,
  ListToolsRequestSchema,
} from '@modelcontextprotocol/sdk/types.js';
import { z } from 'zod';
import { buildAndWriteLab } from './assembler.js';

const PackageSchema = z.object({
  name: z.string(),
  description: z.string(),
  docs_url: z.string(),
});

const TaskSchema = z.object({
  title: z.string(),
  steps_html: z.string(),
  code: z.object({
    lang: z.enum(['cpp', 'glsl', 'bash', 'json']),
    content: z.string(),
  }).optional(),
  code_type: z.enum(['skeleton', 'reference', 'bug']).optional(),
});

const ArgsSchema = z.object({
  module_code: z.string(),
  module_title: z.string(),
  lab_number: z.string(),
  lab_title: z.string(),
  overview_html: z.string(),
  objectives: z.array(z.string()),
  concept_html: z.string(),
  prerequisites: z.array(z.string()).optional(),
  packages: z.array(PackageSchema).optional(),
  tasks: z.array(TaskSchema),
  solution_code: z.string().optional(),
  languages: z.array(z.string()),
  output_path: z.string(),
});

const TOOL_INPUT_SCHEMA = {
  type: 'object',
  properties: {
    module_code:    { type: 'string', description: 'Module code, e.g. ACCA6014' },
    module_title:   { type: 'string', description: 'Full module name, e.g. Graphics Programming' },
    lab_number:     { type: 'string', description: 'Zero-padded two-digit lab number, e.g. 03' },
    lab_title:      { type: 'string', description: 'Short lab title, e.g. Shaders' },
    overview_html:  { type: 'string', description: 'HTML for the Overview section body (one or more <p> tags)' },
    objectives:     { type: 'array',  items: { type: 'string' }, description: 'Learning objective strings (plain text)' },
    concept_html:   { type: 'string', description: 'HTML for the Concept section body (prose paragraphs)' },
    prerequisites:  { type: 'array',  items: { type: 'string' }, description: 'Optional prerequisite plain-text strings' },
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
          title:      { type: 'string', description: 'Task heading, e.g. Task 1 - Create the VAO' },
          steps_html: { type: 'string', description: 'Full HTML body of the task: ol, p, inline pre/code, platform tabs, expected-result div, etc. Code inside pre/code must be HTML-escaped.' },
          code: {
            type: 'object',
            properties: {
              lang:    { type: 'string', enum: ['cpp', 'glsl', 'bash', 'json'] },
              content: { type: 'string', description: 'Raw source code appended as a trailing code block (server HTML-escapes this)' },
            },
            required: ['lang', 'content'],
          },
          code_type: { type: 'string', enum: ['skeleton', 'reference', 'bug'] },
        },
        required: ['title', 'steps_html'],
      },
    },
    solution_code: { type: 'string', description: 'Complete merged solution source (raw, not HTML-escaped). Omit to suppress the Solution section.' },
    languages:     { type: 'array',  items: { type: 'string' }, description: 'Prism language keys used: cpp, glsl, bash, json' },
    output_path:   { type: 'string', description: 'Output file path relative to project root, e.g. labs/lab_03_shaders.html' },
  },
  required: [
    'module_code', 'module_title', 'lab_number', 'lab_title',
    'overview_html', 'objectives', 'concept_html',
    'tasks', 'languages', 'output_path',
  ],
} as const;

const server = new Server(
  { name: 'lab-assembler', version: '1.0.0' },
  { capabilities: { tools: {} } },
);

server.setRequestHandler(ListToolsRequestSchema, async () => ({
  tools: [{
    name: 'assemble_lab',
    description:
      'Assembles a complete UWTSD-branded lab HTML file from structured content and writes it to disk. ' +
      'Handles all CSS, JS, Prism syntax highlighting, UWTSD branding, and the solution reveal button. ' +
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
  const result = buildAndWriteLab(spec, output_path);

  return {
    content: [{
      type: 'text',
      text: JSON.stringify(result),
    }],
  };
});

const transport = new StdioServerTransport();
await server.connect(transport);
