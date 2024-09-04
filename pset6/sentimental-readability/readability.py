def main():
    # promt for text input
    text = input("Text: ")

    # count letters, words, and sentences
    num_letters = count_letters(text)
    num_words = count_words(text)
    num_sentences = count_sentences(text)

    # Calculate Coleman-Liau index
    index = coleman_liau_index(num_letters, num_words, num_sentences)

    # determine grade level
    if index >= 16:
        print("Grade 16+")
    elif index < 1:
        print("Before Grade 1")
    else:
        print(f"Grade {index}")


def count_letters(text):
    return sum(1 for char in text if char.isalpha())


def count_words(text):
    return len(text.split())


def count_sentences(text):
    return sum(1 for char in text if char in ['.', '!', '?'])


def coleman_liau_index(letters, words, sentences):
    L = (letters / words) * 100
    S = (sentences / words) * 100
    index = 0.0588 * L - 0.296 * S - 15.8
    return round(index)


main()
