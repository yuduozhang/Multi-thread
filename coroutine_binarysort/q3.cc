#include "q3binsertsort.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;


void uMain::main(void) {

    if (argc == 1 || argc > 3){
        cerr << "Usage: " << argv[0] << " unsorted-ﬁle [ sorted-ﬁle ]\n";
        exit( EXIT_FAILURE );
    } // if

    else { 
        ifstream inputFile;   
        ofstream outputFile;

        // Stream buf used for redirection
        streambuf* outputBuf;
        streambuf* coutBuf;

        string input = argv[1];

        // Handle uFile access exception
        try{
            inputFile.open(input.c_str());
        } catch ( uFile::FileAccess::OpenFailure ){
            cerr << "Error: unable to open " << input << endl; 
            exit( EXIT_FAILURE );
        } // catch

        if (argc == 3){                         // Specified output file
            string output = argv[2];

            // Handle uFile access exception
            try{
                outputFile.open(output.c_str());
            } catch ( uFile::FileAccess::OpenFailure ){
                cerr << "Error: unable to open " << output << endl; 
                exit( EXIT_FAILURE );
            } // catch

            // To avoid duplicated code for outputing to a
            // file, redirect cout stream buffer to output file after
            // backing up cout's streambuf 
            coutBuf = cout.rdbuf();             // back up cout's streambuf
            outputBuf = outputFile.rdbuf();     // get outputFile's streambuf
            cout.rdbuf(outputBuf);              // assign outpufFile's streambuf to cout
        } // if

        vector < vector <TYPE> > unsortedLists; 

        for ( ;; ){
            int numOfValues;
            inputFile >> numOfValues;
            if (inputFile.fail()) break;

            vector<TYPE> List;
            for (int i = 0; i < numOfValues; i++){
                TYPE value;

                inputFile >> value;
                List.push_back(value);
            } // for
            unsortedLists.push_back(List);
        } // for

        for(unsigned int i = 0; i < unsortedLists.size(); i++){
            vector<TYPE> temp = unsortedLists[i];
            Binsertsort<TYPE> root(SENTINEL);  // construct the root coroutine 

            for (unsigned int j = 0; j < temp.size() ; j++){
                root.sort( temp[j] );
            } 

            // print the original unsorted list
            for (unsigned int j = 0; j< temp.size(); j++){
                cout << temp[j] << " "; 
            } // for
            cout << endl;

            root.sort(SENTINEL);                // signal the root coroutine end of unsorted values

            // print the sorted list
            for(;;){
                TYPE i = root.retrieve();
                if ( i == SENTINEL) break;      // signal of end of sorted values
                cout << i << " ";
            } // for 

            cout << endl << endl;               // output an extra empty line
        } // for

        if (argc == 3){
            cout.rdbuf(coutBuf);                // restore cout's original streambuf, otherwise there will be runtime error
        } // if
        outputFile.close();
    } // else
} // main
