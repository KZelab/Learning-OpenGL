import type { ContentBlock } from '../schema.js';

export function renderVizBlock(block: Extract<ContentBlock, { type: 'visualization' }>): string {
  return `<div class="viz-block">
  <div id="viz-${block.id}" style="width:100%;height:${block.height ?? 400}px;"></div>
  <script>(function(){
    const container = document.getElementById('viz-${block.id}');
    ${block.script}
  })();<\/script>
</div>`;
}
