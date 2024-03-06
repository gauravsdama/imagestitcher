#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include "functions.h"

using namespace std;

#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include "functions.h"

using namespace std;

void loadCorners(std::string filename, Corner imageCorners[MAX_CORNERS], unsigned int& numberOfCorners) {
    ifstream inputFile;
    inputFile.open(filename);
    if (!inputFile.is_open()) {
        throw runtime_error("Failed to open " + filename);
    }

    unsigned int maxCornersToLoad = numberOfCorners + MAX_CORNERS;

    while (numberOfCorners < maxCornersToLoad && inputFile >> imageCorners[numberOfCorners].x >> imageCorners[numberOfCorners].y) {
        numberOfCorners += 1;
    }

    if (numberOfCorners >= maxCornersToLoad) {
        throw std::runtime_error("Too many corners in " + filename);
    }
    inputFile.close();
}

double errorCalculation(Pixel imageBuffer1[][MAX_HEIGHT], const unsigned int width1, const unsigned int height1,
                        Corner corner1,
                        Pixel imageBuffer2[][MAX_HEIGHT], const unsigned int width2, const unsigned int height2,
                        Corner corner2) {
    double totalError = 0.0;
    double rDifference, gDifference, bDifference;

    unsigned int neighborhoodRadius = ERROR_NEIGHBORHOOD_SIZE / 2;

    unsigned int x1LowerBound = corner1.x - neighborhoodRadius;
    unsigned int x1UpperBound = corner1.x + neighborhoodRadius;
    unsigned int y1LowerBound = corner1.y - neighborhoodRadius;
    unsigned int y1UpperBound = corner1.y + neighborhoodRadius;

    unsigned int x2LowerBound = corner2.x - neighborhoodRadius;
    unsigned int x2UpperBound = corner2.x + neighborhoodRadius;
    unsigned int y2LowerBound = corner2.y - neighborhoodRadius;
    unsigned int y2UpperBound = corner2.y + neighborhoodRadius;

    unsigned int widthImage1 = static_cast<int>(width1);
    unsigned int widthImage2 = static_cast<int>(width2);
    unsigned int heightImage1 = static_cast<int>(height1);
    unsigned int heightImage2 = static_cast<int>(height2);

    if (x1LowerBound >= widthImage1 || x1UpperBound >= widthImage2 || x2LowerBound >= widthImage1 || x2UpperBound >= widthImage2 ||
        x1LowerBound < 0 || x1UpperBound < 0 || x2LowerBound < 0 || x2UpperBound < 0) {
        cout << "Invalid bounds";
        return INFINITY;
    }

    if (y1LowerBound >= heightImage1 || y1UpperBound >= heightImage2 || y2LowerBound >= heightImage1 || y2UpperBound >= heightImage2 ||
        y1LowerBound < 0 || y1UpperBound < 0 || y2LowerBound < 0 || y2UpperBound < 0) {
        cout << "Invalid bounds";
        return INFINITY;
    }

    for (unsigned int i = 0; i < ERROR_NEIGHBORHOOD_SIZE; i++) {
        for (unsigned int j = 0; j < ERROR_NEIGHBORHOOD_SIZE; j++) {
            Pixel pixel1 = imageBuffer1[x1LowerBound + i][y1LowerBound + j];
            Pixel pixel2 = imageBuffer2[x2LowerBound + i][y2LowerBound + j];

            rDifference = pixel1.r - pixel2.r;
            gDifference = pixel1.g - pixel2.g;
            bDifference = pixel1.b - pixel2.b;

            totalError += abs(rDifference) + abs(gDifference) + abs(bDifference);
        }
    }

    return totalError;
}

void matchCorners(CornerPair matchedPairs[MAX_CORNERS], unsigned int &matched_count,
                  Pixel imageBuffer1[][MAX_HEIGHT], const unsigned int width1, const unsigned int height1,
                  Corner imageCorners1[], unsigned int imageCornerCount1,
                  Pixel imageBuffer2[][MAX_HEIGHT], const unsigned int width2, const unsigned int height2,
                  Corner imageCorners2[], unsigned int imageCornerCount2) {
    // Initialize an array to keep track of whether image2 corners have been used in matching
    bool* isImage2Used = new bool[imageCornerCount2];
    for (unsigned int i = 0; i < imageCornerCount2; i++){
        isImage2Used[imageCornerCount2] = false;
    }

    // Loop through image1 corners for matching
    for (unsigned int i = 0; i < imageCornerCount1; i++) {
        Corner corner1 = imageCorners1[i];
        double error = 0;
        double minError = std::numeric_limits<double>::infinity();
        int minErrorIndex = -1;

        // Loop through image2 corners for matching with image1 corner
        for (unsigned int j = 0; j < imageCornerCount2; j++) {
            Corner corner2 = imageCorners2[j];

            // Check if the image2 corner has not been used for matching
            if (!isImage2Used[j]) {
                error = errorCalculation(imageBuffer1, width1, height1, corner1, imageBuffer2, width2, height2, corner2);

                // Check if error is not infinity
                if (error != INFINITY) {
                    if (error < minError) {
                        minError = error;
                        minErrorIndex = j;
                    }
                }
            }
        }

        // If a valid match was found, add it to matchedPairs
        if (minErrorIndex != -1) {
            matchedPairs[matched_count].image1Corner = corner1;
            matchedPairs[matched_count].image2Corner = imageCorners2[minErrorIndex];
            matchedPairs[matched_count].error = minError;
            matched_count++;
            isImage2Used[minErrorIndex] = true; // Mark the image2 corner as used
        }
    }
    delete[] isImage2Used;
}

