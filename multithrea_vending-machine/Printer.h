#ifndef __PRINTER_H_
#define __PRINTER_H_

#include <uC++.h>
using namespace std;
#include <vector>

_Monitor Printer {
  public:
    enum Kind { Parent, WATCardOffice, NameServer, Truck, BottlingPlant, Student, Vending, Courier };
  private:
	struct BufferEntry{
		int type; 			//type = {0,1,2}
		unsigned int id;
		char state;
		int value1;
		int value2;
		BufferEntry(int type, unsigned int id, char state, int value1, int value2):
			type(type), id(id), state(state), value1(value1), value2(value2){}
	};
	unsigned int numStudents;
	unsigned int numVendingMachines;
	unsigned int numCouriers;
	unsigned int bufferlength;
	vector<BufferEntry*> buffer; 				//buffer for one row, size of voters
	void terminateFlush(BufferEntry *entry);	//flush for task terminate
	void flush();
	void overWrite(BufferEntry *entry);
	unsigned int getId(Kind kind, unsigned int lid);
  public:
    Printer( unsigned int numStudents, unsigned int numVendingMachines, unsigned int numCouriers );
    void print( Kind kind, char state );
    void print( Kind kind, char state, int value1 );
    void print( Kind kind, char state, int value1, int value2 );
    void print( Kind kind, unsigned int lid, char state );
    void print( Kind kind, unsigned int lid, char state, int value1 );
    void print( Kind kind, unsigned int lid, char state, int value1, int value2 );
	~Printer();
};

#endif
