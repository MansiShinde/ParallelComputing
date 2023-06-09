#include <stdio.h>
#include <sys/time.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <cstdio>
#include <iomanip>
using namespace std;

void unrollOne(int *arr, int length){
   
    struct timeval begin, end;
    int temp1 =0, temp2 =0 ;

    // Start measuring time
    gettimeofday(&begin, 0);

    for (int i=0; i<length; i=i+2) {
        temp1 += arr[i];
        temp2 += arr[i+1];
    }
    int temp = temp1 + temp2;
    
    // Stop measuring time and calculate the elapsed time
    gettimeofday(&end, 0);
    double elapsed = (end.tv_sec - begin.tv_sec ) + (end.tv_usec - begin.tv_usec)/1000000.0;

    cout << "Unroll count: 1";
    cout << " Time Measured: "<< elapsed << setprecision(6) << " seconds" << endl ;
}


void unrollTwo(int *arr, int length){
   
    struct timeval begin, end;
    int temp1=0, temp2=0, temp3 = 0;

    // Start measuring time
    gettimeofday(&begin, 0);

    for (int i=0; i<length; i=i+3) {
        temp1  += arr[i];
        temp2  += arr[i+1];
        temp3  += arr[i+2];
    }
    int temp = temp1 + temp2 + temp3 ;
    
    // Stop measuring time and calculate the elapsed time
    gettimeofday(&end, 0);
    double elapsed = (end.tv_sec - begin.tv_sec ) + (end.tv_usec - begin.tv_usec)/1000000.0;
    
    cout << "Unroll count: 2";
    cout << " Time Measured: "<< elapsed << setprecision(6) << " seconds" << endl ;
}

void unrollThree(int *arr, int length){
   
    struct timeval begin, end;

    // Start measuring time
    gettimeofday(&begin, 0);

    int temp1=0, temp2=0, temp3 = 0, temp4=0;
    for (int i=0; i<length; i=i+4) {
        temp1 += arr[i] ;
        temp2 += arr[i+1] ;
        temp3 += arr[i+2] ;
        temp4 += arr[i+3] ;
    }
    int temp = temp1 + temp2 + temp3 + temp4 ; 
    
    // Stop measuring time and calculate the elapsed time
    gettimeofday(&end, 0);
    double elapsed = (end.tv_sec - begin.tv_sec ) + (end.tv_usec - begin.tv_usec)/1000000.0;
    
    cout << "Unroll count: 3";
    cout << " Time Measured: "<< elapsed << setprecision(6) << " seconds" << endl ;
}


void unrollFour(int *arr, int length){
   
    struct timeval begin, end;
    int temp1=0, temp2=0, temp3 = 0, temp4=0, temp5=0;

    // Start measuring time
    gettimeofday(&begin, 0);

    for (int i=0; i<length; i=i+5) {
        temp1 += arr[i] ;
        temp2 += arr[i+1] ;
        temp3 += arr[i+2] ;
        temp4 += arr[i+3] ;
        temp5 += arr[i+4] ;
    }
    int temp = temp1 + temp2 + temp3 + temp4 +temp5; 
    
    // Stop measuring time and calculate the elapsed time
    gettimeofday(&end, 0);
    double elapsed = (end.tv_sec - begin.tv_sec ) + (end.tv_usec - begin.tv_usec)/1000000.0;
    
    cout << "Unroll count: 4";
    cout << " Time Measured: "<< elapsed << setprecision(6) << " seconds" << endl ;
}


void unrollFive(int *arr, int length){
   
    struct timeval begin, end;
    int temp1=0, temp2=0, temp3 = 0, temp4=0, temp5=0, temp6 = 0;

    // Start measuring time
    gettimeofday(&begin, 0);

    for (int i=0; i<length; i=i+6) {
        temp1 += arr[i] ;
        temp2 += arr[i+1] ;
        temp3 += arr[i+2] ;
        temp4 += arr[i+3] ;
        temp5 += arr[i+4] ;
        temp6 += arr[i+5] ;
    }
    int temp = temp1 + temp2 + temp3 + temp4 +temp5 +temp6; 
    
    // Stop measuring time and calculate the elapsed time
    gettimeofday(&end, 0);
    double elapsed = (end.tv_sec - begin.tv_sec ) + (end.tv_usec - begin.tv_usec)/1000000.0;
    
    cout << "Unroll count: 5";
    cout << " Time Measured: "<< elapsed << setprecision(6) << " seconds" << endl ;
}


void unrollSix(int *arr, int length){
   
    struct timeval begin, end;
    int temp1=0, temp2=0, temp3 = 0, temp4=0, temp5=0, temp6 = 0, temp7 = 0;

    // Start measuring time
    gettimeofday(&begin, 0);

    for (int i=0; i<length; i=i+7) {
        temp1 += arr[i];
        temp2 += arr[i+1];
        temp3 += arr[i+2];
        temp4 += arr[i+3];
        temp5 += arr[i+4];
        temp6 += arr[i+5];
        temp7 += arr[i+6];
    }
    int temp = temp1 + temp2 + temp3 + temp4 + temp5 + temp6 + temp7; 
    
    
    // Stop measuring time and calculate the elapsed time
    gettimeofday(&end, 0);
    double elapsed = (end.tv_sec - begin.tv_sec ) + (end.tv_usec - begin.tv_usec)/1000000.0;

    cout << "Unroll count: 6";
    cout << " Time Measured: "<< elapsed << setprecision(6) << " seconds" << endl ;
}

int main () {

    freopen( "output.txt", "w", stdout );
    freopen( "error.txt", "w", stderr );

    int length = 10000000;

    int *arr = new int[length];
    for(int i=0;i<length;i++)
        arr[i] = 1;

    // Loop unroll - 1
    unrollOne(arr, length);
    
    // Loop unroll - 2
    unrollTwo(arr, length);

    // Loop unroll - 3
    unrollThree(arr, length);

    // Loop unroll - 4
    unrollFour(arr, length);

    // Loop unroll - 5
    unrollFive(arr, length);

    // Loop unroll - 6
    unrollSix(arr, length);

    delete[] arr;

    return 0;
}
