#ifndef __WATCARD_H__
#define __WATCARD_H__

#include <uC++.h>
#include "uFuture.h"
using namespace std;

class WATCard {
    WATCard( const WATCard & );			// prevent copying
    WATCard &operator=( const WATCard & );
    unsigned int balance;

  public:
    WATCard();
    void deposit( unsigned int amount );
    void withdraw( unsigned int amount );
    unsigned int getBalance();
};

typedef Future_ISM<WATCard *> FWATCard;		// future WATCard pointer

#endif
