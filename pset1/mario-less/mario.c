#include <cs50.h>
#include <stdio.h>

void print_row(int spaces, int bricks);

int main(void)
{
    // Prompt the user for the pyramid's height
    int n;
    do
    {
        n = get_int("Height: ");
    }
    while (n < 1 || n > 8);

    // Print a pyramid of that height
    for (int i = 0; i < n; i++)
    {
        // Calculate number of spaces and bricks for each row
        int spaces = n - 1 - i;
        int bricks = i + 1;

        // Print row of bricks
        print_row(spaces, bricks);

        // Move to the next line after printing each row
        printf("\n");
    }
}

void print_row(int spaces, int bricks)
{
    // Print spaces
    for (int j = 0; j < spaces; j++)
    {
        printf(" ");
    }

    // Print bricks
    for (int k = 0; k < bricks; k++)
    {
        printf("#");
    }
}
