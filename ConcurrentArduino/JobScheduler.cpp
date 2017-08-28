#include "Runnable.h"
#include "JobScheduler.h"

void JobScheduler::preStep() {
	
    unsigned long currentTime = micros();

    for (int i=0; i<PRIORITY_NUM; i++) {

        long minimumTime = 2147483647L; //Earliest deadline first
        indexes[i] = -1;

        for (int n=0; n<MAX_JOBS; n++) {
            if (priorities[i][n] != NULL) {
                if (priorities[i][n]->isRunning()) {
                    
                    long thisTime = priorities[i][n]->getNextTargetStart() - currentTime;
                    
                    if (!priorities[i][n]->isStrict()) { //If task is not time strict
                        thisTime = thisTime + (priorities[i][n]->getTargetWaitTime()/2); //Add leniency in the target time;
                    }

                    if (thisTime < minimumTime) {
                        minimumTime = thisTime;
                        indexes[i] = n;
                    }
                } else if (priorities[i][n]->isDestroying()) { //Garbage collection
                    delete priorities[i][n];
                    priorities[i][n] = NULL;
                }
            }
        }
    }

	
}

bool JobScheduler::run() {
    preStep();

    if (runStrict()) {
        return true;
    }
    preStep();
    return runNonStrict();
	
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

void JobScheduler::execute(Runnable * runnable, int priority) {
    if (add(runnable, priority)) {
        runnable->start();
    }
}

bool JobScheduler::runStrict() {
    unsigned long currentTime = micros();

    long maximumAllocatedTime = 2147483647L; //Time allowed for the next job to run without affecting a job of higher priority
    //Starts as max long value, as 0 is the highest priority

    for (int i=0; i<PRIORITY_NUM; i++) {

        if (indexes[i] != -1) { //If there is a job with a deadline in this priority

            //indexes[i] is the job that is the most urgent
            long diffTime = priorities[i][indexes[i]]->getNextTargetStart() - currentTime; //Time difference between the Target time and now

            if (!priorities[i][indexes[i]]->isStrict()) { //If job is not strict
                diffTime = diffTime + (priorities[i][indexes[i]]->getTargetWaitTime()/2); //Add leniency in the target time;
            }
            
            if (diffTime <= 0 && priorities[i][indexes[i]]->getPredictedRunningTime() < maximumAllocatedTime) { //If target time has passed and the predicted running time won't exceed maximum allocated time
                priorities[i][indexes[i]]->run();
                return true;
            }

            maximumAllocatedTime = min(maximumAllocatedTime, diffTime); //The minimum of the diffTimes is the maximum execution time allocated for the lower priorities

        }
    }


    return false;
}

bool JobScheduler::runNonStrict() {
    unsigned long currentTime = micros();

    long maximumAllocatedTime = 2147483647L; //Time allowed for the next job to run without affecting a job of higher priority
    //Starts as max long value, as 0 is the highest priority

    for (int i=0; i<PRIORITY_NUM; i++) {

        if (indexes[i] != -1) { //If there is a job with a deadline in this priority

            //indexes[i] is the job that is the most urgent
            long diffTime = priorities[i][indexes[i]]->getNextTargetStart() - currentTime; //Time difference between the Target time and now
            
            if (!priorities[i][indexes[i]]->isStrict()) { //If job is not strict
                diffTime = diffTime + (priorities[i][indexes[i]]->getTargetWaitTime()/2); //Add leniency in the target time;
            }
            
            maximumAllocatedTime = min(maximumAllocatedTime, diffTime); //The minimum of the diffTimes is the maximum execution time allocated for the Non Strict jobs

        }
        for (int n=0; n<MAX_JOBS; n++) {

            if (priorities[i][n] != NULL) { //If the job is not null
                if (!priorities[i][n]->isStrict()) { //If the job is not strict

                    long diffTimeNonStrict = priorities[i][n]->getNextTargetStart() - currentTime; //Time difference between the Target time and now
                    long halfTargetWaitTime = priorities[i][n]->getTargetWaitTime() / 2; //Half of the wait time
                    
                    if (diffTimeNonStrict <= halfTargetWaitTime && priorities[i][n]->getPredictedRunningTime() < maximumAllocatedTime) { //If the time is close to the target time, and running time won't exceed maximum allocated time
                        priorities[i][n]->run();
                        return true;
                    }
                    maximumAllocatedTime = min(maximumAllocatedTime, diffTimeNonStrict); //The minimum of the diffTimes is the maximum execution time allocated for the lower priority non strict jobs
                }
            }
        }
    }
    //If CPU is to be Idle, sleep for a while
    if (maximumAllocatedTime > 1000) {
        if (maximumAllocatedTime < 100000000L) { //Don't sleep for more than 10 seconds
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