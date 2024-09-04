#include <cs50.h>
#include <stdio.h>

bool validity(long long number);
string get_card_type(long long number);

int main(void)
{
    // Prompt user for credit card number
    long long card_number;
    do
    {
        card_number = get_long_long("Number: ");
    }
    while (card_number < 0);

    // Validate the card number using the luhn algorithm
    if (!validity(card_number))
    {
        printf("INVALID\n");
        return 0;
    }

    // Get the card type
    string card_type = get_card_type(card_number);

    // Print the card type
    printf("%s\n", card_type);

    return 0;
}

// Function to validate credit card number
bool validity(long long number)
{
    int sum = 0;
    bool double_digit = false;

    while (number > 0)
    {
        int digit = number % 10;

        if (double_digit)
        {
            digit *= 2;

            // If doubling results in a two digit number, sum the digits of that number
            if (digit >= 10)
            {
                sum += (digit % 10) + (digit / 10);
            }
            else
            {
                sum += digit;
            }
        }
        else
        {
            sum += digit;
        }

        // Toggle the flag for next digit
        double_digit = !double_digit;

        // Remove the last digit from the number
        number /= 10;
    }

    // Check if the sum modulo 10 equals 0
    return (sum % 10 == 0);
}

// Function to determine card type based on first digits of the card number
string get_card_type(long long number)
{
    // Check for american express
    if ((number >= 340000000000000 && number < 350000000000000) ||
        (number >= 370000000000000 && number < 380000000000000))
    {
        return "AMEX";
    }

    // Check for mastercard
    if (number >= 5100000000000000 && number < 5600000000000000)
    {
        return "MASTERCARD";
    }

    // Check for visa
    if ((number >= 4000000000000 && number < 5000000000000) ||
        (number >= 4000000000000000 && number < 5000000000000000))
    {
        return "VISA";
    }

    // Default case if invalid
    return "INVALID";
}
