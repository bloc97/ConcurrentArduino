#include "Runnable.h"

Runnable * Runnable::createTask(void (*f)(), unsigned long targetIntervalMicrosecond, unsigned long initialWaitMicrosecond, int numTriggers, bool startTimeStrict, bool catchup, bool periodicity) {
	return new Runnable(f, targetIntervalMicrosecond, initialWaitMicrosecond, numTriggers, startTimeStrict, catchup, periodicity);
	
}

Runnable::Runnable(void (*f)(), unsigned long targetIntervalMicrosecond, unsigned long initialWaitMicrosecond, unsigned long numTriggers, bool startTimeStrict, bool catchup, bool periodicity) {
	main = f;
	targetWaitTimeMicrosecond = targetIntervalMicrosecond;
        this->initialWaitMicrosecond = initialWaitMicrosecond;
	nextTargetStartMicros = micros() + initialWaitMicrosecond;
	maxTrigger = numTriggers;
	isStartTimeStrict = startTimeStrict;
	doCatchup = catchup;
	isPeriodicityStrict = periodicity;
	
	lastTargetStartMicros = nextTargetStartMicros - targetIntervalMicrosecond;
	lastStartMicros = 0;
	lastEndMicros = 0;
	averageRunningTimeMicrosecond = 0;
}

void Runnable::run() {
    unsigned long thisTargetStartMicros = nextTargetStartMicros;
    unsigned long thisStartMicros = micros();

    main();

    triggerCount++;
    if (maxTrigger > 0 && triggerCount >= maxTrigger) {
        destroy();
        return;
    }

    unsigned long thisEndMicros = micros();


    if (doCatchup) {
        if (isPeriodicityStrict) {
            nextTargetStartMicros = thisTargetStartMicros + targetWaitTimeMicrosecond;
        } else {
            nextTargetStartMicros = thisTargetStartMicros + (((int)(thisStartMicros - thisTargetStartMicros))%targetWaitTimeMicrosecond ) + targetWaitTimeMicrosecond;
        }
    } else {
        if (isPeriodicityStrict) {
            nextTargetStartMicros = ( thisStartMicros - (((int)(thisStartMicros - thisTargetStartMicros))%targetWaitTimeMicrosecond ) ) + targetWaitTimeMicrosecond;
        } else {
            nextTargetStartMicros = thisStartMicros + targetWaitTimeMicrosecond;
        }
    }
    lastTargetStartMicros = thisTargetStartMicros;
    lastStartMicros = thisStartMicros;
    lastEndMicros = thisEndMicros;

    if (averageRunningTimeMicrosecond == 0) {
        averageRunningTimeMicrosecond = lastEndMicros - lastStartMicros;
    } else {

        averageRunningTimeMicrosecond += (lastEndMicros - lastStartMicros);
        averageRunningTimeMicrosecond /= 2;

    }
}

void Runnable::start() {
    isEnabled = true;
    nextTargetStartMicros = micros() + initialWaitMicrosecond;
}

void Runnable::stop() {
    isEnabled = false;
}

void Runnable::destroy() {
    isEnabled = false;
    isToBeDestroyed = true;
}


void Runnable::setFunction(void(*f)()) {
    main = f;
}

void Runnable::setMaxTrigger(int maxTrigger) {
    maxTrigger = maxTrigger;
}

void Runnable::setNextTargetStart(unsigned long micros) {
    nextTargetStartMicros = micros;
}

void Runnable::setInitialWait(unsigned long micros) {
    initialWaitMicrosecond = micros;
}

void Runnable::setInterval(unsigned long micros) {
    targetWaitTimeMicrosecond = micros;
}

bool Runnable::isRunning() const {
    return isEnabled;
}

bool Runnable::isDestroying() const {
    return isToBeDestroyed;
}
	
bool Runnable::isStrict() const {
    return isStartTimeStrict;
}

bool Runnable::isDoCatchup() const {
    return doCatchup;
}

bool Runnable::isPeriodic() const {
    return isPeriodicityStrict;
}
        
unsigned long Runnable::getLastTargetStart() const {
    return lastTargetStartMicros;
}

unsigned long Runnable::getLastStart() const {
    return lastStartMicros;
}

unsigned long Runnable::getLastEnd() const {
    return lastEndMicros;
}

unsigned long Runnable::getNextTargetStart() const {
    return nextTargetStartMicros;
}

unsigned long Runnable::getLastRunningTime() {
    return lastEndMicros - lastStartMicros;
}

long Runnable::getLastStartOffsetTime() {
    return lastStartMicros - lastTargetStartMicros;
}

unsigned long Runnable::getAverageRunningTime() const {
    return averageRunningTimeMicrosecond;
}
unsigned long Runnable::getPredictedRunningTime() {
    unsigned long lastRunningTime = lastEndMicros - lastStartMicros;
	
    //Using linear interpolation
    return 2 * lastRunningTime - averageRunningTimeMicrosecond;
	
}


unsigned long Runnable::getTargetWaitTime() const {
    return targetWaitTimeMicrosecond;
}
