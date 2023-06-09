#include <stdio.h>
#include <sys/time.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <cstdio>
#include <iomanip>
using namespace std;

int main () {

    freopen( "output.txt", "w", stdout );
    freopen( "error.txt", "w", stderr );

    struct timeval begin, end;
    int sum = 0;
    int length = 10000000;
    int skip = 1;
    double elapsed;


    while(skip <= 64){

        int *arr = new int[skip*length];
        for(int i=0;i<skip*length;i++)
            arr[i] = 1;

        // Start measuring time
        gettimeofday(&begin, 0);
    
        for (int i=0; i< skip*length; i+=skip) {
            sum += arr[i];
        }

        // Stop measuring time and calculate the elapsed time
        gettimeofday(&end, 0);
        elapsed = (end.tv_sec - begin.tv_sec) + (end.tv_usec - begin.tv_usec) / 1000000.0;

        cout << "Skip : " << skip ;
        cout << " Time Measured: "<< elapsed << setprecision(6) << " seconds" << endl ;
        
        skip++;
        sum = 0;

        delete[] arr;
    }

    
    return 0;
}
