def main():
    # prompt the for the pyramid's height
    while True:
        try:
            n = int(input("Height: "))
            if 1 <= n <= 8:
                break
            else:
                print("Height must be between 1 and 8 inclusive.")
        except ValueError:
            print("Invalid input. Please enter a valid integer.")

    # print a pyramid of that height
    for i in range(n):
        spaces = n - 1 - i
        bricks = i + 1

        print_row(spaces, bricks)

        print()


def print_row(spaces, bricks):
    # print spaces
    for s in range(spaces):
        print(" ", end="")

    # print bricks
    for b in range(bricks):
        print("#", end="")

    # print middle spaces
    print("  ", end="")

    # print more bricks
    for b in range(bricks):
        print("#", end="")


main()
