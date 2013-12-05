#ifndef __VENDINGMACHINE_H__
#define __VENDINGMACHINE_H__

#include <uC++.h>

_Monitor Printer;
_Task NameServer;
class WATCard;


_Task VendingMachine {
  public:
    enum Flavours { Blues_Black_Cherry, Classical_Cream_Soda, Rock_Root_Beer, Jazz_Lime, Num_Of_Flavours }; 			// flavours of soda (YOU DEFINE)
  private:
    Printer &prt;
	NameServer &nameServer;
	unsigned int id;					//vending machine id
	unsigned int price;		//soda Cost
	int maxSPF;		//max Stock per Flavour
	unsigned int stock[Num_Of_Flavours];	// array stock storing each Flavour
	bool restoring;
	//unsigned int getStockId(Flavours flavour);
	//typedef int Flavours;
	void main();
  public:
    enum Status { BUY, STOCK, FUNDS };		// purchase status: successful buy, out of stock, insufficient funds
    VendingMachine( Printer &prt, NameServer &nameServer, unsigned int id, unsigned int sodaCost,
                    unsigned int maxStockPerFlavour );
    Status buy( Flavours flavour, WATCard &card );
    unsigned int *inventory();
    void restocked();
    _Nomutex unsigned int cost();
    _Nomutex unsigned int getId();
	~VendingMachine();
};

#endif
