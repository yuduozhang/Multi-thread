#ifndef __AUTOMATICSIGNAL_H
#define __AUTOMATICSIGNAL_H

#define AUTOMATIC_SIGNAL uCondition chairs;

#define WAITUNTIL( pred, before, after )            \
    if(!pred ){                                     \
        while ( !chairs.empty() ) chairs.signal();  \
        before;                                     \
        do {                                        \
            chairs.wait();                          \
        }while(!pred);                              \
        assert(pred);                               \
    }                                               \
    after;                                          \
                                    
#define RETURN(expr)                                \
    while ( !chairs.empty() ) chairs.signal();      \
    return expr;                                    \

#endif //__AUTOMATICSIGNAL_H
