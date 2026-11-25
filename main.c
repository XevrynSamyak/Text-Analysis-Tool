#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>

#define MAX_TEXT 5000
#define MAX_WORDS 500
#define WORD_LEN 100
#define HISTORY_SIZE 10

typedef struct
{
    char word[WORD_LEN];
    int count;
} WordEntry;

typedef struct
{
    int characters;
    int charactersNoSpaces;
    int alphabets;
    int digits;
    int spaces;
    int punctuation;
    int vowels;
    int consonants;
    int words;
    int sentences;
    int paragraphs;
    int uppercase;
    int lowercase;
} Statistics;

char text[MAX_TEXT] = "";
char history[HISTORY_SIZE][MAX_TEXT];
int historyCount = 0;

/* ---------- INPUT FUNCTIONS ---------- */

void readLine(char buffer[], int size)
{
    if (fgets(buffer, size, stdin))
    {
        buffer[strcspn(buffer, "\n")] = '\0';
    }
}
/*
 * readLine: reads one line from stdin into buffer (max size-1 chars)
 * and strips the trailing newline.
 * Input: buffer - where the line is stored, size - capacity of buffer.
 * Returns: nothing (buffer is left unchanged if reading fails).
 */

void readText(char destination[], int size)
{
    char line[500];
    destination[0] = '\0';

    printf("\nEnter your text below.\n");
    printf("Type END on a separate line to finish.\n\n");

    while (1)
    {
        readLine(line, sizeof(line));

        if (strcmp(line, "END") == 0)
            break;

        if (strlen(destination) + strlen(line) + 2 >= size)
        {
            printf("Text limit reached.\n");
            break;
        }

        strcat(destination, line);
        strcat(destination, "\n");
    }
}
/*
 * readText: reads multi-line text from the user until a line containing
 * only "END" is typed, appending each line (plus '\n') to destination.
 * Stops early with a message if the size limit would be exceeded.
 * Input: destination - buffer to fill, size - its capacity.
 * Returns: nothing.
 */

int getChoice()
{
    char input[50];
    char *end;
    long choice;

    printf("\nEnter choice: ");
    readLine(input, sizeof(input));

    choice = strtol(input, &end, 10);

    if (end == input || *end != '\0')
        return -1;

    return (int)choice;
}
/*
 * getChoice: prompts for a menu choice and safely converts the input
 * to an integer using strtol.
 * Input: none (reads from stdin).
 * Returns: the number entered, or -1 if the input isn't a valid integer.
 */

/* ---------- BASIC STRING FUNCTIONS ---------- */

int isVowel(char ch)
{
    ch = (char)tolower((unsigned char)ch);

    return ch == 'a' || ch == 'e' || ch == 'i' ||
           ch == 'o' || ch == 'u';
}
/*
 * isVowel: checks whether a character is a vowel (case-insensitive).
 * Input: ch - the character to test.
 * Returns: 1 if it is a, e, i, o or u; otherwise 0.
 */

int isWordChar(char ch)
{
    return isalnum((unsigned char)ch);
}
/*
 * isWordChar: checks whether a character can be part of a word
 * (letter or digit).
 * Input: ch - the character to test.
 * Returns: non-zero if alphanumeric, otherwise 0.
 */

void removeNewline(char str[])
{
    str[strcspn(str, "\n")] = '\0';
}
/*
 * removeNewline: cuts the string at the first '\n', removing the newline.
 * Input: str - the string to modify in place.
 * Returns: nothing. (Note: not called anywhere in this program.)
 */

void toLowerString(char str[])
{
    int i;

    for (i = 0; str[i] != '\0'; i++)
        str[i] = (char)tolower((unsigned char)str[i]);
}
/*
 * toLowerString: converts every character of a string to lowercase
 * in place.
 * Input: str - the string to convert.
 * Returns: nothing.
 */

