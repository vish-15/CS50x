#include <cs50.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

bool only_digits(string s);
char rotate(char c, int key);

int main(int argc, string argv[])
{
    // program runs with only one command-line argument
    if (argc != 2)
    {
        printf("Usage: ./caesar key\n");
        return 1;
    }

    // ckexk if every character in argv[1] is a digit
    string key_string = argv[1];
    if (!only_digits(key_string))
    {
        printf("Usage: ./caesar key\n");
        return 1;
    }

    // convert argv[1] from a `string` to an `int`
    int n = atoi(key_string);

    // prompt for plaintext
    string plaintext = get_string("plaintext: ");

    // encrypt the plaintext using the key and print ciphertext
    printf("ciphertext: ");
    for (int i = 0; plaintext[i] != '\0'; i++)
    {
        char encrypted_char = rotate(plaintext[i], n);
        printf("%c", encrypted_char);
    }
    printf("\n");

    // if exactly one argument is provided containing only digits, continue with the program
    return 0;
}

bool only_digits(string s)
{
    for (int i = 0; s[i] != '\0'; i++)
    {
        if (!isdigit(s[i]))
        {
            return false;
        }
    }
    return true;
}

char rotate(char c, int n)
{
    if (isalpha(c))
    {
        char base_char = isupper(c) ? 'A' : 'a';
        return (c - base_char + n) % 26 + base_char;
    }
    else
    {
        return c;
    }
}
