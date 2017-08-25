#include "Runnable.h"
#include "JobScheduler.h"

void JobScheduler::calculateIndexes() {
	
	unsigned long currentTime = micros();
	
	for (int i=0; i<PRIORITY_NUM; i++) {
		
		long minimumTime = 2147483647L;
		indexes[i] = -1;
		
		for (int n=0; n<MAX_JOBS; n++) {
			if (priorities[i][n] != NULL) {
				if (priorities[i][n]->isRunning() && priorities[i][n]->isStrict()) {
					long thisTime = priorities[i][n]->getNextTargetStart() - currentTime;
					
					if (thisTime < minimumTime) {
						minimumTime = thisTime;
						indexes[i] = n;
					}
				}
			}
		}
	}
	
	
}


void JobScheduler::collectGarbage() {
	for (int i=0; i<PRIORITY_NUM; i++) {
		for (int n=1; n<MAX_JOBS; n++) {
			if (priorities[i][n] != NULL) {
				if (priorities[i][n]->isDestroying()) {
					delete priorities[i][n];
					priorities[i][n] = NULL;
				}
			}
			
		}
	}
	
}

bool JobScheduler::run() {
	collectGarbage();
	calculateIndexes();
	
	if (runStrict()) {
		return true;
	}
	calculateIndexes();
	return runNonStrict();
	
}

void JobScheduler::add(Runnable *runnable, int priority) {
	if (priority < PRIORITY_NUM) {
		for (int n=0; n<MAX_JOBS; n++) {
			if (priorities[priority][n] == NULL) {
				priorities[priority][n] = runnable;
				return;
			}
		}
	}
	
}

bool JobScheduler::runStrict() {
	unsigned long currentTime = micros();
	
	long maximumAllocatedTime = 2147483647L; //Time allowed for the next job to run without affecting a job of higher priority
	//Starts as max long value, as 0 is the highest priority
	
	for (int i=0; i<PRIORITY_NUM; i++) {
		
		if (indexes[i] != -1) { //If there is a strict job in this priority
			
			//indexes[i] is the job that is the most urgent
			long diffTime = priorities[i][indexes[i]]->getNextTargetStart() - currentTime; //Time difference between the Target time and now
			
			if (diffTime <= 0 && priorities[i][indexes[i]]->getPredictedRunningTime() < maximumAllocatedTime) {
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
		
		if (indexes[i] != -1) { //If there is a strict job in this priority
			
			//indexes[i] is the job that is the most urgent
			long diffTime = priorities[i][indexes[i]]->getNextTargetStart() - currentTime; //Time difference between the Target time and now
			maximumAllocatedTime = min(maximumAllocatedTime, diffTime); //The minimum of the diffTimes is the maximum execution time allocated for the Non Strict jobs
			
		}
		for (int n=0; n<MAX_JOBS; n++) {
			
			if (priorities[i][n] != NULL) { //If the job is not null
				if (!priorities[i][n]->isStrict()) { //If the job is not strict
					
					long diffTimeNonStrict = priorities[i][n]->getNextTargetStart() - currentTime; //Time difference between the Target time and now
					
					long halfTargetWaitTime = priorities[i][n]->getTargetWaitTime() / 2;
					//Serial.println(diffTimeNonStrict);
					//Serial.println(halfTargetWaitTime);
					if (diffTimeNonStrict <= halfTargetWaitTime && priorities[i][n]->getPredictedRunningTime() < maximumAllocatedTime) {
						priorities[i][n]->run();
						return true;
					}
					maximumAllocatedTime = min(maximumAllocatedTime, diffTimeNonStrict); //The minimum of the diffTimes is the maximum execution time allocated for the lower priority non strict jobs
				}
			}
		}
	}
	//Serial.println(maximumAllocatedTime/1000);
	if (maximumAllocatedTime > 1000) {
		if (maximumAllocatedTime < 100000000L) {
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