/* Extract words from text */
int extractWords(const char source[], WordEntry words[])
{
    int count = 0;
    int i = 0, j;

    while (source[i] != '\0')
    {
        while (source[i] != '\0' &&
               !isWordChar(source[i]))
        {
            i++;
        }

        if (source[i] == '\0')
            break;

        j = 0;

        while (source[i] != '\0' &&
               isWordChar(source[i]))
        {
            if (j < WORD_LEN - 1)
                words[count].word[j++] = source[i];

            i++;
        }

        words[count].word[j] = '\0';
        toLowerString(words[count].word);
        words[count].count = 1;

        count++;

        if (count >= MAX_WORDS)
            break;
    }

    return count;
}
/*
 * extractWords: splits text into words (runs of letters/digits), stores
 * each one lowercased in the words array with count = 1. Words longer
 * than WORD_LEN-1 are truncated; stops at MAX_WORDS words.
 * Input: source - text to split, words - output array (size MAX_WORDS).
 * Returns: the number of words extracted.
 */

/* ---------- STATISTICS ---------- */

void calculateStatistics(const char source[], Statistics *s)
{
    int i;
    int inWord = 0;
    int inParagraph = 0;

    memset(s, 0, sizeof(Statistics));

    for (i = 0; source[i] != '\0'; i++)
    {
        char ch = source[i];

        s->characters++;

        if (!isspace((unsigned char)ch))
            s->charactersNoSpaces++;

        if (isalpha((unsigned char)ch))
        {
            s->alphabets++;

            if (isVowel(ch))
                s->vowels++;
            else
                s->consonants++;

            if (isupper((unsigned char)ch))
                s->uppercase++;
            else
                s->lowercase++;
        }
        else if (isdigit((unsigned char)ch))
        {
            s->digits++;
        }
        else if (ispunct((unsigned char)ch))
        {
            s->punctuation++;
        }

        if (isspace((unsigned char)ch))
            s->spaces++;

        if (ch == '.' || ch == '?' || ch == '!')
            s->sentences++;

        /* Word counting */
        if (isWordChar(ch))
        {
            if (!inWord)
            {
                s->words++;
                inWord = 1;
            }
        }
        else
        {
            inWord = 0;
        }

        /* Paragraph counting */
        if (!isspace((unsigned char)ch))
        {
            if (!inParagraph)
            {
                s->paragraphs++;
                inParagraph = 1;
            }
        }
        else if (ch == '\n')
        {
            inParagraph = 0;
        }
    }
}
/*
 * calculateStatistics: scans the text once and fills a Statistics struct
 * with counts of characters, letters, digits, spaces, punctuation,
 * vowels, consonants, upper/lowercase letters, words, sentences
 * (. ? !) and paragraphs (blocks of text separated by newlines).
 * Input: source - text to analyse, s - struct to fill (zeroed first).
 * Returns: nothing (results are written through s).
 */

/* ---------- WORD ANALYSIS ---------- */

void longestShortestWord(const char source[],
                        char longest[],
                        char shortest[])
{
    WordEntry words[MAX_WORDS];
    int count = extractWords(source, words);
    int i;

    longest[0] = '\0';
    shortest[0] = '\0';

    if (count == 0)
        return;

    strcpy(longest, words[0].word);
    strcpy(shortest, words[0].word);

    for (i = 1; i < count; i++)
    {
        if (strlen(words[i].word) > strlen(longest))
            strcpy(longest, words[i].word);

        if (strlen(words[i].word) < strlen(shortest))
            strcpy(shortest, words[i].word);
    }
}
/*
 * longestShortestWord: finds the longest and shortest word in the text
 * (first one wins on ties; words are lowercase).
 * Input: source - text to search, longest/shortest - output buffers
 * (at least WORD_LEN chars each).
 * Returns: nothing; both buffers are empty strings if there are no words.
 */

int uniqueWordCount(const char source[])
{
    WordEntry words[MAX_WORDS];
    int count = extractWords(source, words);
    int unique = 0;
    int i, j;

    for (i = 0; i < count; i++)
    {
        int found = 0;

        for (j = 0; j < i; j++)
        {
            if (strcmp(words[i].word, words[j].word) == 0)
            {
                found = 1;
                break;
            }
        }

        if (!found)
            unique++;
    }

    return unique;
}
/*
 * uniqueWordCount: counts distinct words in the text, ignoring case.
 * A word is unique if no earlier word matches it.
 * Input: source - text to analyse.
 * Returns: number of distinct words.
 */

