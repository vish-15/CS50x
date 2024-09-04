// Implements a dictionary's functionality

#include <ctype.h>
#include <stdbool.h>

#include "dictionary.h"

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
} node;

// TODO: Choose number of buckets in hash table
const unsigned int N = 65536;

// Hash table
node *table[N];

// Loads dictionary into memory, returning true if successful, else false
bool load(const char *dictionary)
{
    // TODO
    // open the dictionary file
    FILE *source = fopen(dictionary, "r");
    if (source == NULL)
    {
        return false;
    }

    for (int i = 0; i < N; i++)
    {
        table[i] = NULL;
    }

    char word[LENGTH + 1];

    // read each word in the file
    while (fscanf(source, "%s", word) != EOF)
    {
        // allocate memory for new node
        node *new_node = malloc(sizeof(node));
        if (new_node == NULL)
        {
            fclose(source);
            return false;
        }
        else
        {
            // copy word into node
            strcpy(new_node->word, word);
            new_node->next = NULL;

            // hash word to obtain hash value
            unsigned int index = hash(new_node->word);

            // insert into list
            new_node->next = table[index];
            table[index] = new_node;
        }
    }

    // close the dictionary file
    fclose(source);
    return true;
}

// Hashes word to a number
unsigned int hash(const char *word)
{
    // TODO: Improve this hash function
    unsigned int hash_value = 0;

    // sum ASCII values of characters in word
    for (int i = 0; word[i] != '\0'; i++)
    {
        hash_value += word[i];
    }

    // reduce hash value to fit within hash table size
    return hash_value % N;
}

// Returns number of words in dictionary if loaded, else 0 if not yet loaded
unsigned int size(void)
{
    // TODO
    unsigned int count = 0;

    // iterate through each bucket in hash table
    for (int i = 0; i < N; i++)
    {
        // count words
        node *cursor = table[i];
        while (cursor != NULL)
        {
            count++;
            cursor = cursor->next;
        }
    }

    return count;
}

// Returns true if word is in dictionary, else false
bool check(const char *word)
{
    // TODO

    // hash word to obtain hash value
    unsigned int index = hash(word);

    // check for word
    node *cursor = table[index];
    while (cursor != NULL)
    {
        if (strcmp(word, cursor->word) == 0)
        {
            return true; // word found
        }
        cursor = cursor->next;
    }
    return false; // word not found
}

// Unloads dictionary from memory, returning true if successful, else false
bool unload(void)
{
    // TODO
    // iterate through each bucket in hash table
    for (int i = 0; i < N; i++)
    {
        // free nodes
        node *cursor = table[i];
        while (cursor != NULL)
        {
            node *temp = cursor;
            cursor = cursor->next;
            free(temp);
        }
        free(cursor);
    }
    return true;
}
