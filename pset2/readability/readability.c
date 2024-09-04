#include <cs50.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

int count_letters(string text);
int count_words(string text);
int count_sentences(string text);

int main(void)
{
    // prompt the user for some text
    string text = get_string("Text: ");

    // count the number of letters, words, and sentences in the text
    int letters = count_letters(text);
    int words = count_words(text);
    int sentences = count_sentences(text);

    /* printf("Letters: %d\n", letters);
     printf("Words: %d\n", words);
     printf("Sentences: %d\n", sentences);*/

    // compute the coleman-liau index
    float L = (float) letters / words * 100;
    float S = (float) sentences / words * 100;
    float index = 0.0588 * L - 0.296 * S - 15.8;

    /* printf("L: %.2f\n", L);
     printf("S: %.2f\n", S);
     printf("Index: %.2f\n", index);*/

    int grade = round(index);

    // print the grade level
    if (grade < 1)
    {
        printf("Before Grade 1\n");
    }
    else if (grade >= 16)
    {
        printf("Grade 16+\n");
    }
    else
    {
        printf("Grade %d\n", grade);
    }
}

int count_letters(string text)
{
    // return the number of letters in text
    int count = 0;
    for (int i = 0, n = strlen(text); i < n; i++)
    {
        if (islower(text[i]) || isupper(text[i]))
        {
            count++;
        }
    }
    return count;
}

int count_words(string text)
{
    // return the number of words in text
    int count = 1; // to count the first word
    for (int i = 0, n = strlen(text); i < n; i++)

        if (text[i] == ' ' && text[i + 1] != ' ')
        {
            count++;
        }
    return count;
}

int count_sentences(string text)
{
    // return the number of sentences in text
    int count = 0;

    for (int i = 0, n = strlen(text); i < n; i++)
    {
        if (text[i] == '.' || text[i] == '?' || text[i] == '!')
        {
            count++;
        }
    }
    return count;
}
