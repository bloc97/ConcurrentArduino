#ifndef Runnable_h
#define Runnable_h
#include <Arduino.h>


class Runnable {
    public:
		
        static Runnable * createRecurrentTask(void (*f)(), unsigned long targetIntervalMicrosecond, unsigned long initialWaitMicrosecond, bool startTimeStrict, bool catchup, bool periodicity);
        static Runnable * createLoopTask(void (*f)(), unsigned long targetIntervalMicrosecond, unsigned long initialWaitMicrosecond, int numTriggers, bool startTimeStrict, bool catchup, bool periodicity);
        static Runnable * createOneTimeTask(void (*f)(), unsigned long initialWaitMicrosecond, bool startTimeStrict);


        void run();

        void start();
        void stop();
        void destroy();

        bool isRunning() const;
        bool isDestroying() const;

        bool isStrict() const;
        bool doCatchup() const;
        bool isPeriodic() const;

        unsigned long getLastTargetStart() const;
        unsigned long getLastStart() const;
        unsigned long getLastEnd() const;
        unsigned long getNextTargetStart() const;

        unsigned long getTargetWaitTime() const;

        unsigned long getAverageRunningTime() const;

        unsigned long getLastRunningTime();
        unsigned long getPredictedRunningTime();
        long getLastStartOffsetTime();
        
        void setFunction(void(*f)());
        void setMaxTrigger(int maxTrigger);
        void setNextTargetStart(unsigned long micros);
        void setInterval(unsigned long micros);
        void resetTriggerCount();

  
    private:
        unsigned long lastTargetStartMicros;
        unsigned long lastStartMicros;
        unsigned long lastEndMicros;
        unsigned long nextTargetStartMicros;

        unsigned long targetWaitTimeMicrosecond;

        unsigned long averageRunningTimeMicrosecond; //Using Exponential moving average
        //float alpha = 0.5;

        bool isStartTimeStrict = false; //Does the scheduler allow the task to run within a +- margin of the target time, when there is free cpu cycles
        bool doCatchup = false; //Does the scheduler try to catch up to dropped cycles
        bool isPeriodicityStrict = false; //Is the timing very important, false calculates nextTargetStart using thisStart, while true uses the thisTargetStart

        bool isEnabled = false;
        bool isToBeDestroyed = false;

        int maxTrigger; //Does the scheduler destroy the task after a running a certain amount of time, -1 to disable;
        unsigned long triggerCount;

        void (*main)();

        Runnable(void (*f)(), unsigned long targetIntervalMicrosecond, unsigned long initialWaitMicrosecond, int numTriggers, bool startTimeStrict, bool catchup, bool periodicity);


		
};

#endif