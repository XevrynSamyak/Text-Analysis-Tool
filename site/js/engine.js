const LS = (() => {
  const MAX_TEXT = 5000;
  const MAX_WORDS = 500;
  const WORD_LEN = 100;
  const enc = new TextEncoder();
  const dec = new TextDecoder();

  const toBytes = s => {
    const u = enc.encode(s);
    let out = '';
    for (let i = 0; i < u.length; i += 8192) out += String.fromCharCode.apply(null, u.subarray(i, i + 8192));
    return out;
  };
  const fromBytes = b => {
    const u = new Uint8Array(b.length);
    for (let i = 0; i < b.length; i++) u[i] = b.charCodeAt(i);
    return dec.decode(u);
  };

  const code = ch => ch.charCodeAt(0);
  const isAlpha = ch => { const c = code(ch); return (c >= 65 && c <= 90) || (c >= 97 && c <= 122); };
  const isDigit = ch => { const c = code(ch); return c >= 48 && c <= 57; };
  const isAlnum = ch => isAlpha(ch) || isDigit(ch);
  const isSpace = ch => { const c = code(ch); return c === 32 || (c >= 9 && c <= 13); };
  const isPunct = ch => { const c = code(ch); return c >= 33 && c <= 126 && !isAlnum(ch); };
  const isUpper = ch => { const c = code(ch); return c >= 65 && c <= 90; };
  const lower = ch => isUpper(ch) ? String.fromCharCode(code(ch) + 32) : ch;
  const upper = ch => { const c = code(ch); return c >= 97 && c <= 122 ? String.fromCharCode(c - 32) : ch; };
  const lowerStr = s => { let o = ''; for (const ch of s) o += lower(ch); return o; };
  const isVowel = ch => 'aeiou'.includes(lower(ch));

  const clip = b => b.length > MAX_TEXT - 1 ? b.slice(0, MAX_TEXT - 1) : b;
  const field = s => toBytes(s).slice(0, WORD_LEN - 1);

  function extractWords(b) {
    const words = [];
    let i = 0;
    while (i < b.length && words.length < MAX_WORDS) {
      while (i < b.length && !isAlnum(b[i])) i++;
      if (i >= b.length) break;
      let w = '';
      while (i < b.length && isAlnum(b[i])) {
        if (w.length < WORD_LEN - 1) w += b[i];
        i++;
      }
      words.push(lowerStr(w));
    }
    return words;
  }

  function statistics(b) {
    const s = { characters: 0, charactersNoSpaces: 0, alphabets: 0, digits: 0, spaces: 0, punctuation: 0, vowels: 0, consonants: 0, words: 0, sentences: 0, paragraphs: 0, uppercase: 0, lowercase: 0 };
    let inWord = false, inParagraph = false, prevNewline = false;
    for (const ch of b) {
      s.characters++;
      if (!isSpace(ch)) s.charactersNoSpaces++;
      if (isAlpha(ch)) {
        s.alphabets++;
        isVowel(ch) ? s.vowels++ : s.consonants++;
        isUpper(ch) ? s.uppercase++ : s.lowercase++;
      } else if (isDigit(ch)) s.digits++;
      else if (isPunct(ch)) s.punctuation++;
      if (isSpace(ch)) s.spaces++;
      if (ch === '.' || ch === '?' || ch === '!') s.sentences++;
      if (isAlnum(ch)) {
        if (!inWord) { s.words++; inWord = true; }
      } else inWord = false;
      if (!isSpace(ch)) {
        if (!inParagraph) { s.paragraphs++; inParagraph = true; }
        prevNewline = false;
      } else if (ch === '\n') {
        if (prevNewline) inParagraph = false;
        prevNewline = true;
      }
    }
    return s;
  }

  function longestShortest(words) {
    if (!words.length) return { longest: '', shortest: '' };
    let longest = words[0], shortest = words[0];
    for (const w of words.slice(1)) {
      if (w.length > longest.length) longest = w;
      if (w.length < shortest.length) shortest = w;
    }
    return { longest, shortest };
  }

  function frequency(words) {
    const order = [], seen = new Map();
    for (const w of words) {
      if (seen.has(w)) seen.get(w).count++;
      else { const e = { word: w, count: 1 }; seen.set(w, e); order.push(e); }
    }
    return order;
  }

  function letters(b) {
    const f = new Array(26).fill(0);
    for (const ch of b) if (isAlpha(ch)) f[code(lower(ch)) - 97]++;
    return f;
  }

  function palindrome(b) {
    let l = 0, r = b.length - 1;
    if (r < 0) return false;
    while (l < r) {
      while (l < r && !isAlnum(b[l])) l++;
      while (l < r && !isAlnum(b[r])) r--;
      if (lower(b[l]) !== lower(b[r])) return false;
      l++; r--;
    }
    return true;
  }

  function anagram(a, c) {
    const f = new Map();
    for (const ch of a) if (isAlpha(ch)) f.set(lower(ch), (f.get(lower(ch)) || 0) + 1);
    for (const ch of c) if (isAlpha(ch)) f.set(lower(ch), (f.get(lower(ch)) || 0) - 1);
    for (const v of f.values()) if (v !== 0) return false;
    return true;
  }

  function quality(b, s) {
    let spaces = 0, punct = 0;
    for (let i = 1; i < b.length; i++) {
      if (isSpace(b[i]) && isSpace(b[i - 1])) spaces++;
      if (isPunct(b[i]) && b[i] === b[i - 1]) punct++;
    }
    const longSentences = s.words > 0 && s.sentences > 0 && s.words / s.sentences > 30;
    const score = Math.max(0, 100 - spaces * 5 - punct * 5 - (longSentences ? 15 : 0));
    const verdict = score >= 90 ? 'Excellent text formatting!' : score >= 70 ? 'Good formatting.' : score >= 50 ? 'Needs some improvement.' : 'Text needs significant cleanup.';
    return { score, verdict, repeatedSpaces: spaces, repeatedPunctuation: punct, longSentences };
  }

  function writing(b, s) {
    let questions = 0, exclamations = 0;
    for (const ch of b) {
      if (ch === '?') questions++;
      if (ch === '!') exclamations++;
    }
    return {
      averageWordLength: s.words > 0 ? s.alphabets / s.words : 0,
      averageSentenceLength: s.sentences > 0 ? s.words / s.sentences : 0,
      readingTime: s.words / 200,
      questions,
      exclamations
    };
  }

  function analyze(text) {
    const full = toBytes(text);
    const b = clip(full);
    const s = statistics(b);
    const words = extractWords(b);
    const freq = frequency(words);
    return {
      truncated: full.length > b.length,
      bytes: b.length,
      stats: s,
      wordsCounted: words.length,
      unique: freq.length,
      ...longestShortest(words),
      palindrome: palindrome(b),
      wordFrequency: freq,
      duplicates: freq.filter(e => e.count > 1),
      letters: letters(b),
      writing: writing(b, s),
      quality: quality(b, s)
    };
  }

  function search(text, target) {
    const t = lowerStr(field(target));
    return extractWords(clip(toBytes(text))).filter(w => w === t).length;
  }

  function replace(text, oldWord, newWord) {
    const b = clip(toBytes(text));
    const oldLower = lowerStr(field(oldWord));
    const rep = field(newWord);
    let out = '', replaced = 0, i = 0;
    while (i < b.length) {
      if (isAlnum(b[i])) {
        let cur = '';
        while (i < b.length && isAlnum(b[i])) {
          if (cur.length < WORD_LEN - 1) cur += b[i];
          i++;
        }
        const hit = lowerStr(cur) === oldLower;
        if (hit) replaced++;
        const piece = hit ? rep : cur;
        if (out.length + piece.length < MAX_TEXT) out += piece;
      } else {
        if (out.length < MAX_TEXT - 1) out += b[i];
        i++;
      }
    }
    return { text: fromBytes(out), replaced };
  }

  const reverse = text => Array.from(fromBytes(clip(toBytes(text)))).reverse().join('');

  function reverseWords(text) {
    const b = clip(toBytes(text));
    const words = [];
    let i = 0;
    while (i < b.length && words.length < MAX_WORDS) {
      while (i < b.length && isSpace(b[i])) i++;
      if (i >= b.length) break;
      let w = '';
      while (i < b.length && !isSpace(b[i])) {
        if (w.length < WORD_LEN - 1) w += b[i];
        i++;
      }
      words.push(w);
    }
    return fromBytes(words.reverse().join(' '));
  }

  function convertCase(text, mode) {
    const b = clip(toBytes(text));
    let out = '', start = true;
    for (const ch of b) {
      if (mode === 1) out += upper(ch);
      else if (mode === 2) out += lower(ch);
      else if (isAlpha(ch)) { out += start ? upper(ch) : lower(ch); start = false; }
      else { out += ch; start = isSpace(ch); }
    }
    return fromBytes(out);
  }

  function cleanSpaces(text) {
    const b = clip(toBytes(text));
    let out = '', prev = true;
    for (const ch of b) {
      if (isSpace(ch)) {
        if (!prev) { out += ' '; prev = true; }
      } else { out += ch; prev = false; }
    }
    if (out.endsWith(' ')) out = out.slice(0, -1);
    return fromBytes(out);
  }

  function compare(a, c) {
    const w1 = extractWords(clip(toBytes(a)));
    const w2 = extractWords(clip(toBytes(c)));
    const u1 = new Set(w1), u2 = new Set(w2);
    let shared = 0;
    for (const w of u1) if (w2.includes(w)) shared++;
    const total = u1.size + u2.size - shared;
    return {
      firstWords: w1.length,
      secondWords: w2.length,
      firstUnique: u1.size,
      secondUnique: u2.size,
      shared,
      sharedWords: [...u1].filter(w => u2.has(w)),
      similarity: total > 0 ? shared / total * 100 : 0
    };
  }

  function reportText(text) {
    const r = analyze(text);
    const s = r.stats;
    return [
      'LEXISCAN TEXT ANALYSIS REPORT',
      '_________________________________',
      'Original Text:',
      fromBytes(clip(toBytes(text))),
      '',
      `Characters: ${s.characters}`,
      `Characters without spaces: ${s.charactersNoSpaces}`,
      `Alphabets: ${s.alphabets}`,
      `Digits: ${s.digits}`,
      `Spaces: ${s.spaces}`,
      `Punctuation: ${s.punctuation}`,
      `Vowels: ${s.vowels}`,
      `Consonants: ${s.consonants}`,
      `Words: ${s.words}`,
      `Unique words: ${r.unique}`,
      `Sentences: ${s.sentences}`,
      `Paragraphs: ${s.paragraphs}`,
      `Longest word: ${r.longest}`,
      `Shortest word: ${r.shortest}`,
      `Palindrome: ${r.palindrome ? 'Yes' : 'No'}`,
      ''
    ].join('\n');
  }

  function fixed2(v) {
    const k = v * 8;
    if (Number.isInteger(k) && Math.abs(k % 2) === 1) {
      const down = Math.floor(v * 100) / 100;
      const digit = Math.round(down * 100) % 10;
      return (digit % 2 === 0 ? down : down + 0.01).toFixed(2);
    }
    return v.toFixed(2);
  }

  return {
    MAX_TEXT, MAX_WORDS, WORD_LEN, fixed2,
    analyze, search, replace, reverse, reverseWords, convertCase, cleanSpaces, compare, reportText,
    palindrome: s => palindrome(field(s)),
    anagram: (a, c) => anagram(field(a), field(c)),
    byteLength: s => enc.encode(s).length
  };
})();

if (typeof module !== 'undefined') module.exports = LS;
