#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
/**
 * Common Data Types
 *
 * The data types in this section are essentially aliases for C/C++
 * primitive data types.
 *
 * Adapted from http://msdn.microsoft.com/en-us/library/cc230309.aspx.
 * See http://en.wikipedia.org/wiki/Stdint.h for more on stdint.h.
 */
typedef uint8_t  BYTE;
typedef uint32_t DWORD;
typedef int32_t  LONG;
typedef uint16_t WORD;

/**
 * BITMAPFILEHEADER
 *
 * The BITMAPFILEHEADER structure contains information about the type, size,
 * and layout of a file that contains a DIB [device-independent bitmap].
 *
 * Adapted from http://msdn.microsoft.com/en-us/library/dd183374(VS.85).aspx.
 */
typedef struct
{
    WORD   bfType;
    DWORD  bfSize;
    WORD   bfReserved1;
    WORD   bfReserved2;
    DWORD  bfOffBits;
} __attribute__((__packed__))
BITMAPFILEHEADER;

/**
 * BITMAPINFOHEADER
 *
 * The BITMAPINFOHEADER structure contains information about the
 * dimensions and color format of a DIB [device-independent bitmap].
 *
 * Adapted from http://msdn.microsoft.com/en-us/library/dd183376(VS.85).aspx.
 */
typedef struct
{
    DWORD  biSize;
    LONG   biWidth;
    LONG   biHeight;
    WORD   biPlanes;
    WORD   biBitCount;
    DWORD  biCompression;
    DWORD  biSizeImage;
    LONG   biXPelsPerMeter;
    LONG   biYPelsPerMeter;
    DWORD  biClrUsed;
    DWORD  biClrImportant;
} __attribute__((__packed__))
BITMAPINFOHEADER;

/**
 * RGBTRIPLE
 *
 * This structure describes a color consisting of relative intensities of
 * red, green, and blue.
 *
 * Adapted from http://msdn.microsoft.com/en-us/library/aa922590.aspx.
 */
typedef struct
{
    BYTE  rgbtBlue;
    BYTE  rgbtGreen;
    BYTE  rgbtRed;
} __attribute__((__packed__))
RGBTRIPLE;




// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width]);

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width]);

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width]);

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width]);




int main(int argc, char *argv[])
{
    // Define allowable filters
    char *filters = "begr";

    // Get filter flag and check validity
    char filter = getopt(argc, argv, filters);
    if (filter == '?')
    {
        printf("Invalid filter.\n");
        return 1;
    }

    // Ensure only one filter
    if (getopt(argc, argv, filters) != -1)
    {
        printf("Only one filter allowed.\n");
        return 2;
    }

    // Ensure proper usage
    if (argc != optind + 2)
    {
        printf("Usage: ./test-filter [flag] infile outfile\n");
        return 3;
    }

    // Remember filenames
    char *infile = argv[optind];
    char *outfile = argv[optind + 1];

    // Open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 4;
    }

    // Open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        printf("Could not create %s.\n", outfile);
        return 5;
    }

    // Read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // Read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // Ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        printf("Unsupported file format.\n");
        return 6;
    }

    // Get image's dimensions
    int height = abs(bi.biHeight);
    int width = bi.biWidth;

    // Allocate memory for image
    RGBTRIPLE(*image)[width] = calloc(height, width * sizeof(RGBTRIPLE));
    if (image == NULL)
    {
        printf("Not enough memory to store image.\n");
        fclose(outptr);
        fclose(inptr);
        return 7;
    }

    // Determine padding for scanlines
    int padding = (4 - (width * sizeof(RGBTRIPLE)) % 4) % 4;

    // Iterate over infile's scanlines
    for (int i = 0; i < height; i++)
    {
        // Read row into pixel array
        fread(image[i], sizeof(RGBTRIPLE), width, inptr);

        // Skip over padding
        fseek(inptr, padding, SEEK_CUR);
    }

    // Filter image
    switch (filter)
    {
        // Blur
        case 'b':
            blur(height, width, image);
            break;

        // Edges
        case 'e':
            edges(height, width, image);
            break;

        // Grayscale
        case 'g':
            grayscale(height, width, image);
            break;

        // Reflect
        case 'r':
            reflect(height, width, image);
            break;
    }

    // Write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // Write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // Write new pixels to outfile
    for (int i = 0; i < height; i++)
    {
        // Write row to outfile
        fwrite(image[i], sizeof(RGBTRIPLE), width, outptr);

        // Write padding at end of row
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, outptr);
        }
    }

    // Free memory for image
    free(image);

    // Close files
    fclose(inptr);
    fclose(outptr);
    return 0;
}

// convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    // iterate through each pixel in the image
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // calculate average of RGB values
            int avg = round(((float) image[i][j].rgbtRed + (float) image[i][j].rgbtGreen +
                             (float) image[i][j].rgbtBlue) /
                            3.00);

            // assign average value to each RGB channel
            image[i][j].rgbtRed = (avg);
            image[i][j].rgbtGreen = (avg);
            image[i][j].rgbtBlue = (avg);
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    // iterate through each row in the image
    for (int i = 0; i < height; i++)
    {
        // iterate through half of the width (only need to swap first half with second half)
        for (int j = 0; j < width / 2; j++)
        {
            // swap pixels across the vertical axis
            RGBTRIPLE temp = image[i][j];
            image[i][j] = image[i][width - 1 - j];
            image[i][width - 1 - j] = temp;
        }
    }
    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    // temporary image to store blurred pixels
    RGBTRIPLE temp[height][width];

    // iterate through each pixel in the image
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int count = 0;
            int totalRed = 0, totalGreen = 0, totalBlue = 0;

            // iterate over a 3x3 grid centered around current pixel (including current pixel)
            for (int di = -1; di <= 1; di++)
            {
                for (int dj = -1; dj <= 1; dj++)
                {
                    int ni = i + di;
                    int nj = j + dj;

                    // check if the neighboring pixel is within bounds
                    if (ni >= 0 && ni < height && nj >= 0 && nj < width)
                    {
                        totalRed += image[ni][nj].rgbtRed;
                        totalGreen += image[ni][nj].rgbtGreen;
                        totalBlue += image[ni][nj].rgbtBlue;
                        count++;
                    }
                }
            }

            // calculate average color values and store in temporary image
            temp[i][j].rgbtRed = round((float) totalRed / count);
            temp[i][j].rgbtGreen = round((float) totalGreen / count);
            temp[i][j].rgbtBlue = round((float) totalBlue / count);
        }
    }

    // copy blurred image from temporary image to original image
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j] = temp[i][j];
        }
    }
    return;
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    // sobel operators for edge detection
    int Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int Gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    // temporary image to store edge-detected pixels
    RGBTRIPLE temp[height][width];

    // iterate through each pixel in the image
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int Gx_red = 0, Gx_green = 0, Gx_blue = 0;
            int Gy_red = 0, Gy_green = 0, Gy_blue = 0;

            // iterate over a 3x3 grid centered around current pixel (including current pixel)
            for (int di = -1; di <= 1; di++)
            {
                for (int dj = -1; dj <= 1; dj++)
                {
                    int ni = i + di;
                    int nj = j + dj;

                    // check if the neighboring pixel is within bounds
                    if (ni >= 0 && ni < height && nj >= 0 && nj < width)
                    {
                        // apply sobel operator to compute Gx and Gy components for each color
                        // channel
                        Gx_red += image[ni][nj].rgbtRed * Gx[di + 1][dj + 1];
                        Gx_green += image[ni][nj].rgbtGreen * Gx[di + 1][dj + 1];
                        Gx_blue += image[ni][nj].rgbtBlue * Gx[di + 1][dj + 1];

                        Gy_red += image[ni][nj].rgbtRed * Gy[di + 1][dj + 1];
                        Gy_green += image[ni][nj].rgbtGreen * Gy[di + 1][dj + 1];
                        Gy_blue += image[ni][nj].rgbtBlue * Gy[di + 1][dj + 1];
                    }
                }
            }

            // calculate magnitude of gradient
            int magnitude_red = round(sqrt(Gx_red * Gx_red + Gy_red * Gy_red));
            int magnitude_green = round(sqrt(Gx_green * Gx_green + Gy_green * Gy_green));
            int magnitude_blue = round(sqrt(Gx_blue * Gx_blue + Gy_blue * Gy_blue));

            // ensure values are within 255
            if (magnitude_red > 255)
            {
                magnitude_red = 255;
            }
            if (magnitude_green > 255)
            {
                magnitude_green = 255;
            }
            if (magnitude_blue > 255)
            {
                magnitude_blue = 255;
            }

            // assign edge-detected pixel values to temporary image
            temp[i][j].rgbtRed = magnitude_red;
            temp[i][j].rgbtGreen = magnitude_green;
            temp[i][j].rgbtBlue = magnitude_blue;
        }
    }

    // copy edge-detected image from temporary image to original image
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j] = temp[i][j];
        }
    }
    return;
}
