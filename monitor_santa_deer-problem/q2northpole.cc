#include "q2northpole.h"
#include<iostream>
#include<stdlib.h>

using namespace std;

//======================================================================
//  Monitor Workshop 
//======================================================================


//----------------------------------------------------------------------
// Constructor

Workshop::Workshop( Printer &prt, unsigned int E, unsigned int N, unsigned int D ) :
    printer(prt), numElves(E), boundReindeer(N), 
    numDeliveries(D), waitingReindeer(0), waitingElves(0), 
    numServeReindeer(0), meetingNum(0), officeNum(0), sleighNum(0){}


//----------------------------------------------------------------------
// sleep()

Workshop::Status Workshop::sleep(){

    // santa keeps rechecking until done
    for ( ;; ){
        if (waitingReindeer == Reindeer::MAX_NUM_REINDEER                               // all five reindeers have checked in
                && numServeReindeer < boundReindeer){                                   // number of service reindeers got is less then the bound

            if (waitingElves >= Elf::CONSULTING_GROUP_SIZE) numServeReindeer++;         // if there are groups of elves waiting, increase number of service reindeers get

            reindeerWait.signal();                                                      // wake up one reindeer to deliver, other reindeers will be waken up by this reindeer in deliver()

            return Delivery;
        }

        else if (waitingElves >= Elf::CONSULTING_GROUP_SIZE) {                          // if there are groups of elves waiting
            
            numServeReindeer = 0;                                                       // when elves get the service, reset number of service reindeers get so that this variable is not 
            elvesWait.signal();                                                         // accumulated unlimitedly

            return Consulting;
        }


        else if (numElves < Elf::CONSULTING_GROUP_SIZE && numDeliveries == 0) {         // return done if not enough number of elvs to form a consultation group and delivery number equals 0
            return  Done;
        }

        printer.print(0, Printer::Blocked);
        santaSleep.wait();
        printer.print(0, Printer::Unblocked);
    }
}


void Workshop::deliver( unsigned int id ){

    if (waitingReindeer == Reindeer::MAX_NUM_REINDEER-1){                               // wake up santa if all 5 reindeers have checked in
        santaSleep.signal(); 
    }

    waitingReindeer++;
    printer.print(id, Printer::Blocked, waitingReindeer);
    reindeerWait.wait();
    printer.print(id, Printer::Unblocked, waitingReindeer);
    waitingReindeer--;

    reindeerWait.signal();                                                              // awake deer needs to wake up other deers

}

bool Workshop::consult( unsigned int id ){

    if ( waitingElves >= Elf::CONSULTING_GROUP_SIZE-1 ) {                               // wake up santa if there are groups of elves waiting
        santaSleep.signal();
    }
    
    if (numElves < Elf::CONSULTING_GROUP_SIZE){                                         // if not enough elves to form a group, wake up other elves to return failed status
        elvesWait.signal();
        return false;
    }

    waitingElves++;
    printer.print(id, Printer::Blocked, waitingElves);
    elvesWait.wait();
    printer.print(id, Printer::Unblocked, waitingElves);
    waitingElves--;

    if (numElves < Elf::CONSULTING_GROUP_SIZE){                                         // check fail again
        elvesWait.signal();
        return false;
    }

    meetingNum++;

    if (meetingNum < Elf::CONSULTING_GROUP_SIZE){                                       // wake up enough elves to form a group
        elvesWait.signal();
    }

    return true;
}

void Workshop::doneConsulting( unsigned int id ){

    if (officeNum < Elf::CONSULTING_GROUP_SIZE){                                        // block if meeting is not done (less than group size elves have called doneConsulting())
        officeNum++;
        printer.print(id, Printer::Blocked, officeNum);
        office.wait();
        printer.print(id, Printer::Unblocked, officeNum);
        officeNum--;
    }     
    
    if (officeNum > 0){                                                                 // wake up other tasks blocking in office
        office.signal(); 
    }

    printer.print(id, Printer::DoneConsulting);
    meetingNum = 0;                                                                     // meeting is done, reset meetingNum to 0
}

