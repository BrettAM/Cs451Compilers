#include <iostream>
#include <vector>
#include <unistd.h>
#include "ParseDriver.hpp"
#include "Error.hpp"
#include "semantic.hpp"
using namespace std;

int main(int argc, char *argv[]) {
    bool printTree = false, printTypedTree = false;

    char c;
    while ((c = getopt (argc, argv, "dpP")) != -1)
        switch (c) {
            case 'd':
                ParseDriver::enableDebug();
                break;
            case 'p':
                printTree = true;
                break;
            case 'P':
                printTypedTree = true;
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
    vector<Error*> errors = Semantics::analyze(r.getAST());

    if(r.getErrorFlag()) cout << r.getError() << endl;
    for(size_t i=0; i<errors.size(); i++){
        cout << errors[i] << endl;
        delete errors[i];
    }
    if(printTree){
        cout << r.getAST()->formatTree(false);
    }
    if(printTypedTree){
        cout << r.getAST()->formatTree(true);
    }
    cout << "Number of warnings: 0" << endl;
    cout << "Number of errors: " << errors.size() << endl;

    r.cleanup();
    if(input != stdin) fclose(input);
    return 0;
}
