#include <iostream>
#include <algorithm>
#include <fstream>

int main(){
    int l, r, m;
    l = 1000000; r = 1000000; m = 3000000;
    srand(5);
    std::ofstream str;
    str.open("test.txt");
    int firstNode, secondNode, fee;
    str << l << " " << r << " " << m << std::endl;
    for (int i = 0; i < m; ++i){
        firstNode = (rand() % l ) + 1;
        secondNode = (rand() % r ) + 1;
        
        str << firstNode  << " " << secondNode << std::endl;
    }
    
    str.close();

    return 0;
   
}