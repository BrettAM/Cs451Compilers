#include <iostream>
#include <vector>
#include <unistd.h>
#include "ParseDriver.hpp"
#include "Error.hpp"
#include "semantic.hpp"
#include "AST.hpp"

using namespace std;
using namespace ParseDriver;

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
            cout << "ERROR(ARGLIST): source file \""
                 << argv[0] << "\" could not be opened." << endl;
            exit(1);
        }
    }

    Result r = ParseDriver::run(
        AST::listof<Source>() << Source::IOLibrary << input);

    //syntax error
    if(r.getErrorFlag()) {
        std::vector<Error*>* errors = r.getErrors();
        for(size_t i=0; i<errors->size(); i++){
            cout << errors->at(i) << endl;
        }
        return 1;
    }

    //semantic analysis
    vector<Error*> errors = Semantics::analyze(r.getAST());
    int errorCount=0, warningCount=0;
    for(size_t i=0; i<errors.size(); i++)
        ((errors[i]->isWarning())? warningCount : errorCount) += 1;

    //untagged print
    if(printTree){
        cout << r.getAST()->formatTree(false);
    }

    //semantic errors
    for(size_t i=0; i<errors.size(); i++){
        cout << errors[i] << endl;
        delete errors[i];
    }

    //tagged print
    if(printTypedTree){
        cout << r.getAST()->formatTree(true);
    }

    cout << "Number of warnings: " << warningCount << endl;
    cout << "Number of errors: " << errorCount << endl;

    r.cleanup();
    if(input != stdin) fclose(input);
    return 0;
}
