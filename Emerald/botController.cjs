const { Parser } = require('expr-eval');

const settingsStorage = {
  cardColor: '#ffffff',
  avatarEmoji: '😊',
  language: 'uk-UA',
  backgroundTheme: 'emerald',
  backgroundColor: '#081d13'
};

const definitions = {
  'штучний інтелект': 'Штучний інтелект - це система, яка виконує завдання, що зазвичай потребують людського інтелекту.',
  'машинне навчання': 'Машинне навчання - це напрямок штучного інтелекту, який навчaє комп\'ютери на даних.',
  'алгоритм': 'Алгоритм - це набір кроків для виконання конкретного завдання.',
  'функція': 'Функція - це правило, яке перетворює вхідні дані у вихідний результат.'
};

const translations = {
  'привіт': 'hello',
  'дякую': 'thank you',
  'як справи': 'how are you',
  'до побачення': 'goodbye',
  'будь ласка': 'please',
  'так': 'yes',
  'ні': 'no'
};

function parseMathExpression(text) {
  const replaced = text
    .replace(/×/g, '*')
    .replace(/÷/g, '/')
    .replace(/,/g, '.')
    .replace(/скільки|буде|обчисли|порахуй|розв'яжи|розвяжи|знайди|число|щo|що|додай|відніми|помнож|поділи/gi, '')
    .trim();

  const cleaned = replaced.replace(/\s+/g, '');
  if (/^[0-9+\-*/^().]+$/.test(cleaned)) {
    return cleaned;
  }
  return null;
}

function safeEvaluate(expression) {
  try {
    if (!expression) {
      return null;
    }
    const cleaned = expression.replace(/\s+/g, '');
    if (!/^[0-9+\-*/^().]+$/.test(cleaned)) {
      return null;
    }
    const parser = new Parser();
    const result = parser.evaluate(cleaned);
    return Number.isFinite(result) ? result : null;
  } catch (error) {
    return null;
  }
}

function summarizeText(text) {
  const cleaned = text.replace(/підсумуй|резюме|коротко|підсумувати/gi, '').trim();
  if (!cleaned) {
    return null;
  }
  const sentences = cleaned.split(/(?<=[.!?])\s+/);
  if (sentences.length > 1) {
    return sentences.slice(0, 2).join(' ');
  }
  const words = cleaned.split(/\s+/).slice(0, 20);
  return words.join(' ');
}

function translatePhrase(text) {
  const normalized = text.trim().toLowerCase();
  const cleaned = normalized.replace(/переклади|translate|англійською|українською|на англійську|на українську/gi, '').trim();
  if (!cleaned) {
    return null;
  }
  const words = cleaned.split(/\s+/);
  const translated = words.map(word => translations[word] || word).join(' ');
  return translated === cleaned ? null : translated;
}

function defineTerm(text) {
  const normalized = text.toLowerCase();
  if (/що таке|хто такий|що таке/i.test(normalized)) {
    const key = Object.keys(definitions).find(term => normalized.includes(term));
    return key ? definitions[key] : 'Вибачте, я поки що не знаю визначення цього терміна.';
  }
  return null;
}

function getReply(message) {
  const normalized = message.trim().toLowerCase();
  if (!normalized) {
    return 'Будь ласка, надішліть повідомлення.';
  }

  const mathExpression = parseMathExpression(normalized);
  if (mathExpression) {
    const answer = safeEvaluate(mathExpression);
    if (answer !== null) {
      return `Результат: ${answer}`;
    }
  }

  if (/підсумуй|резюме|коротко/.test(normalized)) {
    const summary = summarizeText(normalized);
    if (summary) {
      return `Підсумок: ${summary}`;
    }
  }

  if (/переклади|translate/.test(normalized)) {
    const translation = translatePhrase(normalized);
    if (translation) {
      return `Переклад: ${translation}`;
    }
    return 'Я можу перекладати прості фрази: спробуйте написати, наприклад, "переклади привіт англійською".';
  }

  const definition = defineTerm(normalized);
  if (definition) {
    return definition;
  }

  if (/(привіт|здрастуйте|доброго дня|здравствуйте)/i.test(normalized)) {
    return 'Привіт! Я Emerald, ваш віртуальний помічник.';
  }

  if (/час/.test(normalized)) {
    return `Зараз ${new Date().toLocaleTimeString('uk-UA', { hour: '2-digit', minute: '2-digit' })}.`;
  }

  if (/дата/.test(normalized)) {
    return `Сьогодні ${new Date().toLocaleDateString('uk-UA')}.`;
  }

  if (/як справи/.test(normalized)) {
    return 'У мене все добре, дякую! Готовий допомогти.';
  }

  if (/вірш|написати вірш/.test(normalized)) {
    return 'Ось короткий вірш:\nСмарагдове світло в небі блищить,\nEmerald поруч — рішень мить.\nЗапитай мене, я завжди тут, щоб відповісти на твій крик.';
  }

  return 'Я можу обчислювати приклади, підсумовувати текст, перекладати прості фрази, пояснювати терміни або сказати час та дату.';
}

function getSettings() {
  return { ...settingsStorage };
}

function saveSettings(payload) {
  if (payload && typeof payload === 'object') {
    if (typeof payload.cardColor === 'string') {
      settingsStorage.cardColor = payload.cardColor;
    }
    if (typeof payload.avatarEmoji === 'string') {
      settingsStorage.avatarEmoji = payload.avatarEmoji;
    }
    if (typeof payload.language === 'string') {
      settingsStorage.language = payload.language;
    }
    if (typeof payload.backgroundTheme === 'string') {
      settingsStorage.backgroundTheme = payload.backgroundTheme;
    }
    if (typeof payload.backgroundColor === 'string') {
      settingsStorage.backgroundColor = payload.backgroundColor;
    }
  }
  return getSettings();
}

module.exports = { getReply, getSettings, saveSettings };
