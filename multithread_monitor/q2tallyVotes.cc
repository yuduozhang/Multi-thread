#include "q2tallyVotes.h"
#include<map>
#include<vector>
#include<iostream>
#include<stdlib.h>

using namespace std;

//======================================================================
//  TallyVotes Class
//======================================================================

//----------------------------------------------------------------------
// uCondLock Version
#if defined( IMPLTYPE_MC )

//----------------------------------------------------------------------
// Constructor
//
TallyVotes::TallyVotes( unsigned int group, Printer &printer ) :
                    signalFlag(0),
                    waitingCounter(0),
                    votingResult(true),
                    group(group), 
                    printer(printer){}

//----------------------------------------------------------------------
// vote()
//
bool TallyVotes::vote( unsigned int id, bool ballot ){

    // Acquire owner lock for mutual exclusion 
    mlk.acquire();          

    // Barging task waits if signalFlag is 1
    if (signalFlag) {       
        bargingTask.wait(mlk);
    }

    // Print Vote state
    printer.print(id, Voter::Vote, ballot);

    // Vote happens only if not enough voters can form a group
    assert(votingBox.size() < group);

    // Put my voting result into the votingBox
    votingBox[id] = ballot;

    // Wait if not enough voters to form a group 
    if (votingBox.size() < group){

        if(!bargingTask.empty()){
            bargingTask.signal();
        }
        else{ 
            signalFlag = 0;
        }

        waitingCounter++;

        // Print Block state
        printer.print(id, Voter::Block, waitingCounter);

        // Wait for enough voters to form a group
        voters.wait(mlk); 

        waitingCounter--;

        // Print Unblock state
        printer.print(id, Voter::Unblock, waitingCounter);

        if (bargingTask.empty()){
            signalFlag = 0;
        }
    }

    // Compute voting result if enough voters to form a group
    if (votingBox.size() == group){
        unsigned int countTrue = 0;
        map<unsigned int, bool>::iterator iter;      

        for (iter = votingBox.begin(); iter != votingBox.end(); iter++) {
            if (iter->second){
                countTrue++;
            }
        }

        if (countTrue > group/2){
            votingResult = true;
        }

        else {
            votingResult = false;
        }

        // Clear votingBox for next group
        votingBox.clear();

        // Print Complete state
        printer.print(id, Voter::Complete);
    }

    // Signal waiting voters to start a tour
    if(!voters.empty()){
        signalFlag = 1;
        voters.signal();
    }

    else if (!bargingTask.empty()){
        bargingTask.signal();
    }

    bool myVotingResult = votingResult;

    // Print Finished state
    printer.print(id, Voter::Finished, myVotingResult);

    // Release mutual exclusion lock
    mlk.release();

    return myVotingResult;
}
#endif // IMPLTYPE_MC


//----------------------------------------------------------------------
// uSemaphore Version

#if defined( IMPLTYPE_SEM )

//----------------------------------------------------------------------
// Constructor
//
TallyVotes::TallyVotes( unsigned int group, Printer &printer ) :
                    mSmp(1),
                    oneGroup(group),
                    waitingForGroup(0),
                    votingResult(true),
                    group(group), 
                    printer(printer){}
                    
//----------------------------------------------------------------------
// vote()
//
bool TallyVotes::vote( unsigned int id, bool ballot ){

    // For controlling group size
    oneGroup.P();               

    // For mutual exclusion
    mSmp.P();                   

    // Print Vote state
    printer.print(id, Voter::Vote, ballot);

    // Vote happens only if not enough voters can form a group
    assert(votingBox.size() < group);

    // Put my voting result into the votingBox
    votingBox[id] = ballot;

    // Wait if not enough voters to form a group 
    if (abs(waitingForGroup.counter()) < group-1 ){

        // Print Block State 
        printer.print( id, Voter::Block, 
                        (unsigned int)(abs(waitingForGroup.counter())+1) );

        // Unblock mutual exclusion semaphore before waiting
        mSmp.V();

        // Wait for enough voters to form a group
        waitingForGroup.P(); 

        // Require mutual exclusion semaphore again to enter critical
        // section 
        mSmp.P();

        // Print Unblock state
        printer.print(id, Voter::Unblock, 
                        (unsigned int) (abs(waitingForGroup.counter())) );
    }

    // Compute voting result if enough voters to form a group
    if (votingBox.size() == group){
        int countTrue = 0;
        map<unsigned int, bool>::iterator iter;      

        for (iter = votingBox.begin(); iter != votingBox.end(); iter++) {
            if (iter->second){
                countTrue++;
            }
        }

        if (countTrue > group/2){
            votingResult = true;
        }

        else {
            votingResult = false;
        }

        // Clear votingBox for next group
        votingBox.clear();

        // Print Complete state
        printer.print(id, Voter::Complete);
    }

    // Signal waiting voters to start a tour
    if (!waitingForGroup.empty()){
        waitingForGroup.V();
    }

    // Let the next group to vote if no voters are waiting for this group
    else{
        oneGroup.V(group);
    }

    bool myVotingResult = votingResult;

    // Print Finished state
    printer.print(id, Voter::Finished, myVotingResult);

    // Release mutual exclusion semaphore 
    mSmp.V();

    return myVotingResult;
}

