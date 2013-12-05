#include "Printer.h"
#include <iostream>
using namespace std;

void Printer::terminateFlush(BufferEntry *entry){
	//flush the buffer immediately
	flush();
	//print out the terminated task
	for(unsigned int i=0; i<bufferlength; i+=1){
		if(i != entry->id) cout << "...";
		else cout << entry->state;
		if(i != bufferlength-1) cout << '\t';
	}
	cout << endl;
	for(unsigned int i=0; i<bufferlength; i+=1){
		if(buffer[i] != NULL){
			delete buffer[i];
			buffer[i] = NULL;
		}
	}
	delete entry;
}
void Printer::flush(){
	//print the whole stored info of a row
	int notempty = 0;
	unsigned int lastentry = 0;
	for(unsigned int i=0; i<bufferlength; i+=1){
		if(buffer[i] != NULL) notempty = 1; 
	}
	if(notempty == 1){
		for(unsigned int i=0; i<bufferlength; i+=1){
			if(buffer[i] != NULL) lastentry = i; 
		}
		for(unsigned int i=0; i<bufferlength; i+=1){
			if(buffer[i] == NULL){}
			else{
				if(buffer[i]->type == 0) cout << buffer[i]->state;
				else if(buffer[i]->type == 1) cout << buffer[i]->state << buffer[i]->value1;
				else cout << buffer[i]->state << buffer[i]->value1 << "," << buffer[i]->value2;
			}
			if(i == lastentry) break;
			if(i != bufferlength-1){ cout << '\t';}
		}
		cout << endl;
	}
	//flush the buffer
	for(unsigned int i=0; i<bufferlength; i+=1){
		if(buffer[i] != NULL){
			delete buffer[i];
			buffer[i] = NULL;
		}
	}
}
void Printer::overWrite(BufferEntry *entry){
	if(entry->state == 'F'){
		terminateFlush(entry);
		return;
	}
	if(buffer[entry->id] == NULL){
		buffer[entry->id] = entry;
	}
	else{
		flush();
		buffer[entry->id] = entry;
	}
}
unsigned int Printer::getId(Kind kind, unsigned int lid){
	if(kind == Printer::Parent) return 0;
	else if(kind == Printer::WATCardOffice) return 1;
	else if(kind == Printer::NameServer) return 2;
	else if(kind == Printer::Truck) return 3;
	else if(kind == Printer::BottlingPlant)	return 4;
	else if(kind == Printer::Student) return (5 + lid);
	else if(kind == Printer::Vending) return (5 + numStudents + lid);
	else return (5 + numStudents + numVendingMachines + lid);
}

Printer::Printer( unsigned int numStudents, unsigned int numVendingMachines, unsigned int numCouriers ) :
			numStudents(numStudents), numVendingMachines(numVendingMachines), numCouriers(numCouriers){
	bufferlength = 5 + numStudents + numVendingMachines + numCouriers;
	buffer.resize(bufferlength);
	for(unsigned int i=0; i<bufferlength; i+=1){
		if(i == 0) cout << "Parent";
		else if(i == 1) cout << "WATOff";
		else if(i == 2) cout << "Names";
		else if(i == 3) cout << "Truck";
		else if(i == 4) cout << "Plant";
		else if(5 <= i && i < 5+numStudents) cout << "Stud" << (i-5);
		else if(5+numStudents <= i && i < 5+numStudents+numVendingMachines) cout << "Mach" << (i-5-numStudents);
		else cout << "Cour" << (i-5-numStudents-numVendingMachines);
		if(i != bufferlength-1) cout << '\t';
	}
	cout << endl;
	for(unsigned int i=0; i<bufferlength; i+=1){
		cout << "*******";
		if(i != bufferlength-1) cout << '\t';
	}
	cout << endl;
}
void Printer::print( Kind kind, char state ){
	unsigned int id = getId(kind,0);
	Printer::BufferEntry *entry = new Printer::BufferEntry(0,id,state,0,0);
	overWrite(entry);
}
void Printer::print( Kind kind, char state, int value1 ){
	unsigned int id = getId(kind,0);
	Printer::BufferEntry *entry = new Printer::BufferEntry(1,id,state,value1,0);
	overWrite(entry);
}
void Printer::print( Kind kind, char state, int value1, int value2 ){
	unsigned int id = getId(kind,0);
	Printer::BufferEntry *entry = new Printer::BufferEntry(2,id,state,value1,value2);
	overWrite(entry);
}
void Printer::print( Kind kind, unsigned int lid, char state ){
	unsigned int id = getId(kind,lid);
	Printer::BufferEntry *entry = new Printer::BufferEntry(0,id,state,0,0);
	overWrite(entry);
}
void Printer::print( Kind kind, unsigned int lid, char state, int value1 ){
	unsigned int id = getId(kind,lid);
	Printer::BufferEntry *entry = new Printer::BufferEntry(1,id,state,value1,0);
	overWrite(entry);
}
void Printer::print( Kind kind, unsigned int lid, char state, int value1, int value2 ){
	unsigned int id = getId(kind,lid);
	Printer::BufferEntry *entry = new Printer::BufferEntry(2,id,state,value1,value2);
	overWrite(entry);
}

Printer::~Printer(){
	cout << "***********************" << endl;
	for(unsigned int i=0; i<bufferlength; i+=1){
		if(buffer[i] != NULL) delete buffer[i];
	}
}