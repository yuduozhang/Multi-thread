#include "Bank.h"
#include <iostream>
using namespace std;


//----------------------------------------------------------------------
// Bank constructor
Bank::Bank( unsigned int numStudents ) : numStudents(numStudents){
    accountInfo = new unsigned int [numStudents];
    courierBench = new uCondition [numStudents];

    for ( unsigned int i = 0; i < numStudents; i++ ){
        accountInfo[i] = 0;
    }
}

//----------------------------------------------------------------------
// Bank destructor
Bank::~Bank(){
    delete [] accountInfo;
    delete [] courierBench;
}

//----------------------------------------------------------------------
//
void Bank::deposit( unsigned int id, unsigned int amount){
    accountInfo[id] += amount;

    // after depositing, if balance is non-negative and there
    // are couriers wating, wake him up
    if (accountInfo[id] >= 0 && !courierBench[id].empty()){
        courierBench[id].signal();
    }
}

//----------------------------------------------------------------------
//
void Bank::withdraw( unsigned int id, unsigned int amount){
    accountInfo[id] -= amount;

    // if after withdraw, balance becomes negative, wait
    // until it becomes non-negative
    if (accountInfo[id] < 0) courierBench[id].wait();
}
