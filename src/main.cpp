#include <iostream>
#include <vector>
#include "ParseDriver.hpp"
using namespace std;

int main(int argc, char const *argv[]) {
    ParseDriver::Result r = ParseDriver::run("TokenTokenToken");
    for(vector<Token*>::iterator itr = r.begin(); itr != r.end(); ++itr){
        cout << (*itr)->toString() << endl;
    }
    return 0;
}