void wordFrequency(const char source[])
{
    WordEntry words[MAX_WORDS];
    int count = extractWords(source, words);
    int i, j;

    for (i = 0; i < count; i++)
    {
        for (j = 0; j < i; j++)
        {
            if (strcmp(words[i].word, words[j].word) == 0)
            {
                words[j].count++;
                words[i].count = 0;
                break;
            }
        }
    }

    printf("\n--- WORD FREQUENCY ---\n");

    for (i = 0; i < count; i++)
    {
        if (words[i].count > 0)
            printf("%-20s : %d\n",
                   words[i].word, words[i].count);
    }
}
/*
 * wordFrequency: counts how many times each word appears (case-insensitive)
 * by merging repeats into the first occurrence, then prints each word
 * with its count.
 * Input: source - text to analyse.
 * Returns: nothing (prints the table).
 */

/* ---------- ALPHABET ANALYSIS ---------- */

void alphabetFrequency(const char source[])
{
    int frequency[26] = {0};
    int i;

    for (i = 0; source[i] != '\0'; i++)
    {
        if (isalpha((unsigned char)source[i]))
        {
            char ch = (char)tolower((unsigned char)source[i]);
            frequency[ch - 'a']++;
        }
    }

    printf("\n--- ALPHABET FREQUENCY ---\n");

    for (i = 0; i < 26; i++)
    {
        printf("%c : %d\n", 'A' + i, frequency[i]);
    }
}
/*
 * alphabetFrequency: counts how often each letter A-Z appears
 * (case-insensitive) and prints all 26 counts.
 * Input: source - text to analyse.
 * Returns: nothing (prints the table).
 */

/* ---------- PALINDROME ---------- */

int isPalindrome(const char source[])
{
    int left = 0;
    int right = strlen(source) - 1;

    while (left < right)
    {
        while (left < right &&
               !isalnum((unsigned char)source[left]))
            left++;

        while (left < right &&
               !isalnum((unsigned char)source[right]))
            right--;

        if (tolower((unsigned char)source[left]) !=
            tolower((unsigned char)source[right]))
            return 0;

        left++;
        right--;
    }

    return 1;
}
/*
 * isPalindrome: checks whether the text reads the same forwards and
 * backwards, ignoring case, spaces and punctuation, using two pointers
 * moving inward.
 * Input: source - text to check.
 * Returns: 1 if it is a palindrome, 0 otherwise.
 */

/* ---------- ANAGRAM ---------- */

int isAnagram(const char first[], const char second[])
{
    int frequency[256] = {0};
    int i;

    for (i = 0; first[i] != '\0'; i++)
    {
        if (isalpha((unsigned char)first[i]))
        {
            frequency[tolower((unsigned char)first[i])]++;
        }
    }

    for (i = 0; second[i] != '\0'; i++)
    {
        if (isalpha((unsigned char)second[i]))
        {
            frequency[tolower((unsigned char)second[i])]--;
        }
    }

    for (i = 0; i < 256; i++)
    {
        if (frequency[i] != 0)
            return 0;
    }

    return 1;
}
/*
 * isAnagram: checks whether two strings contain exactly the same letters
 * (ignoring case, spaces and non-letters) by adding counts for the first
 * string and subtracting for the second; all counts must end at zero.
 * Input: first, second - the strings to compare.
 * Returns: 1 if they are anagrams, 0 otherwise.
 */

/* ---------- SEARCH WORD ---------- */

int searchWord(const char source[], const char target[])
{
    WordEntry words[MAX_WORDS];
    int count = extractWords(source, words);
    int matches = 0;
    int i;

    char search[WORD_LEN];
    strncpy(search, target, WORD_LEN - 1);
    search[WORD_LEN - 1] = '\0';
    toLowerString(search);

    for (i = 0; i < count; i++)
    {
        if (strcmp(words[i].word, search) == 0)
            matches++;
    }

    return matches;
}
/*
 * searchWord: counts how many times a whole word appears in the text,
 * ignoring case.
 * Input: source - text to search, target - word to look for.
 * Returns: number of matches (0 if none).
 */

/* ---------- REPLACE WORD ---------- */

