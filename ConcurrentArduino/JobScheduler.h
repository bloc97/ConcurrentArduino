#ifndef JobScheduler_h
#define JobScheduler_h

#include "Runnable.h"

class JobScheduler {
	
    public:
        
        bool run();
        bool add(Runnable * runnable, int priority);
        bool execute(Runnable * runnable, int priority);
        
        void collectGarbage();
        void collectGarbage(int priority);
		
    private:
        
        static const int MAX_JOBS = 10;
        static const int PRIORITY_NUM = 3;

        Runnable *priorities[PRIORITY_NUM][MAX_JOBS];
        unsigned int indexes[PRIORITY_NUM];
        
};

#endif