#ifndef __NAMESERVER_H__
#define __NAMESERVER_H__

#include <uC++.h>
#include <iostream>
using namespace std;
#include <vector>

_Monitor Printer;
_Task VendingMachine;

_Task NameServer {
	Printer &prt;
	unsigned int numVM;
	unsigned int numStu;
	vector<VendingMachine*> VM_list;
	vector<int> Stu_list;
    void main();
  public:
    NameServer( Printer &prt, unsigned int numVendingMachines, unsigned int numStudents );
    void VMregister( VendingMachine *vendingmachine );
    VendingMachine *getMachine( unsigned int id );
    VendingMachine **getMachineList();
	~NameServer();
};

#endif
