#include <uC++.h>

template<typename T> _Coroutine Binsertsort {
    const T Sentinel;   // value denoting end of set
    T value;            // communication: value being passed down/up the tree
    T pivot;
    void main(){

        // First resume starts here
        pivot = value;

        // Base case
        if (pivot == Sentinel) { 
            suspend();
            return;                             // It's a coroutine that hasn't been started, terminate
        } 

        Binsertsort less(Sentinel);             // Create a left coroutine 
        Binsertsort greater(Sentinel);          // Create a right coroutine 

        for ( ;; ){
            suspend();

            if (value == Sentinel) break;       // Signal of end of unsorted values

            else if (value <= pivot){ 
                less.sort( value );
            }

            else{ 
                greater.sort( value );
            }
        }

        less.sort(Sentinel);                    // Signal left branch 
        greater.sort(Sentinel);                 // Signal right branch

        suspend();

        // Receive sorted values from left branch          
        for ( ;; ){
            value = less.retrieve();
            if (value == Sentinel) break;       // Signal of end-of-sorted-values from left branch

            suspend();
        }

        // Pass back pivot value
        value = pivot;
        suspend();

        // Receieve sorted values from right branch
        for ( ;; ){
            value = greater.retrieve();
            if (value == Sentinel) break;       // Signal of end-of-sorted-values from right branch

            suspend();
        }

        value = Sentinel;
        return;                                 // Done with this coroutine, terminate
    }

    public:
    Binsertsort( T Sentinel ) : Sentinel( Sentinel ) {}

    void sort( T value ) {                      // value to be sorted
        Binsertsort::value = value;
        resume();
    }

    T retrieve() {                              // retrieve sorted value
        resume();
        return value;
    }
};
