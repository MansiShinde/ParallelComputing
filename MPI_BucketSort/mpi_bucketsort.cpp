#include <stdio.h>
#include "mpi.h"
#include <iostream>
#include <utility>
#include <sys/time.h>
#include <iomanip>
#include <cstdlib>
#include <bits/stdc++.h>

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

int insert_binary_search(int *arr, int size, int value) {
    int high = size - 1;
    int low = 0;
    int insert_pos = -1;

    while (high >= low) {
       int  mid=(low + high)/2;

        if (arr[mid] == value) {
            insert_pos = mid;
            break;
        } 
        if (arr[mid] < value) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    if (insert_pos == -1) {
        insert_pos=low;
    }

    while (arr[insert_pos]<=value && insert_pos < size ) {
        insert_pos++;
    }

    return insert_pos;
}


// Merge k different sorted arrays into one
int kWayMergeOld(int no_of_processors, int *receive_arr, int *final_recv, int total_values, int *receive_displacement, int rank){
    int *start = new int[no_of_processors];
    int i;
    for(i = 0; i < no_of_processors; i++){
        if(i == 0){
            start[0] = 0;
        } else {
            start[i] = receive_displacement[i -1];
        }
    }

    cout<<"process "<<rank<<" has receive buffer before merge:"<<endl;
    for(int i=0;i< total_values ;i++)
        cout<<" "<<receive_arr[i];
    cout<<"####---------"<<endl;


    cout<<"process "<<rank<<" has below start array:"<<endl;
    for(int i=0;i<no_of_processors;i++){
        cout<<" "<<start[i];
    }
     cout<<"^^^^---------"<<endl;



    int tempIndex = 0;
    int min, minProc;
    int valueLeft = 0; //true

    while(valueLeft == 0){
        min = 10000;
        minProc = -1;
        // Find the minimum value from all k arrays' start
        for(i = 1; i < no_of_processors; i++){
            // if(start[i] < receive_displacement[i]){
            //     if(receive_arr[start[i]] < min){
            //         min = receive_arr[start[i]];
            //         minProc = i;
            //     }
            // }

            if(receive_displacement[i]<total_values){
                int recv_min_val = receive_arr[receive_displacement[i-1]] <= receive_arr[receive_displacement[i]] ? receive_arr[receive_displacement[i-1]]:receive_arr[receive_displacement[i]];
                int min_p = receive_arr[receive_displacement[i-1]] < receive_arr[receive_displacement[i]] ? i-1 : i; 
                    if(recv_min_val < min){
                    min = recv_min_val;
                    minProc = min_p;
                }        
            }
        }
        // Check if no more elements
        if(minProc == -1){
            valueLeft = -1;
        } else {
            // Add to temp array with the value found
            final_recv[tempIndex] = min;
            tempIndex++;
            receive_displacement[minProc]++;
        }
    }

    cout<<"process "<<rank<<" has receive buffer after merge:"<<endl;
    for(int i=0;i< total_values ;i++)
        cout<<" "<<final_recv[i];
    cout<<"####---------"<<endl;

    free(start);
    return 0;
}

void serialBucketSort(int *arr, int num_procs, int len) {
    qsort(arr, len, sizeof(int), compare);
}




int main(int argc, char** argv){

    int rank, no_of_processors;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &no_of_processors);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int len = atoi(argv[1]);
    int *arr;
    int *arr_s; 
    int *receive_arr;
    int *receive_counts;
    int *receive_displacements;
    int *send_counts;
    int *send_displacements;
    double start_time, p_end_time, s_end_time;
    int index = 0;
    int *data_sorted;
    int start_index = 0;

    // send and receive counts and displacements 
    send_counts = new int[no_of_processors];
    send_displacements = new int[no_of_processors];
    receive_counts = new int[no_of_processors];
    receive_displacements = new int[no_of_processors];


    //numbers per processor
    int sub_array_size = len / no_of_processors; // (n/p)
    int *sub_array = new int[len]; 

    //local splitters
    int tot_local_splitters = no_of_processors - 1;
    int *local_splitters = new int[tot_local_splitters];

    //receive splitters
    int tot_receive_splitters = no_of_processors * (no_of_processors - 1);
    int *receive_splitters = new int[tot_receive_splitters];



    // generating random data in processor 0
    if(rank  == 0) {
    arr = new int[len]; 
    arr_s = new int[len]; 
    data_sorted = new int[len];

    for(int i=0;i<len;i++){
        arr[i]=rand()%len;
        arr_s[i] = arr[i];  
    }
    // cout<<"unsorted array is:"<<endl;
    // for(int i=0;i<len;i++)
    //     cout<<" "<<arr[i];
    // cout<<"---------"<<endl;
  
    start_time = MPI_Wtime(); //start timer for parallel sort 
    }

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Bcast(&len, 1, MPI_INT, 0, MPI_COMM_WORLD); // broadcast array length to all the processes


    
    //distribute the elements across p processors 
    MPI_Scatter(arr, sub_array_size, MPI_INT, sub_array, sub_array_size, MPI_INT, 0, MPI_COMM_WORLD); 

    // TODO : code to free arr  

    qsort(sub_array, sub_array_size, sizeof(int), compare); //local sort in each processor

    // // // // print locally sorted array in each processor 
    // cout<<"sub_array_size:"<<sub_array_size<<endl;
    // cout<<"process "<<rank<<" has following numbers:"<<endl;
    // for(int i=0;i< sub_array_size ;i++)
    //     cout<<" "<<sub_array[i];
    // cout<<"****---------"<<endl;


    // calculate the local splitters
    int step = sub_array_size / (tot_local_splitters + 1);
    for (int i = 0; i < tot_local_splitters; i++) {
        int index = (i+1) * step;
        local_splitters[i] = sub_array[index];
    }

// //    // print the local splitters 
//     cout<<"process "<<rank<<" has following splitters:"<<endl;
//     for(int i=0;i< tot_local_splitters ;i++)
//         cout<<" "<<local_splitters[i];
//     cout<<"####---------"<<endl;


    // all of all broadcast of local splitters to all the processors
    MPI_Allgather(local_splitters, tot_local_splitters, MPI_INT, receive_splitters,tot_local_splitters, MPI_INT, MPI_COMM_WORLD);


//    // print the receive splitters by each processor
//     cout<<"process "<<rank<<" has following receive splitters:"<<endl;
//     for(int j=0;j< tot_receive_splitters ;j++)
//         cout<<" "<<receive_splitters[j];
//     cout<<"####---------"<<endl;

    //sort the receive splitters 
    qsort(receive_splitters, tot_receive_splitters, sizeof(int), compare);


    // // // print the receive splitters after sorting
    // cout<<"process "<<rank<<" has following receive splitters after sorting:"<<endl;
    // for(int j=0;j< tot_receive_splitters ;j++)
    //     cout<<" "<<receive_splitters[j];
    // cout<<"####---------"<<endl;


    // calculate the global splitters
    step = tot_receive_splitters/ (tot_local_splitters + 1);
    for (int i = 0; i < tot_local_splitters; i++) {
        int index = (i + 1) * step;
        local_splitters[i] = receive_splitters[index];
    }

// //    // print the global splitters 
//     cout<<"process "<<rank<<" has global splitters:"<<endl;
//     for(int i=0;i< tot_local_splitters ;i++)
//         cout<<" "<<local_splitters[i];
//     cout<<"####---------"<<endl;


    // calculate the send counts and displacements
    for (int i = 0; i < tot_local_splitters; i++) {
        int end_index = insert_binary_search(sub_array, sub_array_size, local_splitters[i]);
        send_counts[i] = end_index - start_index;
        send_displacements[i] = start_index;
        start_index = end_index;
    }
    send_counts[no_of_processors - 1] = sub_array_size - start_index;
    send_displacements[no_of_processors - 1] = start_index;


    // cout<<"process "<<rank<<" has below send count and send displacement:"<<endl;
    // for(int i=0;i< no_of_processors ;i++)
    //     cout<<" S process "<<rank<<" s_c: "<<send_counts[i] <<" d_c: "<<send_displacements[i];
    // cout<<"####---------"<<endl;

    // all to all personalized comm to send send_count to respective processor
    MPI_Alltoall( send_counts, 1, MPI_INT,receive_counts,1, MPI_INT, MPI_COMM_WORLD);

    int actual_num_per_proc = 0;  
    int displaced_ind = 0;
    for(int i=0;i<no_of_processors;i++){     
        receive_displacements[i] = displaced_ind;
        displaced_ind += receive_counts[i]; 
        actual_num_per_proc += receive_counts[i];
    }

    
    // int *final_sorted_arr_per_proc = new int[actual_num_per_proc];

    // cout<<"process "<<rank<<" has receive count and displacement:"<<endl;
    // for(int i=0;i< no_of_processors ;i++)
    //     cout<<" R process "<<rank<<" s_c: "<<receive_counts[i] <<"d_c: "<<receive_displacements[i];
    // cout<<"####---------"<<endl;

    // buffer to store personalized data of each processor after all to all personalization
    receive_arr = new int[actual_num_per_proc];

    // now do the all to all personalized broadcast based on the global splitters
    MPI_Alltoallv(sub_array, send_counts, send_displacements, MPI_INT, receive_arr, receive_counts, receive_displacements, MPI_INT,
                  MPI_COMM_WORLD);

    qsort(receive_arr, actual_num_per_proc, sizeof(int), compare);

    //KWayMerge(no_of_processors, receive_counts, receive_arr,receive_displacements, actual_num_per_proc,final_sorted_arr_per_proc ) ;

    // cout<<"process "<<rank<<" has receive buffer after merge:"<<endl;
    // for(int i=0;i< actual_num_per_proc ;i++)
    //     cout<<" "<<receive_arr[i];
    // cout<<"####---------"<<endl;


    /* Number of elements in each processor */  
    MPI_Gather(&actual_num_per_proc, 1, MPI_INT, receive_counts, 1,  MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
    int index = 0; receive_displacements[0] = index;
        for (int i = 1; i < no_of_processors; i++) {
        index = index + receive_counts[i-1];
        receive_displacements[i] = index;
        }
    } 

    /* Gather all sub vectors into one large sorted vector  */
    MPI_Gatherv(receive_arr, actual_num_per_proc, MPI_INT, data_sorted, receive_counts, receive_displacements, MPI_INT, 0, MPI_COMM_WORLD);
    

    if(rank == 0){
        // cout<<"parallel Sorted array is:"<<endl;
        // for(int i=0;i<len;i++){
        //     cout<<" "<<data_sorted[i];
        // }
        // cout<<endl;

        p_end_time = MPI_Wtime() - start_time; //parallel complete
        

        start_time = MPI_Wtime();
        //serial quicksort 
        serialBucketSort(arr_s,no_of_processors,len);
        s_end_time = MPI_Wtime() - start_time; //serial complete

        // cout<<"serial Sorted array is:"<<endl;
        // for(int i=0;i<len;i++){
        //     cout<<" "<<arr_s[i];
        // }
        // cout<<endl;

        double speedup = s_end_time / p_end_time ; 
        double efficiency = speedup/no_of_processors;

        cout<<"Length: "<<len;
        cout<<" no of processors:"<<no_of_processors;
        cout<<" parallel time: "<<p_end_time;
        cout<<" serial time: "<<s_end_time;
        cout<<" Efficiency: "<<efficiency<<endl;
    }

    

MPI_Finalize();
return 0;

}