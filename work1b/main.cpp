#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <iostream>
#include <string.h>
#include <vector>

void blur(unsigned char *img, int num_of_cols, int num_of_rows)
{

    const float kernel[3][3] = {
        {1.0 / 16, 2.0 / 16, 1.0 / 16},
        {2.0 / 16, 4.0 / 16, 2.0 / 16},
        {1.0 / 16, 2.0 / 16, 1.0 / 16}};

    // Create a temporary array for the blurred image
    std::vector<unsigned char> temp(num_of_cols * num_of_rows);

    float new_pixel_value;
    for (int i = 1; i < num_of_rows - 1; i++) // Skip edges
    {
        for (int j = 1; j < num_of_cols - 1; j++)
        {

            new_pixel_value = (kernel[0][0] * img[(i - 1) * num_of_cols + (j - 1)]) +
                              (kernel[0][1] * img[(i - 1) * num_of_cols + j]) +
                              (kernel[0][2] * img[(i - 1) * num_of_cols + (j + 1)]) +
                              (kernel[1][0] * img[i * num_of_cols + (j - 1)]) +
                              (kernel[1][1] * img[i * num_of_cols + j]) +
                              (kernel[1][2] * img[i * num_of_cols + (j + 1)]) +
                              (kernel[2][0] * img[(i + 1) * num_of_cols + (j - 1)]) +
                              (kernel[2][1] * img[(i + 1) * num_of_cols + j]) +
                              (kernel[2][2] * img[(i + 1) * num_of_cols + (j + 1)]);

            // Assign the blurred value to the temporary image
            temp[i * num_of_cols + j] = static_cast<unsigned char>(new_pixel_value);
            new_pixel_value = 0;
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

int main(void)
{
    std::string filepath = "res/textures/GrayScale_Lenna.png";
    int width, height, comps;
    int req_comps = 1;
    unsigned char *buffer = stbi_load(filepath.c_str(), &width, &height, &comps, req_comps);
    for (int i = 1; i < 5; i++)
    {
        blur(buffer, width, height);
    }
    printf("%d\n", comps);
    int result = stbi_write_png("res/textures/Blur_Lenna.png", width, height, req_comps, buffer, width * comps);
    std::cout << result << std::endl;
    stbi_image_free(buffer);
    return 0;
}