void mapCoordinates(const double transformationMatrix[3][3], unsigned int x, unsigned int y,
                    double& mapped_x, double& mapped_y) {
    // Apply the transformation using matrix multiplication

    // Calculate intermediate values
    double a = transformationMatrix[0][0];
    double b = transformationMatrix[0][1];
    double c = transformationMatrix[0][2];
    double d = transformationMatrix[1][0];
    double e = transformationMatrix[1][1];
    double f = transformationMatrix[1][2];
    double g = transformationMatrix[2][0] * x + transformationMatrix[2][1] * y + transformationMatrix[2][2];

    if (g != 0) {
        // Calculate the new x using the formula from documentation
        mapped_x = (a * x + b * y + c) / g;

        // Calculate the new y
        mapped_y = (d * x + e * y + f) / g;
    } else {
        // Handle the case where g is zero to avoid division by zero
        mapped_x = 0.0;
        mapped_y = 0.0;
    }
}
/*
Pixel bilinearInterpolation(Pixel image[][MAX_HEIGHT], const unsigned int width, const unsigned int height, double x, double y) {
    // Implement bilinear interpolation to get pixel value at non-integer coordinates

    unsigned int x_floor = static_cast<unsigned int>(floor(x));
    unsigned int y_floor = static_cast<unsigned int>(floor(y));

    double x_frac = x - x_floor;
    double y_frac = y - y_floor;

    unsigned int x_ceil = x_floor + 1;
    unsigned int y_ceil = y_floor + 1;

    // Perform bilinear interpolation
    Pixel interpolatedPixel = {
        static_cast<unsigned char>((1.0 - x_frac) * (1.0 - y_frac) * image[x_floor][y_floor].r +
                                   x_frac * (1.0 - y_frac) * image[x_ceil][y_floor].r +
                                   (1.0 - x_frac) * y_frac * image[x_floor][y_ceil].r +
                                   x_frac * y_frac * image[x_ceil][y_ceil].r),

        static_cast<unsigned char>((1.0 - x_frac) * (1.0 - y_frac) * image[x_floor][y_floor].g +
                                   x_frac * (1.0 - y_frac) * image[x_ceil][y_floor].g +
                                   (1.0 - x_frac) * y_frac * image[x_floor][y_ceil].g +
                                   x_frac * y_frac * image[x_ceil][y_ceil].g),

        static_cast<unsigned char>((1.0 - x_frac) * (1.0 - y_frac) * image[x_floor][y_floor].b +
                                   x_frac * (1.0 - y_frac) * image[x_ceil][y_floor].b +
                                   (1.0 - x_frac) * y_frac * image[x_floor][y_ceil].b +
                                   x_frac * y_frac * image[x_ceil][y_ceil].b)
    };

    return interpolatedPixel;
}
*/
void mergeImages(Pixel imageBuffer1[][MAX_HEIGHT], unsigned int &width1, unsigned int &height1,
                  Pixel imageBuffer2[][MAX_HEIGHT], const unsigned int width2, const unsigned int height2,
                  double transformationMatrix[3][3]) {
    // Iterate over the entire buffer of image1
    for (unsigned int y = 0; y < height1; y++) {
        for (unsigned int x = 0; x < width1; x++) {
            // Apply the transformation matrix to map coordinates to image2
            double w = transformationMatrix[2][0] * x + transformationMatrix[2][1] * y + transformationMatrix[2][2];
            double newX = (transformationMatrix[0][0] * x + transformationMatrix[0][1] * y + transformationMatrix[0][2]) / w;
            double newY = (transformationMatrix[1][0] * x + transformationMatrix[1][1] * y + transformationMatrix[1][2]) / w;

            // Initialize the merged pixel with (0, 0, 0) as the default value
            Pixel mergedPixel = {0, 0, 0};

            // Check if the mapping is within the bounds of image2
            if (newX >= 0 && newX < width2 && newY >= 0 && newY < height2) {
                // Perform bilinear interpolation to get a pixel value from image2
                Pixel pixel2 = bilinear_interpolation(imageBuffer2, width2, height2, newX, newY);

                // Check if the current pixel in image1 buffer is within the bounds of image1
                if (x < width1 && y < height1) {
                    // Get the pixel at the buffer coordinate in image1
                    Pixel pixel1 = imageBuffer1[x][y];

                    // Create the new pixel value by averaging pixel1 and pixel2
                    mergedPixel = {
                        static_cast<unsigned char>((pixel1.r + pixel2.r) / 2),
                        static_cast<unsigned char>((pixel1.g + pixel2.g) / 2),
                        static_cast<unsigned char>((pixel1.b + pixel2.b) / 2)
                    };
                } else {
                    // If the current pixel in image1 is outside its bounds, use pixel2
                    mergedPixel = pixel2;
                }
            } else if (x < width1 && y < height1) {
                // If the mapping is outside the bounds of image2 but within image1, use pixel1
                mergedPixel = imageBuffer1[x][y];
            }

            // Set the pixel in image1 with the merged value
            imageBuffer1[x][y] = mergedPixel;
        }
    }

    // Update the dimensions of image1 to match the merged image dimensions (width2 and height2)
    width1 = width2;
    height1 = height2;
}

// Additional helper function for bilinear interpolation
