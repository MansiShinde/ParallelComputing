#include <stdio.h>
#include "mpi.h"
#include <iostream>
#include <utility>
#include <sys/time.h>
#include <iomanip>
#include <cstdlib> 


using namespace std;

int compare(const void* a, const void* b)
{
	const int* x = (int*) a;
	const int* y = (int*) b;

	if (*x > *y)
		return 1;
	else if (*x < *y)
		return -1;

	return 0;
}

void quicksort_parallel( int *arr, int len, MPI_Comm comm, int *last_len){

    int no_of_processors, rank;
    int pivot;
    int lowDatasize  = 0;
    int highDatasize = 0;

    MPI_Status status;
    MPI_Comm new_comm;
    MPI_Comm_size(comm, &no_of_processors);
    MPI_Comm_rank(comm, &rank);

    if(no_of_processors == 1){

        qsort(arr, len, sizeof(int), compare);
        *last_len = len;

        return;
    }
    
    int subgroup_size = no_of_processors / 2;

    if (rank == 0) {
        pivot = arr[len/2];
    }

    /* broadcast the pivot to all processors */
    MPI_Bcast(&pivot, 1, MPI_INT, 0, comm);

    /* partition the local data */
    for (int i = 0; i < len; i++) {
        if (arr[i] > pivot) { highDatasize++;} 
        else { lowDatasize++;}
    }
    
   int *lowData = new int[lowDatasize];
   int *highData = new int[highDatasize];
    int j = 0, k = 0;
    for (int i = 0; i < len; i++) {
        if (arr[i] > pivot) { highData[k++] = arr[i];} 
        else { lowData[j++] = arr[i]; }
    }

    int recv_datasize = 0;
    int *recvdata = NULL;
    int updatedArrSize = 0;
    int ind = 0;
    int partner_rank;
    if (rank < no_of_processors / 2) {
        partner_rank = no_of_processors / 2 + rank;
    } else {
        partner_rank = rank - no_of_processors / 2;
    }
    // Sort the two sub-arrays recursively
    if (rank < no_of_processors/2) {
       
        MPI_Send(highData,highDatasize, MPI_INT, partner_rank, 0, comm);

        MPI_Probe(partner_rank, 0, comm, &status);
        MPI_Get_count(&status, MPI_INT, &recv_datasize);
        recvdata = new int[recv_datasize];

        MPI_Recv(recvdata, recv_datasize, MPI_INT, partner_rank, 0, comm, MPI_STATUS_IGNORE);
        updatedArrSize = lowDatasize + recv_datasize ; 
        ind = 0;
        for(int i=0;i<lowDatasize;i++){
            arr[ind++] = lowData[i];
        }
        for(int i=0;i<recv_datasize;i++){
            arr[ind++] = recvdata[i];
        }

    } else {
        
        MPI_Probe(partner_rank, 0, comm, &status);
        MPI_Get_count(&status, MPI_INT, &recv_datasize);
        recvdata = new int[recv_datasize];
        MPI_Recv(recvdata,recv_datasize, MPI_INT, partner_rank, 0, comm, MPI_STATUS_IGNORE);
        updatedArrSize = highDatasize + recv_datasize ; 
        ind = 0;
        for(int i=0;i<highDatasize;i++){
            arr[ind++] = highData[i];
        }
        for(int i=0;i<recv_datasize;i++){
            arr[ind++] = recvdata[i];
        }

        MPI_Send(lowData, lowDatasize, MPI_INT, partner_rank, 0, comm);

    }

  /* Split communicator into two parts, left and right */
  int color = rank/(no_of_processors/2);
  MPI_Comm_split(comm, color, rank, &new_comm);    
  
  quicksort_parallel(arr, updatedArrSize, new_comm, last_len);

}


int main(int argc, char** argv){
    
    int rank, no_of_processors;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &no_of_processors);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int max_length = 10240000;
    int *arr;
    int *arr_s; 
    int *data_sorted;
    int *receive_counts;
    int *receive_displacements;
    int *last_len = new int[1];
    int index = 0;
    double start_time, p_end_time, s_end_time;

    

    for(int len = 1024; len<=max_length; len *=10){

     if(rank  == 0){
            arr = new int[len];
            arr_s = new int[len];
            data_sorted = new int[len];
            receive_counts = new int[no_of_processors];
            receive_displacements = new int[no_of_processors];

            for(int i=0;i<len;i++){
                arr[i]=rand()%len;
                arr_s[i] = arr[i];  
            }

        // cout<<"unsorted array is:"<<endl;
        // for(int i=0;i<len;i++)
        //     cout<<" "<<arr[i];
        // cout<<"---------"<<endl;

        start_time = MPI_Wtime();
        }

        MPI_Barrier(MPI_COMM_WORLD);

        MPI_Bcast(&len, 1, MPI_INT, 0, MPI_COMM_WORLD); // broadcast array length to all the processes

        int sub_array_size = len / no_of_processors;

        int *sub_array = new int[len];
        

        MPI_Scatter(arr, sub_array_size, MPI_INT, sub_array, sub_array_size, MPI_INT, 0, MPI_COMM_WORLD);

        // parallel quicksort 
        quicksort_parallel(sub_array, sub_array_size, MPI_COMM_WORLD, last_len);

        sub_array_size = *last_len; 

        // cout<<"sub_array_size:"<<sub_array_size<<endl;
        // cout<<"process "<<rank<<" has following numbers:"<<endl;
        // for(int i=0;i< sub_array_size ;i++)
        //     cout<<" "<<sub_array[i];
        // cout<<"****---------"<<endl;

        int receive_count = 0;

        MPI_Barrier(MPI_COMM_WORLD);

        /* Number of elements in each processor */  
        MPI_Gather(&sub_array_size, 1, MPI_INT, receive_counts, 1,  MPI_INT, 0, MPI_COMM_WORLD);

        /* Fill displacement vector for gatherv function */
        if (rank == 0) {
            int index = 0; receive_displacements[0] = index;
            for (int i = 1; i < no_of_processors; i++) {
            index = index + receive_counts[i-1];
            receive_displacements[i] = index;
            }
        } 
    
     //  /* Gather all sub vectors into one large sorted vector  */
        MPI_Gatherv(sub_array, sub_array_size, MPI_INT, data_sorted, receive_counts, receive_displacements, MPI_INT, 0, MPI_COMM_WORLD);
    
     //     MPI_Barrier(MPI_COMM_WORLD);
        if(rank == 0){
        // cout<<"Sorted array is:"<<endl;
        // for(int i=0;i<len;i++){
        //     cout<<" "<<data_sorted[i];
        // }
        // }

        p_end_time = MPI_Wtime() - start_time; //parallel complete
        

        start_time = MPI_Wtime();
        //serial quicksort 
        qsort(arr_s, len, sizeof(int), compare);

        s_end_time = MPI_Wtime() - start_time; //serial complete
        
        double speedup = s_end_time / p_end_time ; 

        cout<<"Length: "<<len;
        cout<<" parallel time: "<<p_end_time;
        cout<<" serial time: "<<s_end_time;
        cout<<" SpeedUp: "<<speedup<<endl;

     }

    }

    MPI_Finalize();
    return 0;


}
