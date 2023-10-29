/***************************************************************/
/*                                                             */
/* lizard.cpp                                                  */
/*                                                             */
/* To compile, you need all the files listed below             */
/*   lizard.cpp                                                */
/*                                                             */
/* Be sure to use the -lpthread option for the compile command */
/*   g++ -g -Wall -std=c++11 lizard.cpp -o lizard -pthread     */
/*                                                             */
/* Execute with the -d command-line option to enable debugging */
/* output.  For example,                                       */
/*   ./lizard -d                                               */
/*                                                             */
/***************************************************************/

#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>       
#include <semaphore.h>


using namespace std;

/*
 * This is a stub file.  It contains very little code and what
 * it does contain may need to be altered or removed.  It is
 * only provided for a starting point.
 *
 * The comments are probably useful.
 */


/*
 * Define "constant" values here
 */

/*
 * Make this 1 to check for lizards travelling in both directions
 * Leave it 0 to allow bidirectional travel
 */
#define UNIDIRECTIONAL       0

/*
 * Set this to the number of seconds you want the lizard world to
 * be simulated.  
 * Try 30 for development and 120 for more thorough testing.
 */
#define WORLDEND             30

/*
 * Number of lizard threads to create
 */
#define NUM_LIZARDS          20

/*
 * Number of cat threads to create
 */
#define NUM_CATS             2

/*	
 * Maximum lizards crossing at once before alerting cats
 */
#define MAX_LIZARD_CROSSING  4

/*
 * Maximum seconds for a lizard to sleep
 */
#define MAX_LIZARD_SLEEP     3

/*
 * Maximum seconds for a cat to sleep
 */
#define MAX_CAT_SLEEP        3

/*
 * Maximum seconds for a lizard to eat
 */
#define MAX_LIZARD_EAT       5

/*
 * Number of seconds it takes to cross the driveway
 */
#define CROSS_SECONDS        2


/*
 * Declare global variables here
 */


/**************************************************/
/* Please leave these variables alone.  They are  */
/* used to check the proper functioning of your   */
/* program.  They should only be used in the code */
/* I have provided.                               */
/**************************************************/
int numCrossingSago2MonkeyGrass;
int numCrossingMonkeyGrass2Sago;
int debug;
int running;
/**************************************************/



/**
 * This class models a cat that sleep, wakes-up, checks on lizards in the driveway
 * and goes back to sleep. If the cat sees enough lizards it "plays" with them.
 */
class Cat {
	
	int       _id;        // the Id of the cat
	thread*   _catThread; // the thread simulating the cat
	sem_t*  _sidewalk_sem; 		// AB - semaphore for sidewalk
	mutex*  _running_mutex; 	// AB - mutex for running var
	mutex*  _monkeyGrass2Sago_mtx;			// AB - mutex for numCrossingMonkeyGrass2Sago
	mutex*  _sago2MonkeyGrass_mtx;			// AB - mutex for numCrossingSago2MonkeyGrass
	
	public:
		Cat (int id, sem_t*, mutex*, mutex*, mutex*); // AB - pass in pointers
		int getId();
		void run();
		void wait();
    
    private:
		void sleepNow();

        static void catThread (Cat *aCat); 
};

/**
 * Constructs a cat.
 *
 * @param id - the Id of the cat 
 */
Cat::Cat (int id, sem_t* sem, mutex* mtx, mutex* sago_mtx, mutex* monkey_mtx)
{
	_id = id;
	this->_sidewalk_sem = sem; 					// AB - set sidewalk semaphore
	this->_running_mutex = mtx; 				// AB - set running mutex
	this->_monkeyGrass2Sago_mtx = monkey_mtx; 	// AB - set monkeyGrass2Sago mutex
	this->_sago2MonkeyGrass_mtx = sago_mtx; 	// AB - set sago2MonkeyGrass mutex	

}

/**
 * Returns the Id of the cat.
 *
 * @return the Id of a cat
 */
int Cat::getId()
{
	return _id;
}


/**
 * Launches a cat thread.
 * 
 * Status: Incomplete - Make changes to this code.
 */
 void Cat::run() 
 {
	 // launch the thread to simulate the cat's behavior	 
    this->_catThread = new thread ( catThread, this); 		// AB - init thread on heap
	 
 }
 
 /**
  * Waits for a cat to finish running.
  *
  * Status: Incomplete - Make changes to this code.
  */
 void Cat::wait()
 {
	 // wait for the thread to terminate
	 this->_catThread->join(); 		// AB - wait for the thread
 }
 
 
