#include <mpi.h>
#include <iostream>
#include <cstring>
#include <stdio.h>
#include <sys/time.h>
#include <iomanip>

using namespace std;

int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);
    // Find out rank, size
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size % 2 != 0) {
    printf("The no of processors should be multiples of 2.\n");
    MPI_Abort(MPI_COMM_WORLD, 1);
   }

    int max_length = 100000;
    int ping_pongs = 100; 
    double elapsed_time = 0; 
    double total_rtt = 0;
    int partner_rank;

    std::cout << std::fixed;

    if (rank < size / 2) {
        partner_rank = size / 2 + rank;
    } else {
        partner_rank = rank - size / 2;
    }


    for(int length = 1000; length<=max_length;length+=5000){

        char* msg = new char[length];
        elapsed_time = 0;

        for(int i=1;i<=ping_pongs;i++){
            
            if(rank < size/2){

                // Start measuring time
                double t0 = MPI_Wtime();

                MPI_Send(msg, length, MPI_CHAR, partner_rank, 0, MPI_COMM_WORLD);

                MPI_Recv(msg, length, MPI_CHAR, partner_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // Stop measuring time and calculate the elapsed time
                double t1 = MPI_Wtime();
                
                elapsed_time += (t1 - t0) ; 
           
            }   else {

                MPI_Recv(msg, length, MPI_CHAR, partner_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
                MPI_Send(msg, length, MPI_CHAR, partner_rank, 0, MPI_COMM_WORLD);
            }

        }
        if(rank < size/2){
            elapsed_time = (elapsed_time/(ping_pongs)); 
        }

        MPI_Reduce(&elapsed_time, &total_rtt, 1, MPI_DOUBLE, MPI_SUM, 0,MPI_COMM_WORLD);

        if(rank == 0){
            total_rtt = total_rtt / (size/2);
            double comm_time = total_rtt/2; 
            cout<<"RTT for length "<<length<<" is:"<<total_rtt<<setprecision(10)<<" seconds";
            cout<<" Communication Time:"<<comm_time<<setprecision(10)<<" seconds"<<endl;
        }

        delete[] msg;
    }



    MPI_Finalize();

    return 0;
}