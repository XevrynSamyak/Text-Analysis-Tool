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

/* ---------- BASIC STRING FUNCTIONS ---------- */

int isVowel(char ch)
{
    ch = (char)tolower((unsigned char)ch);

    return ch == 'a' || ch == 'e' || ch == 'i' ||
           ch == 'o' || ch == 'u';
}

int isWordChar(char ch)
{
    return isalnum((unsigned char)ch);
}

void removeNewline(char str[])
{
    str[strcspn(str, "\n")] = '\0';
}

void toLowerString(char str[])
{
    int i;

    for (i = 0; str[i] != '\0'; i++)
        str[i] = (char)tolower((unsigned char)str[i]);
}

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
