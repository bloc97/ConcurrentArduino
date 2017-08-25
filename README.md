# Concurrent Arduino
This library is a full-fledged scheduler for AVR (Arduino) chips.  

The implemented scheduler is a Cooperative Earliest Deadline First (CEDF) Scheduler.  

### By using this library you fully acknowledge the advantages, side-effects and drawbacks of CEDF:  
**Cooperative**: All tasks must be well-behaved<sup>**[1]**</sup> by themselves since there is no way to force terminate functions.  
**Earliest Deadline First**: Tasks that are the closest to the deadline have priority over others (except those who have an inherent higher priority)  

[1] *By well behaved I mean tasks should have a somewhat predictable execution time, if the execution time fluctuates violently and randomly (eg. 300ms, 16ms, 5725ms, 4822ms, 2ms), the scheduler will not be able to predict*<sup>**[2]**</sup> *the run time and pause the misbehaving task if there are higher priority tasks waiting.*  
[2] *This scheduler uses Exponential Moving Average Smoothing and First-Order (Linear) Interpolation to predict future task execution time.*

# Features
