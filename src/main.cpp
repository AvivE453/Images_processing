#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

#include <iostream>
#include <string.h>

void blur(unsigned char *img, int num_of_cols, int num_of_rows)
{ // width= cols , height =rows
    char **matrix = new char *[num_of_rows];
    for (int i = 0; i < num_of_rows; i++)
    {
        matrix[i] = new char[num_of_cols];
        for (int j = 0; j < num_of_cols; j++)
        {
            matrix[i][j] = img[j + i * num_of_cols];
        }
    }

    for (int i = 0; i < num_of_rows - 2; i += 3)
    { // should we do with corners?
        for (int j = 0; j < num_of_cols - 2; j += 3)
        {
            matrix[i][j] = matrix[i][j] * (1.0 / 9);
            matrix[i][j + 1] = matrix[i][j + 1] * (1.0 / 9);
            matrix[i][j + 2] = matrix[i][j + 2] * (1.0 / 9);
            matrix[i + 1][j] = matrix[i + 1][j] * (1.0 / 9);
            matrix[i + 1][j + 1] = matrix[i + 1][j + 1] * (1.0 / 9);
            matrix[i + 1][j + 2] = matrix[i + 1][j + 2] * (1.0 / 9);
            matrix[i + 2][j] = matrix[i + 2][j] * (1.0 / 9);
            matrix[i + 2][j + 1] = matrix[i + 2][j + 1] * (1.0 / 9);
            matrix[i + 2][j + 2] = matrix[i + 2][j + 2] * (1.0 / 9);
        }
    }
    size_t k = 0;
    for (int i = 0; i < num_of_rows; i++)
    {
        for (int j = 0; j < num_of_cols; j++)
        {
            img[k] = matrix[i][j];
            k++;
        }
    }

    // delete memory
    for (int i = 0; i < num_of_rows; i++)
    {
        delete[] matrix[i];
    }
    delete[] matrix;
}

int main(void)
{
    std::string filepath = "res/textures/GrayScale_Lenna.png";
    int width, height, comps;
    int req_comps = 1;
    unsigned char *buffer = stbi_load(filepath.c_str(), &width, &height, &comps, req_comps);
    blur(buffer, width, height);
    printf("%d\n", comps);
    int result = stbi_write_png("res/textures/Blur_Lenna.png", width, height, req_comps, buffer, width * comps);
    std::cout << result << std::endl;
    return 0;
}