/**
 * Simulate a cat sleeping for a random amount of time
 *
 *
 * @return N/A
 *
 * Status: Completed - No need to change any of this code.
 */
void Cat::sleepNow()
{
	int sleepSeconds;

	sleepSeconds = 1 + (int)(random() / (double)RAND_MAX * MAX_CAT_SLEEP);

	if (debug)
    {
		cout << "[" << _id << "] cat sleeping for " << sleepSeconds << " seconds" << endl;
		cout << flush;
    }

	sleep( sleepSeconds );

	if (debug)
    {
		cout << "[" << _id << "] cat awake" << endl;
		cout << flush;
    }
}

/**
  *
  * This simulates a cat that is sleeping and occasionally checking on
  * the driveway on lizards.
  * 
  * @param aCat - a cat that is being run concurrently
  * 
  * @return - N/A
  *
  * Status: Incomplete - Make changes as you see are necessary.
  */
void Cat::catThread (Cat *aCat)
{
	
	if (debug)
    {
		cout << "[" << aCat->getId() << "] cat is alive\n";
		cout << flush;
    }

	// AB - wait for the world to start
	aCat->_running_mutex->lock(); 		// AB - lock running var
	while(running == 0)
	{
		aCat->_running_mutex->unlock(); 		// AB - unlock running var
		usleep(1 * 1000);						// AB - sleep for 1ms
		aCat->_running_mutex->lock(); 		// AB - lock running var
	}
	aCat->_running_mutex->unlock(); 		// AB - unlock running var

	aCat->_running_mutex->lock(); 		// AB - lock running var
	while(running)
    {
		aCat->_running_mutex->unlock(); 		// AB - unlock running var
		aCat->sleepNow();


		aCat->_sago2MonkeyGrass_mtx->lock(); 		// AB - lock
		aCat->_monkeyGrass2Sago_mtx->lock(); 		// AB - lock
		/*
	     * Check for too many lizards crossing
	     */
		if (numCrossingSago2MonkeyGrass + numCrossingMonkeyGrass2Sago > MAX_LIZARD_CROSSING)
		{
		  cout << "\tThe cats are happy - they have toys.\n";
		  exit( -1 );
		}
		aCat->_sago2MonkeyGrass_mtx->unlock(); 		// AB - unlock
		aCat->_monkeyGrass2Sago_mtx->unlock(); 		// AB - unlock
		aCat->_running_mutex->lock(); 		// AB - lock running var

    }
	aCat->_running_mutex->unlock(); 		// AB - unlock running var
}

 

class Lizard {
	int     _id;      // the Id of the lizard
	thread* _aLizard; // the thread simulating the lizard

	sem_t*  _sidewalk_sem; 					// AB - semaphore for sidewalk
	mutex*  _running_mutex; 				// AB - mutex for running var
	mutex*  _monkeyGrass2Sago_mtx;			// AB - mutex for numCrossingMonkeyGrass2Sago
	mutex*  _sago2MonkeyGrass_mtx;			// AB - mutex for numCrossingSago2MonkeyGrass

	public:
		Lizard(int id, sem_t*, mutex*, mutex*, mutex*);
		int getId();
        void run();
        void wait();

    private:
		void sago2MonkeyGrassIsSafe();
		void crossSago2MonkeyGrass();
		void madeIt2MonkeyGrass();
		void eat();
		void monkeyGrass2SagoIsSafe();
		void crossMonkeyGrass2Sago();
		void madeIt2Sago();
		void sleepNow();

        static void lizardThread(Lizard *aLizard);
	
};


/**
 * Constructs a lizard.
 *
 * @param id - the Id of the lizard 
 */
Lizard::Lizard (int id, sem_t* sem, mutex* mtx, mutex* sago_mtx, mutex* monkey_mtx)
{
	_id = id;
	this->_sidewalk_sem = sem; 					// AB - set sidewalk semaphore
	this->_running_mutex = mtx; 				// AB - set running mutex
	this->_monkeyGrass2Sago_mtx = monkey_mtx; 	// AB - set monkeyGrass2Sago mutex
	this->_sago2MonkeyGrass_mtx = sago_mtx; 	// AB - set sago2MonkeyGrass mutex	
}

