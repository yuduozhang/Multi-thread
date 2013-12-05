#ifndef __PARENT_H__
#define __PARENT_H__

#include<uC++.h>
using namespace std;

_Monitor Printer;
_Monitor Bank;

_Task Parent {
	Printer &prt;
	Bank &bank;
	unsigned int numStudents;
	unsigned int parentalDelay;
    void main();
  public:
    Parent( Printer &prt, Bank &bank, unsigned int numStudents, unsigned int parentalDelay );
	~Parent();
};


#endif // __PARENT_H__
