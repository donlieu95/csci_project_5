//Project5.cpp
//Centralized algorithm
//Don Lieu and Zeph Johnson
//ei9700fv and

#include <iostream>
#include <string>
#include <mutex>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#define CHOPSTICKS 5  //Zeph: this probably needs to be sent through the process creation
 
pthread_mutex_t Out = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Stick = PTHREAD_MUTEX_INITIALIZER;

void println (char *s)
{
	pthread_mutex_lock (&Out);
	printf ("%s", s);
	pthread_mutex_unlock (&Out);
}


class Scheduler
{//Zeph: useless class that might be useful for the token ring alg?
	private:
		int next;
		bool chopstick[CHOPSTICKS];//True if available for use, false if taken
	public:
		Scheduler ();
		Scheduler (int);
		bool getChopstick (int chopstick1, int chopstick2);
		void putChopstick (int chopstick1, int chopstick2);
		void initialize();
};

Scheduler::Scheduler ()
{
	next = 0;
	bool chopstick[CHOPSTICKS];
}
bool Scheduler::getChopstick(int chopstick1, int chopstick2)
{
	if ( (chopstick[chopstick1] == true) && (chopstick[chopstick2] == true) )
	{
		chopstick[chopstick1] = false;
		chopstick[chopstick2] = false;
		return true;
	}
	else
	{
		return false;
	}
}
void Scheduler::putChopstick(int chopstick1, int chopstick2)
{
	chopstick[chopstick1] = true;
	chopstick[chopstick2] = true;
}
void Scheduler::initialize()
{
	for (int i = 0; i < CHOPSTICKS; i++)
	{
		chopstick[i] = true;
	}
}

class Process 
{
    private:
      int id;
      Scheduler* scheduler;
      int eatingTime;
      int thinkingTime;
    public:
      Process (int, Scheduler*);
      void operator () ();
      void run ();
};

Process::Process (int identifier, Scheduler* schd)
{
	id = identifier;
	scheduler = schd;
	srand (id);
	eatingTime = rand() % 10;
	thinkingTime = rand() % 5;
}

void Process::run () 
{
	char rstr[100];
	
	sprintf (rstr, "Process %d thinking...\n", id);
	println ( rstr );	
	sleep(thinkingTime);
	int ch1, ch2;
	if (id == CHOPSTICKS-1)
	//Don: cleaned up chopstick assignment for philosophers
	{
		ch1 = id;
		ch2 = 0;
	}
	else
	{
		ch1 = id;
		ch2 = id+1;
	}
	
	//Zeph: implemented mutual exclusion, cleaned up the next few lines using % instead of if statements.
	//While either nearby chopstick isn't available, sleep:
	pthread_mutex_lock (&Stick); //mutual exclusion for checking/changing chopsticks
	
	while ( scheduler->getChopstick(ch1, ch2) == false )
    	{
        	pthread_mutex_unlock (&Stick); //unlock while sleeping
        	sleep(1);
        	pthread_mutex_lock (&Stick); //lock again to check chopsticks then take if necessary
	}
	
	//eat after taking chopsticks:
	pthread_mutex_unlock (&Stick);
	sprintf (rstr, "Process %d eating with chopsticks %d and %d...\n", id, ch1, ((ch2) % (CHOPSTICKS)));
	println ( rstr );
	sleep(eatingTime); //simulate eating
	
	//return chopsticks:
	pthread_mutex_lock (&Stick);
	scheduler->putChopstick(ch1, ch2);
	pthread_mutex_unlock (&Stick);
    	sprintf (rstr, "Process %d finished eating and returned chopsticks %d and %d\n", id, ch1, ((ch2) % (CHOPSTICKS)));
	println ( rstr );
}
 
void *callRun (void* process)
{
 	((Process *)process) -> run();
}

int main (int argc, const char * argv[])
{
	int err;
	char rstr[100];
	pthread_t tidArray[CHOPSTICKS];
	Process *processArray[CHOPSTICKS];

	
	Scheduler* schd = new Scheduler (); 

	schd->initialize();
	for(int i = 0; i < CHOPSTICKS; i++)
	{
		processArray[i] = new Process(i, schd);
		err = pthread_create(&tidArray[i], NULL, callRun, processArray[i]);
		if (err)
		{
			sprintf (rstr, "Create of thread %d failed.\n", i);
			println (rstr);
			exit(i);
		}
	}
	
	sprintf (rstr, "All processes are active.\n");
	println (rstr);
    
	for(int i = 0; i < CHOPSTICKS; i++)
	{
		pthread_join(tidArray[i], NULL);
	}
	
	sprintf (rstr, "All processes have completed.\n");
	println (rstr);
	
	return 0;
}
