//Simple Arduino Project to make a LED Blink

#include <JobScheduler.h>
#include <Runnable.h>
#include <RunnableBuilder.h>

JobScheduler scheduler; //Create the scheduler

int ledPin = 2; //Digital Out Pin 2
bool ledState = false;

void blink() { //Function to toggle LED state
  if (ledState) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
  ledState = !ledState;
}

void setup() {
  pinMode(ledPin, OUTPUT);
  RunnableBuilder(blink).setRecurrent(1000).start(scheduler, 1); //Start the task
}

void loop() {
  scheduler.run();
}
