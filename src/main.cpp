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

vector<float> gradientCalculation(unsigned char *img, int num_of_cols, int num_of_rows)
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
    vector<float> angles_vector(num_of_cols * num_of_rows);

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

                    int pixel = img[(i + k - 1) * num_of_cols + (j + p - 1)]; // find the pixel in the matrix
                    gradient_x += Gx[k][p] * pixel;
                    gradient_y += Gy[k][p] * pixel;
                }
            }

            int gradient = (sqrt(gradient_x * gradient_x + gradient_y * gradient_y));
            temp[i * num_of_cols + j] = min(255, gradient); // set the gradient in range 0-255

            float angle = atan2((float)gradient_y, (float)gradient_x);
            angles_vector[i * num_of_cols + j] = angle;
        }
    }

    for (int i = 0; i < num_of_rows * num_of_cols; i++)
    {
        img[i] = temp[i];
    }

    return angles_vector;
}

void nonMaxSuppression(unsigned char *img, int num_of_cols, int num_of_rows, vector<float> angles)
{
    vector<unsigned char> temp(num_of_cols * num_of_rows);

    float angle;
    int max_pixel = 0;
    for (int i = 1; i < num_of_rows - 1; i++)
    {
        for (int j = 1; j < num_of_cols - 1; j++)
        {
            angle = angles[i * num_of_cols + j];
            if (angle < 0)
            {
                angle += 3.14159;
            }

            if (0 <= angle <= 22.5) //(i,j+1),(i,j-1)
            {
                int k = i, p = j;
                while (p != num_of_cols)
                {
                    if (img[k * num_of_cols + p] >= img[k * num_of_cols + p + 1])
                    {
                        max_pixel = img[k * num_of_cols + p];
                    }
                    p++;
                }
                while (p != 0)
                {
                    if (img[k * num_of_cols + p] >= img[k * num_of_cols + p - 1])
                    {
                        max_pixel = img[k * num_of_cols + p];
                    }
                    p--;
                }
                if (max_pixel == img[i * num_of_cols + j])
                {
                    temp[i * num_of_cols + j] = max_pixel; // else temp =0
                }
            }
            else if (22.5 < angle <= 45) //(i+1,j+1),(i-1,j-1)
            {
                int k = i, p = j;
                while (p != num_of_cols && k != num_of_rows)
                {
                    if (img[k * num_of_cols + p] >= img[(k + 1) * num_of_cols + p + 1])
                    {
                        max_pixel = img[k * num_of_cols + p];
                    }
                    p++;
                    k++;
                }
                while (p != 0 && k != 0)
                {
                    if (img[k * num_of_cols + p] >= img[(k - 1) * num_of_cols + p - 1])
                    {
                        max_pixel = img[k * num_of_cols + p];
                    }
                    p--;
                    k--;
                }
                if (max_pixel == img[i * num_of_cols + j])
                {
                    temp[i * num_of_cols + j] = max_pixel; // else temp =0
                }
            }
            else if (45 < angle <= 67.5) //(i+1,j),(i-1,j)
            {                            // while ((k != 0) && (p != 0) && (k != num_of_rows) && (p != num_of_cols))
                int k = i, p = j;
                while (k != num_of_rows)
                {
                    if (img[k * num_of_cols + p] >= img[(k + 1) * num_of_cols + p])
                    {
                        max_pixel = img[k * num_of_cols + p];
                    }
                    k++;
                }
                while (k != 0)
                {
                    if (img[k * num_of_cols + p] >= img[(k - 1) * num_of_cols + p])
                    {
                        max_pixel = img[k * num_of_cols + p];
                    }
                    k--;
                }
                if (max_pixel == img[i * num_of_cols + j])
                {
                    temp[i * num_of_cols + j] = max_pixel; // else temp =0
                }
            }
        }
    }
}

int main(void)
{
    std::string filepath = "res/textures/GrayScale_Lenna.png";
    int width, height, comps;
    int req_comps = 1;
    unsigned char *buffer = stbi_load(filepath.c_str(), &width, &height, &comps, req_comps);
    blur(buffer, width, height);
    vector<float> angles = gradientCalculation(buffer, width, height);

    printf("%d\n", comps);
    int result = stbi_write_png("res/textures/Gradient_Lenna.png", width, height, req_comps, buffer, width * comps);
    std::cout << result << std::endl;
    stbi_image_free(buffer);
    return 0;
}