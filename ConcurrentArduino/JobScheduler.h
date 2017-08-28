#ifndef JobScheduler_h
#define JobScheduler_h

#include "Runnable.h"

class JobScheduler {
	
    public:
        
        bool run();
        bool add(Runnable * runnable, int priority);
        bool remove(Runnable * runnable, int priority);
        int indexOf(Runnable * runnable, int priority);
        bool destroy(Runnable * runnable);
        bool changePriority(Runnable * runnable, int oldPriority, int newPriority);
        bool execute(Runnable * runnable, int priority);
        bool executeSynchronised(Runnable * runnable, int priority, unsigned long modulo);
        
        
        void collectGarbage();
        void collectGarbage(int priority);
		
    private:
        
        static const int MAX_JOBS = 10;
        static const int PRIORITY_NUM = 3;

        Runnable *priorities[PRIORITY_NUM][MAX_JOBS];
        unsigned int indexes[PRIORITY_NUM];
        
};

#endif