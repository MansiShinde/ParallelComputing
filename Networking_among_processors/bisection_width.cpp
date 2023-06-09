#include <iostream>
#include <vector>
#include <queue>
#include <utility>
#include <stdio.h>
#include <cmath>
#include <algorithm>
#include <climits>

using namespace std; 

void ring_network(vector<vector<int> >& ring_connections, int p){
        for (int i = 0; i < p; i++) {
    int left = (i == 0) ? (p - 1) : (i - 1);
    int right = (i == (p - 1)) ? 0 : (i + 1);
    int left2 = (left == 0) ? (p - 1) : (left - 1);
    int left3 = (left2 == 0) ? (p-1) : (left2-1);
    int left4 = (left3 == 0) ? (p-1) : (left3-1);

    // adding connections to current processor's neighbors
    ring_connections[i].push_back(left);
    ring_connections[left].push_back(i);
    ring_connections[i].push_back(right);
    ring_connections[right].push_back(i);
    ring_connections[i].push_back(left2);
    ring_connections[left2].push_back(i);
    ring_connections[i].push_back(left3);
    ring_connections[left3].push_back(i);
    ring_connections[i].push_back(left4);
    ring_connections[left4].push_back(i); 
  }
}

void mesh_network(vector<vector<int> >& mesh_connections, int p, int k){
    int p_per_dim = sqrt(p); // processors per dimension

    vector<vector<int> > processors_in_grid(p_per_dim, vector<int>(p_per_dim));

        // initialize grid with processor IDs
    int processor_id = 0;
    for(int i = 0; i < p_per_dim; i++) {
        for(int j = 0; j < p_per_dim; j++) {
            processors_in_grid[i][j] = processor_id++;
        }
    }

    for(int i = 0; i < p_per_dim; i++) {
        for(int j = 0; j < p_per_dim; j++) {
            if(j-1>=0 && i>=0)
                mesh_connections[processors_in_grid[i][j]].push_back(processors_in_grid[i][j-1]);
            if(i-1>=0 && j>=0)
                mesh_connections[processors_in_grid[i][j]].push_back(processors_in_grid[i-1][j]);
            if(j+1<p_per_dim && i<p_per_dim)
                mesh_connections[processors_in_grid[i][j]].push_back(processors_in_grid[i][j+1]);
            if(i+1<p_per_dim && j<p_per_dim)
                mesh_connections[processors_in_grid[i][j]].push_back(processors_in_grid[i+1][j]);
        }
    }

        // randomly add 4 edges to all the processors 
    srand(time(0));
    for(int i=0;i<p;i++){
        
        for (int j = 0; j < k; j++) {
            int random_number = 0 + rand() % ((p-1) - 0 + 1);
            mesh_connections[i].push_back(random_number);
        }
    }

}

void tree_network(vector<vector<int> >& tree_connections, int p, int k){
    std::vector<int> storeNode ;
    storeNode.push_back(0);

    for( int level = 0; level <= log2(p); level++ ){
        int num2 = pow(2, level), size =  storeNode.size();
       
        for(int i = 0; i < size; i++ ){
            int newNum = storeNode[i]^num2;
            
            if( newNum < p){
                storeNode.push_back(newNum);
                tree_connections[storeNode[i]].push_back(newNum);
                tree_connections[newNum].push_back(storeNode[i]);
            }
        }
    }

    // randomly add 4 edges to all the processors 
    srand(time(0));
    for(int i=0;i<p;i++){
        for (int j = 0; j < k; j++) {
            int random_number = 0 + rand() % ((p-1) - 0 + 1);
            tree_connections[i].push_back(random_number);
        }
    }
}

// Function to calculate the bisection width of a network
int calculateBisectionWidth(vector<vector<int> >& graph, int p) {
  
    int bisection_width = 0;
    std::vector<int> vector_1;
    std::vector<int> vector_2;
    srand(time(0));
    for(int i=0;i<p/2;i++){
        int random_number = 0 + rand() % ((p-1) - 0 + 1);
        // If the number doesn't exist yet, add it to the vector
        if (find(vector_1.begin(), vector_1.end(), random_number) == vector_1.end())
        {
             vector_1.push_back(random_number);
        }
  }

  for(int i=0;i<p;i++){
    if (find(vector_1.begin(), vector_1.end(), i) == vector_1.end())
        {
             vector_2.push_back(i);
        }
  }

int links = 0;
for(int current_node = 0; current_node < vector_1.size();current_node++){
  for(int edge_node = 0; edge_node < graph[current_node].size(); edge_node++){
    if (find(vector_2.begin(), vector_2.end(), edge_node) != vector_2.end())
        {
             links++;
        }
  }
}

return links; 
}


