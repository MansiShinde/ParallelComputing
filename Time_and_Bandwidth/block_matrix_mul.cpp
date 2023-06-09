#include <stdio.h>
#include <sys/time.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <cstdio>
#include <iomanip>

using namespace std;

double matrixMultiplication(int **a, int **b, long n){

    int** result = new int*[n];
    for(int i = 0; i < n; ++i){
        result[i] = new int[n];
        for(int j = 0; j < n; ++j)
            result[i][j] = 0;
    }

    struct timeval begin, end;

    // Start measuring time
    gettimeofday(&begin, 0);

    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            for(int k=0;k<n;k++){
                result[i][j] += a[i][k]*b[k][j];
            }
        }
    }

    // Stop measuring time
    gettimeofday(&end, 0);

    for(int i = 0; i < n; ++i)
        delete result[i];
    delete[] result;

    return (end.tv_sec - begin.tv_sec ) + (end.tv_usec - begin.tv_usec)/1000000.0;
}

double blockMatrixMultiplication(int **a, int **b, long n, int block){

    int** result = new int*[n];
    for(int i = 0; i < n; ++i)
        result[i] = new int[n]; // build rows
    
    for(int i = 0; i < n; ++i)
        for(int j = 0; j < n; ++j)
            result[i][j] = 0;
    
    struct timeval begin, end;

    // Start measuring time
    gettimeofday(&begin, 0);

    for(int ii = 0; ii<n; ii = ii + block){
        for(int jj = 0;jj<n; jj = jj + block){
            for(int kk = 0;kk<n; kk = kk + block){

                for(int i=ii; i<ii+block; i++){
                    for(int j=jj; j<jj+block; j++){
                        for(int k=kk; k<kk+block; k++){
                            result[i][j] +=  a[i][k] * b[k][j]; 
                        }
                    }
                }
            }
        }
    }

    // stop measuring time 
    gettimeofday(&end, 0);

    for(int i = 0; i < n; ++i)
        delete result[i];
    delete[] result;

    return (end.tv_sec - begin.tv_sec ) + (end.tv_usec - begin.tv_usec)/1000000.0;
}


int main () {

    freopen( "output.txt", "w", stdout );
    freopen( "error.txt", "w", stderr );

    int length[] = {100,500,1000,1500,2000,2500};
    int len= 0 ;
    double elapsed =0;


    while(len < sizeof(length)/sizeof(length[0])){

        long n = length[len];
        
        int** a = new int*[n];
        for(int i = 0; i < n; ++i){
            a[i] = new int[n]; // build rows
            for(int j = 0; j < n; ++j)
                a[i][j] = 1;
        }
        
        int** b = new int*[n];
        for(int i = 0; i < n; ++i){
            b[i] = new int[n]; // build rows
            for(int j = 0; j < n; ++j)
                b[i][j] = 1;
        }

        cout << "Length :" << n  ;
        
        elapsed  = matrixMultiplication(a,b,n);
        double computationRate = 2*n*n*n/elapsed;
        cout << " Matrix Mul Computation Rate: "<< computationRate << setprecision(10)  ;

        cout << " block size:" << 25 ;
        elapsed = blockMatrixMultiplication(a, b, n, 25);
        computationRate = 2*n*n*n/elapsed;
        cout << " Block Matrix Mul Computation Rate: "<< computationRate << setprecision(10) <<endl ;



        for(int i = 0; i < n; ++i){
             delete a[i];
             delete b[i];
        }
        delete[] a;
        delete[] b;

        len++;
    }

    return 0;
}



