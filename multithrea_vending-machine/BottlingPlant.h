#ifndef __BOTTLINGPLANT_H__
#define __BOTTLINGPLANT_H__

#include <uC++.h>
#include "VendingMachine.h"

_Monitor Printer;
_Task NameServer;
//_Task VendingMachine;
_Task Truck;

_Task BottlingPlant {
	Printer &prt;
	NameServer &nameServer;
	unsigned int numVM;
	unsigned int maxShippedPF;
	unsigned int maxStockPF;
	unsigned int timeBS;
	unsigned int prodSum;
	unsigned int prod[VendingMachine::Num_Of_Flavours];
	bool closing;
	Truck *car;
	void productionRun();
    void main();
  public:
    BottlingPlant( Printer &prt, NameServer &nameServer, unsigned int numVendingMachines,
                 unsigned int maxShippedPerFlavour, unsigned int maxStockPerFlavour,
                 unsigned int timeBetweenShipments );
    bool getShipment( unsigned int cargo[] );
	~BottlingPlant();
};


#endif
