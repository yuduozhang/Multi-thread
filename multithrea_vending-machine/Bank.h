#ifndef __BANK_H__
#define __BANK_H__

#include <uC++.h>
using namespace std;

_Monitor Bank {
    unsigned int numStudents;
    unsigned int * accountInfo;

    uCondition * courierBench;      // each student has a bench for the 
                                    // courier that is transfering money for him

  public:
    Bank( unsigned int numStudents );
	~Bank();
    void deposit( unsigned int id, unsigned int amount );
    void withdraw( unsigned int id, unsigned int amount );
};

#endif // __BANK_H__
