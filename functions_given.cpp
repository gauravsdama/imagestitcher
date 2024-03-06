#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include "functions.h"
using namespace std;

#define N 9	 // Number of unknowns
void gaussianElimination(double mat[N][N+1]);
int forwardElim(double mat[N][N+1]);
void backSub(double mat[N][N+1]);
void swap_row(double mat[N][N+1], int i, int j);
void print(double mat[N][N+1]);

double solution[N];

void initializeImage(Pixel image[][MAX_HEIGHT]) {
    // iterate through columns
    for (unsigned int col = 0; col < MAX_WIDTH; col++) {
        // iterate through rows
        for (unsigned int row = 0; row < MAX_HEIGHT; row++) {
            // initialize pixel
            image[col][row] = {0, 0, 0};
        }
    }
}

void transformEstimation(double transformationMatrix[3][3], CornerPair pairs[MAX_CORNERS], const int matchCount){
    const int PAIRS = 4;

    if (matchCount < PAIRS){
        throw runtime_error ("not enough matched pairs\n");
    }

    // find 3 pairs with least error
    CornerPair p[PAIRS];
    int least_error_indices[PAIRS];

    for (int i = 0; i < PAIRS; i++){
        double min_error = INFINITY;
        int min_index;
        for (int j = 0; j < matchCount; j++){
            // find if index exists
            bool match_found = false;
            for (int k = 0; k < PAIRS; k++){
                if (least_error_indices[k] == j){
                    match_found = true;
                    break;
                }
            }
            if (match_found){
                continue;
            }

            if (pairs[j].error < min_error){
                // additional heuristic on distance
                double distancex = abs((double)pairs[j].image1Corner.x - pairs[j].image2Corner.x);
                double distancey = abs((double)pairs[j].image1Corner.y - pairs[j].image2Corner.y);
                if ((distancex > 100) && (distancey < 100)){
                    // cout << "matched\n";
                    min_error = pairs[j].error;
                    min_index = j;
                }
            }
        }
        least_error_indices[i] = min_index;
    }

    for (int i = 0; i < PAIRS; i++){
        p[i] = pairs[least_error_indices[i]];
        // cout << p[i].image1Corner.x << " " << p[i].image1Corner.y << endl;
        // cout << p[i].image2Corner.x << " " << p[i].image2Corner.y << endl;
    }

    double A [N][N+1] =  
    {
        {-(double)p[0].image1Corner.x, -(double)p[0].image1Corner.y, -1, 0, 0 , 0, (double)p[0].image1Corner.x * p[0].image2Corner.x, (double)p[0].image1Corner.y * p[0].image2Corner.x , (double)p[0].image2Corner.x, 0},
        {0 , 0, 0, -(double)p[0].image1Corner.x, -(double)p[0].image1Corner.y, -1, (double)p[0].image1Corner.x * p[0].image2Corner.y, (double)p[0].image1Corner.y * p[0].image2Corner.y , (double)p[0].image2Corner.y, 0},
        {-(double)p[1].image1Corner.x, -(double)p[1].image1Corner.y, -1, 0, 0 , 0, (double)p[1].image1Corner.x * p[1].image2Corner.x, (double)p[1].image1Corner.y * p[1].image2Corner.x , (double)p[1].image2Corner.x, 0},
        {0 , 0, 0, -(double)p[1].image1Corner.x, -(double)p[1].image1Corner.y, -1, (double)p[1].image1Corner.x * p[1].image2Corner.y, (double)p[1].image1Corner.y * p[1].image2Corner.y , (double)p[1].image2Corner.y, 0},
        {-(double)p[2].image1Corner.x, -(double)p[2].image1Corner.y, -1, 0, 0 , 0, (double)p[2].image1Corner.x * p[2].image2Corner.x, (double)p[2].image1Corner.y * p[2].image2Corner.x , (double)p[2].image2Corner.x, 0},
        {0 , 0, 0, -(double)p[2].image1Corner.x, -(double)p[2].image1Corner.y, -1, (double)p[2].image1Corner.x * p[2].image2Corner.y, (double)p[2].image1Corner.y * p[2].image2Corner.y , (double)p[2].image2Corner.y, 0},
        {-(double)p[3].image1Corner.x, -(double)p[3].image1Corner.y, -1, 0, 0 , 0, (double)p[3].image1Corner.x * p[3].image2Corner.x, (double)p[3].image1Corner.y * p[3].image2Corner.x , (double)p[3].image2Corner.x, 0},
        {0 , 0, 0, -(double)p[3].image1Corner.x, -(double)p[3].image1Corner.y, -1, (double)p[3].image1Corner.x * p[3].image2Corner.y, (double)p[3].image1Corner.y * p[3].image2Corner.y , (double)p[3].image2Corner.y, 0},
        {1,1,1,1,1,1,1,1,1,1}
    };

    // double A [N][N+1] =  
    // {
    //     {(double)p[0].image1Corner.x, (double)p[0].image1Corner.y, 1, 0, 0 , 0, (double)p[0].image2Corner.x},
    //     {0 , 0, 0, (double)p[0].image1Corner.x, (double)p[0].image1Corner.y, 1, (double)p[0].image2Corner.y},
    //     {(double)p[1].image1Corner.x, (double)p[1].image1Corner.y, 1, 0, 0 , 0, (double)p[1].image2Corner.x},
    //     {0 , 0, 0, (double)p[1].image1Corner.x, (double)p[1].image1Corner.y, 1, (double)p[1].image2Corner.y},
    //     {(double)p[2].image1Corner.x, (double)p[2].image1Corner.y, 1, 0, 0 , 0, (double)p[2].image2Corner.x},
    //     {0 , 0, 0, (double)p[2].image1Corner.x, (double)p[2].image1Corner.y, 1, (double)p[2].image2Corner.y}
    //     // {-(double)p[3].image1Corner.x, -(double)p[3].image1Corner.y, -1, 0, 0 , 0, (double)p[3].image2Corner.x, 0},
    //     // {0 , 0, 0, -(double)p[3].image1Corner.x, -(double)p[3].image1Corner.y, -1, (double)p[3].image2Corner.y, 0},
    // };

    // print(A);
    gaussianElimination(A);
    transformationMatrix[0][0] = solution[0];
    transformationMatrix[0][1] = solution[1];
    transformationMatrix[0][2] = solution[2];
    transformationMatrix[1][0] = solution[3];
    transformationMatrix[1][1] = solution[4];
    transformationMatrix[1][2] = solution[5];
    transformationMatrix[2][0] = solution[6];
    transformationMatrix[2][1] = solution[7];
    transformationMatrix[2][2] = solution[8];

}

