#include <iostream>
#include <vector>
#include <queue>
#include <utility>
#include <stdio.h>
#include <cmath>
#include <algorithm>

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

int CalculateDiameter(vector<vector<int> >& connections, int p){

    int diameter = 0;
    for (int i = 0; i < p; i++) {
    std::vector<int> distances(p, -1);
    std::queue<std::pair<int, int> > q;
    q.push({i, 0});
    distances[i] = 0;

    while (!q.empty()) {
      auto node = q.front();
      q.pop();
      int current = node.first;
      int distance = node.second;

      for (int j = 0; j < connections[current].size(); j++) {
        int neighbor = connections[current][j];
        if (distances[neighbor] == -1) {
          distances[neighbor] = distance + 1;
          q.push({neighbor, distance + 1});
        }
      }
    }

    for (int j = 0; j < p; j++) {
      diameter = std::max(diameter, distances[j]);
    }
  }

  return diameter; 
}

int main(){

    freopen( "output.txt", "w", stdout );
    freopen( "error.txt", "w", stderr );

    int noOfProcessors[] = {16,32,64,100, 128,225, 256, 512, 1024, 2048,2500};
    int proc = 0;

// Ring Network
cout<<"Ring Network:"<<endl;
    while(proc < sizeof(noOfProcessors)/sizeof(noOfProcessors[0])){
        int p = noOfProcessors[proc];
        int diameter = 0;

        std::vector<std::vector<int> > ring_connections(p);
        ring_network(ring_connections, p);
        diameter = CalculateDiameter(ring_connections, p);

        cout<<"Processors:"<<p<<" Diameter:"<<diameter<<endl;

        proc++;
    }

    proc = 0;
// Mesh Network
cout<<"Mesh Network"<<" k:4"<<endl;
    while(proc < sizeof(noOfProcessors)/sizeof(noOfProcessors[0])){
        int p = noOfProcessors[proc];
        int diameter = 0;

        int k = 4;
        std::vector<std::vector<int> > mesh_connections(p);
        mesh_network(mesh_connections, p,k);
        diameter = CalculateDiameter(mesh_connections,p);

        cout<<"Processors:"<<p<<" Diameter:"<<diameter<<endl;

        proc++;
    }

proc = 0;
cout<<"Mesh Network"<<" k:16"<<endl;
    while(proc < sizeof(noOfProcessors)/sizeof(noOfProcessors[0])){
    int p = noOfProcessors[proc];
    int diameter = 0;

    int k = 16;
    std::vector<std::vector<int> > mesh_connections(p);
    mesh_network(mesh_connections, p,k);
    diameter = CalculateDiameter(mesh_connections,p);

    cout<<"Processors:"<<p<<" Diameter:"<<diameter<<endl;

    proc++;
}

proc = 0;
// Tree Network
cout<<"Tree Network"<<" k:4"<<endl;
    while(proc < sizeof(noOfProcessors)/sizeof(noOfProcessors[0])){
        int p = noOfProcessors[proc];
        int diameter = 0;
        int k=4;
        std::vector<std::vector<int> > tree_connections(p);
        tree_network(tree_connections, p,k);
        diameter = CalculateDiameter(tree_connections,p);

        cout<<"Processors:"<<p<<" Diameter:"<<diameter<<endl;
        
        proc++;
    }

proc = 0;
cout<<"Tree Network"<<" k:16"<<endl;
    while(proc < sizeof(noOfProcessors)/sizeof(noOfProcessors[0])){
    int p = noOfProcessors[proc];
    int diameter = 0;
    int k=16;
    std::vector<std::vector<int> > tree_connections(p);
    tree_network(tree_connections, p,k);
    diameter = CalculateDiameter(tree_connections,p);

    cout<<"Processors:"<<p<<" Diameter:"<<diameter<<endl;
    
    proc++;
}
    return 0;
}