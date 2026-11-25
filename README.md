LexiScan — C Text Analyzer
LexiScan is a menu-driven text analysis program written in C. It accepts multiline text, analyzes it, and provides tools for searching, editing, comparing, transforming, and saving text.
Features
Enter multiline text (type END on its own line to finish).
View text statistics and longest/shortest words.
Word frequency (case-insensitive).
Alphabet frequency (A–Z).
Palindrome check (ignores case and non-alphanumeric characters).
Anagram check (ignores case, spaces, and non-letters).
Search for a whole word and count occurrences.
Replace a word.
Reverse text.
Reverse word order.
Convert to uppercase, lowercase, or title case.
Remove extra spaces.
Find duplicate words.
Writing analysis.
Compare two texts.
Text cleanliness/quality score.
Save a report to a file.
Load text from a file.
View analysis/edit history.
Exit.
Requirements
A C compiler (GCC or Clang)
Standard C library
Compile
Linux / macOS
gcc -Wall -Wextra -std=c11 Word_A.c -o lexiscan
Run
./lexiscan
On Windows using MinGW:
gcc -Wall -Wextra -std=c11 Word_A.c -o lexiscan.exe
lexiscan.exe
Usage notes
Select a menu option by entering its number.
For option 1, enter text one line at a time; type END on a separate line to stop.
Some menu options require text to have been entered or loaded first.
The program stores text and history in memory during the current run. History is limited to 10 entries.
Input text capacity is 5,000 characters. Word extraction stores up to 500 words; individual extracted words are limited to 99 characters.
Option 18 loads text from a file path entered at the prompt.
Project files
Word_A.c — source code.
README.md — project documentation.
Sample_Test_Cases.md — example test inputs and expected outputs for selected features.
Notes
The test cases document focuses on the visible results of selected menu options. Some output can vary slightly depending on the terminal and how the program is run.
