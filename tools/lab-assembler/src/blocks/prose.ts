import type { ContentBlock } from '../schema.js';

export function renderProseBlock(block: Extract<ContentBlock, { type: 'prose' }>): string {
  return block.html;
}
