#include "Runnable.h"
#include "JobScheduler.h"

void JobScheduler::collectGarbage() {
    for (int i=0; i<PRIORITY_NUM; i++) {
        collectGarbage(i);
    }
}

void JobScheduler::collectGarbage(int priority) {
    if (priority < PRIORITY_NUM) {
        for (int n=1; n<MAX_JOBS; n++) {
            if (priorities[priority][n] != NULL) {
                if (priorities[priority][n]->isDestroying()) {
                    delete priorities[priority][n];
                    priorities[priority][n] = NULL;
                }
            }
        }
    }
}

int JobScheduler::indexOf(Runnable * runnable, int priority) {
    if (priority < PRIORITY_NUM) {
        for (int n=0; n<MAX_JOBS; n++) {
            if (priorities[priority][n] == runnable) {
                return n;
            }
        }
    }
    return -1;
}

bool JobScheduler::add(Runnable * runnable, int priority) {
    if (priority < PRIORITY_NUM) {
        for (int n=0; n<MAX_JOBS; n++) {
            if (priorities[priority][n] == NULL) {
                priorities[priority][n] = runnable;
                return true;
            }
        }
    }
    return false;
}

bool JobScheduler::remove(Runnable * runnable, int priority) {
    if (priority < PRIORITY_NUM) {
        for (int n=0; n<MAX_JOBS; n++) {
            if (priorities[priority][n] == runnable) {
                priorities[priority][n] = NULL;
                return true;
            }
        }
    }
    return false;
}

bool JobScheduler::destroy(Runnable * runnable) {
    bool isDestroyed = false;
    for (int i=0; i<PRIORITY_NUM; i++) {
        for (int n=0; n<MAX_JOBS; n++) {
            if (priorities[i][n] == runnable) {
                delete priorities[i][n];
                priorities[i][n] = NULL;
                isDestroyed = true;
            }
        }
    }
    return isDestroyed;
}

bool JobScheduler::changePriority(Runnable * runnable, int oldPriority, int newPriority) {
    int index = indexOf(runnable, oldPriority);
    if (index > -1) { //If exists
        if (add(runnable, newPriority)) { //If task was successfully added
            priorities[oldPriority][index] = NULL;
            return true;
        }
    }
    return false;
}
        
bool JobScheduler::execute(Runnable * runnable, int priority) {
    if (add(runnable, priority)) {
        runnable->start();
        return true;
    }
    return false;
}

bool JobScheduler::executeSynchronised(Runnable * runnable, int priority, unsigned long modulo) {
    if (add(runnable, priority)) {
        runnable->start();
        runnable->setNextTargetStart(micros() - (micros%modulo) + runnable->getInitialWaitTime());
        return true;
    }
    return false;
}
    
bool JobScheduler::run() {
    unsigned long currentTime = micros();

    long maximumAllocatedTime = 2147483647L; //Time allowed for the next job to run without affecting a job of higher priority
    //Starts as max long value, as 0 is the highest priority

    for (int i=0; i<PRIORITY_NUM; i++) {

        long minimumTime = 2147483647L; //Earliest deadline first
        long minimumTimeUnstrict = 2147483647L; //Earliest deadline first
        indexes[i] = -1;
        int unStrictIndex = -1;

        for (int n=0; n<MAX_JOBS; n++) { //Find the task with earliest deadline
            if (priorities[i][n] != NULL) {
                if (priorities[i][n]->isRunning()) {
                    
                    long thisTime = priorities[i][n]->getNextTargetStart() - currentTime;
                    
                    if (!priorities[i][n]->isStrict()) { //If task is not time strict
                        if (thisTime < minimumTimeUnstrict) { //Sort the earliest unstrict task
                            minimumTimeUnstrict = thisTime;
                            unStrictIndex = n;
                        }
                        thisTime = thisTime + (priorities[i][n]->getTargetWaitTime()/2); //Add leniency in the target time;
                    }

                    if (thisTime < minimumTime) { //Sort the earliest strict task
                        minimumTime = thisTime;
                        indexes[i] = n;
                    }
                } else if (priorities[i][n]->isDestroying()) { //Garbage collection
                    delete priorities[i][n];
                    priorities[i][n] = NULL;
                }
            }
        }
        
        if (indexes[i] != -1) { //If there is a job with a deadline in this priority

            //indexes[i] is the job that is the most urgent
            /*
            long diffTime = priorities[i][indexes[i]]->getNextTargetStart() - currentTime; //Time difference between the Target time and now

            if (!priorities[i][indexes[i]]->isStrict()) { //If job is not strict
                diffTime = diffTime + (priorities[i][indexes[i]]->getTargetWaitTime()/2); //Add leniency in the target time;
            }*/
            
            //if (diffTime <= 0 && priorities[i][indexes[i]]->getPredictedRunningTime() < maximumAllocatedTime) {
            if (minimumTime <= 0 && priorities[i][indexes[i]]->getPredictedRunningTime() < maximumAllocatedTime) { //If target time has passed and the predicted running time won't exceed maximum allocated time
                priorities[i][indexes[i]]->run();
                return true;
            }

            //maximumAllocatedTime = min(maximumAllocatedTime, diffTime); //The minimum of the diffTimes is the maximum execution time allocated for the lower priorities
            maximumAllocatedTime = min(maximumAllocatedTime, minimumTime); //The minimum of the diffTimes is the maximum execution time allocated for the lower priorities

        }
        
        if (unStrictIndex != -1) { //If there are leftover Unstrict tasks in this priority
            //long diffTimeNonStrict = priorities[i][unStrictIndex]->getNextTargetStart() - currentTime; //Time difference between the Target time and now
            long halfTargetWaitTime = priorities[i][unStrictIndex]->getTargetWaitTime() / 2; //Half of the wait time

            //if (diffTimeNonStrict <= halfTargetWaitTime && priorities[i][unStrictIndex]->getPredictedRunningTime() < maximumAllocatedTime) {
            if (minimumTimeUnstrict <= halfTargetWaitTime && priorities[i][unStrictIndex]->getPredictedRunningTime() < maximumAllocatedTime) { //If the time is close to the target time, and running time won't exceed maximum allocated time
                priorities[i][unStrictIndex]->run();
                return true;
            }
            //maximumAllocatedTime = min(maximumAllocatedTime, diffTimeNonStrict); //The minimum of the diffTimes is the maximum execution time allocated for the lower priority non strict jobs
        }
        
    }

    //If CPU is to be Idle, sleep for a while
    if (maximumAllocatedTime > 1000) {
        if (maximumAllocatedTime < 10000000L) { //Don't sleep for more than 10 seconds
            delay(maximumAllocatedTime/1000);
            delayMicroseconds(maximumAllocatedTime%1000);
        } else {
            delay(10000);
        }
    } else {
        delayMicroseconds(maximumAllocatedTime);
    }
    return false;
}
