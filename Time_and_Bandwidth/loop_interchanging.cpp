#include <stdio.h>
#include <sys/time.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <cstdio>
#include <iomanip>
using namespace std;

double MatrixVecMultiply(int *b, int **matrix, int n){
    struct timeval begin, end;
    int *result = new int[n];
    for(int i=0;i<n;i++)
        result[i] = 0;

    // Start measuring time
    gettimeofday(&begin, 0);

    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++) {
            result[i] += matrix[i][j]*b[j];
        }
    }

    gettimeofday(&end, 0);
  
    delete[] result;

    return (end.tv_sec - begin.tv_sec ) + (end.tv_usec - begin.tv_usec)/1000000.0;
}

double LoopInterchangeMatrixVecMultiply(int *b, int **matrix, int n){
    struct timeval begin, end;
    int *result = new int[n];
    for(int i=0;i<n;i++)
        result[i] = 0;

    // Start measuring time
    gettimeofday(&begin, 0);

    for (int j = 0; j < n; j++){
        for (int i = 0; i < n; i++) {
            result[i] += matrix[i][j]*b[j];
        }
    }

    // Stop measuring time
    gettimeofday(&end, 0);

    delete[] result;

    return (end.tv_sec - begin.tv_sec ) + (end.tv_usec - begin.tv_usec)/1000000.0;
}


int main () {

    freopen( "output.txt", "w", stdout );
    freopen( "error.txt", "w", stderr );

    int length[] = {1000, 10000,100000};
    double elapsed;
    int len= 0 ;

    while(len < sizeof(length)/sizeof(length[0])){

        int n = length[len];

        int *b = new int[n];
        for(int i=0;i<n;i++)
            b[i] = 1;

        int** matrix = new int*[n]; 
        for(int i = 0; i < n; ++i) {
            matrix[i] = new int[n]; 
            for(int j = 0; j < n; ++j)
                matrix[i][j] = 1;
        }

        elapsed =  MatrixVecMultiply(b, matrix, n);
        cout << "Matrix Vector Multiply, length :" << n;
        cout << " Time Measured: "<< elapsed << setprecision(6) << " seconds" << endl ;

        elapsed = LoopInterchangeMatrixVecMultiply(b, matrix, n);
        cout << "Matrix Vector Multiply InterChange, length :" << n;
        cout << " Time Measured: "<< elapsed << setprecision(6) << " seconds" << endl ;
        
        
        for(int i = 0; i < n; ++i)
            delete matrix[i];
        delete[] matrix;

        len++;
    }

    return 0;
}
