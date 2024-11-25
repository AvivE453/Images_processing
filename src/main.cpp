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
    const float pi = 3.1415926;
    float angle;
    unsigned char pixel;
    unsigned char neighbor1, neighbor2;

    for (int i = 1; i < num_of_rows - 1; i++)
    {
        for (int j = 1; j < num_of_cols - 1; j++)
        {
            angle = angles[i * num_of_cols + j];
            pixel = img[i * num_of_cols + j];

            if (angle < 0) // angles range is [-pi,pi]
            {
                angle += pi;
            }

            if ((0 <= angle && angle <= (pi / 8)) || ((7 * pi / 8) < angle && angle <= pi)) //(i,j+1),(i,j-1) 0 <= angle < 22.5 || 157.5 < angle <= 180
            {
                neighbor1 = img[i * num_of_cols + j + 1];
                neighbor2 = img[i * num_of_cols + j - 1];
            }
            else if ((pi / 8) < angle && angle <= (3 * pi / 8)) //(i+1,j+1),(i-1,j-1) 22.5 < angle <= 67.5
            {
                neighbor1 = img[(i + 1) * num_of_cols + j + 1];
                neighbor2 = img[(i - 1) * num_of_cols + j - 1];
            }
            else if ((3 * pi / 8) < angle && angle <= (5 * pi / 8)) //(i+1,j),(i-1,j) 67.5 < angle <= 112.5
            {
                neighbor1 = img[(i + 1) * num_of_cols + j];
                neighbor2 = img[(i - 1) * num_of_cols + j];
            }
            else if ((5 * pi / 8) < angle && angle <= (7 * pi / 8)) //(i+1,j-1),(i-1,j+1) 112.5 < angle <= 157.5
            {
                neighbor1 = img[(i + 1) * num_of_cols + j - 1];
                neighbor2 = img[(i - 1) * num_of_cols + j + -1];
            }
            if (neighbor1 <= pixel && neighbor2 <= pixel)
            {
                temp[i * num_of_cols + j] = pixel; // else temp =0
            }
        }
    }
    for (int i = 0; i < num_of_rows * num_of_cols; i++)
    {
        img[i] = temp[i];
    }
}

unsigned char findArea(unsigned char value)
{
    if (value > 160)
    {
        return 255; // strong edges
    }
    else if (value > 80 && value <= 160)
    {
        return 1; // weak edges
    }
    else
    {
        return 0; // non relevant edges
    }
}

void doubleTresholding(unsigned char *img, int num_of_cols, int num_of_rows)
{
    unsigned char pixel;
    for (int i = 0; i < num_of_rows; i++)
    {
        for (int j = 0; j < num_of_cols; j++)
        {
            pixel = img[i * num_of_cols + j];
            img[i * num_of_cols + j] = findArea(pixel);
        }
    }
}

void hysteresis(unsigned char *img, int num_of_cols, int num_of_rows)
{
    unsigned char pixel;
    bool flag = false;
    vector<unsigned char> temp(num_of_cols * num_of_rows);

    for (int i = 1; i < num_of_rows - 1; i++)
    {
        for (int j = 1; j < num_of_cols - 1; j++)
        {
            pixel = img[i * num_of_cols + j];
            if (pixel == 255)
            {
                temp[i * num_of_cols + j] = 255;
            }
            else if (pixel == 1) // weak edges
            {
                for (int k = 0; k < 3 && !flag; k++)
                {
                    for (int p = 0; p < 3 && !flag; p++)
                    {

                        if (img[(i + k - 1) * num_of_cols + (j + p - 1)] == 255)
                        {
                            temp[i * num_of_cols + j] = 255;
                            flag = true;
                        }
                    }
                }
            }
            flag = false;
        }
    }

    /*
        for (int i = 0; i < num_of_cols; i++) // Top edge
        {
            if (img[i] == 255)
            {
                temp[i] = 255; // else 0
            }
        }
        for (int i = 0; i < num_of_cols; i++) // Bottom edge
        {
            if (img[(num_of_rows - 1) * num_of_cols + i] == 255)
            {
                temp[(num_of_rows - 1) * num_of_cols + i] = 255;
            }
        }
        for (int i = 0; i < num_of_rows; i++) // Left edge
        {
            if (img[i * num_of_cols] == 255)
            {
                temp[i * num_of_cols] = 255;
            }
        }
        for (int i = 0; i < num_of_rows; i++) // Right edge
        {
            if (img[i * num_of_cols + (num_of_cols - 1)] == 255)
            {
                temp[i * num_of_cols + (num_of_cols - 1)] = 255;
            }
        }
    */
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
    vector<float> angles = gradientCalculation(buffer, width, height);
    nonMaxSuppression(buffer, width, height, angles);
    doubleTresholding(buffer, width, height);
    hysteresis(buffer, width, height);
    printf("%d\n", comps);
    int result = stbi_write_png("res/textures/Canny_Lenna.png", width, height, req_comps, buffer, width * comps);
    std::cout << result << std::endl;
    stbi_image_free(buffer);
    return 0;
}