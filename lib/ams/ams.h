/*
 * ams.h
 * Embedded System Term Project arduino ams
 *
 * Created by Lee, JaeYoon on 2021/06/13.
 * Copyright © 2021 Lee, JaeYoon. All rights reserved.
 *
*/
#ifndef AMS_H_
#define AMS_H_

#include "Arduino.h"

#define US_TO_CM(t) (t / 29 / 2)

void led_setup(int pin);
void led_on(int pin);
void led_off(int pin);
void led_blink(int pin, int interval);

void buzzer_setup(int pin);
void buzzer_on(int pin);
void buzzer_off(int pin);

void motor_setup(int pmot, int nmot, int mode);
void motor_mov(int pmot, int nmot, int direct, struct buzzer_t buzz);

void ultrasonic_setup(int trig, int echo);
double get_dist(int trig, int echo);
void update_hp(struct ultrasonic_t uso, struct motor_t mot, struct buzzer_t buzz);
void __motor(struct ultrasonic_t uso, double dist, struct motor_t mot, struct buzzer_t buzz);

void switch_setup(int pin);
int is_push(int pin);

void joystick_setup(int pin_x, int pin_y, int sw);

enum mot_direct {
    MOT_DOWN,
    MOT_STOP,
    MOT_UP
};

struct led_t {
    int pin;
    struct {
        void (*setup)(int pin);
        void (*on)(int pin);
        void (*off)(int pin);
        void (*blink)(int pin, int interval);
    } ops;
};

struct buzzer_t {
    int pin;
    struct {
        void (*setup)(int pin);
        void (*on)(int pin);
        void (*off)(int pin);
    } ops;
};

struct motor_t {
    int pmot;
    int nmot;
    struct {
        void (*setup)(int pmot, int nmot, int mode);
        void (*mov)(int pmot, int nmot, int direct, struct buzzer_t buzz);
    } ops;
};

struct ultrasonic_t {
    int trig;
    int echo;
    int atime;
    double hp;
    struct {
        void (*setup)(int trig, int echo);
        double (*get_dist)(int trig, int echo);
        void (*update_hp)(struct ultrasonic_t uso, struct motor_t mot, struct buzzer_t buzz);
        void (*__motor)(struct ultrasonic_t uso, double dist, struct motor_t mot, struct buzzer_t buzz);
    } ops;
};

struct switch_t {
    int pin;
    struct {
        void (*setup)(int pin);
        int (*is_push)(int pin);
    } ops;
};

struct joystick_t {
    int pin_x;
    int pin_y;
    int sw;
    struct {
        void (*setup)(int pin_x, int pin_y, int sw);
    } ops;
};

#endif /* !AMS_H_ */