/**
 * Returns the Id of the lizard.
 *
 * @return the Id of a lizard
 */
int Lizard::getId()
{
	return _id;
}

/**
 * Launches a lizard thread.
 * 
 * Status: Incomplete - Make changes to this code.
 */
 void Lizard::run() 
 {
	 // launch the thread to simulate the lizard's behavior
    _aLizard = new thread ( lizardThread, this);
 }
 
 /**
  * Waits for a lizard to finish running.
  *
  * Status: Incomplete - Make changes to this code.
  */
 void Lizard::wait()
 {
	 // wait for the thread to terminate
	if (_aLizard != NULL) {
        _aLizard->join();
    } 
 }
 





/**
 * Simulate a lizard sleeping for a random amount of time
 *
 * Status: Completed - No need to change any of this code.
 */
void Lizard::sleepNow()
{
	int sleepSeconds;

	sleepSeconds = 1 + (int)(random() / (double)RAND_MAX * MAX_LIZARD_SLEEP);

	if (debug)
    {
      cout << "[" << _id << "] sleeping for " << sleepSeconds << " seconds" << endl;
      cout << flush;
    }

	sleep( sleepSeconds );

	if (debug)
    {
      cout << "[" << _id << "] awake" << endl;
      cout << flush;
    }
}



 
/**
 *
 * Returns when it is safe for this lizard to cross from the sago
 * to the monkey grass.   Should use some synchronization 
 * facilities (lock/semaphore) here.
 *
 * Status: Incomplete - Make changes as you see are necessary.
 */
void Lizard::sago2MonkeyGrassIsSafe()
{
	if (debug)
    {
		cout << "[" << _id << "] checking  sago -> monkey grass" << endl;
		cout << flush;
    }


	sem_wait(this->_sidewalk_sem); 		// AB - wait on sidewalk semaphore


	if (debug)
    {
		cout << "[" << _id << "] thinks  sago -> monkey grass  is safe" << endl;
		cout << flush;
    }   
	
}


/**
 *
 * Delays for 1 second to simulate crossing from the sago to
 * the monkey grass. 
 *
 * Status: Incomplete - Make changes as you see are necessary.
 */
void Lizard::crossSago2MonkeyGrass()
{
	if (debug)
    {
      cout << "[" << _id << "] crossing  sago -> monkey grass" << endl;
      cout << flush;
    }

	/*
	 * One more crossing this way
	 */
	this->_sago2MonkeyGrass_mtx->lock(); 		// AB - lock 
	numCrossingSago2MonkeyGrass++;
	this->_sago2MonkeyGrass_mtx->unlock(); 		// AB - unlock 

	/*
     * Check for lizards cross both ways
     */
	if (numCrossingMonkeyGrass2Sago && UNIDIRECTIONAL)
    {
		cout << "\tCrash!  We have a pile-up on the concrete." << endl;
		cout << "\t" << numCrossingSago2MonkeyGrass << " crossing sago -> monkey grass" << endl;
		cout << "\t" << numCrossingMonkeyGrass2Sago << " crossing monkey grass -> sago" << endl;
		exit( -1 );
    }


	/*
     * It takes a while to cross, so simulate it
     */
	sleep( CROSS_SECONDS );

    /*
     * That one seems to have made it
     */
	this->_sago2MonkeyGrass_mtx->lock(); 		// AB - lock 
    numCrossingSago2MonkeyGrass--;
	this->_sago2MonkeyGrass_mtx->unlock(); 		// AB - unlock 
}


/**
 * Tells others they can go now
 *
 * Status: Incomplete - Make changes as you see are necessary.
 */
void Lizard::madeIt2MonkeyGrass()
{
	sem_post(this->_sidewalk_sem); 				// AB - release semaphore
	/*
     * Whew, made it across
     */
	if (debug)
    {
		cout << "[" << _id << "] made the  sago -> monkey grass  crossing" << endl;
		cout << flush;
    }





}


/**
 * Simulate a lizard eating for a random amount of time
 *
 * Status: Completed - No need to change any of this code.
 */
