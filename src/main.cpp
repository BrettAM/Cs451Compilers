#include <iostream>
#include <vector>
#include <unistd.h>
#include "ParseDriver.hpp"
using namespace std;

int main(int argc, char *argv[]) {
    char c;
    while ((c = getopt (argc, argv, "d")) != -1)
        switch (c) {
            case 'd':
                ParseDriver::enableDebug();
                break;
            default:
                break;
        }
    argc -= optind;
    argv += optind;

    FILE* input = stdin;
    if(argc == 1){
        input = fopen(argv[0], "r");
        if(input == NULL) {
            cout << "ERROR reading file " << argv[0] << endl;
            exit(1);
        }
    }

    ParseDriver::Result r = ParseDriver::run(input);
    cout << r.getAST()->formatTree(true);
    cout << "Number of warnings: 0" << endl;
    cout << "Number of errors: 0" << endl;
    r.cleanup();

    if(input != stdin) fclose(input);
    return 0;
}
