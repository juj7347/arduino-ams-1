/*
 * main.ino
 * Embedded System Term Project arduino ams
 *
 * Created by Lee, JaeYoon on 2021/06/13.
 * Copyright © 2021 Lee, JaeYoon. All rights reserved.
 *
*/
#include "low_power.h"
#include "ams.h"

struct led_t led_r = {
    .pin = 3,
    .ops = {
        .setup = led_setup,
        .on = led_on,
        .off = led_off,
        .blink = led_blink
    }
};

struct led_t led_y = {
    .pin = 4,
    .ops = {
        .setup = led_setup,
        .on = led_on,
        .off = led_off,
        .blink = led_blink
    }
};

struct led_t led_g = {
    .pin = 5,
    .ops = {
        .setup = led_setup,
        .on = led_on,
        .off = led_off,
        .blink = led_blink
    }
};

struct buzzer_t buzz = {
    .pin = 7,
    .ops = {
        .setup = buzzer_setup,
        .on = buzzer_on,
        .off = buzzer_off
    }
};

struct motor_t mot = {
    .pmot = 6,
    .nmot = 11,
    .ops = {
        .setup = NULL,
        .mov = motor_mov
    }
};

struct ultrasonic_t uso = {
    .trig = 8,
    .echo = A2,
    .atime = 0,
    .hp = 100.0,
    .ops = {
        .setup = ultrasonic_setup,
        .get_dist = get_dist,
        .update_hp = update_hp,
        .__motor = __motor
    }
};

struct switch_t aut = {
    .pin = 9,
    .ops = {
        .setup = switch_setup,
        .is_push = is_push
    }
};

struct switch_t power = {
    .pin = 10,
    .ops = {
        .setup = switch_setup,
        .is_push = is_push
    }
};

struct joystick_t joy = {
    .pin_x = A0,
    .pin_y = A1,
    .sw = 2,
    .ops = {
        .setup = joystick_setup
    }
};

bool dev_power = false;
bool aut_mode = true;

void setup() {
    led_r.ops.setup(led_r.pin);
    led_y.ops.setup(led_y.pin);
    led_g.ops.setup(led_g.pin);
    buzz.ops.setup(buzz.pin);
    uso.ops.setup(uso.trig, uso.echo);
    aut.ops.setup(aut.pin);
    power.ops.setup(power.pin);
    joy.ops.setup(joy.pin_x, joy.pin_y, joy.sw);
    uso.ops.update_hp(uso, mot, buzz);
}


void loop() {
    if (power.ops.is_push(power.pin))
        dev_power = !dev_power;

    if (dev_power == false) {
        mot.ops.mov(mot.pmot, mot.nmot, MOT_UP, buzz);
        delay(1000);
        led_g.ops.off(led_g.pin);
        led_r.ops.on(led_r.pin);
    }

    while (!dev_power)
        if (power.ops.is_push(power.pin))
            dev_power = !dev_power;

     if (dev_power == true) {
        mot.ops.mov(mot.pmot, mot.nmot, MOT_DOWN, buzz);
        delay(1000);
        led_r.ops.off(led_r.pin);
        led_g.ops.on(led_g.pin);
    }

    if (aut.ops.is_push(aut.pin))
        aut_mode = !aut_mode;

    if (aut_mode == false) {
        while (aut_mode == false) {
            if (analogRead(A0) >= 0 && analogRead(A0) <= 450)
                mot.ops.mov(mot.pmot, mot.nmot, MOT_UP, buzz);
            else if (analogRead(A0) >= 570 && analogRead(A0) <= 1023)
                mot.ops.mov(mot.pmot, mot.nmot, MOT_DOWN, buzz);

            if (aut.ops.is_push(aut.pin))
                aut_mode = !aut_mode;
        }
    }

    led_r.ops.blink(led_r.pin, 300);
    uso.ops.__motor(uso, uso.ops.get_dist(uso.trig, uso.echo), mot, buzz);
    led_r.ops.off(led_r.pin);

    if (uso.atime >= 100) {
        LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    }
}
