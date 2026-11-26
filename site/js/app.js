(() => {
  const $ = id => document.getElementById(id);
  const $$ = (sel, root = document) => Array.from(root.querySelectorAll(sel));
  const fmt = n => n.toLocaleString('en-US');
  const LIMIT = LS.MAX_TEXT - 1;
  const STORE = 'lexiscan:text';

  const SAMPLE = [
    'The quiet harbour town woke slowly. Fishing boats knocked against the pier while gulls argued over the nets.',
    '',
    'By noon the market was loud, and the harbour smelled of salt and diesel. Nobody in the town remembered a calmer spring. Was it the weather? Or was it the new harbour master, who answered every letter by hand?'
  ].join('\n');

  const CMP_A = 'The cat sat on the mat and watched the harbour.';
  const CMP_B = 'The dog sat on a log and watched the boats.';

  const el = {
    text: $('text'), mirror: $('mirror'), bytes: $('bytes'), note: $('note'),
    case: $('case'), caseSum: $('case-sum'),
    search: $('search'), repOld: $('rep-old'), repNew: $('rep-new'),
    editOut: $('edit-out'), cmpA: $('cmp-a'), cmpB: $('cmp-b')
  };

  const state = { text: '', history: [], sort: 'count', op: 'reverse', caseMode: 1, mark: '', result: null };

  const store = {
    get() { try { return localStorage.getItem(STORE); } catch (e) { return null; } },
    set(v) { try { localStorage.setItem(STORE, v); } catch (e) {} }
  };

  const letters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'.split('');
  const cells = letters.map(l => {
    const c = document.createElement('div');
    c.className = 'cell';
    c.setAttribute('role', 'listitem');
    c.innerHTML = '<span class="fill"></span><b>' + l + '</b><i>0</i>';
    el.case.appendChild(c);
    return c;
  });
  const rest = document.createElement('div');
  rest.className = 'rest';
  el.case.appendChild(rest);

  function renderCase(r) {
    const max = Math.max(1, ...r.letters);
    const total = r.letters.reduce((a, b) => a + b, 0);
    cells.forEach((c, i) => {
      const n = r.letters[i];
      c.style.flexGrow = (1 + 3 * n / max).toFixed(3);
      c.style.setProperty('--h', (n / max * 78).toFixed(1) + '%');
      c.dataset.zero = n === 0 ? 'true' : 'false';
      c.lastChild.textContent = fmt(n);
      c.setAttribute('aria-label', letters[i] + ', ' + n);
    });
    if (!total) { el.caseSum.textContent = 'No letters yet'; return; }
    const top = r.letters.indexOf(max);
    el.caseSum.textContent = letters[top] + ' leads with ' + fmt(max) + ' of ' + fmt(total);
  }

  function renderReport(r) {
    const s = r.stats;
    const v = Object.assign({}, s, { unique: r.unique, longest: r.longest || 'N/A', shortest: r.shortest || 'N/A', palindrome: r.palindrome ? 'Yes' : 'No' });
    $$('[data-f]').forEach(d => {
      const x = v[d.dataset.f];
      d.textContent = typeof x === 'number' ? fmt(x) : x;
    });
  }

  function renderFreq(r) {
    const list = $('freq-list');
    const items = r.wordFrequency.slice();
    if (state.sort === 'count') items.sort((a, b) => b.count - a.count);
    list.textContent = '';
    if (!items.length) {
      list.innerHTML = '<li class="empty">No words yet.</li>';
      return;
    }
    const max = items.reduce((m, e) => Math.max(m, e.count), 1);
    const frag = document.createDocumentFragment();
    items.forEach(e => {
      const li = document.createElement('li');
      li.style.setProperty('--p', (e.count / max * 100).toFixed(1) + '%');
      const w = document.createElement('span');
      w.className = 'w';
      w.textContent = e.word;
      const n = document.createElement('span');
      n.className = 'n';
      n.textContent = fmt(e.count);
      li.append(w, n);
      frag.appendChild(li);
    });
    list.appendChild(frag);
  }

  function renderDups(r) {
    const list = $('dup-list');
    list.textContent = '';
    if (!r.duplicates.length) {
      list.innerHTML = '<li class="empty">' + (r.wordsCounted ? 'No repeated words.' : 'No words yet.') + '</li>';
      return;
    }
    const frag = document.createDocumentFragment();
    r.duplicates.forEach(e => {
      const li = document.createElement('li');
      const w = document.createElement('span');
      w.className = 'w';
      w.textContent = e.word;
      const n = document.createElement('span');
      n.className = 'n';
      n.textContent = e.count + ' times';
      li.append(w, n);
      frag.appendChild(li);
    });
    list.appendChild(frag);
  }

  function renderWriting(r) {
    const w = r.writing, q = r.quality;
    $('w-awl').textContent = LS.fixed2(w.averageWordLength);
    $('w-wps').textContent = LS.fixed2(w.averageSentenceLength);
    $('w-rt').textContent = LS.fixed2(w.readingTime);
    $('w-q').textContent = fmt(w.questions);
    $('w-e').textContent = fmt(w.exclamations);
    $('q-score').textContent = q.score;
    $('q-verdict').textContent = q.verdict;
    $('q-sp').textContent = fmt(q.repeatedSpaces);
    $('q-pu').textContent = fmt(q.repeatedPunctuation);
    $('q-ls').textContent = q.longSentences ? 'Yes' : 'No';
  }

  function renderSearch() {
    const term = el.search.value.trim();
    const n = term ? LS.search(state.text, term) : 0;
    $('search-n').textContent = fmt(n);
    $('search-l').textContent = n === 1 ? 'occurrence' : 'occurrences';
  }

  function renderEdit() {
    const t = state.text;
    let out = '';
    if (state.op === 'reverse') out = LS.reverse(t);
    else if (state.op === 'words') out = LS.reverseWords(t);
    else if (state.op === 'case') out = LS.convertCase(t, state.caseMode);
    else out = LS.cleanSpaces(t);
    state.result = out;
    el.editOut.value = out;
  }

  function renderCompare() {
    const c = LS.compare(el.cmpA.value, el.cmpB.value);
    $('c-1').textContent = fmt(c.firstWords);
    $('c-2').textContent = fmt(c.secondWords);
    $('c-s').textContent = fmt(c.shared);
    $('c-p').textContent = LS.fixed2(c.similarity);
    const list = $('c-list');
    list.textContent = '';
    if (c.sharedWords.length) {
      list.append('Shared: ');
      c.sharedWords.forEach((w, i) => {
        const b = document.createElement('b');
        b.textContent = w;
        list.append(b, i < c.sharedWords.length - 1 ? ', ' : '');
      });
    }
  }

  function verdict(node, yes, label) {
    node.dataset.state = yes ? 'yes' : 'no';
    node.querySelector('use').setAttribute('href', yes ? '#i-yes' : '#i-no');
    node.querySelector('span').textContent = label;
  }

  function renderChecks() {
    const p = $('pal').value;
    const pOut = $('pal-out');
    if (!p.trim()) { pOut.dataset.state = 'idle'; pOut.querySelector('span').textContent = 'Type something to check'; }
    else verdict(pOut, LS.palindrome(p), LS.palindrome(p) ? 'Yes, a palindrome' : 'No, not a palindrome');
    const a = $('ana-a').value, b = $('ana-b').value;
    const aOut = $('ana-out');
    if (!a.trim() && !b.trim()) { aOut.dataset.state = 'idle'; aOut.querySelector('span').textContent = 'Type two words to compare'; }
    else verdict(aOut, LS.anagram(a, b), LS.anagram(a, b) ? 'Yes, an anagram' : 'No, not an anagram');
  }

  const esc = s => s.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');

  function renderMirror() {
    const t = state.text;
    const term = state.mark.toLowerCase();
    let html;
    if (!term) html = esc(t);
    else {
      html = '';
      let last = 0;
      const re = /[A-Za-z0-9]+/g;
      let m;
      while ((m = re.exec(t))) {
        if (m[0].toLowerCase() === term) {
          html += esc(t.slice(last, m.index)) + '<mark>' + esc(m[0]) + '</mark>';
          last = m.index + m[0].length;
        }
      }
      html += esc(t.slice(last));
    }
    el.mirror.innerHTML = html + '\n';
    el.mirror.scrollTop = el.text.scrollTop;
  }

  function renderCounter(r) {
    const over = r.truncated;
    el.bytes.textContent = fmt(LS.byteLength(state.text)) + ' / ' + fmt(LIMIT) + ' bytes';
    el.bytes.dataset.over = over ? 'true' : 'false';
    let note = '';
    if (over) note = 'Only the first ' + fmt(LIMIT) + ' bytes are analyzed.';
    else if (r.stats.words > LS.MAX_WORDS) note = 'Word tools read the first ' + LS.MAX_WORDS + ' words.';
    el.note.textContent = note;
  }

  let queued = false;
  function refresh() {
    if (queued) return;
    queued = true;
    requestAnimationFrame(() => {
      queued = false;
      const r = LS.analyze(state.text);
      renderCounter(r);
      renderCase(r);
      renderReport(r);
      renderFreq(r);
      renderDups(r);
      renderWriting(r);
      renderSearch();
      renderEdit();
      renderMirror();
    });
  }

  function setText(t, opts = {}) {
    state.text = t;
    if (el.text.value !== t) el.text.value = t;
    store.set(t);
    if (opts.record) pushHistory(t);
    refresh();
  }

  function pushHistory(t) {
    if (!t.trim()) return;
    const last = state.history[state.history.length - 1];
    if (last === t) return;
    state.history.push(t);
    if (state.history.length > 10) state.history.shift();
    renderHistory();
  }

  function renderHistory() {
    const list = $('hist-list');
    list.textContent = '';
    if (!state.history.length) {
      list.innerHTML = '<li class="empty">Nothing yet. Versions are kept as you type, the latest 10.</li>';
      return;
    }
    state.history.slice().reverse().forEach(t => {
      const li = document.createElement('li');
      const b = document.createElement('button');
      b.type = 'button';
      const a = document.createElement('span');
      a.className = 'h-t';
      a.textContent = t.replace(/\s+/g, ' ').trim().slice(0, 80);
      const m = document.createElement('span');
      m.className = 'h-m';
      m.textContent = fmt(LS.byteLength(t)) + ' bytes';
      b.append(a, m);
      b.addEventListener('click', () => { setText(t); closeHistory(); el.text.focus(); });
      li.appendChild(b);
      list.appendChild(li);
    });
  }

  function openHistory() {
    $('hist').hidden = false;
    $('hist-btn').setAttribute('aria-expanded', 'true');
  }
  function closeHistory() {
    $('hist').hidden = true;
    $('hist-btn').setAttribute('aria-expanded', 'false');
  }

  let idle = 0;
  el.text.addEventListener('input', () => {
    setText(el.text.value);
    clearTimeout(idle);
    idle = setTimeout(() => pushHistory(state.text), 1400);
  });
  el.text.addEventListener('scroll', () => { el.mirror.scrollTop = el.text.scrollTop; });

  $('sample').addEventListener('click', () => setText(SAMPLE, { record: true }));
  $('clear').addEventListener('click', () => { pushHistory(state.text); setText(''); el.text.focus(); });
  $('load').addEventListener('click', () => $('file').click());
  $('file').addEventListener('change', e => {
    const f = e.target.files[0];
    if (!f) return;
    const rd = new FileReader();
    rd.onload = () => {
      let t = String(rd.result).replace(/\r\n?/g, '\n');
      setText(t, { record: true });
      e.target.value = '';
    };
    rd.readAsText(f);
  });

  function download(name, content) {
    const url = URL.createObjectURL(new Blob([content], { type: 'text/plain;charset=utf-8' }));
    const a = document.createElement('a');
    a.href = url;
    a.download = name;
    document.body.appendChild(a);
    a.click();
    a.remove();
    setTimeout(() => URL.revokeObjectURL(url), 1000);
  }

  async function copy(text, btn) {
    try { await navigator.clipboard.writeText(text); } catch (e) {
      const t = document.createElement('textarea');
      t.value = text;
      document.body.appendChild(t);
      t.select();
      document.execCommand('copy');
      t.remove();
    }
    if (!btn) return;
    const label = btn.lastChild;
    const old = label.textContent;
    label.textContent = 'Copied';
    setTimeout(() => { label.textContent = old; }, 1400);
  }

  $('save').addEventListener('click', () => {
    if (!state.text.trim()) { el.note.textContent = 'Enter some text first.'; return; }
    download('lexiscan-report.txt', LS.reportText(state.text));
  });
  $('copy-report').addEventListener('click', e => {
    if (!state.text.trim()) { el.note.textContent = 'Enter some text first.'; return; }
    copy(LS.reportText(state.text), e.currentTarget);
  });

  $('hist-btn').addEventListener('click', () => ($('hist').hidden ? openHistory() : closeHistory()));
  document.addEventListener('click', e => { if (!e.target.closest('.pop-wrap')) closeHistory(); });
  document.addEventListener('keydown', e => { if (e.key === 'Escape') closeHistory(); });

  $$('[data-sort]').forEach(b => b.addEventListener('click', () => {
    state.sort = b.dataset.sort;
    $$('[data-sort]').forEach(x => x.setAttribute('aria-pressed', String(x === b)));
    renderFreq(LS.analyze(state.text));
  }));

  function syncMark() {
    const a = document.activeElement;
    const term = a === el.repOld ? el.repOld.value.trim() : el.search.value.trim();
    state.mark = /^[A-Za-z0-9]+$/.test(term) ? term : '';
    renderMirror();
  }
  el.search.addEventListener('input', () => { renderSearch(); syncMark(); });
  el.repOld.addEventListener('input', syncMark);
  el.repOld.addEventListener('focus', syncMark);
  el.search.addEventListener('focus', syncMark);

  $('rep-go').addEventListener('click', () => {
    const msg = $('rep-msg');
    if (!state.text.trim()) { msg.textContent = 'Enter some text first.'; return; }
    if (!el.repOld.value.trim()) { msg.textContent = 'Type the word to replace.'; return; }
    const res = LS.replace(state.text, el.repOld.value.trim(), el.repNew.value);
    msg.textContent = res.replaced + ' occurrence(s) replaced.';
    if (res.replaced) setText(res.text, { record: true });
  });

  ['pal', 'ana-a', 'ana-b'].forEach(id => $(id).addEventListener('input', renderChecks));

  $$('[data-op]').forEach(b => b.addEventListener('click', () => {
    state.op = b.dataset.op;
    $$('[data-op]').forEach(x => x.setAttribute('aria-pressed', String(x === b)));
    $('case-seg').hidden = state.op !== 'case';
    renderEdit();
  }));
  $$('[data-case]').forEach(b => b.addEventListener('click', () => {
    state.caseMode = Number(b.dataset.case);
    $$('[data-case]').forEach(x => x.setAttribute('aria-pressed', String(x === b)));
    renderEdit();
  }));
  $('edit-apply').addEventListener('click', () => { if (state.text.trim()) setText(state.result, { record: true }); });
  $('edit-copy').addEventListener('click', e => copy(state.result || '', e.currentTarget));

  el.cmpA.value = CMP_A;
  el.cmpB.value = CMP_B;
  el.cmpA.addEventListener('input', renderCompare);
  el.cmpB.addEventListener('input', renderCompare);
  $('cmp-use').addEventListener('click', () => { el.cmpA.value = state.text; renderCompare(); });

  const BUILD = 'gcc Word_A.c -o lexiscan && ./lexiscan';

  function highlight(src) {
    const re = /(\/\*[\s\S]*?\*\/)|("(?:\\.|[^"\\\n])*"|'(?:\\.|[^'\\\n])*')|(^[ \t]*#[^\n]*)|\b(\d+)\b|\b([A-Za-z_]\w*)\b(?=\s*\()|\b([A-Za-z_]\w*)\b/gm;
    const kw = new Set(['if', 'else', 'for', 'while', 'do', 'switch', 'case', 'default', 'break', 'continue', 'return', 'sizeof', 'typedef', 'struct', 'const']);
    const ty = new Set(['int', 'char', 'void', 'long', 'double', 'float', 'size_t', 'FILE', 'WordEntry', 'Statistics']);
    const out = [];
    let last = 0, m;
    const plain = s => { if (s) out.push([null, s]); };
    while ((m = re.exec(src))) {
      plain(src.slice(last, m.index));
      last = m.index + m[0].length;
      let cls = null;
      if (m[1]) cls = 'c';
      else if (m[2]) cls = 's';
      else if (m[3]) cls = 'p';
      else if (m[4]) cls = 'n';
      else if (m[5]) cls = kw.has(m[5]) ? 'k' : ty.has(m[5]) ? 't' : 'f';
      else if (m[6]) cls = kw.has(m[6]) ? 'k' : ty.has(m[6]) ? 't' : null;
      out.push([cls, m[0]]);
    }
    plain(src.slice(last));
    const lines = [''];
    out.forEach(([cls, text]) => {
      text.split('\n').forEach((part, i) => {
        if (i > 0) lines.push('');
        if (part) lines[lines.length - 1] += cls ? '<span class="tk-' + cls + '">' + esc(part) + '</span>' : esc(part);
      });
    });
    return lines;
  }

  function buildSource() {
    const src = window.LS_SOURCE.replace(/\r\n?/g, '\n').replace(/\n$/, '');
    const lines = highlight(src);
    const code = $('code');
    const frag = document.createDocumentFragment();
    lines.forEach((h, i) => {
      const d = document.createElement('div');
      d.className = 'l';
      d.dataset.n = i + 1;
      d.innerHTML = h;
      frag.appendChild(d);
    });
    code.appendChild(frag);

    const raw = src.split('\n');
    const fns = [];
    raw.forEach((ln, i) => {
      const m = /^\s*(?:int|void|char|double)\s+(\w+)\s*\(/.exec(ln);
      if (m) fns.push({ name: m[1], line: i + 1 });
    });
    $('src-meta').textContent = 'Word_A.c  ·  ' + fmt(raw.length) + ' lines  ·  ' + fns.length + ' functions  ·  ' + BUILD;

    const list = $('fn-list');
    fns.forEach(f => {
      const li = document.createElement('li');
      const b = document.createElement('button');
      b.type = 'button';
      b.innerHTML = '<span></span><span class="ln-n">' + f.line + '</span>';
      b.firstChild.textContent = f.name;
      b.addEventListener('click', () => {
        $$('.hit', code).forEach(x => x.classList.remove('hit'));
        $$('button', list).forEach(x => x.removeAttribute('aria-current'));
        b.setAttribute('aria-current', 'true');
        const target = code.children[f.line - 1];
        target.classList.add('hit');
        code.scrollTo({ top: Math.max(0, (f.line - 3) * 22), behavior: 'smooth' });
      });
      li.appendChild(b);
      list.appendChild(li);
    });
  }
  $('copy-build').addEventListener('click', e => copy(BUILD, e.currentTarget));

  const links = $$('[data-spy]');
  const spy = new IntersectionObserver(entries => {
    entries.forEach(en => {
      if (!en.isIntersecting) return;
      links.forEach(a => {
        if (a.dataset.spy === en.target.id) a.setAttribute('aria-current', 'true');
        else a.removeAttribute('aria-current');
      });
    });
  }, { rootMargin: '-25% 0px -65% 0px' });
  links.forEach(a => spy.observe($(a.dataset.spy)));

  const saved = store.get();
  state.text = saved === null ? SAMPLE : saved;
  el.text.value = state.text;
  if (state.text.trim()) state.history.push(state.text);
  renderHistory();
  renderChecks();
  renderCompare();
  buildSource();
  refresh();
})();