void replaceWord(char source[], const char oldWord[],
                 const char newWord[])
{
    char result[MAX_TEXT];
    char oldLower[WORD_LEN];
    char current[WORD_LEN];

    int i = 0, j = 0;
    int replaced = 0;

    strncpy(oldLower, oldWord, WORD_LEN - 1);
    oldLower[WORD_LEN - 1] = '\0';
    toLowerString(oldLower);

    result[0] = '\0';

    while (source[i] != '\0')
    {
        if (isWordChar(source[i]))
        {
            int start = i;
            int k = 0;

            while (isWordChar(source[i]))
            {
                if (k < WORD_LEN - 1)
                    current[k++] = source[i];

                i++;
            }

            current[k] = '\0';

            char lowerCurrent[WORD_LEN];
            strcpy(lowerCurrent, current);
            toLowerString(lowerCurrent);

            if (strcmp(lowerCurrent, oldLower) == 0)
            {
                if (strlen(result) + strlen(newWord) < MAX_TEXT)
                    strcat(result, newWord);

                replaced++;
            }
            else
            {
                if (strlen(result) + strlen(current) < MAX_TEXT)
                    strcat(result, current);
            }

            j = i;
            (void)start;
            (void)j;
        }
        else
        {
            int length = strlen(result);

            if (length < MAX_TEXT - 2)
            {
                result[length] = source[i];
                result[length + 1] = '\0';
            }

            i++;
        }
    }

    strcpy(source, result);

    printf("%d occurrence(s) replaced.\n", replaced);
}
/*
 * replaceWord: replaces every whole-word occurrence of oldWord
 * (case-insensitive) with newWord, keeping punctuation and spacing
 * as they were. Builds the new text in a temporary buffer, then copies
 * it back over source and prints how many replacements were made.
 * Input: source - text to modify in place, oldWord - word to find,
 * newWord - replacement.
 * Returns: nothing.
 */

/* ---------- REVERSE TEXT ---------- */

void reverseText(char source[])
{
    int left = 0;
    int right = strlen(source) - 1;

    while (left < right)
    {
        char temp = source[left];
        source[left] = source[right];
        source[right] = temp;

        left++;
        right--;
    }
}
/*
 * reverseText: reverses the whole string character by character in place
 * by swapping from both ends toward the middle.
 * Input: source - the string to reverse.
 * Returns: nothing.
 */

/* Reverse word order */
void reverseWordOrder(const char source[])
{
    char words[MAX_WORDS][WORD_LEN];
    int count = 0;
    int i = 0, j;

    while (source[i] != '\0')
    {
        while (isspace((unsigned char)source[i]))
            i++;

        if (source[i] == '\0')
            break;

        j = 0;

        while (source[i] != '\0' &&
               !isspace((unsigned char)source[i]))
        {
            if (j < WORD_LEN - 1)
                words[count][j++] = source[i];

            i++;
        }

        words[count][j] = '\0';
        count++;

        if (count >= MAX_WORDS)
            break;
    }

    printf("\nReversed word order:\n");

    for (i = count - 1; i >= 0; i--)
    {
        printf("%s", words[i]);

        if (i > 0)
            printf(" ");
    }

    printf("\n");
}
/*
 * reverseWordOrder: splits the text on whitespace and prints the words
 * in reverse order on one line. Does not modify the original text.
 * Input: source - text to process.
 * Returns: nothing (prints the result).
 */

/* ---------- CASE CONVERSION ---------- */

void convertCase(const char source[], int mode)
{
    char result[MAX_TEXT];
    int i;
    int newWord = 1;

    strcpy(result, source);

    for (i = 0; result[i] != '\0'; i++)
    {
        if (mode == 1)
            result[i] = (char)toupper((unsigned char)result[i]);

        else if (mode == 2)
            result[i] = (char)tolower((unsigned char)result[i]);

        else if (mode == 3)
        {
            if (isalpha((unsigned char)result[i]))
            {
                if (newWord)
                    result[i] = (char)toupper((unsigned char)result[i]);
                else
                    result[i] = (char)tolower((unsigned char)result[i]);

                newWord = 0;
            }
            else
            {
                newWord = isspace((unsigned char)result[i]);
            }
        }
    }

    printf("\nConverted text:\n%s\n", result);
}
/*
 * convertCase: prints a converted copy of the text; the original is not
 * changed. Modes: 1 = UPPERCASE, 2 = lowercase, 3 = Title Case
 * (first letter of each word capitalised, rest lowercase).
 * Input: source - text to convert, mode - 1, 2 or 3.
 * Returns: nothing (prints the result).
 */

/* ---------- CLEAN EXTRA SPACES ---------- */

