/*
 * ams.cpp
 * Embedded System Term Project arduino ams
 *
 * Created by Lee, JaeYoon on 2021/06/13.
 * Copyright © 2021 Lee, JaeYoon. All rights reserved.
 *
*/
#include "ams.h"

void led_setup(int pin) {
    pinMode(pin, OUTPUT);
}

void led_on(int pin) {
    digitalWrite(pin, HIGH);
}

void led_off(int pin) {
    digitalWrite(pin, LOW);
}

void led_blink(int pin, int interval) {
    digitalWrite(pin, HIGH);
    delay(interval);
    digitalWrite(pin, LOW);
}

void buzzer_setup(int pin) {
    pinMode(pin, OUTPUT);
}

void buzzer_on(int pin) {
    digitalWrite(pin, HIGH);
}

void buzzer_off(int pin) {
    digitalWrite(pin, LOW);
}

void motor_setup(int pmot, int nmot, int mode) {
}

void motor_mov(int pmot, int nmot, int direct, struct buzzer_t buzz) {
    switch (direct) {
        case MOT_DOWN:
            analogWrite(nmot, 100);
            analogWrite(pmot, 0);
            buzz.ops.on(buzz.pin);
            break;
        case MOT_STOP:
            analogWrite(nmot, 0);
            analogWrite(pmot, 0);
            buzz.ops.off(buzz.pin);
            break;
        case MOT_UP:
            analogWrite(nmot, 0);
            analogWrite(pmot, 100);
            buzz.ops.on(buzz.pin);
            break;
        default:
            break;
    }
}

void ultrasonic_setup(int trig, int echo) {
    pinMode(trig, OUTPUT);
    pinMode(echo, INPUT);
}

double get_dist(int trig, int echo) {
    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);
    return US_TO_CM(pulseIn(echo, HIGH));
}

void update_hp(struct ultrasonic_t uso, struct motor_t mot,
               struct buzzer_t buzz) {
    while (1) {
        double dist1 = uso.ops.get_dist(uso.trig, uso.echo);
        double dist2 = uso.ops.get_dist(uso.trig, uso.echo);
        double dist3 = uso.ops.get_dist(uso.trig, uso.echo);
        double dist4 = uso.ops.get_dist(uso.trig, uso.echo);
        
        if ((dist1 + dist2 + dist3 + dist4) / 4 <= 100) {
            buzz.ops.off(buzz.pin);
            mot.ops.mov(mot.pmot, mot.nmot, MOT_STOP, buzz);
            break;
        } else {
            buzz.ops.on(buzz.pin);
            mot.ops.mov(mot.pmot, mot.nmot, MOT_DOWN, buzz);
        }
    }
}

void __motor(struct ultrasonic_t uso, double dist, struct motor_t mot,
             struct buzzer_t buzz) {
    if(uso.hp + 5 < dist) {
        mot.ops.mov(mot.pmot, mot.nmot, MOT_DOWN, buzz);
        uso.atime = 0;
    } else if(dist < uso.hp - 5) {
        mot.ops.mov(mot.pmot, mot.nmot, MOT_UP, buzz);
        uso.atime = 0;
    } else {
        mot.ops.mov(mot.pmot, mot.nmot, MOT_STOP, buzz);
        ++uso.atime;
    }
}

void switch_setup(int pin) {
    pinMode(pin, INPUT_PULLUP);
}

int is_push(int pin) {
    return digitalRead(pin);
}

void joystick_setup(int pin_x, int pin_y, int sw) {
    pinMode(pin_x, INPUT);
    pinMode(pin_y, INPUT);
}
