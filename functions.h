#ifndef FUNCTIONS_H
#define FUNCTIONS_H

// struct for Pixel (short is a 2 byte signed integer)
struct Pixel {
  short r; // red
  short g; // green
  short b; // blue
};

struct Corner {
  unsigned int x; // x-coordinate of the corner
  unsigned int y; // y-coordinate of the corner
};

struct CornerPair {
  struct Corner image1Corner; // corner from fist image
  struct Corner image2Corner; // corner from second image
  double error;               // error estimate for the pair
};

// maximum image dimensions (do not change these constants. If you do, you will not pass the tests)
const unsigned int MAX_WIDTH = 1280;
const unsigned int MAX_HEIGHT = 600;
const unsigned int MAX_CORNERS = 100;
const unsigned int ERROR_NEIGHBORHOOD_SIZE = 21;

// already implemented
void initializeImage(Pixel image[][MAX_HEIGHT]);
void transformEstimation(double transformationMatrix[3][3], CornerPair pairs[MAX_CORNERS], const int matchCount);

// functions carried over from last week
void loadImage(std::string filename, Pixel image[][MAX_HEIGHT], unsigned int& width, unsigned int& height);
void outputImage(std::string filename, Pixel image[][MAX_HEIGHT], unsigned int width, unsigned int height);
Pixel bilinear_interpolation(Pixel image[][MAX_HEIGHT], unsigned int width, unsigned int height, double x, double y);

// functions to be implemented
void loadCorners(std::string filename, Corner imageCorners[MAX_CORNERS], unsigned int& numberOfCorners);

double errorCalculation(Pixel image1[][MAX_HEIGHT], const unsigned int width1, const unsigned int height1,
                      Corner image1corner,
                      Pixel image2[][MAX_HEIGHT], const unsigned int width2, const unsigned int height2,
                      Corner image2corner);

void matchCorners(CornerPair matchedPairs[MAX_CORNERS], unsigned int &matched_count,
                  Pixel image1[][MAX_HEIGHT], const unsigned int width1, const unsigned int height1,
                  Corner image1corners[], unsigned int image1CornerCount,
                  Pixel image2[][MAX_HEIGHT], const unsigned int width2, const unsigned int height2,
                  Corner image2corners[], unsigned int image2CornerCount);

void mapCoordinates(const double transformationMatrix[3][3], unsigned int x, unsigned int y,
                    double& mapped_x, double& mapped_y);

void mergeImages( Pixel image1[][MAX_HEIGHT], unsigned int &width1, unsigned int &height1,
                  Pixel image2[][MAX_HEIGHT], const unsigned int width2, const unsigned int height2,
                  double H[3][3] );

#endif