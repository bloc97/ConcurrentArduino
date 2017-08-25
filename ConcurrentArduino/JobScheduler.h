#ifndef JobScheduler_h
#define JobScheduler_h

#include "Runnable.h"

class JobScheduler {
	
    public:
	
        bool run();
        void add(Runnable *runnable, int priority);
		
    private:
		
        static const int MAX_JOBS = 10;
        static const int PRIORITY_NUM = 3;

        Runnable *priorities[PRIORITY_NUM][MAX_JOBS];
        unsigned int indexes[PRIORITY_NUM];


        void calculateIndexes();

        void collectGarbage();

        bool runStrict(); //Jobs where start time is strict, must happen closest to target Microsecond counter
        bool runNonStrict(); //Jobs where start time is not strict, allows to run on idle cpu
		
};

#endif