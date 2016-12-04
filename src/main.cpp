#include <iostream>
#include <vector>
#include <unistd.h>
#include "ParseDriver.hpp"
#include "Error.hpp"
#include "semantic.hpp"
#include "AST.hpp"
#include "CodeGen.hpp"

using namespace std;
using namespace ParseDriver;

int main(int argc, char *argv[]) {
    bool printTree = false, printTypedTree = false, printTokens = false;

    char c;
    while ((c = getopt (argc, argv, "dpPT")) != -1)
        switch (c) {
            case 'd':
                ParseDriver::enableDebug();
                break;
            case 'p':
                printTree = true;
                break;
            case 'P':
                printTypedTree = true;
                break;
            case 'T':
                printTokens = true;
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
            cout << "ERROR(ARGLIST): source file \""
                 << argv[0] << "\" could not be opened." << endl;
            exit(1);
        }
    }

    // Run flex and bison
    Result r = ParseDriver::run(
        AST::listof<Source>() << Source::IOLibrary << input);

    vector<Error*> semErrors;
    AST::listof<Error*> errorList;
    errorList.addAll(r.getErrors());

    // if there are no syntax errors, run semantic analysis
    if(!r.getErrorFlag()) {
        semErrors = Semantics::analyze(r.getAST());
        errorList.addAll(&semErrors);
    }
    vector<Error*> errors = errorList;

    // Token print
    if(printTokens){
        const std::vector<const Token*>* tokens = r.getTokens();
        for(size_t i=0; i<tokens->size(); i++){
            cout << tokens->at(i) << endl;
        }
    }

    // untagged print
    if(!r.getErrorFlag() && printTree){
        cout << r.getAST()->formatTree(false);
    }

    // semantic errors
    for(size_t i=0; i<errors.size(); i++){
        cout << errors[i] << endl;
    }

    // tagged print
    if(!r.getErrorFlag() && printTypedTree){
        cout << r.getAST()->formatTree(true);
    }

    // count error/warning totals
    int errorCount=0, warningCount=0;
    for(size_t i=0; i<errors.size(); i++)
        ((errors[i]->isWarning())? warningCount : errorCount) += 1;

    cout << "Number of warnings: " << warningCount << endl;
    cout << "Number of errors: " << errorCount << endl;

    // syntax errors lead to malformed trees, so deleting it is perilous
    if(!r.getErrorFlag()) r.cleanup();

    for(size_t i=0; i<semErrors.size(); i++) delete semErrors[i];
    if(input != stdin) fclose(input);
    return 0;
}
