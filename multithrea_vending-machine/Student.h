#ifndef __STUDENT_H__
#define __STUDENT_H__

#include <uC++.h>
using namespace std;


_Monitor Printer;
_Task NameServer;
_Task WATCardOffice;

_Task Student {
    Printer &prt;
    NameServer &nameServer;
    WATCardOffice &cardOffice;
    unsigned int sid;
    unsigned int maxPurchases;

    unsigned int initialBalance;
    
    void main();
  public:
    Student( Printer &prt, NameServer &nameServer, WATCardOffice &cardOffice, unsigned int id,
             unsigned int maxPurchases );
	~Student();

};

#endif // __STUDENT_H__
