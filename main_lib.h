/*
Team 74 P7
 */

#pragma once

#include <stdio.h>
#include "common.h"

// Enumerations for operation modes and move states
enum Mode {
    Default,
    TraverseMap,
    PerformingTask,
};

enum MoveState {
    Stationary,
    Forward,
    TurnLeft,
    TurnRight,
    Backward,
};

// Current mode and move state variables
enum Mode currMode;
enum MoveState currMoveState;

// Sensor flags for use with sensor.h
bool leftSensor = false;
bool rightSensor = false;

// Motor control variables for use with motor_controller.h
uint8_t leftSliceNum, rightSliceNum;
bool direction = false;

// WiFi status variable for use with wifi.h
bool wifiEnabled = false;

// Wheel encoder variables for use with wheel_encoder.h
volatile int l_triggered = 0;
volatile int r_triggered = 0;

uint64_t r_start_time = 0;
uint64_t r_prev_time = 0;

uint64_t l_start_time = 0;
uint64_t l_prev_time = 0;

// Wheel speed variables
volatile float l_speed = 0.0;
volatile float r_speed = 0.0;

// Duty cycle and PID control variables
static float duty_cycle = 0.8;
static float updated_duty_cycle = 0.0;

float integral = 0.0;
float prev_error = 0.0;

// Ultrasonic sensor variables for use with ultrasonic.h
volatile absolute_time_t startTime_ultra;
volatile absolute_time_t endTime_ultra;

const int timeout = 26100;  // Timeout in microseconds (100ms)
volatile bool echoReceived = false;
bool ultrasonicTimeoutReceived = false;