// C++ program to demonstrate working of Gaussian Elimination
// method

// function to reduce matrix to r.e.f. Returns a value to 
// indicate whether matrix is singular or not

// function to calculate the values of the unknowns

// function to get matrix content
void gaussianElimination(double mat[N][N+1])
{
	/* reduction into r.e.f. */
	int singular_flag = forwardElim(mat);

	/* if matrix is singular */
	if (singular_flag != -1)
	{
        // cout << singular_flag << "\n";
		throw runtime_error("Singular Matrix. \n");

		/* if the RHS of equation corresponding to
		zero row is 0, * system has infinitely
		many solutions, else inconsistent*/
		if (mat[singular_flag][N])
			throw runtime_error("Inconsistent System.\n");
		else
			throw runtime_error("May have infinitely many solutions.\n");

		return;
	}

	/* get solution to system and print it using
	backward substitution */
	backSub(mat);
}

// function for elementary operation of swapping two rows
void swap_row(double mat[N][N+1], int i, int j)
{
	//printf("Swapped rows %d and %d\n", i, j);

	for (int k=0; k<=N; k++)
	{
		double temp = mat[i][k];
		mat[i][k] = mat[j][k];
		mat[j][k] = temp;
	}
}

// function to print matrix content at any stage
void print(double mat[N][N+1])
{
	for (int i=0; i<N; i++, printf("\n"))
		for (int j=0; j<=N; j++)
			printf("%lf ", mat[i][j]);

	printf("\n");
}

// function to reduce matrix to r.e.f.
int forwardElim(double mat[N][N+1])
{
	for (int k=0; k<N; k++)
	{
		// Initialize maximum value and index for pivot
		int i_max = k;
		int v_max = mat[i_max][k];

		/* find greater amplitude for pivot if any */
		for (int i = k+1; i < N; i++)
			if (abs(mat[i][k]) > v_max)
				v_max = mat[i][k], i_max = i;

		/* if a principal diagonal element is zero,
		* it denotes that matrix is singular, and
		* will lead to a division-by-zero later. */
		if (!mat[k][i_max])
			return k; // Matrix is singular

		/* Swap the greatest value row with current row */
		if (i_max != k)
			swap_row(mat, k, i_max);


		for (int i=k+1; i<N; i++)
		{
			/* factor f to set current row kth element to 0,
			* and subsequently remaining kth column to 0 */
			double f = mat[i][k]/mat[k][k];

			/* subtract fth multiple of corresponding kth
			row element*/
			for (int j=k+1; j<=N; j++)
				mat[i][j] -= mat[k][j]*f;

			/* filling lower triangular matrix with zeros*/
			mat[i][k] = 0;
		}

		//print(mat);	 //for matrix state
	}
	//print(mat);		 //for matrix state
	return -1;
}

