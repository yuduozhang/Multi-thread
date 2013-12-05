#include <uC++.h>					                    
#include <queue>
#include <iostream>

using namespace std;

#ifdef BUSY
template<typename T> class BoundedBuffer {
    uOwnerLock mlk;
    uCondLock producerLock;
    uCondLock consumerLock;

    queue<T> boundedBuffer;

    unsigned int size;
    unsigned int count;

  public:
    BoundedBuffer( const unsigned int size = 10 ){
        BoundedBuffer::size = size;
        count = 0;
    }

    void insert( T elem ){
        mlk.acquire();                          // acquire mutual exclusion locker
        while (count == size){                  // wait if buffer is full
            producerLock.wait(mlk);             // busy waiting in a looping
        }
        assert(count < size );              
        boundedBuffer.push(elem);
        count++;
        consumerLock.signal();                  // wake up consumers since items are inserted
        mlk.release();                          // release mutual exclusion locker
    }

    T remove(){
        mlk.acquire();                          // acquire mutual exclusion locker
        while (count == 0){                     // wait if buffer is empty
            consumerLock.wait(mlk);             // busy waiting in a looping
        }
        assert(count > 0);
        T front = boundedBuffer.front();
        boundedBuffer.pop();
        count--;
        producerLock.signal();                  // wake up producers since there are empty spots
        mlk.release();                          // release mutual exclusion locker
        return front;
    }
};

#endif // BUSY

#ifdef NOBUSY
template<typename T> class BoundedBuffer {
    uOwnerLock mlk;                         
    uCondLock producerLock;
    uCondLock consumerLock;
    uCondLock bargingTaskLock;

    queue<T> boundedBuffer;

    unsigned int size;
    unsigned int count;
    unsigned int signalFlag;

  public:
    BoundedBuffer( const unsigned int size = 10 ){
        BoundedBuffer::size = size;
        count = 0;                              // Initialize count
        signalFlag = 0;                         // Initialize signalFlag
    }

    void insert( T elem ){

        cout << "before first lock acauire producer" << endl;
        mlk.acquire();                          // acquire mutual exclusion locker
        cout << "after first lock acauire producer" << endl;

        if (signalFlag) {                   
            cout << "before barging wait producer" << endl;
            bargingTaskLock.wait(mlk);          // bargingTask waits if signaled tasks are being restarted 
            cout << "after barging wait producer" << endl;
        }

        if (count == size){                     

            if (!bargingTaskLock.empty()){      // if bargingTaskLock is not empty, signal bargingTask
                bargingTaskLock.signal();
            }


            cout << "before producer wait" << endl;
            producerLock.wait(mlk);             // wait if buffer is full
            cout << "after producer wait" << endl;

            if (bargingTaskLock.empty()){       // if bargingTaskLock is empty, reset signalFlag to 0
                signalFlag = 0;
            }
        }

        assert(count < size );

        boundedBuffer.push(elem);
        count++;

        if (!consumerLock.empty()){             // signal if there are consumers waiting, and set signalFlag to 1
            signalFlag = 1;
            consumerLock.signal();
        }

        else if (!bargingTaskLock.empty()){     // if no consumers are waiting, signal bargingTask before exiting buffer
            bargingTaskLock.signal();
        }

        else {
            cout << "producer: here signal is: " << signalFlag << endl;
            signalFlag = 0;
        }

        mlk.release();                          // release mutual exclusion locker
    }

    T remove(){

        cout << "before first lock acauire consumer" << endl;
        mlk.acquire();                          // acquire mutual exclusion locker
        cout << "after first lock acauire consumer" << endl;

        if (signalFlag) {                       // bargingTask waits if signaled tasks are being restarted
            cout << "before barging wait consumer" << endl;
            bargingTaskLock.wait(mlk);
            cout << "after barging wait consumer" << endl;
        }

        if (count == 0){

            if (!bargingTaskLock.empty()){      // if bargingTaskLock is not empty, signal bargingTask
                bargingTaskLock.signal();
            }

            cout << "before consumer wait" << endl;
            consumerLock.wait(mlk);             // wait if buffer is empty
            cout << "after consumer wait" << endl;

            if (bargingTaskLock.empty()){       // if bargingTaskLock is empty, reset signalFlag to 0
                signalFlag = 0;
            }
        }

        assert(count > 0);

        T front = boundedBuffer.front();
        boundedBuffer.pop();
        count--;

        if (!producerLock.empty()){             // signal if there are consumers waiting, and set signalFlag to 1
            signalFlag = 1;
            producerLock.signal();
        }

        else if (!bargingTaskLock.empty()){     // if no consumers are waiting, signal bargingTask before exiting buffer
            bargingTaskLock.signal();
        }

        else {
            cout << "consumer here signal is: " << signalFlag << endl;
            signalFlag = 0;
        }

        mlk.release();

        return front;
    }
};
#endif // NOBUSY

_Task Producer {
    BoundedBuffer<int> &boundedBuffer;

    const int produce;
    const int delay;

    void main(){
        unsigned int seed = getpid();
        unsigned int yieldNum = rand_r(&seed) % delay;
        yield(yieldNum);                        
        for (int i = 1; i <= produce; i++){
            boundedBuffer.insert(i);
        }
    }

  public:
    Producer( BoundedBuffer<int> &buffer, const int Produce, const int Delay ): 
        boundedBuffer(buffer), produce(Produce), delay(Delay){}
};

_Task Consumer {

    BoundedBuffer<int> &boundedBuffer;

    const int delay;
    const int sentinel;
    int &sum;

    void main(){
        unsigned int seed = getpid();
        int yieldNum = rand_r(&seed) % delay;
        yield(yieldNum);

        for (;;){
            int item = boundedBuffer.remove();
            if (item == sentinel) break;
            sum = sum + item;
        }
    }

  public:
    Consumer( BoundedBuffer<int> &buffer, const int Delay, const int Sentinel, int &sum ): 
        boundedBuffer (buffer), delay (Delay), sentinel (Sentinel), sum(sum){}
    
};
