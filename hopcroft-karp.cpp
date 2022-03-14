
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <cstdlib>
#include <stdio.h>
#include "get_time.h"

unsigned int CILK_NWORKERS = 6;



class BipartiteG{   
    private:
        int l, r;                               //sizes of left and right paritions
        std::vector<std::vector<int> > left;    //adjacency list for left side nodes
        //std::vector<std::vector<int> > right;   //adjacency list for right side nodes
        int *leftpair, *rightpair, *dist;       //arrays used for finding max cardinality matching

    public:
        BipartiteG(int leftsize, int rightsize){    
            l = leftsize; 
            for (unsigned int i = 0; i < l; ++i){
                std::vector<int> adjacencyList;
                left.push_back(adjacencyList);
            }
            r = rightsize;
            for (unsigned int i = 0; i < r; ++i){
                std::vector<int> adjacencyList;
                //right.push_back(adjacencyList);
            }

        }
        void addNewEdge(int leftnode, int rightnode){
            left.at(leftnode - 1).push_back(rightnode);
            //right.at(rightnode - 1).push_back(leftnode);
        }

        bool bfs();
        void bfs_helper(int start_idx, int end_idx, int * vals){
            int counter = 0;
            for (int i =  start_idx; i <= end_idx; ++i){   // at top of alternating level graph, everything has distance 0
                if (leftpair[i] == 0){  
                    dist[i] = 0;
                    vals[counter] = i;                        //populate top level of alternating level tree with free vertices of left partition
                    ++counter;
                }
            else 
                dist[i] = std::numeric_limits<int>::max();      //else consider it 'infinitely far'
            }
        }
        bool dfs(int leftnode);
        int hopcroftkarp(){
            rightpair = new int[r + 1];
            for(unsigned i = 1; i <= r; ++i){
                rightpair[i] = 0;               // initially 0 (not in matching)
            }
            leftpair = new int[l + 1]; 
            for(unsigned i = 0; i <= l; ++i){                    
                leftpair[i] = 0;                // initially 0 (not in matching)
            }
            dist = new int[l + 1];
            for(unsigned i = 0; i <= l; ++i){                    
                dist[i] = std::numeric_limits<int>::max();                // initially 'infinitely far'
            }
            int max_cardinality = 0;

            while(bfs()){                       //while we can make an alternating level tree with augmenting paths
                for (int i = 1; i <= l; ++i){
                    if(!leftpair[i] && dfs(i)){     //if the vertex is free and can be augmented, do so and increment cardinality 
                        ++max_cardinality;
                    }
                }
            }
            // // this part is optional, it prints the node by node matching
            // int counter = 1;                         
            // for (unsigned i = 1; i <= l; ++i){
            //     if (leftpair[i] != 0){
            //         std::cout << counter << ".   " << i << " ---> " << leftpair[i] << std::endl;
            //         counter++;
            //     }
            // }
            // std::cout << std::endl << max_cardinality << std::endl;
            return max_cardinality;
        }
};

int main(){
    int l, r, m;        // l = num vertices in left partition, r = num vertices in right partition, m = number of undirected edges
    FILE* fp;
    fp = fopen("test.txt", "r");            //the graph is randomly generated already in test.txt  . If you haven't done so already, run bipartite_generator
    fscanf(fp, "%d %d %d", &l, &r, &m);
    int person1, person2;

    BipartiteG graph = BipartiteG(l, r);

    //take bipartite graph into adjacency lists from the edges listed in test.txt
    for (unsigned int i = 0; i < m; ++i){
        fscanf(fp, "%d %d", &person1, &person2);
        graph.addNewEdge(person1, person2);
    }
    
    //use get_time.h to measure runtime of the algorithm (excluding parse time) 
    int NUM_ROUNDS = 5;
    double tot_time = 0.0;
    for (int round = 0; round <= NUM_ROUNDS; round++) {
        timer t; t.start();
        int ans = graph.hopcroftkarp();
        t.stop();
        double tm = t.get_total();
        std::cout << "Round " << round << ", time: " << tm << std::endl;
        std::cout << "Round " << round << ", result: " << ans << std::endl;
        if (round != 0) tot_time += tm;
    }

    std::cout << "Average time: " << tot_time/NUM_ROUNDS << std::endl;

    
    return 0;
}




bool BipartiteG::bfs(){                       //construct the alternating graph that reveals any augmenting paths
    // std::cout << "enter bfs  ";
    std::queue<int> alt_level_graph;          //my alternating level graph stores the free vertices of left partition
    dist[0] = std::numeric_limits<int>::max();
    std::vector<int* > queue_vals;
    for(int i = 0; i < CILK_NWORKERS; ++i){
        int* arr = (int*)malloc(l/CILK_NWORKERS * sizeof(int));
        queue_vals.push_back(arr);
    }
    for (int i = 1; i <= l; ++i){   // at top of alternating level graph, everything has distance 0
        if (leftpair[i] == 0){  
            dist[i] = 0;
            alt_level_graph.push(i);                        //populate top level of alternating level tree with free vertices of left partition
        }
        else 
            dist[i] = std::numeric_limits<int>::max();      //else consider it 'infinitely far'
    }
    //attempt to parallelize 
    // bfs_helper(1,  l / CILK_NWORKERS, queue_vals.at(0));   // at top of alternating level graph, everything has distance 0
    // for (int i = 1; i < CILK_NWORKERS; ++i){    
    //     cilk_spawn bfs_helper(i * l / CILK_NWORKERS + 1, (i + 1) *  l / CILK_NWORKERS, queue_vals.at(i));
    // }

    // cilk_sync;
    // for(int i = 0; i < CILK_NWORKERS; ++i){
    //     for (int j = 0; j < l / CILK_NWORKERS; ++j){
    //         if(queue_vals.at(i)[j])
    //             alt_level_graph.push(queue_vals.at(i)[j]);
    //         else
    //             break;
    //     }
    // }

    while (!alt_level_graph.empty()) {   //while there are still free left vertices
        int curr = alt_level_graph.front();   
        alt_level_graph.pop(); 
        if (dist[curr] < dist[0]){ 
            for (unsigned i = 0; i < left.at(curr - 1).size(); ++i){    // Get all adj vertices
                int nextleftnode = rightpair[left.at(curr-1).at(i)];
                if (dist[nextleftnode] == std::numeric_limits<int>::max()) //reassign its distance if it was infinite
                {
                    dist[nextleftnode] = dist[curr] + 1;
                    alt_level_graph.push(nextleftnode);
                }
            }
        }
    }
 
    // this value was overwritten if a node from right partition that was unreachable is determined as reachable.
    // This indicates an augmenting path. In that case we can return the value 'true'
    return (dist[0] != std::numeric_limits<int>::max());
}

bool BipartiteG::dfs(int leftnode){         //applies augmenting paths
    // std::cout << "enter dfs  ";
    if(leftnode != 0){  //if in matching
        for (unsigned i = 0; i < left.at(leftnode - 1).size(); ++i){   //traverse adj list
            int nextleftnode = rightpair[left.at(leftnode - 1).at(i)];     //nextleftnode is a potential 
            if(dist[nextleftnode] == dist[leftnode] + 1){   
                if(dfs(nextleftnode)){      //that node has an augmenting path, so leftnode is free to be added to matching! (steals its pair)
                    rightpair[left.at(leftnode - 1).at(i)] = leftnode;
                    leftpair[leftnode] = left.at(leftnode - 1).at(i);
                    return true;
                }
            }
        }
        dist[leftnode] = std::numeric_limits<int>::max();   //no augmenting path, so it won't be added to matching
        return false;
    }
    else
        return true;

}