int main(){

    freopen( "output.txt", "w", stdout );
    freopen( "error.txt", "w", stderr );

    int noOfProcessors[] = {16,32,64, 128, 256, 512, 1024, 2048};
    int proc = 0;

// Ring Network
cout<<"Ring Network:"<<endl;
    while(proc < sizeof(noOfProcessors)/sizeof(noOfProcessors[0])){
        int p = noOfProcessors[proc];
        int bisection_width = INT_MAX;

        std::vector<std::vector<int> > ring_connections(p);
        ring_network(ring_connections, p);

        int sampling = 1000;
        while(sampling > 0){
            int bw = calculateBisectionWidth(ring_connections, p);
            if(bw!=0){
                bisection_width = min(bw, bisection_width);
            }
            sampling--;
        }

        cout<<"Processors:"<<p<<" Bisection Width:"<<bisection_width<<endl;

        proc++;
    }

    proc = 0;
// Mesh Network
cout<<"Mesh Network"<<" k:4"<<endl;
    while(proc < sizeof(noOfProcessors)/sizeof(noOfProcessors[0])){
        int p = noOfProcessors[proc];
        int bisection_width = INT_MAX;;

        int k = 4;
        std::vector<std::vector<int> > mesh_connections(p);
        mesh_network(mesh_connections, p,k);

        int sampling = 1000;
        while(sampling > 0){
            int bw = calculateBisectionWidth(mesh_connections, p);
            if(bw!=0){
                bisection_width = min(bw, bisection_width);
            }
            sampling--;
        }

        cout<<"Processors:"<<p<<" Bisection Width:"<<bisection_width<<endl;

        proc++;
    }

proc = 0;
cout<<"Mesh Network"<<" k:16"<<endl;
    while(proc < sizeof(noOfProcessors)/sizeof(noOfProcessors[0])){
    int p = noOfProcessors[proc];
    int bisection_width = INT_MAX;;

    int k = 16;
    std::vector<std::vector<int> > mesh_connections(p);
    mesh_network(mesh_connections, p,k);

        int sampling = 1000;
        while(sampling > 0){
            int bw = calculateBisectionWidth(mesh_connections, p);
            if(bw!=0){
                bisection_width = min(bw, bisection_width);
            }
            sampling--;
        }

        cout<<"Processors:"<<p<<" Bisection Width:"<<bisection_width<<endl;

    proc++;
}

proc = 0;
// Tree Network
cout<<"Tree Network"<<" k:4"<<endl;
    while(proc < sizeof(noOfProcessors)/sizeof(noOfProcessors[0])){
        int p = noOfProcessors[proc];
        int bisection_width = INT_MAX;;
        int k=4;
        std::vector<std::vector<int> > tree_connections(p);
        tree_network(tree_connections, p,k);
        int sampling = 1000;
        while(sampling > 0){
            int bw = calculateBisectionWidth(tree_connections, p);
            if(bw!=0){
                bisection_width = min(bw, bisection_width);
            }
            sampling--;
        }

        cout<<"Processors:"<<p<<" Bisection Width:"<<bisection_width<<endl;
        
        proc++;
    }

proc = 0;
cout<<"Tree Network"<<" k:16"<<endl;
    while(proc < sizeof(noOfProcessors)/sizeof(noOfProcessors[0])){
    int p = noOfProcessors[proc];
    int bisection_width = INT_MAX;;
    int k=16;
    std::vector<std::vector<int> > tree_connections(p);
    tree_network(tree_connections, p,k);
        int sampling = 1000;
        while(sampling > 0){
            int bw = calculateBisectionWidth(tree_connections, p);
            if(bw!=0){
                bisection_width = min(bw, bisection_width);
            }
            sampling--;
        }

        cout<<"Processors:"<<p<<" Bisection Width:"<<bisection_width<<endl;
    
    proc++;
}
    return 0;
}