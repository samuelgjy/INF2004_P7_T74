#pragma once

#include <stdio.h>
#include <stdint.h>
#include "common.h"

// Define constants for PWM clock cycle
#define PWM_CYCLE_COUNT 10000.0f

#define LEFT_MOTOR_PIN 21
#define LEFT_REVERSE_PIN 2
#define LEFT_CLOCKWISE_PIN 3

#define RIGHT_MOTOR_PIN 20
#define RIGHT_REVERSE_PIN 10
#define RIGHT_CLOCKWISE_PIN 11

// Forward declaration of enum
enum pwm_chan;

void reset_speed(uint8_t left_slice_num, uint8_t right_slice_num);
void move_forward();
void turn_left(uint8_t left_slice_num, uint8_t right_slice_num, bool direction);
void turn_right(uint8_t left_slice_num, uint8_t right_slice_num, bool direction);
void stop_motors(uint8_t left_slice_num, uint8_t right_slice_num);
void update_speed(uint8_t slice_num, enum pwm_chan channel, float duty_cycle);
void reverse();

void initMotorController(uint8_t *left_slice_num, uint8_t *right_slice_num, bool *direction);

// Safety
#ifndef PWM_CYCLE_COUNT
#define PWM_CYCLE_COUNT 10000.0f
#endif

#ifndef LEFT_MOTOR_PIN
#define LEFT_MOTOR_PIN 21
#endif

#ifndef LEFT_REVERSE_PIN
#define LEFT_REVERSE_PIN 2
#endif

#ifndef LEFT_CLOCKWISE_PIN
#define LEFT_CLOCKWISE_PIN 3
#endif

#ifndef RIGHT_MOTOR_PIN
#define RIGHT_MOTOR_PIN 20
#endif

#ifndef RIGHT_REVERSE_PIN
#define RIGHT_REVERSE_PIN 10
#endif

#ifndef RIGHT_CLOCKWISE_PIN
#define RIGHT_CLOCKWISE_PIN 11
#endif
