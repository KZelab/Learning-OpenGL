import { z } from 'zod';

// ── Enums ─────────────────────────────────────────────────────────────────────
export const LANG_ENUM      = ['cpp', 'glsl', 'bash', 'json', 'javascript', 'typescript', 'python', 'java', 'sql', 'html', 'css'] as const;
export const CODE_TYPE_ENUM = ['skeleton', 'reference', 'bug'] as const;

// ── TypeScript types ──────────────────────────────────────────────────────────
export type CodeLang = typeof LANG_ENUM[number];
export type CodeType = typeof CODE_TYPE_ENUM[number];

export interface LabOptions {
  solution?:       boolean;
  copy_protect?:   boolean;
  print_friendly?: boolean;
}

export interface FillBlankItem {
  question_html: string;
  answers:       (string | string[])[];
}

export interface MultipleChoiceItem {
  question:     string;
  options:      string[];
  correct:      number;
  explanation?: string;
}

export interface ShortAnswerItem {
  question:     string;
  model_answer: string;
}

export type ContentBlock =
  | { type: 'prose';           html: string }
  | { type: 'code';            lang: CodeLang; content: string; filename?: string; code_type?: CodeType }
  | { type: 'fill_blanks';     interactive?: boolean; questions: FillBlankItem[] }
  | { type: 'multiple_choice'; interactive?: boolean; questions: MultipleChoiceItem[] }
  | { type: 'short_answer';    interactive?: boolean; questions: ShortAnswerItem[] }
  | { type: 'math';            latex: string; display?: boolean }
  | { type: 'visualization';   id: string; height?: number; script: string }

export interface LabTask {
  title:  string;
  blocks: ContentBlock[];
}

export interface LabPackage {
  name:        string;
  description: string;
  docs_url:    string;
}

export interface SolutionFile {
  filename?: string;
  content:   string;
}

export interface LabSpec {
  options?:        LabOptions;
  module_code:     string;
  module_title:    string;
  lab_number:      string;
  lab_title:       string;
  overview_html:   string;
  objectives:      string[];
  concept_html:    string;
  packages?:       LabPackage[];
  tasks:           LabTask[];
  solution_code?:  string;
  solution_files?: SolutionFile[];
  languages:       string[];
}

// ── Zod schemas ───────────────────────────────────────────────────────────────
export const LabOptionsSchema = z.object({
  solution:       z.boolean().optional(),
  copy_protect:   z.boolean().optional(),
  print_friendly: z.boolean().optional(),
});

export const FillBlankItemSchema = z.object({
  question_html: z.string(),
  answers:       z.array(z.union([z.string(), z.array(z.string())])),
});

export const MultipleChoiceItemSchema = z.object({
  question:    z.string(),
  options:     z.array(z.string()),
  correct:     z.number().int().min(0),
  explanation: z.string().optional(),
});

export const ShortAnswerItemSchema = z.object({
  question:     z.string(),
  model_answer: z.string(),
});

export const ContentBlockSchema = z.discriminatedUnion('type', [
  z.object({ type: z.literal('prose'), html: z.string() }),
  z.object({
    type:      z.literal('code'),
    lang:      z.enum(LANG_ENUM),
    content:   z.string(),
    filename:  z.string().optional(),
    code_type: z.enum(CODE_TYPE_ENUM).optional(),
  }),
  z.object({
    type:        z.literal('fill_blanks'),
    interactive: z.boolean().optional(),
    questions:   z.array(FillBlankItemSchema),
  }),
  z.object({
    type:        z.literal('multiple_choice'),
    interactive: z.boolean().optional(),
    questions:   z.array(MultipleChoiceItemSchema),
  }),
  z.object({
    type:        z.literal('short_answer'),
    interactive: z.boolean().optional(),
    questions:   z.array(ShortAnswerItemSchema),
  }),
  z.object({
    type:    z.literal('math'),
    latex:   z.string(),
    display: z.boolean().optional(),
  }),
  z.object({
    type:   z.literal('visualization'),
    id:     z.string(),
    height: z.number().optional(),
    script: z.string(),
  }),
]);

export const PackageSchema = z.object({
  name:        z.string(),
  description: z.string(),
  docs_url:    z.string(),
});

export const TaskSchema = z.object({
  title:  z.string(),
  blocks: z.array(ContentBlockSchema),
});

export const SolutionFileSchema = z.object({
  filename: z.string().optional(),
  content:  z.string(),
});

export const LabSpecSchema = z.object({
  options:        LabOptionsSchema.optional(),
  module_code:    z.string(),
  module_title:   z.string(),
  lab_number:     z.string(),
  lab_title:      z.string(),
  overview_html:  z.string(),
  objectives:     z.array(z.string()),
  concept_html:   z.string(),
  packages:       z.array(PackageSchema).optional(),
  tasks:          z.array(TaskSchema),
  solution_code:  z.string().optional(),
  solution_files: z.array(SolutionFileSchema).optional(),
  languages:      z.array(z.string()),
});

export const ArgsSchema = LabSpecSchema.extend({
  output_path: z.string(),
});
