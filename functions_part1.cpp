#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include "functions.h"

using namespace std;

// Copy code from last week for local run. Gradescope will use it's own copy

void loadImage(string filename, Pixel image[][MAX_HEIGHT], unsigned int& width, unsigned int& height) {
  // TODO: implement image loading
  // Reading file
  ifstream ifs(filename); // Open the input file stream

    if (!ifs.is_open()) {
        throw runtime_error("Error: Unable to open file - " + filename); // Throw an error if the file can't be opened
    }

    string type;
    ifs >> type;

    // Ensure the image type is valid (P3)
    if (type != "P3" && type != "p3") {
        throw runtime_error("Error: Invalid image type - " + type); // Throw an error for an invalid image type
    }

    ifs >> width >> height; // Read and store image width and height

    // Check if the dimensions are within valid range
    if (!ifs || width <= 0 || height <= 0 || width > MAX_WIDTH || height > MAX_HEIGHT) {
        throw runtime_error("Error: Invalid dimensions"); // Throw an error for invalid dimensions
    }

    unsigned int maxColor;
    ifs >> maxColor;

    // Ensure the color value is valid (should be 255)
    if (!ifs || maxColor != 255) {
        throw runtime_error("Error: Invalid color value"); // Throw an error for an invalid color value
    }

    // Loop through the image pixels, read, and store the RGB values
    for (unsigned int j = 0; j < height; j++) {
        for (unsigned int i = 0; i < width; i++) {
            unsigned int r, g, b;
            ifs >> r >> g >> b;

            // Check if the color values are within the valid range (0-255)
            if (!ifs || r > 255 || g > 255 || b > 255) {
                throw runtime_error("Error: Invalid color value"); // Throw an error for invalid color values
            }

            // Store the RGB values in the current pixel
            Pixel& currentPixel = image[i][j];
            currentPixel.r = static_cast<int>(r);
            currentPixel.g = static_cast<int>(g);
            currentPixel.b = static_cast<int>(b);
        }
    }

    unsigned int extra;
    if (ifs >> extra) {
        throw runtime_error("Error: Too many values"); // Throw an error if there are extra values in the file
    }
}


void outputImage(string filename, Pixel image[][MAX_HEIGHT], unsigned int width, unsigned int height) {
  // TODO: implement writing image to file
  std::ofstream ofs(filename);

    // Check if the file was successfully opened
    if (!ofs.is_open()) {
        throw std::invalid_argument("Failed to open " + filename); // Throw an error if the file can't be opened
    }

    ofs << "P3" << endl; // Write the image type
    ofs << width << " " << height << endl; // Write the image dimensions
    ofs << 255 << endl; // Maximum color value

    for (unsigned int row = 0; row < height; row++) {
        for (unsigned int column = 0; column < width; column++) {
            int redVal = image[column][row].r;
            int greenVal = image[column][row].g;
            int blueVal = image[column][row].b;

            ofs << redVal << " " << greenVal << " " << blueVal;

            if (column < width - 1) {
                ofs << " ";
            } else {
                ofs << std::endl; // Start a new line after each row
            }
        }
    }

    ofs.close(); // Close the output file stream

}


Pixel bilinear_interpolation(Pixel image[][MAX_HEIGHT], unsigned int width, unsigned int height, double x, double y) {
  unsigned int x1 = static_cast<unsigned int>(x);
    unsigned int y1 = static_cast<unsigned int>(y);

    // Ensure x2 and y2 don't go beyond the image boundaries
    unsigned int x2 = (x1 + 1 >= width) ? width - 1 : x1 + 1;
    unsigned int y2 = (y1 + 1 >= height) ? height - 1 : y1 + 1;

    // Retrieve the four surrounding pixels
    Pixel p1 = image[x1][y1];
    Pixel p2 = image[x2][y1];
    Pixel p3 = image[x1][y2];
    Pixel p4 = image[x2][y2];

    // Calculate the fractional part of coordinates
    double deltaX = x - static_cast<double>(x1);
    double deltaY = y - static_cast<double>(y1);

    // Perform bilinear interpolation for each color channel
    double upred = p1.r * (1.0 - deltaX) + p2.r * deltaX;
    double upgreen = p1.g * (1.0 - deltaX) + p2.g * deltaX;
    double upblue = p1.b * (1.0 - deltaX) + p2.b * deltaX;

    double lowred = p3.r * (1.0 - deltaX) + p4.r * deltaX;
    double lowgreen = p3.g * (1.0 - deltaX) + p4.g * deltaX;
    double lowblue = p3.b * (1.0 - deltaX) + p4.b * deltaX;

    // Perform linear interpolation along the Y-axis
    double intred = upred * (1.0 - deltaY) + lowred * deltaY;
    double intgreen = upgreen * (1.0 - deltaY) + lowgreen * deltaY;
    double intblue = upblue * (1.0 - deltaY) + lowblue * deltaY;

    // Round and store the interpolated values in a Pixel
    Pixel result;
    result.r = round(intred);
    result.g = round(intgreen);
    result.b = round(intblue);

    return result;
}

