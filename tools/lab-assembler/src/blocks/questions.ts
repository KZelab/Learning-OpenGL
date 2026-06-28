import type { FillBlankItem, MultipleChoiceItem, ShortAnswerItem } from '../schema.js';
import { escapeHtml } from './code.js';

export function renderFillBlanks(questions: FillBlankItem[], interactive: boolean): string {
  if (!interactive) {
    const items = questions.map(fb => {
      const html = fb.question_html.replace(/\{\{blank\}\}/g, '<span class="blank-static">_____________</span>');
      return `<div class="blank-question-static"><p>${html}</p></div>`;
    }).join('\n');
    return `<div class="fill-blanks-group">${items}</div>`;
  }

  const items = questions.map(fb => {
    const answersJson = escapeHtml(JSON.stringify(fb.answers));
    let idx = 0;
    const html = fb.question_html.replace(/\{\{blank\}\}/g, () =>
      `<input class="blank-input" type="text" autocomplete="off" aria-label="blank ${++idx}">`
    );
    return `<div class="blank-question" data-answers="${answersJson}">\n  <p>${html}</p>\n  <span class="blank-feedback"></span>\n</div>`;
  }).join('\n');
  return `<div class="fill-blanks-group">\n${items}\n</div>\n<button class="check-blanks-btn" onclick="checkBlanks(this)">Check Answers</button>`;
}

export function renderMultipleChoice(questions: MultipleChoiceItem[], interactive: boolean, uid: number): string {
  const LABELS = ['A', 'B', 'C', 'D', 'E'];

  if (!interactive) {
    const items = questions.map(q => {
      const opts = q.options.map(o => `<li>${o}</li>`).join('\n');
      return `<div class="mc-item-static">
  <p class="mc-question-text">${q.question}</p>
  <ol class="mc-options-static" type="A">${opts}</ol>
</div>`;
    }).join('\n');
    return `<div class="mc-block">${items}</div>`;
  }

  const items = questions.map((q, qi) => {
    const name = `mc-${uid}-${qi}`;
    const opts = q.options.map((o, i) => `<label class="mc-option">
    <input type="radio" name="${name}" onchange="revealMCAnswer(this)">
    <span>${LABELS[i]}. ${o}</span>
  </label>`).join('\n');
    const explanation = q.explanation
      ? `<div class="mc-explanation">${q.explanation}</div>`
      : '';
    return `<div class="mc-item" data-correct="${q.correct}">
  <p class="mc-question-text">${q.question}</p>
  <div class="mc-options">${opts}</div>
  <div class="mc-feedback"></div>
  ${explanation}
</div>`;
  }).join('\n');
  return `<div class="mc-block">${items}</div>`;
}

export function renderShortAnswer(questions: ShortAnswerItem[], interactive: boolean): string {
  if (!interactive) {
    const items = questions.map(q => `<div class="sa-item-static">
  <p class="sa-question-text">${q.question}</p>
  <div class="sa-lines"></div>
</div>`).join('\n');
    return `<div class="sa-block">${items}</div>`;
  }

  const items = questions.map(q => `<div class="sa-item">
  <p class="sa-question-text">${q.question}</p>
  <textarea class="sa-textarea" rows="4" placeholder="Write your answer here..."></textarea>
  <button class="sa-reveal-btn" onclick="toggleModelAnswer(this)">Show Model Answer</button>
  <div class="sa-answer-panel">${q.model_answer}</div>
</div>`).join('\n');
  return `<div class="sa-block">${items}</div>`;
}
