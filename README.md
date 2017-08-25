# Concurrent Arduino
This library is a full-fledged multitasking scheduler for AVR (Arduino) chips.  

The implemented multitasking scheduler is a Cooperative Earliest Deadline First (CEDF) Scheduler.  

### By using this library you fully acknowledge the advantages, side-effects and drawbacks of CEDF:  
**Cooperative**: All tasks must be well-behaved<sup>**[1]**</sup> by themselves since there is no way to force terminate functions.  
**Earliest Deadline First**: Tasks that are the closest to the deadline have priority over others (except those who have an inherent higher priority)  

###### [1] *By well behaved I mean tasks should have a somewhat predictable execution time, if the execution time fluctuates violently and randomly (eg. 300ms, 16ms, 5725ms, 4822ms, 2ms), the scheduler will not be able to predict*<sup>**[2]**</sup> *the run time and pause the misbehaving task if there are higher priority tasks waiting.*  
###### [2] *This scheduler uses Exponential Moving Average Smoothing and First-Order (Linear) Interpolation to predict future task execution time.*

# Features

* Supports recurrent, n-iteration and run-once tasks
* Very easy to use with a chainable builder pattern
* Supports an arbitrary amount of priority levels *(Default is 3)*
* Supports an arbitrary amount of 'simultaneous' tasks *(Default is 30, or 10 per priority level)*
* Dynamic parameters *(Change the task properties at will)*
* Power saving when Idle
* Creation of time-critical, periodic or lenient tasks
* Prediction of future task execution time.
* Automatic pausing of tasks that would affect higher priority tasks.

Future features:

* Logging
* Resource query (Show all processes, CPU usage and memory usage)

# Theory

### Seven things are customisable on each task:

**1. Priority**

   Higher priority tasks run first. Tasks closest to the deadline run first.

**2. Recurring Interval**

   When to run tasks. If task is iterative or recurrent, the scheduler will try to run the tasks after each interval.

**3. Initial Wait**

   When to start running the task. This is the initial wait time before the tasks start. Useful if you need a task to start before another, or waiting for a sensor/attachment to spin up.

**4. Iteration Limit**

  Imposes a hard limit on the task. The task will stop after reaching the iteration limit. Useful for making a loop without hogging all the resources.

**5. Time Strictness**

  This allows the scheduler to know if the task is time-sensitive. If yes, the scheduler will try its best to meet the deadline and run the task at exactly the time specified. If no, the scheduler can run the task ahead or behind its time when there are free CPU cycles.      
  Is task time-dependent and time-sensitive? If you are interfacing a morse-code machine, you would need time strictness to update the speaker/solenoid. Otherwise if you are just getting a temperature reading 6 times per minute, do you really need it to be exactly at 0, 10, 20, 30, 40, 50, 60 seconds? Or it is OK to run the task at 1, 9, 22, 30, 41, 53, 59 seconds?
  
  For now, it is only On/Off. In the future you can expect to see an "alpha" value that defines the strictness.
  
  ![Time Strictness Comparison Timeline](https://github.com/bloc97/ConcurrentArduino/blob/master/img/Strict.png "Time Strictness Comparison Timeline")

  ###### Time Strictness is enabled by default on all tasks unless specified.

**6. Catch-up**

  This allows the scheduler to know if a task needs to catch-up if it is late. If yes, a task that was not able to run for the past 5 intervals will run 5 times consecutively when the CPU is available. If no, the scheduler will drop those 5 last runs and continue on.
  
  If you are implementing a counter, you need Catch-up, since if the task missed its deadline 5 times, it will need to add 5 times when the CPU is available. But if you are implementing a real-time interface, catching up is useless since the event (say a button press) already happened and you missed it. No need to check the button press 5 times in a row within nanoseconds.
  
  ![Catch-up Comparison Timeline](https://github.com/bloc97/ConcurrentArduino/blob/master/img/Catchup.png "Catch-up Comparison Timeline")
  
  ###### Catch-up is enabled by default on all tasks unless specified.*


**7. Periodicity Strictness**

  This allows the scheduler to know if a task depends on its periodicity. If yes, a task will be targeted to run at specific intervals, independently of when it actually ran. If no, a task that is late will make all subsequent runs late by the same amount.
  
  Time sensitive applications will need periodicity strictness, but not all applications.
  
  ![Periodicity Strictness Comparison Timeline](https://github.com/bloc97/ConcurrentArduino/blob/master/img/Periodicity.png "Periodicity Strictness Comparison Timeline")
  
  ###### Periodicity Strictness is enabled by default on all tasks unless specified.

**Note: Background Tasks**

  By turning 5, 6 and 7 off, then setting the recurring interval to 0, you can create a 'background task'. This task will be ran as often as possible whenever the CPU is not used by higher priority tasks. It is recommended to put background tasks at the lowest priority.

# Benchmarks

  I know this scheduler is not as lightweight than other similar libraries, but by trading some efficiency, you get better features and better management of idle CPU resources.

(TODO)

# Usage
It is very easy to get started.

```cpp
#include <JobScheduler.h>
#include <Runnable.h>
#include <RunnableBuilder.h>

JobScheduler scheduler; //Creates the scheduler (It is recommended to only have one)

void customFunction() {
  //Custom code here
}

void setup() {
  RunnableBuilder(customFunction).setRecurrent(1000).start(scheduler, 1);
  /*
    This will run customFunction() each second with a medium priority
    
    0 is highest priority, 2 is lowest (by default)
    You can add priority levels by changing the constant "PRIORITY_NUM = 3" in JobScheduler.h
    
    By default, the JobScheduler can fit 10 Tasks per Priority Queue, if a queue is full, adding a task will do nothing.
    You can change the size of the queue by changing the constant "MAX_JOBS = 10" in JobScheduler.h
  */
}

void loop() {
  scheduler.run(); //Run the scheduler in the loop, not recommended to run anything else in the main loop as it might interfere with the scheduler.
}

```

# Examples

Nothing teaches faster than examples.

### Recurrent Tasks

Start a medium priority 1-second recurrent task after waiting 2 seconds.  
`RunnableBuilder(customFunction).setRecurrent(1000, 2000).start(scheduler, 1);`

Start a high priority 100-microsecond recurrent task immediately.  
`RunnableBuilder(customFunction).setRecurrentMicroseconds(100).start(scheduler, 0);`

Starts a low priority background task by disabling Strict Time, Catch-up and Strict Periodicity, and setting the interval to 0.  
`RunnableBuilder(customFunction).setRecurrent(0).setIsStartTimeStrict(false).setDoCatchup(false).setIsPeriodicityStrict(false).start(scheduler, 2);`

### Iterative Tasks

Start a medium priority 5-second iterative task for 10 runs immediately.  
`RunnableBuilder(customFunction).setLoop(5000, 10).start(scheduler, 1);`

### Run-Once Tasks

Start a high priority one-time task immediately.  
`RunnableBuilder(customFunction).setOnce().start(scheduler, 0);`

Start a low priority one-time task after 50ms.  
`RunnableBuilder(customFunction).setOnce(50).start(scheduler, 2);`

### Building without starting

You can build a Runnable and save it to a pointer without starting it yet.  
`Runnable * myRunnable = RunnableBuilder(customFunction).setOnce().build();`  
You can then start it using another task or an event like so.  
`scheduler.execute(runnable, 1); //Starts the Runnable using priority 1`  
Or like so  
```cpp
scheduler.add(runnable, 1);
runnable->start();
```

# Reference

(TODO)
