#include "NameServer.h"
#include "Printer.h"
#include "VendingMachine.h"
#include <iostream>
using namespace std;

/***************************************************************************
	IMPORTANT NOTE: Most of the codes are derived from my 
					past assignment in cs246(fall 2010)
****************************************************************************/


void NameServer::main(){
	for(;;){
		_Accept(~NameServer){
			break;
		} or _Accept(VMregister,getMachineList,getMachine){
		}
	}
}

NameServer::NameServer ( Printer &prt, unsigned int numVendingMachines, unsigned int numStudents ) : prt(prt){
  numVM = numVendingMachines;
  numStu = numStudents;
  prt.print( Printer::NameServer, 'S');
  for ( unsigned int i = 0 ; ; i += 1 ) {       	// assign each student initially with a vending machine
    if ( i >= numStu ) break;
    Stu_list.push_back ( i % numVM );
  }
}

NameServer::~NameServer() {
  prt.print( Printer::NameServer, 'F' );
}

void NameServer::VMregister( VendingMachine *vendingmachine ) {
  VM_list.push_back( vendingmachine );	// register the vending machine in the order that they called VMreigster  				       	
  prt.print( Printer::NameServer, 'R', vendingmachine->getId());
}

VendingMachine *NameServer::getMachine( unsigned int id ) {
  VendingMachine *v = VM_list[ Stu_list.at( id ) ];
  prt.print( Printer::NameServer, 'N', id, v->getId() );
  // ensure student can get another vending machine when getMachine is called next time 
  Stu_list.at( id ) = ( Stu_list.at( id ) + 1 ) % numVM;  					
  return v;						// return the student's assigned vending machine
}

VendingMachine **NameServer::getMachineList() {
  VendingMachine **v = &VM_list.at( 0 );	// chagned vector into array for Truck
  return v;
}