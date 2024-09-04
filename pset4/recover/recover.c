#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // accept a single command-line argument
    if (argc != 2)
    {
        printf("Usage: ./recover FILE\n");
        return 1;
    }

    // open the memory card
    FILE *card = fopen(argv[1], "r");

    // buffer for a block of data
    uint8_t buffer[512];

    // counter for JPEG files found
    int jpeg_counter = 0;

    // pointer for current JPEG being written
    FILE *jpeg = NULL;

    // while there's still data left to read from the memory card
    while (fread(buffer, 1, 512, card) == 512)
    {
        // create JPEGs from the data
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff)
        {
            if (jpeg != NULL)
            {
                fclose(jpeg);
            }

            char filename[8];
            sprintf(filename, "%03i.jpg", jpeg_counter);
            jpeg = fopen(filename, "w");
            if (jpeg == NULL)
            {
                fclose(card);
                fprintf(stderr, "Could not create file %s.\n", filename);
                return 1;
            }
            jpeg_counter++;
        }
        if (jpeg != NULL)
        {
            fwrite(buffer, 1, 512, jpeg);
        }
    }

    // close any remaining files
    if (jpeg != NULL)
    {
        fclose(jpeg);
    }

    // close the memory card
    fclose(card);

    return 0;
}
