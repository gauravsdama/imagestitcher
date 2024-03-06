#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include "functions.h"

using std::cout, std::endl, std::cin, std::string, std::stringstream;

// small program to stitch images together

int main() {
  // 2D array of pixels
  static Pixel image1[MAX_WIDTH][MAX_HEIGHT];
  static Pixel image2[MAX_WIDTH][MAX_HEIGHT];
  
  // initialize 2D array
  initializeImage(image1);
  initializeImage(image2);

  // declare variables
  string image1File;
  string image2File;
  string image1CornersFile;
  string image2CornersFile;
  unsigned int image1Width = 0;
  unsigned int image1Height = 0;
  unsigned int image2Width = 0;
  unsigned int image2Height = 0;
  Corner image1corners[MAX_CORNERS];
  unsigned int image1CornersCount;
  Corner image2corners[MAX_CORNERS];
  unsigned int image2CornersCount;
  
  // accept filename
  cout << "Input image1 filename: ";
  cin >> image1File;

  try {
    // attempt to load image
    loadImage(image1File, image1, image1Width, image1Height);
  } catch (std::exception &ex) {
    cout << "Exception thrown: " << ex.what() << endl;
    return 1; // exit with error
  }

  cout << "Input image1 corners filename: ";
  cin >> image1CornersFile;

  try {
    // attempt to load image
    loadCorners(image1CornersFile, image1corners, image1CornersCount);
  } catch (std::exception &ex) {
    cout << "Exception thrown: " << ex.what() << endl;
    return 1; // exit with error
  }


  // accept filename
  cout << "Input image2 filename: ";
  cin >> image2File;

  try {
    // attempt to load image
    loadImage(image2File, image2, image2Width, image2Height);
  } catch (std::exception &ex) {
    cout << "Exception thrown: " << ex.what() << endl;
    return 1; // exit with error
  }

  cout << "Input image2 corners filename: ";
  cin >> image2CornersFile;

  try {
    // attempt to load image
    loadCorners(image2CornersFile, image2corners, image2CornersCount);
  } catch (std::exception &ex) {
    cout << "Exception thrown: " << ex.what() << endl;
    return 1; // exit with error
  }

  cout << "Start image stitching..." << endl;

  CornerPair matchedCorners[MAX_CORNERS];
  unsigned int matched_count = 0;

  matchCorners(matchedCorners, matched_count,
              image1, image1Width , image1Height, image1corners, image1CornersCount,
              image2, image2Width , image2Height, image2corners, image2CornersCount);

  // cout << matched_count << endl;

  double transformationMatrix[3][3];
  transformEstimation(transformationMatrix, matchedCorners, matched_count);

  mergeImages(image1, image1Width, image1Height,
                   image2, image2Width , image2Height,
                   transformationMatrix);

  outputImage("merged.ppm", image1, image1Width, image1Height);

  return 0; // exit without errors
}

