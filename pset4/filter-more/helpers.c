#include "helpers.h"
#include "math.h"
#include "stdio.h"

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
