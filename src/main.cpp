#include <fstream>
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

string makeOutFileName(string filename){
    size_t pathSplit = filename.find_last_of('/') + 1;
    size_t typeSplit = filename.find(".c-");
    // no path, so start from the beginning
    if(pathSplit==std::string::npos) pathSplit = 0;
    // no file extension, so use the whole ending
    if(typeSplit==std::string::npos) typeSplit = filename.size();
    // Return the base file name with .tm appended
    ostringstream oss;
    oss << filename.substr(pathSplit, typeSplit-pathSplit) << ".tm";
    return oss.str();
}

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
    string filename = "a.c-";
    if(argc == 1){
        filename = argv[0];
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

    // count error/warning totals
    int errorCount=0, warningCount=0;
    for(size_t i=0; i<errors.size(); i++)
        ((errors[i]->isWarning())? warningCount : errorCount) += 1;

    // still no errors; generate code based on this file
    if(errorCount == 0){
        string outfileName = makeOutFileName(filename);
        ofstream outfile;
        outfile.open(outfileName.c_str(), ios::out);
        CodeGen::generate(r.getAST(), outfile);
        outfile.close();
    }

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

    cout << "Number of warnings: " << warningCount << endl;
    cout << "Number of errors: " << errorCount << endl;

    // syntax errors lead to malformed trees, so deleting it is perilous
    if(!r.getErrorFlag()) r.cleanup();

    for(size_t i=0; i<semErrors.size(); i++) delete semErrors[i];
    if(input != stdin) fclose(input);
    return 0;
}