#endif // IMPLTYPE_SEM


//======================================================================
//  Voter Task
//======================================================================

//----------------------------------------------------------------------
// Constructor
//
Voter::Voter( unsigned int id, 
        TallyVotes &voteTallier, 
        Printer &printer ) : 
    id(id), 
    voteTallier(voteTallier),
    printer(printer){}

//----------------------------------------------------------------------
// main()
//
void Voter::main(){
    printer.print(id, Voter::Start); 
    unsigned int yieldNum = prng(0,19);         
    yield(yieldNum);
    int ballot = prng(0,1); 
    voteTallier.vote(id, ballot);
}

//======================================================================
// Printer Monitor
//======================================================================

//----------------------------------------------------------------------
// Constructor
//
Printer::Printer( unsigned int voters ){
    numVoters = voters;

    for (unsigned int i = 0; i < voters; i++){
        VoterInfo temp;
        temp.id = i;
        temp.empty = 1;
        buffer[i] = temp;
    }

    for (unsigned int i = 0; i < voters; i++){
        VoterInfo temp = buffer[i];
        cout << "Voter" << temp.id << "\t";
    }

    cout << endl;

    for (unsigned int i = 0; i < voters; i++){
        cout << "=======\t";
    }

    cout << endl;
}

//----------------------------------------------------------------------
// Destructor
//
Printer::~Printer(){
    cout << "=================\n" << "All tours started" << endl;
}

//----------------------------------------------------------------------
// Flush for unfinished voter tasks
//
void Printer::flush(){
    for (unsigned int i = 0; i < numVoters; i++){
        VoterInfo temp = buffer[i];

        // If empty VoterInfo, output empty column
        if (temp.empty){
            cout << "\t";
        }

        // If not empty
        else if (temp.state == Voter::Start){
            cout << temp.state << "\t";
        } 

        else if (temp.state == Voter::Vote){
            cout << temp.state << " " << temp.vote << "\t";
        }

        else if (temp.state == Voter::Block){
            cout << temp.state << " " << temp.numBlocked << "\t";
        }

        else if (temp.state == Voter::Unblock){
            cout << temp.state << " " << temp.numBlocked << "\t";
        }

        else if (temp.state == Voter::Complete){
            cout << temp.state << "\t";
        }

        temp.empty = 1;
        buffer[i] = temp;
    }

    cout << endl;
}

//----------------------------------------------------------------------
// Flush for finished voter tasks
//
void Printer::flushFinished(unsigned int id){
    for (unsigned int i = 0; i < numVoters; i++){
        VoterInfo temp = buffer[i];

        if (i != id){
            cout << "...\t";
        }
        else {
            assert(temp.state == Voter::Finished);
            cout << temp.state << " " << temp.vote << "\t";
        }
        temp.empty = 1;
        buffer[i] = temp;
    }    
    cout << endl;
}


//----------------------------------------------------------------------
// Print for Start and Complete states
//
void Printer::print( unsigned int id, Voter::States state ){
    VoterInfo stateOnly = buffer[id];
    if(!stateOnly.empty){
        flush();
    }
    stateOnly.state = state;
    stateOnly.empty = 0;
    buffer[id] = stateOnly;
}

//----------------------------------------------------------------------
// Print for Vote and Finished states
//
void Printer::print( unsigned int id, Voter::States state, bool vote ){
    VoterInfo voteType = buffer[id];
    if(!voteType.empty){
        flush();
    }
    voteType.state = state;
    voteType.vote = vote;
    voteType.empty = 0;
    buffer[id] = voteType;

    if(state == Voter::Finished){
        flushFinished(id);
    }
}

//----------------------------------------------------------------------
// Print for Block and Unblock states
//
void Printer::print( unsigned int id, 
                     Voter::States state, 
                     unsigned int numBlocked ){
    VoterInfo blockType = buffer[id];
    if(!blockType.empty){
        flush();
    }
    blockType.state = state;
    blockType.numBlocked = numBlocked;
    blockType.empty = 0;
    buffer[id] = blockType;
}
