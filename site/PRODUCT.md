# Product

<!-- impeccable:product-schema 1 -->

## Platform

web

## Stack

Static HTML, CSS and vanilla JS. No build step; opens from index.html and works offline.

## Users

Anyone with a piece of text who wants it measured or reshaped: counts, frequencies, checks, quick edits. The C source is a secondary audience for people curious how it works.

## Product Purpose

LexiScan is a text analyzer originally written in C (Word_A.c, 1462 lines, 20 menu options). The site makes the same analyzer usable in the browser, with the same rules, and publishes the source.

## Positioning

Every result on the site is computed by a line-for-line port of the C functions, so the browser and the terminal program give the same numbers on the same input, including the program's own definitions (a sentence ends at . ? !, words are runs of letters and digits, reading time is words / 200).

## Capabilities and Constraints

- The 20 menu options: enter text, full report, word frequency, letter frequency, palindrome, anagram, search, replace, reverse text, reverse word order, case (upper, lower, title), remove extra spaces, duplicate words, writing statistics, compare two texts, quality score, save report, load file, history, exit.
- C limits: MAX_TEXT 5000 chars, MAX_WORDS 500 words, WORD_LEN 100, HISTORY_SIZE 10.
- Quality score: starts at 100, minus 5 per repeated whitespace char, minus 5 per repeated punctuation mark, minus 15 if average sentence is over 30 words, floor 0.
- Similarity in compare is shared unique words / total unique words x 100 (Jaccard-style).

## Brand Commitments

- Name: LexiScan (from the program's own menu and report headers).
- Author credit: Samyak Jain (user-supplied).

## Evidence on Hand

- Source: Word_A.c (copied into the site as source/Word_A.c).
- No testimonials, users, benchmarks, or download counts exist. Do not invent any.

## Product Principles

- Same input, same output as the C program.
- Show results, not claims about results.
- The source is part of the product, not a footnote.
