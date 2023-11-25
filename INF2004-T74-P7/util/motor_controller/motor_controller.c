/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#include "motor_controller.h"

// Define GPIO pins for motor control
#define LEFT_REVERSE_PIN 14
#define LEFT_CLOCKWISE_PIN 15
#define RIGHT_REVERSE_PIN 18
#define RIGHT_CLOCKWISE_PIN 19

// Define motor-related constants
#define LEFT_MOTOR_PIN 2
#define RIGHT_MOTOR_PIN 3
#define PWM_CYCLE_COUNT 10000.0f

// Initialize all Motor Controller pins
void initialize_gpio_pins() {
    gpio_init(LEFT_REVERSE_PIN);
    gpio_init(LEFT_CLOCKWISE_PIN);
    gpio_init(RIGHT_REVERSE_PIN);
    gpio_init(RIGHT_CLOCKWISE_PIN);
    gpio_set_dir(LEFT_REVERSE_PIN, GPIO_OUT);
    gpio_set_dir(LEFT_CLOCKWISE_PIN, GPIO_OUT);
    gpio_set_dir(RIGHT_REVERSE_PIN, GPIO_OUT);
    gpio_set_dir(RIGHT_CLOCKWISE_PIN, GPIO_OUT);
}

// Update PWM parameters to set the speed of a specified motor by modifying the duty cycle
void update_speed(uint8_t slice_num, enum pwm_chan channel, float duty_cycle) {
    pwm_set_chan_level(slice_num, channel, PWM_CYCLE_COUNT * duty_cycle);
}

// Retrieves the slice number attached to a specific motor
uint8_t get_slice_num(int motor_pin) {
    return pwm_gpio_to_slice_num(motor_pin);
}

// Starts up the motors with default PWM parameters
void enable_motors(uint8_t left_slice_num, uint8_t right_slice_num) {
    // Allocate the GPIO pins of the left and right motor to the PWM
    gpio_set_function(LEFT_MOTOR_PIN, GPIO_FUNC_PWM);
    gpio_set_function(RIGHT_MOTOR_PIN, GPIO_FUNC_PWM);

    // Set the divider of the PWM clock
    pwm_set_clkdiv(left_slice_num, 100);
    pwm_set_clkdiv(right_slice_num, 100);

    // Set the highest value for the PWM before the clock wraps around back to 0
    pwm_set_wrap(left_slice_num, PWM_CYCLE_COUNT);
    pwm_set_wrap(right_slice_num, PWM_CYCLE_COUNT);

    // Set the default duty cycles of both PWM channels to be 50%
    pwm_set_chan_level(left_slice_num, PWM_CHAN_A, PWM_CYCLE_COUNT * 0.7);
    pwm_set_chan_level(right_slice_num, PWM_CHAN_B, PWM_CYCLE_COUNT * 0.7);

    // Set the respective PWMs to run
    pwm_set_enabled(left_slice_num, true);
    pwm_set_enabled(right_slice_num, true);
}

// Enables the car to stop
void stop_motors(uint8_t left_slice_num, uint8_t right_slice_num) {
    pwm_set_chan_level(left_slice_num, PWM_CHAN_A, 0);
    pwm_set_chan_level(right_slice_num, PWM_CHAN_B, 0);
}


// Enables the car to move forward
void move_forward() {
    gpio_put(LEFT_CLOCKWISE_PIN, 1);
    gpio_put(LEFT_REVERSE_PIN, 0);
    gpio_put(RIGHT_CLOCKWISE_PIN, 1);
    gpio_put(RIGHT_REVERSE_PIN, 0);
}

// Enables the car to reverse
void reverse() {
    gpio_put(LEFT_CLOCKWISE_PIN, 0);
    gpio_put(LEFT_REVERSE_PIN, 1);
    gpio_put(RIGHT_CLOCKWISE_PIN, 0);
    gpio_put(RIGHT_REVERSE_PIN, 1);
}

// Resets the speed back to default
void reset_speed(uint8_t left_slice_num, uint8_t right_slice_num) {
    update_speed(left_slice_num, PWM_CHAN_A, 0.5);
    update_speed(right_slice_num, PWM_CHAN_B, 0.5);
}

void initMotorController(uint8_t *left_slice_num, uint8_t *right_slice_num, bool *direction) {
    // Initialize all GPIO pins
    initialize_gpio_pins();
    // Get the respective PWM slices
    *left_slice_num = get_slice_num(LEFT_MOTOR_PIN);
    *right_slice_num = get_slice_num(RIGHT_MOTOR_PIN);
    *direction = true;

    enable_motors(*left_slice_num, *right_slice_num);
}

// Enables the car to make a right turn
void turn_right(uint8_t left_slice_num, uint8_t right_slice_num, bool direction) {
    // update_speed(left_slice_num, PWM_CHAN_A, 0.8);
    // update_speed(right_slice_num, PWM_CHAN_B, 0.3);
    if (direction) {
        gpio_put(LEFT_CLOCKWISE_PIN, 1);
        gpio_put(LEFT_REVERSE_PIN, 0);
        gpio_put(RIGHT_CLOCKWISE_PIN, 0);
        gpio_put(RIGHT_REVERSE_PIN, 1);
    } else {
        gpio_put(LEFT_CLOCKWISE_PIN, 0);
        gpio_put(LEFT_REVERSE_PIN, 1);
        gpio_put(RIGHT_CLOCKWISE_PIN, 1);
        gpio_put(RIGHT_REVERSE_PIN, 0);
    }
}


// Enables the car to make a left turn
void turn_left(uint8_t left_slice_num, uint8_t right_slice_num, bool direction) {
    // update_speed(left_slice_num, PWM_CHAN_A, 0);
    // update_speed(right_slice_num, PWM_CHAN_B, 0.8);
    if (direction) {
        gpio_put(LEFT_CLOCKWISE_PIN, 0);
        gpio_put(LEFT_REVERSE_PIN, 1);
        gpio_put(RIGHT_CLOCKWISE_PIN, 1);
        gpio_put(RIGHT_REVERSE_PIN, 0);
    } else {
        gpio_put(LEFT_CLOCKWISE_PIN, 1);
        gpio_put(LEFT_REVERSE_PIN, 0);
        gpio_put(RIGHT_CLOCKWISE_PIN, 0);
        gpio_put(RIGHT_REVERSE_PIN, 1);
    }
}