void Workshop::doneDelivering( unsigned int id ){

    if (sleighNum < Reindeer::MAX_NUM_REINDEER) {                                       // block if delivery is not done (some deers haven't called doneConsulting())
        sleighNum++;
        printer.print(id, Printer::Blocked, sleighNum);
        sleigh.wait();
        printer.print(id, Printer::Unblocked, sleighNum);
        sleighNum--;
    }

    if (sleighNum > 0) {                                                                // wake up other tasks blocking on sleigh
        sleigh.signal();
    }

    else {
        numDeliveries--;                                                                // one deliver is done, so decrement numDeliveries by one
    }

    printer.print(id, Printer::DoneDelivering);

    if (numDeliveries == 0){                                                            // if no deliveries left, wake up santa to check if "Done" should be returned
        santaSleep.signal();
    }
}

void Workshop::termination( unsigned int id ){
    numElves--;                                                                         // decrease elve number by one

    if (numElves < Elf::CONSULTING_GROUP_SIZE){                                         // if not enough elves to form a group, wake up other waiting elves to terminate
        elvesWait.signal();
    }

    if (numElves == 0){                                                                 // last elve needs to signal santa to make sure santa has a chance to return "Done"
        santaSleep.signal();
    }
}

//======================================================================
//  Santa Task
//======================================================================

//----------------------------------------------------------------------
// Constructor

Santa::Santa( Workshop &wrk, Printer &prt ) :
    id(0),workshop(wrk), printer(prt){}

//----------------------------------------------------------------------
// main()

void Santa::main(){
    unsigned int yieldNum = prng(0,10);         
    yield(yieldNum);
    printer.print(id, Printer::Starting);
   
    for( ;; ){
        yieldNum = prng(0,3);                                                           
        yield(yieldNum);
        printer.print(id, Printer::Napping);                                

        // Block in workshop if nothing to do
        Workshop::Status s = workshop.sleep();

        printer.print(id, Printer::Awake);

        if (s == Workshop::Done) break;
            
        // delivery toys
        else if (s == Workshop::Delivery){
            // simulate delivering toys
            printer.print(id, Printer::DeliveringToys);
            yieldNum = prng(0,5);         
            yield(yieldNum);
            workshop.doneDelivering(id);
        }

        // consult
        else if (s == Workshop::Consulting){
            // simulate consulting
            printer.print(id, Printer::Consulting);
            yieldNum = prng(0,3);         
            yield(yieldNum);
            workshop.doneConsulting(id);
        }
    } 

    printer.print(id, Printer::Finished);
}

//======================================================================
//  Elf Task
//======================================================================

//----------------------------------------------------------------------
// Constructor

Elf::Elf( unsigned int id, Workshop &wrk, Printer &prt, unsigned int numConsultations ) :
    id(id), workshop(wrk), printer(prt), numC(numConsultations){}

//----------------------------------------------------------------------
// main()

void Elf::main(){
    unsigned int yieldNum = prng(0,10);         
    yield(yieldNum);
    printer.print(id, Printer::Starting);

    // Loop until done number of consultations
    for( ;numC > 0; numC-- ){
        yieldNum = prng(0,3);         
        yield(yieldNum);
        printer.print(id, Printer::Working);

        // simulate working
        yieldNum = prng(0,5);         
        yield(yieldNum);

        printer.print(id, Printer::NeedHelp);
        
        // if consult returns false, break since consulting failed
        if (!workshop.consult(id)){
            printer.print(id, Printer::ConsultingFailed);
            break;
        }

        printer.print(id, Printer::Consulting);

        // simulate consulting
        yieldNum = prng(0,3);         
        yield(yieldNum);

        workshop.doneConsulting(id);
    } 

    // indicate termination
    workshop.termination(id);
    printer.print(id, Printer::Finished);
}


//======================================================================
//  Reindeer Task
//======================================================================

//----------------------------------------------------------------------
// Constructor

Reindeer::Reindeer( unsigned int id, Workshop &wrk, Printer &prt, unsigned int numDeliveries) :
    id(id), workshop(wrk), printer(prt), numD(numDeliveries){}

//----------------------------------------------------------------------
// main()

