import katex from 'katex';
import type { ContentBlock } from '../schema.js';

export function renderMathBlock(block: Extract<ContentBlock, { type: 'math' }>): string {
  const html = katex.renderToString(block.latex, {
    displayMode:  block.display ?? true,
    throwOnError: false,
  });
  return `<div class="math-block">${html}</div>`;
}
