#ifndef Runnable_h
#define Runnable_h
#include <Arduino.h>


class Runnable {
public:

    Runnable(void (*f)(), unsigned long targetIntervalMicrosecond, unsigned long initialWaitMicrosecond, unsigned long numTriggers, bool startTimeStrict, bool catchup, bool periodicity);
    
    void run();

    void start();
    void stop();
    void destroy();

    bool isRunning() const;
    bool isDestroying() const;

    bool isStrict() const;
    bool isDoCatchup() const;
    bool isPeriodic() const;

    unsigned long getLastTargetStart() const;
    unsigned long getLastStart() const;
    unsigned long getLastEnd() const;
    unsigned long getNextTargetStart() const;

    unsigned long getTargetWaitTime() const;

    unsigned long getAverageRunningTime() const;
    
    
    unsigned long getMaxTrigger() const;
    unsigned long getTriggerCount() const;

    unsigned long getLastRunningTime();
    unsigned long getPredictedRunningTime();
    long getLastStartOffsetTime();

    void setFunction(void(*f)());
    void setProperties(bool isStrict, bool doCatchup, bool isPeriodic);
    void setMaxTrigger(int maxTrigger);
    void setNextTargetStart(unsigned long micros);
    void setInitialWait(unsigned long micros);
    void setInterval(unsigned long micros);
    void resetTriggerCount();
   
    
private:
    unsigned long lastTargetStartMicros;
    unsigned long lastStartMicros;
    unsigned long lastEndMicros;
    unsigned long nextTargetStartMicros;

    unsigned long initialWaitMicrosecond;
    unsigned long targetWaitTimeMicrosecond;

    unsigned long averageRunningTimeMicrosecond; //Using Exponential moving average
    //float alpha = 0.5;

    bool isStartTimeStrict = false; //Does the scheduler allow the task to run within a +- margin of the target time, when there is free cpu cycles
    bool doCatchup = false; //Does the scheduler try to catch up to dropped cycles
    bool isPeriodicityStrict = false; //Is the timing very important, false calculates nextTargetStart using thisStart, while true uses the thisTargetStart

    bool isEnabled = false;
    bool isToBeDestroyed = false;

    unsigned long maxTrigger; //Does the scheduler destroy the task after a running a certain amount of time, 0 to disable;
    unsigned long triggerCount;

    void (*main)();

};

#endif