// function to calculate the values of the unknowns
void backSub(double mat[N][N+1])
{
	double x[N]; // An array to store solution

	/* Start calculating from last equation up to the
	first */
	for (int i = N-1; i >= 0; i--)
	{
		/* start with the RHS of the equation */
		x[i] = mat[i][N];

		/* Initialize j to i+1 since matrix is upper
		triangular*/
		for (int j=i+1; j<N; j++)
		{
			/* subtract all the lhs values
			* except the coefficient of the variable
			* whose value is being calculated */
			x[i] -= mat[i][j]*x[j];
		}

		/* divide the RHS by the coefficient of the
		unknown being calculated */
		x[i] = x[i]/mat[i][i];
	}

	// printf("\nSolution for the system:\n");
	for (int i=0; i<N; i++){
		solution[i] = x[i];
        // printf("%lf\n", x[i]);
    }
}

// // Driver program
// int main()
// {
// 	/* input matrix */
// 	double mat[N][N+1] = {{3.0, 2.0,-4.0, 3.0},
// 						{2.0, 3.0, 3.0, 15.0},
// 						{5.0, -3, 1.0, 14.0}
// 						};

// 	gaussianElimination(mat);

// 	return 0;
// }


    // double A [N][N+1] =  
    // {
    //     {-(double)p[0].image1Corner.x, -(double)p[0].image1Corner.y, -1, 0, 0 , 0, (double)p[0].image1Corner.x * p[0].image2Corner.x, (double)p[0].image1Corner.y * p[0].image2Corner.x , (double)p[0].image2Corner.x, 0},
    //     {0 , 0, 0, -(double)p[0].image1Corner.x, -(double)p[0].image1Corner.y, -1, (double)p[0].image1Corner.x * p[0].image2Corner.y, (double)p[0].image1Corner.y * p[0].image2Corner.y , (double)p[0].image2Corner.y, 0},
    //     {-(double)p[1].image1Corner.x, -(double)p[1].image1Corner.y, -1, 0, 0 , 0, (double)p[1].image1Corner.x * p[1].image2Corner.x, (double)p[1].image1Corner.y * p[1].image2Corner.x , (double)p[1].image2Corner.x, 0},
    //     {0 , 0, 0, -(double)p[1].image1Corner.x, -(double)p[1].image1Corner.y, -1, (double)p[1].image1Corner.x * p[1].image2Corner.y, (double)p[1].image1Corner.y * p[1].image2Corner.y , (double)p[1].image2Corner.y, 0},
    //     {-(double)p[2].image1Corner.x, -(double)p[2].image1Corner.y, -1, 0, 0 , 0, (double)p[2].image1Corner.x * p[2].image2Corner.x, (double)p[2].image1Corner.y * p[2].image2Corner.x , (double)p[2].image2Corner.x, 0},
    //     {0 , 0, 0, -(double)p[2].image1Corner.x, -(double)p[2].image1Corner.y, -1, (double)p[2].image1Corner.x * p[2].image2Corner.y, (double)p[2].image1Corner.y * p[2].image2Corner.y , (double)p[2].image2Corner.y, 0},
    //     {-(double)p[3].image1Corner.x, -(double)p[3].image1Corner.y, -1, 0, 0 , 0, (double)p[3].image1Corner.x * p[3].image2Corner.x, (double)p[3].image1Corner.y * p[3].image2Corner.x , (double)p[3].image2Corner.x, 0},
    //     {0 , 0, 0, -(double)p[3].image1Corner.x, -(double)p[3].image1Corner.y, -1, (double)p[3].image1Corner.x * p[3].image2Corner.y, (double)p[3].image1Corner.y * p[3].image2Corner.y , (double)p[3].image2Corner.y, 0},
    //     {1,1,1,1,1,1,1,1,1,1}
    // };