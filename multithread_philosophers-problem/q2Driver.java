import java.util.Calendar;
import java.util.HashMap;
import java.util.Map;
import java.util.Random;

//======================================================================
// Random Number Generator Class
//======================================================================

class PRNG {
	volatile static Random generator = new Random();
	public static void setSeed( long seed ) {
		generator.setSeed( seed );
	}
	
	public static int random(int a){
		return generator.nextInt(a);
	}
}

//======================================================================
//  Printer Monitor
//======================================================================

class Printer {

	private class PhilosopherInfo{
		public int id;
		public boolean empty;                           // true if empty
		public char state;                     
		public int biteOrLeftFork;
		public int noodlesOrRightFork;

		public PhilosopherInfo(int id, boolean empty) {
			this.id = id;
			this.empty = empty;
		}
	};

	private int noOfPhil;
	Map<Integer, PhilosopherInfo> buffer = 
		new HashMap<Integer, PhilosopherInfo>();         // store Philosopher's information

	public Printer(int noOfPhil ){
		this.noOfPhil = noOfPhil;

		for  ( int i = 0; i < noOfPhil; i++){
			PhilosopherInfo temp = new PhilosopherInfo(i, true);
			buffer.put(i, temp);
		}  

		for ( int i = 0; i < noOfPhil; i++){
			PhilosopherInfo temp = buffer.get(i);
			System.out.print("Phil" + temp.id + "\t");
		}

		System.out.print("\n");

		for ( int i = 0; i < noOfPhil; i++){
			System.out.print("******\t");
		}

		System.out.print("\n");
	}
	
	private synchronized void flush(){
		// for unfinished Philosopher tasks
		for (int i = 0; i < noOfPhil; i++){
			PhilosopherInfo temp = buffer.get(i);
			
			// If empty PhilosopherInfo, output empty column
			if (temp.empty){
				System.out.print("\t");
			}

			// If not empty
			else if (temp.state == Philosopher.States.Thinking.val
					||
					temp.state == Philosopher.States.Hungry.val){
				System.out.print(temp.state + "\t");
			} 

			else if (temp.state == Philosopher.States.Eating.val
					||
					temp.state == Philosopher.States.Waiting.val){

				System.out.print(Character.toString(temp.state) + 
						temp.biteOrLeftFork +
						"," + 
						temp.noodlesOrRightFork + 
				"\t");
			}

			temp.empty = true;
			buffer.put(i,  temp);
		}
		System.out.print("\n");
	}

	private synchronized void flushFinished(int id){
		// for finished Philosopher tasks
		for ( int i = 0; i < noOfPhil; i++){
			PhilosopherInfo temp = buffer.get(i);

			if (i != id){
				System.out.print("...\t");
			}
			else {
				assert(temp.state == Philosopher.States.Finished.val);
				System.out.print(temp.state + "\t");
			}
			temp.empty = true;
			buffer.put(i, temp);
		}
		System.out.print("\n");
	}

	public synchronized void print(int id, Philosopher.States state ){
		PhilosopherInfo stateOnly = buffer.get(id);
		if(!stateOnly.empty){
			flush();
		}
		else if (state == Philosopher.States.Finished){
			flush();
		}

		stateOnly.state = state.val;
		stateOnly.empty = false;
		buffer.put(id, stateOnly);

		if (state == Philosopher.States.Finished){
			flushFinished(id);
		}
	}

	public synchronized void print(int id, Philosopher.States state, int bite, int noodles ){
		PhilosopherInfo temp = buffer.get(id);
		if(!temp.empty){
			flush();
		}
		temp.state = state.val;
		temp.biteOrLeftFork = bite;
		temp.noodlesOrRightFork = noodles;    
		temp.empty = false;
		buffer.put(id, temp);
	}
}

//======================================================================
//  Philosopher Thread 
//======================================================================

class Philosopher extends Thread {

	private int id;
	private int noodles;
	private Table table;
	private Printer printer;

	public enum States {
		Thinking( 'T' ), Hungry( 'H' ), Eating( 'E' ), Waiting( 'W' ), Finished( 'F' );
		public final char val;
		States( char v ) { val = v; }
	} // Philosopher.states


	public Philosopher(int id, int noodles, Table table, Printer prt){
		this.id = id;
		this.noodles = noodles;
		this.table = table;
		this.printer = prt;
	}
	
	private void yield(int yieldNum){
		for( int i = 0; i < yieldNum; i++) yield(); 
	}

