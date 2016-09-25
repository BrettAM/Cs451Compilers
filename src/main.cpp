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

    /*const vector<const Token*>* t = r.getTokens();
    for(vector<const Token*>::const_iterator itr = t->begin(); itr != t->end(); ++itr){
        cout << (*itr)->toString() << endl;
    }*/
    r.getAST()->formatTree(0, std::cout);

    r.cleanup();

    if(input != stdin) fclose(input);
    return 0;
}
