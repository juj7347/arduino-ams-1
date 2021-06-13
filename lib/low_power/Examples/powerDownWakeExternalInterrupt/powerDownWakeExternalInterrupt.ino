#include "low_power.h"

const int wakeUpPin = 2;

void wakeUp() {

}

void setup() {
    pinMode(wakeUpPin, INPUT);
}

void loop() {
    attachInterrupt(0, wakeUp, LOW);
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
    detachInterrupt(0);
}