void Lizard::eat()
{
	int eatSeconds;

	eatSeconds = 1 + (int)(random() / (double)RAND_MAX * MAX_LIZARD_EAT);

	if (debug)
    {
		cout << "[" << _id << "] eating for " << eatSeconds << " seconds" << endl;
		cout << flush;
    }

	/*
     * Simulate eating by blocking for a few seconds
     */
	sleep( eatSeconds );

	if (debug)
    {
      cout << "[" << _id << "] finished eating" << endl;
      cout << flush;
    }
}


/**
 * Returns when it is safe for this lizard to cross from the monkey
 * grass to the sago.   Should use some synchronization 
 * facilities (lock/semaphore) here.
 *
 * Status: Incomplete - Make changes as you see are necessary.
 */
void Lizard::monkeyGrass2SagoIsSafe()
{
	if (debug)
    {
		cout << "[" << _id << "] checking  monkey grass -> sago" << endl;
		cout << flush;
    }


	sem_wait(this->_sidewalk_sem); 		// AB - wait on sidewalk semaphore


	if (debug)
    {
		cout << "[" << _id << "] thinks  monkey grass -> sago  is safe" << endl;
		cout << flush;
    }
	
}



/**
 * Delays for 1 second to simulate crossing from the monkey
 * grass to the sago. 
 *
 * Status: Incomplete - Make changes as you see are necessary.
 */
void Lizard::crossMonkeyGrass2Sago()
{
	if (debug)
    {
		cout << "[" << _id << "] crossing  monkey grass -> sago" << endl;
		cout << flush;
    }

    /*
     * One more crossing this way
     */
	this->_monkeyGrass2Sago_mtx->lock(); 		// AB - lock 
	numCrossingMonkeyGrass2Sago++;
	this->_monkeyGrass2Sago_mtx->unlock(); 		// AB - unlock

  
    /*
     * Check for lizards cross both ways
     */
	if (numCrossingSago2MonkeyGrass && UNIDIRECTIONAL)
    {
		cout << "\tOh No!, the lizards have cats all over them." << endl;
		cout << "\t " << numCrossingSago2MonkeyGrass << " crossing sago -> monkey grass" << endl;
		cout << "\t " << numCrossingMonkeyGrass2Sago << " crossing monkey grass -> sago" << endl;
		exit( -1 );
    }

	/*
     * It takes a while to cross, so simulate it
     */
	sleep( CROSS_SECONDS );

	/*
     * That one seems to have made it
     */
	this->_monkeyGrass2Sago_mtx->lock(); 		// AB - lock 
	numCrossingMonkeyGrass2Sago--;
	this->_monkeyGrass2Sago_mtx->unlock(); 		// AB - unlock
}


/**
 *
 * Tells others they can go now
 *
 * Status: Incomplete - Make changes as you see are necessary.
 */
void Lizard::madeIt2Sago()
{
	sem_post(this->_sidewalk_sem);		// AB - release semaphore
	/*
     * Whew, made it across
     */
	if (debug)
    {
		cout << "[" << _id << "] made the  monkey grass -> sago  crossing" << endl;
		cout << flush;
    }
}

/**
  * Follows the algorithm provided in the assignment
  * description to simulate lizards crossing back and forth
  * between a sago palm and some monkey grass. 
  *  
  * @param aLizard - the lizard to be executed concurrently
  *
  * @return N/A
  
  * Status: Incomplete - Make changes as you see are necessary.
  */
void Lizard::lizardThread(Lizard *aLizard)
{	
	if (debug)
    {
      cout << "[" << aLizard->getId() << "] lizard is alive" << endl;
      cout << flush;
    }

	// AB - wait for the world to start
	aLizard->_running_mutex->lock(); 		// AB - lock running var
	while(running == 0)
	{
		aLizard->_running_mutex->unlock(); 		// AB - unlock running var
		usleep(1 * 1000);						// AB - sleep for 1ms
		aLizard->_running_mutex->lock(); 		// AB - lock running var
	}
	aLizard->_running_mutex->unlock(); 		// AB - unlock running var

	
	aLizard->_running_mutex->lock(); 		// AB - lock running var
	while(running)
    {
		aLizard->_running_mutex->unlock(); 		// AB - unlock running var
      /* 
       * Follow the algorithm given in the assignment
       * using calls to the functions declared above.
       * You'll need to complete the implementation of
       * some functions by filling in the code.  Some  
       * are already completed - see the comments.
       */

		aLizard->sago2MonkeyGrassIsSafe();
		aLizard->crossSago2MonkeyGrass();
		aLizard->madeIt2MonkeyGrass();
		aLizard->eat();

		aLizard->monkeyGrass2SagoIsSafe();
		aLizard->crossMonkeyGrass2Sago();
		aLizard->madeIt2Sago();
		aLizard->sleepNow();

		aLizard->_running_mutex->lock(); 		// AB - lock running var
    }
	aLizard->_running_mutex->unlock(); 		// AB - unlock running var

}
 



