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
  Is task time-dependent and time-sensitive? If you are making a clock, you would need time strictness to update the internal values. Otherwise if you are just getting a temperature reading 6 times per minute, do you really need it to be exactly at 0, 10, 20, 30, 40, 50, 60 seconds? Or it is OK to run the task at 1, 9, 22, 30, 41, 53, 59 seconds?
  
  For now, it is only On/Off. In the future you can expect to see an "alpha" value that defines the strictness.

**6. Catch-up**

  This allows the scheduler to know if a task needs to catch-up if it is late. If yes, a task that was not able to run for the past 5 intervals will run 5 times consecutively when the CPU is available. If no, the scheduler will drop those 5 last runs and continue on.
  
  If you are implementing a counter, you need Catch-up, since if the task missed its deadline 5 times, it will need to add 5 times when the CPU is available. But if you are implementing a real-time interface, catching up is useless since the event (say a button press) already happened and you missed it. No need to check the button press 5 times in a row within nanoseconds.

**7. Periodicity Strictness**

# Benchmarks

(TODO)