void cleanSpaces(char source[])
{
    char result[MAX_TEXT];
    int i = 0, j = 0;
    int previousSpace = 1;

    while (source[i] != '\0')
    {
        if (isspace((unsigned char)source[i]))
        {
            if (!previousSpace)
            {
                result[j++] = ' ';
                previousSpace = 1;
            }
        }
        else
        {
            result[j++] = source[i];
            previousSpace = 0;
        }

        i++;
    }

    if (j > 0 && result[j - 1] == ' ')
        j--;

    result[j] = '\0';

    strcpy(source, result);

    printf("Extra spaces removed.\n");
}
/*
 * cleanSpaces: collapses any run of whitespace (spaces, tabs, newlines)
 * into a single space and removes leading/trailing whitespace. Modifies
 * the text in place, so line breaks are lost as well.
 * Input: source - text to clean.
 * Returns: nothing.
 */

/* ---------- DUPLICATE WORDS ---------- */

void duplicateWords(const char source[])
{
    WordEntry words[MAX_WORDS];
    int count = extractWords(source, words);
    int i, j;

    printf("\n--- DUPLICATE WORDS ---\n");

    for (i = 0; i < count; i++)
    {
        int alreadyPrinted = 0;

        for (j = 0; j < i; j++)
        {
            if (strcmp(words[i].word, words[j].word) == 0)
                alreadyPrinted = 1;
        }

        if (alreadyPrinted)
            continue;

        int frequency = 0;

        for (j = 0; j < count; j++)
        {
            if (strcmp(words[i].word, words[j].word) == 0)
                frequency++;
        }

        if (frequency > 1)
            printf("%s appears %d times\n",
                   words[i].word, frequency);
    }
}
/*
 * duplicateWords: prints every word that appears more than once,
 * along with how many times it appears. Each word is reported only once.
 * Input: source - text to analyse.
 * Returns: nothing (prints the list).
 */

/* ---------- WRITING ANALYSIS ---------- */

void writingAnalysis(const char source[])
{
    Statistics s;
    calculateStatistics(source, &s);

    double averageWordLength = 0;
    double averageSentenceLength = 0;
    double readingTime = 0;

    if (s.words > 0)
        averageWordLength = (double)s.alphabets / s.words;

    if (s.sentences > 0)
        averageSentenceLength = (double)s.words / s.sentences;

    readingTime = (double)s.words / 200.0;

    printf("\n--- WRITING ANALYSIS ---\n");
    printf("Average word length: %.2f letters\n",
           averageWordLength);
    printf("Average words per sentence: %.2f\n",
           averageSentenceLength);
    printf("Estimated reading time: %.2f minute(s)\n",
           readingTime);

    printf("Questions: ");
    int questions = 0, exclamations = 0, i;

    for (i = 0; source[i] != '\0'; i++)
    {
        if (source[i] == '?')
            questions++;

        if (source[i] == '!')
            exclamations++;
    }

    printf("%d\n", questions);
    printf("Exclamations: %d\n", exclamations);
}
/*
 * writingAnalysis: prints writing metrics: average word length (letters
 * per word), average words per sentence, estimated reading time
 * (assuming 200 words per minute), and the number of question marks
 * and exclamation marks.
 * Input: source - text to analyse.
 * Returns: nothing (prints the analysis).
 */

/* ---------- TEXT COMPARISON ---------- */

void compareTexts()
{
    char first[MAX_TEXT];
    char second[MAX_TEXT];

    WordEntry w1[MAX_WORDS];
    WordEntry w2[MAX_WORDS];

    int n1, n2;
    int shared = 0;
    int i, j;

    printf("\nEnter first text:\n");
    readText(first, MAX_TEXT);

    printf("\nEnter second text:\n");
    readText(second, MAX_TEXT);

    n1 = extractWords(first, w1);
    n2 = extractWords(second, w2);

    int unique1 = uniqueWordCount(first);
    int unique2 = uniqueWordCount(second);

    for (i = 0; i < n1; i++)
    {
        int found = 0;

        for (j = 0; j < n2; j++)
        {
            if (strcmp(w1[i].word, w2[j].word) == 0)
            {
                found = 1;
                break;
            }
        }

        if (found)
        {
            int alreadyCounted = 0;

            for (j = 0; j < i; j++)
            {
                if (strcmp(w1[i].word, w1[j].word) == 0)
                    alreadyCounted = 1;
            }

            if (!alreadyCounted)
                shared++;
        }
    }

    int totalUnique = unique1 + unique2 - shared;
    double similarity = 0;

    if (totalUnique > 0)
        similarity = (double)shared / totalUnique * 100.0;

    printf("\n--- TEXT COMPARISON ---\n");
    printf("First text words: %d\n", n1);
    printf("Second text words: %d\n", n2);
    printf("Shared unique words: %d\n", shared);
    printf("Vocabulary similarity: %.2f%%\n", similarity);
}
/*
 * compareTexts: asks the user to enter two separate texts, then compares
 * their vocabulary. Prints each text's word count, the number of unique
 * words they share, and a similarity percentage
 * (shared / total unique words across both * 100, a Jaccard-style score).
 * Input: none (reads both texts from stdin).
 * Returns: nothing (prints the comparison).
 */

