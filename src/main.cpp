#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <cmath>
using namespace std;

void blur(unsigned char *img, int num_of_cols, int num_of_rows)
{

    const float kernel[3][3] = {
        {1.0 / 16, 2.0 / 16, 1.0 / 16},
        {2.0 / 16, 4.0 / 16, 2.0 / 16},
        {1.0 / 16, 2.0 / 16, 1.0 / 16}};

    // Create a temporary array for the blurred image
    vector<unsigned char> temp(num_of_cols * num_of_rows);

    float pixel;
    for (int i = 1; i < num_of_rows - 1; i++) // Skip edges
    {
        for (int j = 1; j < num_of_cols - 1; j++)
        {

            pixel = (kernel[0][0] * img[(i - 1) * num_of_cols + (j - 1)]) +
                    (kernel[0][1] * img[(i - 1) * num_of_cols + j]) +
                    (kernel[0][2] * img[(i - 1) * num_of_cols + (j + 1)]) +
                    (kernel[1][0] * img[i * num_of_cols + (j - 1)]) +
                    (kernel[1][1] * img[i * num_of_cols + j]) +
                    (kernel[1][2] * img[i * num_of_cols + (j + 1)]) +
                    (kernel[2][0] * img[(i + 1) * num_of_cols + (j - 1)]) +
                    (kernel[2][1] * img[(i + 1) * num_of_cols + j]) +
                    (kernel[2][2] * img[(i + 1) * num_of_cols + (j + 1)]);

            // Assign the blurred value to the temporary image
            temp[i * num_of_cols + j] = (unsigned char)(pixel);
            pixel = 0;
        }
    }

    for (int i = 0; i < num_of_cols; i++) // Top edge
    {
        temp[i] = img[i];
    }
    for (int i = 0; i < num_of_cols; i++) // Bottom edge
    {
        temp[(num_of_rows - 1) * num_of_cols + i] = img[(num_of_rows - 1) * num_of_cols + i];
    }
    for (int i = 0; i < num_of_rows; i++) // Left edge
    {
        temp[i * num_of_cols] = img[i * num_of_cols];
    }
    for (int i = 0; i < num_of_rows; i++) // Right edge
    {
        temp[i * num_of_cols + (num_of_cols - 1)] = img[i * num_of_cols + (num_of_cols - 1)];
    }

    for (int i = 0; i < num_of_rows * num_of_cols; i++)
    {
        img[i] = temp[i];
    }
}

void gradientCalculation(unsigned char *img, int num_of_cols, int num_of_rows)
{

    const int Gx[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}};

    const int Gy[3][3] = {
        {-1, -2, -1},
        {0, 0, 0},
        {1, 2, 1}};

    vector<unsigned char> temp(num_of_cols * num_of_rows);

    int gradient_x, gradient_y;

    for (int i = 1; i < num_of_rows - 1; i++)
    {
        for (int j = 1; j < num_of_cols - 1; j++)
        {

            gradient_x = 0;
            gradient_y = 0;

            for (int k = 0; k < 3; k++)
            {
                for (int p = 0; p < 3; p++)
                {

                    int pixel = img[(i + k - 1) * num_of_cols + (j + p - 1)]; // find pixel in the matrix
                    gradient_x += Gx[k][p] * pixel;
                    gradient_y += Gy[k][p] * pixel;
                }
            }

            int gradient = (sqrt(gradient_x * gradient_x + gradient_y * gradient_y));
            temp[i * num_of_cols + j] = min(255, gradient); // set the gradient in range 0-255
        }
    }

    for (int i = 0; i < num_of_rows * num_of_cols; i++)
    {
        img[i] = temp[i];
    }
}

int main(void)
{
    std::string filepath = "res/textures/GrayScale_Lenna.png";
    int width, height, comps;
    int req_comps = 1;
    unsigned char *buffer = stbi_load(filepath.c_str(), &width, &height, &comps, req_comps);
    blur(buffer, width, height);
    gradientCalculation(buffer, width, height);

    printf("%d\n", comps);
    int result = stbi_write_png("res/textures/Gradient_Lenna.png", width, height, req_comps, buffer, width * comps);
    std::cout << result << std::endl;
    stbi_image_free(buffer);
    return 0;
}