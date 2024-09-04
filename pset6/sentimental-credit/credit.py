import re
import cs50


def main():
 # prompt for credit card number
    while True:
        try:
            card_number = cs50.get_string("Number: ")
            if re.match(r"^\d+$", card_number):  # Check if input contains only digits
                card_number = int(card_number)
                if card_number > 0:
                    break
                else:
                    print("Please enter a positive number.")
            else:
                print("Invalid input. Please enter digits only.")
        except ValueError:
            print("Invalid input. Please enter a valid number.")

    # validate the card number using the Luhn algorithm
    if not validity(card_number):
        print("INVALID")
        return

    # get card type
    card_type = get_card_type(card_number)

    # print card type as required
    print(card_type)


def validity(number):

    # validate credit card numbers using Luhn's algorithm
    num_str = str(number)
    sum = 0
    length = len(num_str)
    is_second_digit = False

    for i in range(length - 1, -1, -1):
        digit = int(num_str[i])

        if is_second_digit:
            digit *= 2

            if digit > 9:
                digit = digit % 10 + digit // 10

        sum += digit
        is_second_digit = not is_second_digit

    return sum % 10 == 0


def get_card_type(number):
    # determine card type based on number patterns
    num_str = str(number)
    length = len(num_str)

    # check for American Express(AMEX)
    if re.match(r"^(34|37)", num_str) and length == 15:
        return "AMEX"

    # chexk for Mastercard
    if re.match(r"^(51|52|53|54|55)", num_str) and length == 16:
        return "MASTERCARD"

    # chexk for Visa
    if re.match(r"^4", num_str) and (length == 13 or length == 16):
        return "VISA"

    return "INVALID"


main()