void Reindeer::main(){
    unsigned int yieldNum = prng(0,10);         
    yield(yieldNum);
    printer.print(id, Printer::Starting);
   
    // Loop until done number of deliveries
    for( ;numD > 0; numD--){
        yieldNum = prng(0,3);         
        yield(yieldNum);
        printer.print(id, Printer::OnVacation);

        // simulate vacation
        yieldNum = prng(0,5);         
        yield(yieldNum);

        printer.print(id, Printer::CheckingIn);
        
        // Wait for delivery
        workshop.deliver(id);
        printer.print(id, Printer::DeliveringToys);

        // simulate delivering toys
        yieldNum = prng(0,5);         
        yield(yieldNum);

        // indicate delivery done
        workshop.doneDelivering(id);
    } 

    printer.print(id, Printer::Finished);
}


//======================================================================
// Printer Monitor
//======================================================================

//----------------------------------------------------------------------
// Constructor

Printer::Printer( const unsigned int MAX_NUM_ELVES ){
    numElves = MAX_NUM_ELVES;

    TaskInfo santa;
    santa.taskType = "Sa";
    santa.id = 0;
    santa.empty = 1;
    buffer[0] = santa;

    for (unsigned int i = 1; i <= numElves; i++){
        TaskInfo tempElve;
        tempElve.taskType = "E";
        tempElve.id = i;
        tempElve.empty = 1;
        buffer[i] = tempElve;
    }

    for (unsigned int i = numElves+1; i <= numElves + 5; i++){
        TaskInfo tempReindeer;
        tempReindeer.taskType = "R";
        tempReindeer.id = i;
        tempReindeer.empty = 1;
        buffer[i] = tempReindeer;
    }

    for (unsigned int i = 0; i < buffer.size(); i++){
        TaskInfo temp = buffer[i];
        if (temp.id == 0){          // Santa's id equals 0
            cout << temp.taskType << "\t";
        }
        else{ 
            cout << temp.taskType << temp.id << "\t";
        }
    }

    cout << endl;

    for (unsigned int i = 0; i < buffer.size(); i++){
        cout << "--\t";
    }

    cout << endl;
}


//----------------------------------------------------------------------
// Destructor
//
Printer::~Printer(){
    cout << "Workshop closed" << endl;
}

//----------------------------------------------------------------------
// Flush for unfinished tasks
//
void Printer::flush(){
    for (unsigned int i = 0; i < buffer.size(); i++){
        TaskInfo temp = buffer[i];


        // If empty TaskInfo, output empty column
        if (temp.empty){
            cout << "\t";
        }

        // If not empty
        else if (temp.state == Blocked || temp.state == Unblocked){

            // negative numBlocked means it's not set, hence don't print blocked/unblocked number
            if (temp.numBlocked < 0){                                  
                cout << temp.state << "\t";
            }
            else {
                cout << temp.state << " " << temp.numBlocked << "\t";
            }
        }

        else {
            cout << temp.state << "\t";
        }

        temp.empty = 1; 
        buffer[i] = temp;
    }

    cout << endl;
}

//----------------------------------------------------------------------
// Flush for finished tasks
//
void Printer::flushFinished(unsigned int id){
    for (unsigned int i = 0; i < buffer.size(); i++){
        TaskInfo temp = buffer[i];

        if (i != id){
            cout << "...\t";
        }
        else {
            assert(temp.state == Finished);
            cout << temp.state << "\t";
        }
        temp.empty = 1;
        buffer[i] = temp;
    }    
    cout << endl;
}

//----------------------------------------------------------------------
// Print for ordinary states
//
void Printer::print( unsigned int id, States state ){
    TaskInfo stateOnly = buffer[id];
    if(!stateOnly.empty){
        flush();
    }
    else if(state == Finished){
        flush();
    }

    stateOnly.state = state;
    stateOnly.empty = 0;
    stateOnly.numBlocked = -1;
    buffer[id] = stateOnly;

    if(state == Finished){
        flushFinished(id);
    }
}

//----------------------------------------------------------------------
// Print for blocked/unblocked states
//
void Printer::print( unsigned int id, States state, unsigned int numBlocked ){
    TaskInfo blockType = buffer[id];
    if(!blockType.empty){
        flush();
    }
    blockType.state = state;
    blockType.numBlocked = numBlocked;
    blockType.empty = 0;
    buffer[id] = blockType;
}