/* ---------- CLEANLINESS SCORE ---------- */

void cleanlinessScore(const char source[])
{
    int score = 100;
    int i;
    int repeatedSpaces = 0;
    int repeatedPunctuation = 0;

    Statistics s;
    calculateStatistics(source, &s);

    for (i = 1; source[i] != '\0'; i++)
    {
        if (isspace((unsigned char)source[i]) &&
            isspace((unsigned char)source[i - 1]))
            repeatedSpaces++;

        if (ispunct((unsigned char)source[i]) &&
            source[i] == source[i - 1])
            repeatedPunctuation++;
    }

    score -= repeatedSpaces * 5;
    score -= repeatedPunctuation * 5;

    if (s.words > 0 && s.sentences > 0)
    {
        double avg = (double)s.words / s.sentences;

        if (avg > 30)
            score -= 15;
    }

    if (score < 0)
        score = 0;

    printf("\n--- TEXT CLEANLINESS SCORE ---\n");
    printf("Score: %d / 100\n", score);

    if (score >= 90)
        printf("Excellent text formatting!\n");
    else if (score >= 70)
        printf("Good formatting.\n");
    else if (score >= 50)
        printf("Needs some improvement.\n");
    else
        printf("Text needs significant cleanup.\n");
}
/*
 * cleanlinessScore: gives the text a formatting score out of 100.
 * Starts at 100 and deducts 5 for each repeated whitespace character,
 * 5 for each repeated punctuation mark (like "!!"), and 15 if the
 * average sentence is over 30 words. Prints the score and a verdict.
 * Input: source - text to score.
 * Returns: nothing (prints the score and message).
 */

/* ---------- REPORT ---------- */

void printReport(const char source[])
{
    Statistics s;
    char longest[WORD_LEN];
    char shortest[WORD_LEN];

    calculateStatistics(source, &s);
    longestShortestWord(source, longest, shortest);

    printf("\n========================================\n");
    printf("          LEXISCAN REPORT\n");
    printf("========================================\n");

    printf("Characters (including spaces): %d\n", s.characters);
    printf("Characters (excluding spaces): %d\n",
           s.charactersNoSpaces);
    printf("Alphabetic characters: %d\n", s.alphabets);
    printf("Digits: %d\n", s.digits);
    printf("Spaces: %d\n", s.spaces);
    printf("Punctuation: %d\n", s.punctuation);

    printf("\nVowels: %d\n", s.vowels);
    printf("Consonants: %d\n", s.consonants);
    printf("Uppercase letters: %d\n", s.uppercase);
    printf("Lowercase letters: %d\n", s.lowercase);

    printf("\nWords: %d\n", s.words);
    printf("Unique words: %d\n", uniqueWordCount(source));
    printf("Sentences: %d\n", s.sentences);
    printf("Paragraphs: %d\n", s.paragraphs);

    printf("Longest word: %s\n",
           strlen(longest) ? longest : "N/A");

    printf("Shortest word: %s\n",
           strlen(shortest) ? shortest : "N/A");

    printf("Palindrome: %s\n",
           isPalindrome(source) ? "Yes" : "No");

    printf("========================================\n");
}
/*
 * printReport: prints the full statistics report for the text: character
 * counts, letter types, words, unique words, sentences, paragraphs,
 * longest/shortest word and whether the whole text is a palindrome.
 * Input: source - text to report on.
 * Returns: nothing (prints the report).
 */