/*
 * main()
 *
 * Should initialize variables, locks, semaphores, etc.
 * Should start the cat thread and the lizard threads
 * Should block until all threads have terminated
 * Status: Incomplete - Make changes to this code.
 */
int main(int argc, char **argv)
{
	/*
	 * Declare local variables
     */
	sem_t sidewalk_sem; 						// AB - limit amount of lizards on sidewalk
	mutex running_mtx; 						// AB - mutex for running var
	mutex sago2MonkeyGrass_mtx; 				// AB - mutex for numCrossingSago2MonkeyGrass
	mutex monkeyGrass2Sago_mtx; 				// AB - mutex for numCrossingMonkeyGrass2Sago

	/*
     * Check for the debugging flag (-d)
     */
	debug = 0;
	if (argc > 1 && argv[1] != NULL && (strncmp(argv[1], "-d", 2) == 0))
    debug = 1;


	/*
     * Initialize variables
     */
	numCrossingSago2MonkeyGrass = 0;
	numCrossingMonkeyGrass2Sago = 0;
	running = 1;


	/*
     * Initialize random number generator
	 */
	srandom( (unsigned int)time(NULL) );


	/*
     * Initialize locks and/or semaphores
     */
	sem_init(&sidewalk_sem, 0, MAX_LIZARD_CROSSING); 		// AB - init sidewalk semaphore



	/*
     * Create NUM_LIZARDS lizard threads
     */
    vector<Lizard*> allLizards;
    for (int i=0; i < NUM_LIZARDS; i++) {
	    allLizards.push_back(new Lizard(i, &sidewalk_sem, &running_mtx, &sago2MonkeyGrass_mtx, &monkeyGrass2Sago_mtx));
    }    
    

    /*
     * Create NUM_CATS cat threads
     */
    vector<Cat*> allCats; 		// AB - create vector of cat threads
    for (int i=0; i < NUM_CATS; i++) {
	    allCats.push_back(new Cat(i, &sidewalk_sem, &running_mtx, &sago2MonkeyGrass_mtx, &monkeyGrass2Sago_mtx));
    }    
    

	/*
	 * Run NUM_LIZARDS and NUM_CATS threads
	 */
    for (int i=0; i < NUM_LIZARDS; i++) {
        allLizards[i]->run();
    }
    for (int i=0; i < NUM_CATS; i++) {
        allCats[i]->run();
    }

	/*
     * Now let the world run for a while
     */
	running_mtx.lock(); 		// AB - lock running var
	running = 1;
	running_mtx.unlock(); 	// AB - unlock running var
	sleep( WORLDEND );


	/*
     * That's it - the end of the world
     */
	running_mtx.lock(); 		// AB - lock running var
	running = 0;
	running_mtx.unlock(); 	// AB - unlock running var

    /*
     * Wait until all threads terminate
     */
	for (int i=0; i < NUM_LIZARDS; i++) { 		// AB - wait on all threads
		allLizards[i]->wait();
	}
	for (int i=0; i < NUM_CATS; i++) {
		allCats[i]->wait();
	}






	/*
     * Delete the locks and semaphores
     */
	 sem_destroy(&sidewalk_sem); 		// AB - destroy sidewalk semaphore

	 
	 
	/*
	 * Delete all cat and lizard objects
	 */
	for (int i=0; i < NUM_LIZARDS; i++) { 		// AB - delete all threads
		delete allLizards[i];
	}
	for (int i=0; i < NUM_CATS; i++) {
		delete allCats[i];
	}



	/*
     * Exit happily
     */
	return 0;
}






