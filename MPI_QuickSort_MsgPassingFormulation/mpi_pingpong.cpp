#include <mpi.h>
#include <iostream>
#include <cstring>
#include <stdio.h>
#include <sys/time.h>
#include <iomanip>

using namespace std;

int main(int argc, char** argv) {

    // Initialize the MPI environment
    MPI_Init(&argc, &argv);
    // Find out rank, size
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 2) {
        cerr << "This program requires exactly 2 processes" << endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    
    }

    std::cout << std::fixed;

    int max_length = 100000;
    int ping_pongs = 100; 
    double elapsed_time = 0; 

    int partner_rank = (rank + 1) % 2;

    for(int length = 1000; length<=max_length;length+=5000){

        char* sendbuf = new char[length];
        char* recvbuf = new char[length];

        for(int i=1; i<=ping_pongs;i++){
            
            if(rank == 0){

               // Start measuring time
               double t0 = MPI_Wtime();

                MPI_Send(sendbuf, length, MPI_CHAR, partner_rank, 0, MPI_COMM_WORLD);
                
                MPI_Recv(recvbuf, length, MPI_CHAR, partner_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
               
                // Stop measuring time and calculate the elapsed time
                double t1 = MPI_Wtime();

                elapsed_time = elapsed_time + (t1 - t0) ; 
           
            } else if(rank == 1){

                MPI_Recv(recvbuf, length, MPI_CHAR, partner_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
                MPI_Send(sendbuf, length, MPI_CHAR, partner_rank, 0, MPI_COMM_WORLD);
            }
        }

         if(rank ==0 ){
            elapsed_time = elapsed_time/ping_pongs; 
            double comm_time = elapsed_time/2; 
            cout<<"RTT for length "<<length<<": "<<elapsed_time<<setprecision(12)<<" seconds"<<endl;
            cout<<"Communication Time: "<<comm_time<<setprecision(12)<<" seconds"<<endl;
         }

        
        delete[] sendbuf;
        delete[] recvbuf;

    }


    MPI_Finalize();

    return 0;
}