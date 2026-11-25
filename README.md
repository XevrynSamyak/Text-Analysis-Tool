# 📊 LexiScan — C Text Analyzer

![C11](https://img.shields.io/badge/language-C11-blue) ![Dependencies](https://img.shields.io/badge/dependencies-none-green) ![Interface](https://img.shields.io/badge/interface-terminal-lightgrey)

A menu-driven text analysis program written in C. Paste in multiline text and LexiScan reports statistics, word and letter frequencies, writing metrics and a formatting score — then lets you search, replace, transform, compare and save your work.

---

## ✨ What it can do

### Analyze
- Full text report — characters (with and without spaces), letters, digits, spaces, punctuation, vowels, consonants, and upper/lowercase counts
- Word count, unique word count, and longest/shortest word
- Sentence and paragraph counts
- Word frequency and A–Z letter frequency
- Writing analysis — average word length, average words per sentence, estimated reading time (200 wpm), questions and exclamations

### Search & edit
- Whole-word search with occurrence count (case-insensitive)
- Word replacement
- Palindrome and anagram checks
- Duplicate word detection

### Transform
- Reverse the text or reverse word order
- Uppercase, lowercase, or title case
- Collapse extra spaces

### Compare & save
- Compare two texts by shared vocabulary (Jaccard-style similarity)
- Text cleanliness score out of 100
- Save a report to a file, load text from a file, and browse session history

---

## 🔧 Requirements

- A C11 compiler — GCC or Clang
- Standard C library only — no external packages

---

## ⚙️ Build & run

**macOS / Linux**

```bash
gcc -Wall -Wextra -std=c11 main.c -o lexiscan
./lexiscan
```

**Windows (MinGW)**

```bash
gcc -Wall -Wextra -std=c11 main.c -o lexiscan.exe
lexiscan.exe
```

---

## 🚀 First run

1. Launch the program — you'll see the 20-option menu.
2. Choose `1. Enter Text`.
3. Type your text one line at a time. Type `END` on a line by itself to finish.
4. Choose `2` for a full report, or try any other option.

> `END` must be uppercase and on its own line — it's the sentinel that ends text input.

---

## 📋 Menu

| # | Option | What it does |
|---|--------|--------------|
| 1 | Enter Text | Read multiline text until `END`, then add it to history |
| 2 | View Text Statistics & Longest Word | Print the full report |
| 3 | Word Frequency | Count each word (case-insensitive) |
| 4 | Letter Frequency | Count A–Z occurrences |
| 5 | Check Palindrome | Test a word or sentence you type |
| 6 | Check Anagram | Compare two words you type |
| 7 | Search for a Word | Count whole-word matches in the current text |
| 8 | Replace a Word | Replace every matching word |
| 9 | Reverse Text | Reverse the whole text in place |
| 10 | Reverse Word Order | Reverse the order of words |
| 11 | Change Letter Case | Uppercase / lowercase / title case |
| 12 | Remove Extra Spaces | Collapse repeated whitespace |
| 13 | Find Duplicate Words | List words that appear more than once |
| 14 | Writing Statistics | Readability-style metrics |
| 15 | Compare Two Texts | Shared vocabulary between two texts |
| 16 | Text Quality Score | Formatting score out of 100 |
| 17 | Save Report | Write a report to a file |
| 18 | Load Text from File | Replace the current text with a file's contents |
| 19 | View History | Show snapshots saved during this session |
| 20 | Exit | Close the program |

Most options need text to be entered first (option 1 or 18); the program tells you if you skip that step. Options 5, 6 and 15 ask for their own input.

---

## 🧠 Behavior notes

- **Words** are runs of letters and digits; extraction lowercases them before counting.
- **Sentences** are counted from `.`, `?` and `!` characters.
- **Paragraphs** are blocks separated by blank lines.
- **Palindromes** ignore case, spaces and punctuation — only letters and digits are compared.
- **Anagrams** ignore case, spaces and non-letters.
- **Quality score** starts at 100, deducts 5 per repeated space, 5 per repeated punctuation mark, and 15 if the average sentence exceeds 30 words.

---

## 📏 Limits

| Limit | Value |
|-------|-------|
| Text buffer | 5,000 characters |
| Extracted words | 500 |
| Length of one extracted word | 99 characters |
| History entries | 10 (oldest dropped when full) |
| Persistence | In memory only — use "Save Report" to export |

---

## 📁 Project structure

```text
Text-Analysis-Tool/
├── main.c     # Full implementation and CLI menu
└── README.md  # This file
```
