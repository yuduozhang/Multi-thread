#include <uC++.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstring>
#include <cerrno>
#include "PRNG.h"

using namespace std;

PRNG prng;

_Coroutine Station {
    public:
    
    struct Frame { 
        enum frameType { token, data, ack, nack }; 
        frameType frame;
        unsigned int sourceID;
        unsigned int destinationID; 

        Frame(frameType f, unsigned int sID, unsigned int dID){
            frame = f;
            sourceID = sID;
            destinationID = dID;
        }
    };

    private:
    _Event Election {
      public:
        Station &candidate;
        Election( Station &candidate ) : candidate( candidate ) {}
    };

    static unsigned int value;                // global variable, increasing transmission value
    static std::vector<Station *> active;     // global variable, active stations
    Station *remove( Station *victim ){       // delete failed object and return new partner
        for (int i = 0; i < active.size(); i++){
           if (victim->id() == active[i]->id()){ 
                active.erase ( active.begin() + i );
                break;
           }
        }

        return  victim->partner(); 
    }

    void vote(){          // perform election voting
        resume();
    }

    unsigned int myID;
    unsigned int myFailure;
    bool fail;
    int tokenCounter;

    const Frame *myFrame;
    Station *myPartner;

    void main() {
        tokenCounter = prng(0,1);
        //cout << "id: " << myID << " tokenCounter1: " << tokenCounter << endl;
        
        Station * This = this;
        try {
            _Enable {                         // allow delivery of nonlocal exceptions


                for (;;){
                    suspend();                    // establish starter for termination

                    /*for (int i = 0; i < active.size(); i++){
                        cout << "myid: " << active[i]->id() << " partner: " << active[i]->partner()->id() << endl; 
                    }*/

                    if (active.size() == 1){
                        delete myFrame;
                        cout << "no more partners" << endl;
                        break;
                    }

                    while (myPartner->failed()){
                        
                        //cout << "here1" << endl;
                        cout << myID << "(fail " << myPartner->id() << ", part " << myPartner->partner()->id() << ") ";

                        Station *oldPartner = myPartner;

                        if (oldPartner == coordinator){
                            cout << myID << "(elec ";
                            coordinator = This;
                            _Resume Election(*coordinator);
                            cout << "win " << coordinator->id() << ") ";
                            assert(coordinator->failed() == false);
                        }

                        if (myFrame->destinationID == oldPartner->id()){
                            if (myFrame->frame == Frame::ack
                               ){
                                cout << myID << "(drop Ack)" << endl;
                                delete myFrame;
                                myFrame = new Frame(Frame::token, 0, 0);
                            }
                                   
                            else if (myFrame->frame == Frame::nack
                               ){
                                cout << myID << "(drop Nack)" << endl;
                                delete myFrame;
                                myFrame = new Frame(Frame::token, 0, 0);
                            }

                            else if (myFrame->frame == Frame::data){
                                if (myFrame->sourceID == myFrame->destinationID){
                                    cout << myID << "(drop Data)" << endl;
                                    delete myFrame;
                                    myFrame = new Frame(Frame::token, 0, 0);
                                }
                                else {
                                    unsigned int dID = myFrame->sourceID;
                                    unsigned int sID = myFrame->destinationID;
                                    delete myFrame;
                                    myFrame = new Frame(Frame::nack, sID, dID);
                                } // else
                            } // if
                        } // if

                        if (myFrame->frame == Frame::data 
                                && 
                            myFrame->sourceID == oldPartner->id()
                           ){
                            cout << myID << "(rec " << value << ", from " << myFrame->sourceID << ") ";
                            cout << myID << "(drop Ack)" << endl;
                            delete myFrame;
                            myFrame = new Frame(Frame::token, 0, 0);
                        } // if

                        myPartner = coordinator->remove(oldPartner);
                    } // while 


                    assert(partner()->failed() == false); 

                    if ( prng(0,myFailure-1) == 0 ){
                        fail = true;
                    }

                    else {
                        fail = false;
                    }

                    if (myFrame->frame == Frame::token){
                        if (tokenCounter > 0){
                            //cout << "id: " << myID << " tokenCounter2: " << tokenCounter << endl;
                            tokenCounter--;
                            cout << myID << "(skip) ";
                            myPartner->transmit(* myFrame);
                        }

                        else {
                            tokenCounter = prng(1,3);
                            unsigned int dIndex = prng(0,active.size()-1);
                            unsigned int dID = active[dIndex]->id();
                            delete myFrame;
                            myFrame = new Frame(Frame::data, myID, dID);
                            value++;
                            cout << myID << "(send " << value << ", to " << myFrame->destinationID << ") ";
                            myPartner->transmit( *myFrame );
                        }
                    }

                    else if (myFrame->frame == Frame::data){
                        if (myFrame->destinationID == myID){
                            cout << myID << "(rec " << value << ", from " << myFrame->sourceID << ") ";
                            unsigned int dID = myFrame->sourceID;
                            unsigned int sID = myFrame->destinationID;
                            delete myFrame;
                            myFrame = new Frame(Frame::ack, sID, dID);
                            myPartner->transmit( *myFrame );
                        }                      
                        else {
                            cout << myID << "(for " << myFrame->destinationID << ", " << value << ") ";
                            myPartner->transmit( *myFrame );
                        }
                    }

                    else if (myFrame->frame == Frame::ack){
                        if (myFrame->destinationID == myID){
                            delete myFrame;
                            myFrame = new Frame(Frame::token, 0, 0);
                            cout << myID << "(ack)" << endl;
                            myPartner->transmit( *myFrame );
                        }
                        else {
                            cout << myID << "(ack " << myFrame->destinationID << ") ";
                            myPartner->transmit( *myFrame );
                        }
                    }

                    else{ 
                        if (myFrame->destinationID == myID){
                            delete myFrame;
                            myFrame = new Frame(Frame::token, 0, 0);
                            cout << myID << "(nack)" << endl ;
                            myPartner->transmit( *myFrame );
                        }
                        else {
                            cout << myID << "(nack " << myFrame->destinationID << ") ";
                            myPartner->transmit( *myFrame );
                        }
                    }

                } 

                return;
            } // _Enable
        } _CatchResume( Election &e ) ( Station *This ) {
            
            if (This != This->coordinator && This != This->coordinator->partner()){
                if (This->failed() == false){
                    cout << e.candidate.id() << "?" << This->id() << " ";

                    if (e.candidate.id() < This->id()){
                        This->coordinator = This;
                    }
                } 
                 else {
                     cout << e.candidate.id() << ">" << This->id() << " ";
                } 
            }

            if (This->partner() != This->coordinator){
                _Resume Election(*(This->coordinator)) _At *This->partner();
            }

           This->partner()->vote();
           This->suspend();
        } 
    } // main

    public:
    static Station *coordinator;              // global variable, shared among all instances


    Station( unsigned int id, unsigned int failure ) {
        myID = id;
        myFailure = failure;
        fail = 0;
        
        vector<Station *>::iterator it;
        it = active.begin();

        active.insert(it, this);
    }
    
    void start( Station *partner ){         // supply partner
        myPartner = partner;
        resume();
    }

    void transmit( const Frame &f ) {          // pass frame
    //   cout << "in transmit" << endl;
        myFrame = &f;
        resume();
    }

    unsigned int id() {                       // station id
        return myID;
    }

    Station *partner() {                      // station partner
        return myPartner;
    }
    bool failed(){                            // station status
        return fail;
    }

};