	public void run(){
		for ( ; ; ){
			printer.print(id, Philosopher.States.Hungry);
			
			int yieldNum = PRNG.random(5);
			yield(yieldNum);
			
			table.pickup(id);

			int bite = PRNG.random(5) + 1;
			if (bite > noodles) bite = noodles;
			noodles = noodles - bite;
			
			printer.print(id, Philosopher.States.Eating, bite, noodles);
			
			yieldNum = PRNG.random(5);
			yield(yieldNum);

			table.putdown(id);

			if (noodles == 0) break;

			printer.print(id, Philosopher.States.Thinking);

			yieldNum = PRNG.random(20);
			yield(yieldNum);
		}
		
		printer.print(id, Philosopher.States.Finished);
	}
} // Philosopher


//======================================================================
//  Table Monitor
//======================================================================

class Table {
	private int noOfPhil;
	private Printer printer;
	private boolean states[];                                           // true for eating, false for not eating
	private int tickets[];          

	public Table(int noOfPhil, Printer prt) {
		this.noOfPhil = noOfPhil;
		this.printer = prt;
		states = new boolean[noOfPhil];
		for (int i = 0; i < noOfPhil; i++){
			states[i] = false;                                          // Initialize all states as not eating
		}

		tickets = new int[noOfPhil];
		for (int i = 0; i < noOfPhil; i++){
			tickets[i] = Integer.MAX_VALUE;                             // Initialize all tickets as maximum integer
		}
	}

    private synchronized int left(int id){
        return (id + noOfPhil -1)%noOfPhil;
    }

    private synchronized int right(int id){
        return (id + 1)%noOfPhil;
    }

	public synchronized void pickup(int id) {
		for (int i = 0; ; i++){
            if (tickets[id] == Integer.MAX_VALUE){                      // Get ticket if doesn't have one
                tickets[id] = 0;
                int max = 0;
                for(int j=0;j<noOfPhil;j++) {                           // for largest ticket
                    if ( max < tickets[j] 
                            && tickets[j] < Integer.MAX_VALUE ) 
                        max = tickets[j];
                }
                tickets[id] = max + 1;                                  // advance ticket
            }

            for ( int j = 0; j < noOfPhil ; j += 1 ) {                  // check tickets
                while( tickets[j] < tickets[id] ||
                        (tickets[j] == tickets[id] && j < id) ){
                    try {
                        notifyAll();                                    // Let barging philosophers wait here
                        wait();
                    } catch (InterruptedException e) {
                    }
                        }
            }

            if ( !states[left(id)] && !states[right(id)] ){
                states[id] = true;
                assert(!states[left(id)] && !states[right(id)]);
                tickets[id] = Integer.MAX_VALUE;
                return;
            }
            else{
                try {                                                   // print waiting message once before waiting
                    if (i==0) printer.print(id, Philosopher.States.Waiting, id, right(id));
                    wait();
                } catch (InterruptedException e) {
                }
            }
        }
    }  // pickup

	public synchronized void putdown(int id) {
        assert(states[id]);
		states[id] = false;
		notifyAll();
	} // putdown
} // Table


//======================================================================
//  q2Driver Class
//======================================================================

public class q2Driver {

	public static void usage() {
		System.out.println( "Usage: " + q2Driver.class.getName() +
				" [ philosophers (> 1) " +
				"[ noodles (> 0) " +
				"[ Seed (> 0) " +
		"] ] ]");
		System.exit(1);			            // TERMINATE
	} // usage

	public static void main(String[] argv) throws InterruptedException {
		Calendar cal = Calendar.getInstance();
		int philosopherNum = 5;                             // Default value of number of philosopher
		int noodleNum = 30;                                 // Default value of number of producers 
		long seed = cal.getTimeInMillis();                  // Default value of seed

		switch ( argv.length ) {
		case 3:
			try {
				seed = Long.parseLong(argv[2]);
				if (seed <= 0) usage();
			} catch (Exception e){
				usage ();
			}
			// FALL THROUGH
		case 2:
			try {
				noodleNum = Integer.parseInt(argv[1]);
				if (noodleNum <= 0) usage();
			} catch (Exception e){
				usage ();
			}
			// FALL THROUGH
		case 1:
			try {
				philosopherNum = Integer.parseInt(argv[0]);
				if (philosopherNum <= 1) usage();
			} catch (Exception e){
				usage ();
			}
			// FALL THROUGH
		case 0:
			break;
		default:						                                // wrong number of options
			usage();
		} // switch


		// Initialize prng seed
		PRNG.setSeed(seed);

		Printer printer = new Printer(philosopherNum);
		Table table = new Table(philosopherNum, printer);
		Philosopher philosophers[] = new Philosopher[philosopherNum];

		for (int i = 0; i < philosopherNum; i++){                       // start threads
			philosophers[i] = new Philosopher(i, noodleNum, table, printer);
			philosophers[i].start();
		}

		for (int i = 0; i < philosopherNum; i++){                       // wait for threads to finish
			philosophers[i].join();
		}

		System.out.println("***********************");
		System.out.println("Philosophers terminated");
	}
}
