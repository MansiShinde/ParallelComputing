#include <stdio.h>
#include <vector>
#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <iomanip>

using namespace std;

int main () {
    freopen( "output.txt", "w", stdout );
    freopen( "error.txt", "w", stderr );

    struct timeval begin, end;
    int sum = 0;
    int length = 1000;
    double elapsed = 0;

    while(length <= 10000000){

        int *arr = new int[length];
        for(int i=0;i<length;i++)
            arr[i] = 1;

        // Start measuring time
        gettimeofday(&begin, 0);
        
        for (int i=0; i<length; i++) {
            sum += arr[i];
        }
        
        // Stop measuring time and calculate the elapsed time
        gettimeofday(&end, 0);
        elapsed = (end.tv_sec - begin.tv_sec) + (end.tv_usec - begin.tv_usec)/1000000.0;
        cout << "Length: " << length ;
        cout << " Time Measured: "<< elapsed << std::setprecision(6) << " seconds" << endl ; 

        length = length * 10;
        sum = 0;

        delete [] arr;
    }
    
    return 0;
}