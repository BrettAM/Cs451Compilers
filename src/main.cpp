#include <iostream>
#include <vector>
#include "ParseDriver.hpp"
using namespace std;

int main(int argc, char const *argv[]) {
    FILE* input = stdin;
    if(argc == 2){
        input = fopen(argv[1], "r");
        if(input == NULL) {
            cout << "ERROR reading file " << argv[1] << endl;
            exit(1);
        }
    }

    ParseDriver::Result r = ParseDriver::run(input);
    for(vector<Token*>::iterator itr = r.begin(); itr != r.end(); ++itr){
        cout << (*itr)->toString() << endl;
    }

    for(vector<Token*>::iterator itr = r.begin(); itr!= r.end(); ++itr){
        delete (*itr);
    }

    if(input != stdin) fclose(input);
    return 0;
}