/* ---------- SAVE REPORT ---------- */

void saveReport()
{
    FILE *file;
    char filename[100];

    printf("Enter filename (example: report.txt): ");
    readLine(filename, sizeof(filename));

    file = fopen(filename, "w");

    if (file == NULL)
    {
        printf("Unable to create file.\n");
        return;
    }

    Statistics s;
    char longest[WORD_LEN];
    char shortest[WORD_LEN];

    calculateStatistics(text, &s);
    longestShortestWord(text, longest, shortest);

    fprintf(file, "LEXISCAN TEXT ANALYSIS REPORT\n");
    fprintf(file, "=============================\n");
    fprintf(file, "Original Text:\n%s\n\n", text);

    fprintf(file, "Characters: %d\n", s.characters);
    fprintf(file, "Characters without spaces: %d\n",
            s.charactersNoSpaces);
    fprintf(file, "Alphabets: %d\n", s.alphabets);
    fprintf(file, "Digits: %d\n", s.digits);
    fprintf(file, "Spaces: %d\n", s.spaces);
    fprintf(file, "Punctuation: %d\n", s.punctuation);

    fprintf(file, "Vowels: %d\n", s.vowels);
    fprintf(file, "Consonants: %d\n", s.consonants);
    fprintf(file, "Words: %d\n", s.words);
    fprintf(file, "Unique words: %d\n", uniqueWordCount(text));
    fprintf(file, "Sentences: %d\n", s.sentences);
    fprintf(file, "Paragraphs: %d\n", s.paragraphs);

    fprintf(file, "Longest word: %s\n", longest);
    fprintf(file, "Shortest word: %s\n", shortest);

    fprintf(file, "Palindrome: %s\n",
            isPalindrome(text) ? "Yes" : "No");

    fclose(file);

    printf("Report saved successfully!\n");
}
/*
 * saveReport: asks for a filename and writes a text analysis report of
 * the global text (the original text plus its statistics) to that file.
 * Prints an error and returns if the file can't be created.
 * Input: none (reads the filename from stdin, uses global text).
 * Returns: nothing.
 */

/* ---------- LOAD TEXT ---------- */

void loadFile()
{
    FILE *file;
    char filename[100];
    int ch;
    int count = 0;

    printf("Enter filename to load: ");
    readLine(filename, sizeof(filename));

    file = fopen(filename, "r");

    if (file == NULL)
    {
        printf("File not found.\n");
        return;
    }

    while ((ch = fgetc(file)) != EOF &&
           count < MAX_TEXT - 1)
    {
        text[count++] = (char)ch;
    }

    text[count] = '\0';

    fclose(file);

    printf("Text loaded successfully!\n");
}
/*
 * loadFile: asks for a filename and reads the file's contents into the
 * global text buffer (up to MAX_TEXT-1 characters), replacing whatever
 * was there. Prints an error and returns if the file can't be opened.
 * Input: none (reads the filename from stdin).
 * Returns: nothing.
 */

/* ---------- HISTORY ---------- */

void addHistory()
{
    if (strlen(text) == 0)
        return;

    if (historyCount < HISTORY_SIZE)
    {
        strcpy(history[historyCount], text);
        historyCount++;
    }
    else
    {
        int i;

        for (i = 1; i < HISTORY_SIZE; i++)
            strcpy(history[i - 1], history[i]);

        strcpy(history[HISTORY_SIZE - 1], text);
    }
}
/*
 * addHistory: saves a copy of the current global text in the history
 * list. If the list is full (HISTORY_SIZE entries), the oldest entry is
 * dropped and the rest shift up. Does nothing if the text is empty.
 * Input: none (uses global text).
 * Returns: nothing.
 */

void showHistory()
{
    int i;

    printf("\n--- ANALYSIS HISTORY ---\n");

    if (historyCount == 0)
    {
        printf("No history available.\n");
        return;
    }

    for (i = 0; i < historyCount; i++)
    {
        printf("%d. %.70s\n", i + 1, history[i]);
    }
}
/*
 * showHistory: prints the saved history as a numbered list, showing only
 * the first 70 characters of each entry. Prints a message if empty.
 * Input: none (uses global history).
 * Returns: nothing.
 */

/* ---------- MAIN MENU ---------- */

