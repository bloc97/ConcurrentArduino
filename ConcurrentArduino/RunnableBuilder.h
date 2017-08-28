/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RunnableBuilder.h
 * Author: bowen
 *
 * Created on August 25, 2017, 3:05 PM
 */


#ifndef RUNNABLEBUILDER_H
#define RUNNABLEBUILDER_H


#include "Runnable.h"


class RunnableBuilder {

public:
    
    
    
    RunnableBuilder(void (*f)()) {
        this->f = f;
    }

    RunnableBuilder & setRecurrent(unsigned long interval) {
        return setRecurrentMicroseconds(interval*1000);
    }
    RunnableBuilder & setRecurrent(unsigned long interval, unsigned long initialWait) {
        return setRecurrentMicroseconds(interval*1000, initialWait*1000);
    }
    RunnableBuilder & setLoop(unsigned long interval, unsigned long loopCount) {
        return setLoopMicroseconds(interval*1000, loopCount);
    }
    RunnableBuilder & setLoop(unsigned long interval, unsigned long initialWait, unsigned long loopCount) {
        return setLoopMicroseconds(interval*1000, initialWait*1000, loopCount);
    }
    RunnableBuilder & setOnce() {
        return setOnceMicroseconds();
    }
    RunnableBuilder & setOnce(unsigned long initialWait) {
        return setOnceMicroseconds(initialWait*1000);
    }


    RunnableBuilder & setRecurrentMicroseconds(unsigned long intervalMicroseconds) {
        this->interval = intervalMicroseconds;
        this->maxTrigger = 0;
        return *this;
    }
    RunnableBuilder & setRecurrentMicroseconds(unsigned long intervalMicroseconds, unsigned long initialWaitMicroseconds) {
        this->interval = intervalMicroseconds;
        this->initialWait = initialWaitMicroseconds;
        this->maxTrigger = 0;
        return *this;
    }
    RunnableBuilder & setLoopMicroseconds(unsigned long intervalMicroseconds, unsigned long loopCount) {
        this->interval = intervalMicroseconds;
        this->maxTrigger = loopCount;
        return *this;
    }
    RunnableBuilder & setLoopMicroseconds(unsigned long intervalMicroseconds, unsigned long initialWaitMicroseconds, unsigned long loopCount) {
        this->interval = intervalMicroseconds;
        this->initialWait = initialWaitMicroseconds;
        this->maxTrigger = loopCount;
        return *this;
    }
    RunnableBuilder & setOnceMicroseconds() {
        this->maxTrigger = 1;
        return *this;
    }
    RunnableBuilder & setOnceMicroseconds(unsigned long initialWaitMicroseconds) {
        this->initialWait = initialWaitMicroseconds;
        this->maxTrigger = 1;
        return *this;
    }

    /*
    RunnableBuilder & setInitialWait(unsigned long initialWait) {
        setInitialWaitMicroseconds(initialWait * 1000);
    }
    RunnableBuilder & setInitialWaitMicroseconds(unsigned long initialWaitMicroseconds) {
        this->initialWait = initialWaitMicroseconds;
        return *this;
    }

    RunnableBuilder & setInterval(unsigned long interval) {
        setIntervalMicroseconds(interval*1000);
    }
    RunnableBuilder & setIntervalMicroseconds(unsigned long intervalMicroseconds) {
        this->interval = intervalMicroseconds;
        return *this;
    }*/

    RunnableBuilder & setLoopCount(unsigned long loopCount) {
        this->maxTrigger = loopCount;
        return *this;
    }

    RunnableBuilder & setIsStartTimeStrict(boolean isStartTimeStrict) {
        this->isStartTimeStrict = isStartTimeStrict;
        return *this;
    }

    RunnableBuilder & setDoCatchup(boolean doCatchup) {
        this->doCatchup = doCatchup;
        return *this;
    }

    RunnableBuilder & setIsPeriodicityStrict(boolean isPeriodicityStrict) {
        this->isPeriodicityStrict = isPeriodicityStrict;
        return *this;
    }

    Runnable * build() {
        return new Runnable(f, interval, initialWait, maxTrigger, isStartTimeStrict, doCatchup, isPeriodicityStrict);
    }

    Runnable * start(JobScheduler & scheduler, int priority) {
        if (interval == 0) {
            doCatchup = false;
            isPeriodicityStrict = false;
        }
        Runnable * runnable = build();
        runnable->start();
        scheduler.add(runnable, priority);
        return runnable;
    }
    
    Runnable * startSynchronised(JobScheduler & scheduler, int priority, unsigned long modulo) {
        return startSynchronisedMicroseconds(scheduler, priority, modulo * 1000);
    }
    
    Runnable * startSynchronisedMicroseconds(JobScheduler & scheduler, int priority, unsigned long modulo) {
        Runnable * runnable = start(scheduler, priority);
        unsigned long microCount = micros();
        runnable->setNextTargetStart(microCount - (microCount % modulo) + initialWait);
        return runnable;
    }

private:

    void (*f)();

    unsigned long initialWait = 0;
    unsigned long interval = 1000;

    bool isStartTimeStrict = true;
    bool doCatchup = true;
    bool isPeriodicityStrict = true;

    unsigned long maxTrigger = 1;
};

#endif /* RUNNABLEBUILDER_H */

