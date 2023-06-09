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

       //randomly add 4 edges to all the processors 
    srand(time(0));
    for(int i=0;i<p;i++){
        
        for (int j = 0; j < k; j++) {
            int random_number = 0 + rand() % ((p-1) -  0 + 1);
            mesh_connections[i].push_back(random_number);
        }
    }
}

int DilationBFS(vector<vector<int> >& graph, int start, int end) {
    int n = graph.size();
    vector<int> distance(n, -1);
    queue<int> q;
    int cnt_set = 0;
    
    distance[start] = 0;
    q.push(start);

    while (!q.empty()) {
        int node = q.front();
        q.pop();

        for (int neighbor : graph[node]) {

            if (distance[neighbor] == -1) {
                distance[neighbor] = distance[node] + 1;
                if(neighbor == end)
                  return distance[end];

                q.push(neighbor);
            }
        }
    }

    return distance[end];
}

vector<int> findPath(vector<vector<int> >& graph, int start, int end){

// cout<<"start:"<<start<<" end:"<<end<<endl;
  // find path using BFS 
  int n = graph.size();
  vector<int> parent(n*n, -1);
  vector<int> isVisited(n*n, 0);
  queue<int> q;
  isVisited[start] = 1;
  parent[start] = -1;

  q.push(start);

  while(!q.empty()){
    int u = q.front();
    q.pop();
    // cout<<u<<endl; 

    if(u == end){
      vector<int> path; 
      int curr = u;
      while (curr != -1) {
        path.push_back(curr);
        curr = parent[curr];
      }

      reverse(path.begin(), path.end());

      return path;
    }

    for (int neighbor : graph[u]) {
        // cout<<"NeighBour: "<<neighbor<<endl; 
        int v = neighbor;
        if(isVisited[v] == 0){
          // cout<<"is Visited: "<<isVisited[v]<<endl; 
          isVisited[v] = 1;
          parent[v] = u;
          q.push(v);
        }
    }

  }
  return vector<int>();
}

int main() {

    freopen( "output.txt", "w", stdout );
    freopen( "error.txt", "w", stderr );

    int noOfProcessors[] = {16,32,64, 128, 256,512,1024};
    int proc = 0;

cout<<"Mapping of Ring to Mesh with k=4:"<<endl;
    while(proc < sizeof(noOfProcessors)/sizeof(noOfProcessors[0])){
      int p = noOfProcessors[proc];

      std::vector<std::vector<int> > ring_connections(p);
      ring_network(ring_connections, p);

      int k = 4; 
      std::vector<std::vector<int> > mesh_connections(p);
      mesh_network(mesh_connections, p,k);

      std::vector<std::vector<int> > each_edge_cnt(p, std::vector<int>(p));

      int dilation = 0;

      for(int node = 0; node<ring_connections.size();node++){
      for(int edge_node = 0; edge_node<ring_connections[node].size();edge_node++){    
       int edges = DilationBFS(mesh_connections, node, ring_connections[node][edge_node]);
       dilation = std::max(dilation, edges);  
    }
  }

  for(int node = 0; node<ring_connections.size();node++){
    for(int edge_node = 0; edge_node<ring_connections[node].size();edge_node++){ 
     vector<int> path = findPath(mesh_connections,node, ring_connections[node][edge_node]);
      for(int i=0;i<path.size()-1;i++){
      int node1 = path[i];
      int node2 = path[i+1];

      each_edge_cnt[node1][node2] += 1;
      each_edge_cnt[node2][node1] += 1;
      
  }
}
}
  int congestion = 0;
    for (int i = 0; i < p; i++) {
    for (int j = 0; j < p; j++) {
      congestion = std::max(congestion,each_edge_cnt[i][j]);
    } 
  }

  cout<<"No of Processors:"<<p<<endl;
  cout<<"Dilation is :"<<dilation<< endl;
  cout<<"Congestion is: "<<congestion/2<<endl;

      proc++;
  }

proc=0;
  cout<<"Mapping of Ring to Mesh with k=16:"<<endl;
    while(proc < sizeof(noOfProcessors)/sizeof(noOfProcessors[0])){
      int p = noOfProcessors[proc];

      std::vector<std::vector<int> > ring_connections(p);
      ring_network(ring_connections, p);

      int k = 4; 
      std::vector<std::vector<int> > mesh_connections(p);
      mesh_network(mesh_connections, p,k);

      std::vector<std::vector<int> > each_edge_cnt(p, std::vector<int>(p));

      int dilation = 0;

      for(int node = 0; node<ring_connections.size();node++){
      for(int edge_node = 0; edge_node<ring_connections[node].size();edge_node++){    
       int edges = DilationBFS(mesh_connections, node, ring_connections[node][edge_node]);
       dilation = std::max(dilation, edges);  
    }
  }

  for(int node = 0; node<ring_connections.size();node++){
    for(int edge_node = 0; edge_node<ring_connections[node].size();edge_node++){ 
     vector<int> path = findPath(mesh_connections,node, ring_connections[node][edge_node]);
      for(int i=0;i<path.size()-1;i++){
      int node1 = path[i];
      int node2 = path[i+1];

      each_edge_cnt[node1][node2] += 1;
      each_edge_cnt[node2][node1] += 1;
      
  }
}
}
  int congestion = 0;
    for (int i = 0; i < p; i++) {
    for (int j = 0; j < p; j++) {
      congestion = std::max(congestion,each_edge_cnt[i][j]);
    } 
  }

  cout<<"No of Processors:"<<p<<endl;
  cout<<"Dilation is :"<<dilation<< endl;
  cout<<"Congestion is: "<<congestion/2<<endl;

      proc++;
  }

  return 0;
}



