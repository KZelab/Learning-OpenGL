import type { ContentBlock } from '../schema.js';

export function escapeHtml(str: string): string {
  return str
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;');
}

export function renderCodeBlock(block: Extract<ContentBlock, { type: 'code' }>): string {
  const fileHeader = block.filename
    ? `<div class="code-file-header">${block.filename}</div>`
    : '';
  const preStyle = block.filename ? ' style="border-radius:0 0 6px 6px;margin-top:0"' : '';
  return `${fileHeader}\n<pre${preStyle}><code class="language-${block.lang}">${escapeHtml(block.content)}</code></pre>`;
}
