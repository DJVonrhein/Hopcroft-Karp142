
#include <iostream>
#include <vector>

int main(){
    int l, r, m; // l = num vertices in left partition, r = num vertices in right partition, m = num undirected edges
    std::cin >> l >> r >> m;
    std::cin.ignore();
    int person1, person2;
    //take bipartite graph as adjacency lists
    std::vector<std::vector<int> > left;            //each left partition node has an adjacency list  
    for (unsigned int i = 0; i < l; ++i){
        std::vector<int> adjacencyList;
        left.push_back(adjacencyList);
    }
    std::vector<std::vector<int> > right;            //each right partition node has an adjacency list  
    for (unsigned int i = 0; i < r; ++i){
        std::vector<int> adjacencyList;
        right.push_back(adjacencyList);
    }

    for (unsigned int i = 0; i < m; ++i){
        std::cin >> person1 >> person2;
        std::cin.ignore();
        left.at(person1 - 1).push_back(person2);    //graph is unweighted and undirected
        right.at(person2 - 1).push_back(person1);
    }
    //initalize M to be an empty matching
    int rightincluded[r];
    for(unsigned i = 0; i < r; ++i){
        rightincluded[i] = 0;
    }
    std::vector<std::pair<int, int> > M;
    int leftincluded[l]; 
    for(unsigned i = 0; i < l; ++i){                    //first matching
        leftincluded[i] = 0;
        std::pair<int, int> startingEdge(i + 1, -1);
        for(unsigned j = 0; j < left.at(i).size();++j){
            if(!rightincluded[left.at(i).at(j) - 1]){
                rightincluded[left.at(i).at(j) - 1] = 1;
                startingEdge.second = left.at(i).at(j);
                break;
            }
        }
        
        M.push_back(startingEdge);
    }
    
    int max_cardinality = 0;
 
    // while(1){

    //     for(unsigned int i = 0; i < left.size(); ++i){
    //         if (!leftincluded[i]){
    //             for(unsigned int j = 0; j < left.at(i).size(); ++j){
    //                 if (!rightincluded[left.at(i).at(j)]){
    //                     std::pair <int, int> newEdge(i, left.at(i).at(j));
    //                     leftincluded[i] = 1;
    //                     rightincluded[left.at(i).at(j)] = 1;
    //                     break;
    //                 }

    //             }
    //         }
    //     }
    // }
    //
    int counter = 1;
    for (unsigned i = 0; i < M.size(); ++i){
        if (M.at(i).second != -1){
            std::cout << counter << ".   " << M.at(i).first << " ---> " << M.at(i).second << std::endl;
            counter++;
        }
    }
    return 0;
}