Station *Station::coordinator = NULL;
unsigned int Station::value = 0;
std::vector<Station *> Station::active;   

void uMain::main(void) {
    /* To form the ring of stations:
     * 1. Call the start member for eac station to link the
     * stations together in reverse order.
     * 2. Station::start() also resumes the station to set uMain as
     * its starter (needed during termination). 
     * 3. Station::main() suspends back immediately.
     * 4. Start the next station.
     * 5. Set the coordinator to station 0.
     * 6. Transmit the token to the coordinator. (Station 0 at first)
     */

    if (argc == 1 || argc > 4){
        cerr << "Usage: " << argv[0] << "S [ F [ Seed ] ]\n";
        exit( EXIT_FAILURE );
    } // if

    else {
        vector<Station *> deleteList;

        int numStation;
        int failChance = 7;
        int seed = 17844;

        numStation = atoi(argv[1]);

        prng.seed(seed);

        //Initialize with the ring
        Station *lastone = new Station(numStation-1, failChance);
        deleteList.push_back(lastone);

        Station *lastStation = lastone;

        for (int i = numStation - 2; i >= 0; i--){
            Station *temp = new Station(i,failChance);
            deleteList.push_back(temp);
            //cout << temp << endl;
            temp->start(lastStation);   
            lastStation = temp;
        }
        
        lastone->start(lastStation);

        Station::coordinator = lastStation; //In this case, lastStation is station 0
        Station::Frame *firstFrame = new Station::Frame(Station::Frame::token, 0, 0);
        Station::coordinator->transmit( *firstFrame );

        // delete heap memory
        for (int i = 0; i < deleteList.size(); i++){
            delete deleteList[i];
        }
    }
}