void displayMenu()
{
    printf("\n========================================\n");
    printf("          LEXISCAN TEXT ANALYZER\n");
    printf("========================================\n");
    printf("1.  Enter new text\n");
    printf("2.  Complete text statistics\n");
    printf("3.  Word frequency analysis\n");
    printf("4.  Alphabet frequency analysis\n");
    printf("5.  Palindrome checker\n");
    printf("6.  Anagram checker\n");
    printf("7.  Search a word\n");
    printf("8.  Replace a word\n");
    printf("9.  Reverse complete text\n");
    printf("10. Reverse word order\n");
    printf("11. Convert case\n");
    printf("12. Remove extra spaces\n");
    printf("13. Detect duplicate words\n");
    printf("14. Writing analysis\n");
    printf("15. Compare two texts\n");
    printf("16. Text cleanliness score\n");
    printf("17. Save report\n");
    printf("18. Load text from file\n");
    printf("19. View analysis history\n");
    printf("20. Exit\n");
    printf("========================================\n");
}
/*
 * displayMenu: prints the LexiScan main menu with all 20 options.
 * Input: none.
 * Returns: nothing.
 */

int main()
{
    int choice;
    char first[WORD_LEN];
    char second[WORD_LEN];
    char search[WORD_LEN];
    char oldWord[WORD_LEN];
    char newWord[WORD_LEN];

    printf("Welcome to LexiScan!\n");

    while (1)
    {
        displayMenu();
        choice = getChoice();

        switch (choice)
        {
            case 1:
                readText(text, MAX_TEXT);
                addHistory();
                printf("Text saved in memory.\n");
                break;

            case 2:
                if (strlen(text) == 0)
                    printf("Please enter text first.\n");
                else
                    printReport(text);
                break;

            case 3:
                if (strlen(text) == 0)
                    printf("Please enter text first.\n");
                else
                    wordFrequency(text);
                break;

            case 4:
                if (strlen(text) == 0)
                    printf("Please enter text first.\n");
                else
                    alphabetFrequency(text);
                break;

            case 5:
                printf("Enter word or sentence: ");
                readLine(first, sizeof(first));

                printf("Palindrome: %s\n",
                       isPalindrome(first) ? "Yes" : "No");
                break;

            case 6:
                printf("Enter first word: ");
                readLine(first, sizeof(first));

                printf("Enter second word: ");
                readLine(second, sizeof(second));

                printf("Anagram: %s\n",
                       isAnagram(first, second) ? "Yes" : "No");
                break;

            case 7:
                printf("Enter word to search: ");
                readLine(search, sizeof(search));

                printf("Occurrences: %d\n",
                       searchWord(text, search));
                break;

            case 8:
                printf("Word to replace: ");
                readLine(oldWord, sizeof(oldWord));

                printf("Replace with: ");
                readLine(newWord, sizeof(newWord));

                replaceWord(text, oldWord, newWord);
                addHistory();
                break;

            case 9:
                reverseText(text);
                printf("Reversed text:\n%s\n", text);
                addHistory();
                break;

            case 10:
                reverseWordOrder(text);
                break;

            case 11:
                printf("1. Uppercase\n");
                printf("2. Lowercase\n");
                printf("3. Title Case\n");

                choice = getChoice();

                if (choice >= 1 && choice <= 3)
                    convertCase(text, choice);
                else
                    printf("Invalid choice.\n");
                break;

            case 12:
                cleanSpaces(text);
                printf("%s\n", text);
                addHistory();
                break;

            case 13:
                duplicateWords(text);
                break;

            case 14:
                writingAnalysis(text);
                break;

            case 15:
                compareTexts();
                break;

            case 16:
                cleanlinessScore(text);
                break;

            case 17:
                saveReport();
                break;

            case 18:
                loadFile();
                addHistory();
                break;

            case 19:
                showHistory();
                break;

            case 20:
                printf("\nThank you for using LexiScan!\n");
                return 0;

            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}
/*
 * main: program entry point. Shows the menu in a loop, reads the user's
 * choice and calls the matching function (statistics, searching,
 * replacing, reversing, comparing, saving/loading, history, etc.) on the
 * global text buffer. Menu option 20 exits the loop and ends the program.
 * Input: none.
 * Returns: 0 on normal exit.
 */
