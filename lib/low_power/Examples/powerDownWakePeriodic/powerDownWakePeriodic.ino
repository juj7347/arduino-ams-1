#include "low_power.h"

void setup() {

}

void loop() {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}
